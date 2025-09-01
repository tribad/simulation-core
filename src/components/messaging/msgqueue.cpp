#include <CEvent.h>
#include "msg.h"
#include "msgqueue.h"

CMsgQueue::CMsgQueue()
{

}

CMsgQueue::~CMsgQueue() {

}

void CMsgQueue::Put(tMsg *msg) {
    if (msg != 0) {
        Lock();
        msgqueue.push_back(msg);
        Unlock();
        Set();
    }
}

void CMsgQueue::PutBack(tMsg *msg) {
    if (msg != 0) {
        Lock();
        msgqueue.push_front(msg);
        Unlock();
        //
        //  Do not signal that something has been put back into the queue.
        //  This may race the thread who holds the queue.
    }
}

tMsg* CMsgQueue::Get() {
    tMsg *msg;
    Lock();
    if (!msgqueue.empty()) {
        msg=msgqueue.front();
        msgqueue.pop_front();
    } else {
        msg = nullptr;
    }
    Unlock();

    return msg;
}

bool CMsgQueue::CheckCondition() {
    return (msgqueue.empty());
}
