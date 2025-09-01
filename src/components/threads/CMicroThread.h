// **************************************************************************
//
// Modul-Name        : CMicroThread.h
// Author            : 
// Creation-Date     : 19.04.2011
// Modification-Date : 03.11.2018
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef __EXTENSIONLIBS_THREADS_LINUXTHREADS_CMICROTHREAD_INC
#define __EXTENSIONLIBS_THREADS_LINUXTHREADS_CMICROTHREAD_INC
// **************************************************************************
//                             F o r w a r d s
// **************************************************************************
// **************************************************************************
//                  C l a s s    d e c l a r a t i o n
// **************************************************************************
class CMicroThread {
public:
    CMicroThread();
    void Start();
    virtual void Run();
    void Terminate();
private:
    bool bRunning;
};
#endif // __EXTENSIONLIBS_THREADS_LINUXTHREADS_CMICROTHREAD_INC
