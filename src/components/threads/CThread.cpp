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
// Modul-Name        : CThread.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 16.01.2011
// Modification-Date : 03.11.2018
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
// **************************************************************************
//                   E x t r a   I n c l u d e   L i s t
// **************************************************************************
#include <string>
#include <stddef.h>
#include <stdint.h>
#include <pthread.h>
#include <memory.h>
#include <signal.h>
// **************************************************************************
//                   F i r s t   I n c l u d e   L i s t
// **************************************************************************
#include "CThread.h"

int CThread::Count;
extern "C" {
// **************************************************************************
//
//  Method-Name       : startfnc()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 30.04.2011 09:54:29
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void* startfnc(void* ptr){
    CThread *lpThread;
    void *return_code=0;
    
    lpThread=((CThread *)(ptr));
    if (lpThread->InitInstance()!=0) {
        CThread::Count++;
        return_code = (void*)(((CThread *)(ptr))->Run());
        lpThread->ExitInstance();
        if (lpThread->DoDelete()) {
            delete lpThread;
        }
        CThread::Count--;
        pthread_exit(return_code);
    }
    return (return_code);
}
} //  extern C

// **************************************************************************
//
//  Method-Name       : CThread()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 16.01.2011
//  Modification-Date : 30.04.2011 09:45:55
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CThread::CThread() {
    bDoDelete = true;
    ThreadID  = 0;
    Running   = false;
}
// **************************************************************************
//
//  Method-Name       : CThread()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 30.04.2011 09:45:54
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CThread::CThread(const std::string& aName) {
    Name      = aName;
    bDoDelete = true;
    ThreadID  = 0;
    Running   = false;
}
// **************************************************************************
//
//  Method-Name       : CThread()
//  Author            : 
//  Creation-Date     : 24.04.2011
//  Modification-Date : 30.04.2011 09:45:54
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CThread::CThread(const CThread& aOld) {
    if (aOld.ThreadID==0) {
        bDoDelete=true;
    } else {
        ThreadID=aOld.ThreadID;
        Name=aOld.Name;
        bDoDelete=aOld.bDoDelete;
    }
}
// **************************************************************************
//
//  Method-Name       : ~CThread()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 18.02.2011 13:53:26
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CThread::~CThread() {
    
}
// **************************************************************************
//
//  Method-Name       : InitInstance()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 16.01.2011
//  Modification-Date : 18.02.2011 13:48:40
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool CThread::InitInstance() {
    sigset_t signals;
    int      err = 0;
    /*
     * We must block all signals to prevent exceptions here.
     * Therefor we create a set of signals.
     */
    sigemptyset(&signals);
    for (int i=SIGRTMIN;i<SIGRTMAX;++i) {
        sigaddset(&signals, i);
    }
    sigaddset(&signals, SIGHUP);
    sigaddset(&signals, SIGPIPE);
    sigaddset(&signals, SIGURG );
    sigaddset(&signals, SIGIO);

    err=sigprocmask(SIG_BLOCK , &signals, NULL);

    return (err == 0);
}
// **************************************************************************
//
//  Method-Name       : Create()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 18.02.2011 14:14:40
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CThread::Create() {
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&ThreadID, &attr, startfnc, (void *)(this));
    
}
// **************************************************************************
//
//  Method-Name       : Run()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 18.02.2011 14:14:50
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
long CThread::Run() {
    return (-1);
}
// **************************************************************************
//
//  Method-Name       : ExitInstance()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 18.02.2011
//  Modification-Date : 18.02.2011 14:11:42
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CThread::ExitInstance() {
    
}
// **************************************************************************
//
//  Method-Name       : SetName()
//  Author            : 
//  Creation-Date     : 24.04.2011
//  Modification-Date : 24.04.2011 10:06:28
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CThread::SetName(const std::string& aName) {
    Name=aName;
}
// **************************************************************************
//
//  Method-Name       : GetName()
//  Author            : 
//  Creation-Date     : 24.04.2011
//  Modification-Date : 24.04.2011 10:07:11
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
std::string& CThread::GetName() {
    return (Name);
}
// **************************************************************************
//
//  Method-Name       : DoDelete()
//  Author            : 
//  Creation-Date     : 29.04.2011
//  Modification-Date : 29.04.2011 07:06:27
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool CThread::DoDelete() {
    return (bDoDelete);
}
