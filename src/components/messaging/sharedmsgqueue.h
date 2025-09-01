// **************************************************************************
//
// Modul-Name        :
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     :
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef CSHAREDMSGQUEUE_H
#define CSHAREDMSGQUEUE_H

#include <list>
#include <CEvent.h>
#include <memory>

class tMsg;

class CSharedMsgQueue : public CEvent
{
public:
    CSharedMsgQueue();
    virtual ~CSharedMsgQueue();
    void Put(std::shared_ptr<tMsg> msg);
    void PutBack(std::shared_ptr<tMsg> msg);
    std::shared_ptr<tMsg> Get(void);
    size_t Size() {return msgqueue.size();}
    void Clear() {
        Lock();
        msgqueue.clear();
        Unlock();
    }
    //
    //  This is the queue checker for the event base class.
    virtual bool CheckCondition();
private:
    std::list<std::shared_ptr<tMsg> > msgqueue;
};

#endif // CSHAREDMSGQUEUE_H
