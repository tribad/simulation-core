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
// Modul-Name        : CMutex.cpp
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
#include "CMutex.h"
// **************************************************************************
//
//  Method-Name       : CMutex()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 18.02.2011 14:06:13
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CMutex::CMutex() {
    pthread_mutex_init(&mutex, 0);
}
// **************************************************************************
//
//  Method-Name       : ~CMutex()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 18.02.2011 14:06:49
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CMutex::~CMutex() {
    pthread_mutex_destroy(&mutex);
}
// **************************************************************************
//
//  Method-Name       : Lock()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 18.02.2011 14:07:12
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CMutex::Lock() {
    pthread_mutex_lock(&mutex);
}
// **************************************************************************
//
//  Method-Name       : Unlock()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 18.02.2011 14:07:35
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CMutex::Unlock() {
    pthread_mutex_unlock(&mutex);
}
// **************************************************************************
//
//  Method-Name       : Check()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 18.02.2011 14:08:16
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool CMutex::Check() {
    return (pthread_mutex_trylock(&mutex));
}
