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
