// **************************************************************************
//
// Modul-Name        :
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     :
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#include "sharedmsgqueue.h"

CSharedMsgQueue::CSharedMsgQueue()
{

}


CSharedMsgQueue::~CSharedMsgQueue() {

}

void CSharedMsgQueue::Put(std::shared_ptr<tMsg> msg) {
    if (msg != 0) {
        Lock();
        msgqueue.push_back(msg);
        Unlock();
        Set();
    }
}

void CSharedMsgQueue::PutBack(std::shared_ptr<tMsg> msg) {
    if (msg != 0) {
        Lock();
        msgqueue.push_front(msg);
        Unlock();
        //
        //  Do not signal that something has been put back into the queue.
        //  This may race the thread who holds the queue.
    }
}

std::shared_ptr<tMsg> CSharedMsgQueue::Get() {
    std::shared_ptr<tMsg> msg;

    Lock();
    if (!msgqueue.empty()) {
        msg=msgqueue.front();
        msgqueue.pop_front();
    }
    Unlock();

    return msg;
}

bool CSharedMsgQueue::CheckCondition() {
    return (msgqueue.empty());
}
