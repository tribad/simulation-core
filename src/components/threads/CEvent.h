// **************************************************************************
//
// Modul-Name        : CEvent.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 18.02.2011
// Modification-Date : 03.11.2018
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#pragma once
#ifndef __EXTENSIONLIBS_THREADS_LINUXTHREADS_CEVENT_INC
#define __EXTENSIONLIBS_THREADS_LINUXTHREADS_CEVENT_INC

#include <pthread.h>
#include <stdint.h>
#include "CMutex.h"
// **************************************************************************
//                  C l a s s    d e c l a r a t i o n
// **************************************************************************
class CEvent  : public CMutex {
public:
    CEvent();
    ~CEvent();
    bool Wait(uint64_t aWait=0);
    bool Set();
    void Reset();
    virtual bool CheckCondition();
private:
    pthread_cond_t condition;
};
#endif // __EXTENSIONLIBS_THREADS_LINUXTHREADS_CEVENT_INC
