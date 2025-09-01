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
