// **************************************************************************
//
// Modul-Name        : CMutex.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 18.02.2011
// Modification-Date : 03.11.2018
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef __EXTENSIONLIBS_THREADS_LINUXTHREADS_CMUTEX_INC
#define __EXTENSIONLIBS_THREADS_LINUXTHREADS_CMUTEX_INC
// **************************************************************************
//                  C l a s s    d e c l a r a t i o n
// **************************************************************************
class CMutex {
public:
    CMutex();
    ~CMutex();
    void Lock();
    void Unlock();
    bool Check();
protected:
    pthread_mutex_t mutex;
};
#endif // __EXTENSIONLIBS_THREADS_LINUXTHREADS_CMUTEX_INC
