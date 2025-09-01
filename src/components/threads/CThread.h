// **************************************************************************
//
// Modul-Name        : CThread.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 16.01.2011
// Modification-Date : 03.11.2018
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef __EXTENSIONLIBS_THREADS_LINUXTHREADS_CTHREAD_INC
#define __EXTENSIONLIBS_THREADS_LINUXTHREADS_CTHREAD_INC
#include <string>
// **************************************************************************
//                             F o r w a r d s
// **************************************************************************
extern "C" {
void* startfnc(void* ptr);
} // extern C 
// **************************************************************************
//                  C l a s s    d e c l a r a t i o n
// **************************************************************************
class CThread {
public:
    CThread();
    CThread(const std::string& aName);
    CThread(const CThread& aOld);
    virtual ~CThread();
    void Create();
    virtual bool InitInstance();
    virtual long Run();
    virtual void ExitInstance();
    void SetName(const std::string& aName);
    std::string& GetName();
    bool DoDelete();
    void Stop() {Running = false;}
public:
    static int Count;
    bool       Running;
protected:
    std::string Name;
    pthread_t ThreadID;
private:
    bool bDoDelete;
};
#endif // __EXTENSIONLIBS_THREADS_LINUXTHREADS_CTHREAD_INC
