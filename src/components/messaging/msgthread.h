#ifndef MSGTHREAD_H
#define MSGTHREAD_H

#include <CThread.h>
#include <msgqueue.h>
#include <sharedmsgqueue.h>

class CMsgThread : public CThread
{
public:
    CMsgThread();
    CMsgThread(const std::string& aName, uint64_t aWaitTimeout = 0) : CThread(aName) , waittimeout(aWaitTimeout) {}
    virtual ~CMsgThread() {}
    virtual bool InitInstance() {return true;}
    virtual long Run(void);
    virtual void Process(tMsg* aMsg) {delete aMsg;}
    virtual void ProcessWaitTimeout() {}
    virtual void ExitInstance() {}
public:
    uint64_t  waittimeout;
    CMsgQueue q;
};

#endif // MSGTHREAD_H
