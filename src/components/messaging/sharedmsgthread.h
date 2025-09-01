#ifndef SHAREDMSGTHREAD_H
#define SHAREDMSGTHREAD_H

#include <CThread.h>
#include <sharedmsgqueue.h>

class CSharedMsgThread : public CThread
{
public:
    CSharedMsgThread();
    CSharedMsgThread(const std::string& aName, uint64_t aWaitTimeout = 0) : CThread(aName) , waittimeout(aWaitTimeout) {}
    virtual ~CSharedMsgThread() {}
    virtual bool InitInstance() {return true;}
    virtual long Run(void);
    virtual void Process(std::shared_ptr<tMsg> aMsg) {(void)aMsg;}
    virtual void ProcessWaitTimeout() {}
    virtual void ExitInstance() {}
public:
    uint64_t        waittimeout;
    CSharedMsgQueue q;
};

#endif // SHAREDMSGTHREAD_H
