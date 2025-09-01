// MIT License
//
// Copyright (c) 2011 Hans-Juergen Lange<hjl@simulated-universe.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// **************************************************************************
//
// Modul-Name        : CEvent.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 18.02.2011
// Modification-Date : 03.11.2018
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
// **************************************************************************
//                   E x t r a   I n c l u d e   L i s t
// **************************************************************************
#include <pthread.h>
#include <stdint.h>
#include "CEvent.h"
// **************************************************************************
//
//  Method-Name       : CEvent()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 18.02.2011 13:59:02
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CEvent::CEvent() {
    pthread_cond_init(&condition, 0);
}
// **************************************************************************
//
//  Method-Name       : ~CEvent()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 18.02.2011 13:59:28
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CEvent::~CEvent() {
    pthread_cond_destroy(&condition);
}
// **************************************************************************
//
//  Method-Name       : Wait()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 03.11.2018
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool CEvent::Wait(uint64_t aWait) {
    int result = 0;
    
    pthread_mutex_lock(&mutex);
    if (CheckCondition()) {
        if (aWait == 0u) {
            result = pthread_cond_wait(&condition, &mutex);
        } else {
            struct timespec end;
            /*
             * Get the time we have now.
             */
            clock_gettime(CLOCK_REALTIME, &end);
            /*
             * Because we give the delay in milliseconds and the timespec has
             * seconds an nanoseconds we take the modulo an make it nanoseconds
             * to be added to the now value.
             */
            end.tv_nsec+=((aWait%1000ul)*1000000ul);
            /*
             * If come across a second we add a seconds to the tv_sec and reduce the
             * nanosecond part according.
             */
            if (end.tv_nsec >= 1000000000l) {
                end.tv_sec++;
                end.tv_nsec -= 1000000000l;
            }
            /*
             * At the end we add the seconds to the now value.
             */
            end.tv_sec += (aWait/1000ul);

            result = pthread_cond_timedwait(&condition, &mutex, &end);
        }
    }
    pthread_mutex_unlock(&mutex);
    
    return (!(long)result);
}
// **************************************************************************
//
//  Method-Name       : Set()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 18.02.2011 14:03:51
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool CEvent::Set() {
    int result;
    
    pthread_mutex_lock(&mutex);
    result=pthread_cond_broadcast(&condition);
    pthread_mutex_unlock(&mutex);
    
    return (result);
}
// **************************************************************************
//
//  Method-Name       : Reset()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 18.02.2011 13:56:31
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CEvent::Reset() {
    
}
// **************************************************************************
//
//  Method-Name       : CheckCondition
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 03.11.2018
//  Modification-Date : 03.11.2018
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool CEvent::CheckCondition() {
    return true;
}
