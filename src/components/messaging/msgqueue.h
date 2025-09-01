// MIT License
//
// Copyright (c) 2018 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include <list>
#include <msg.h>
#include <CEvent.h>

class CMsgQueue : public CEvent
{
public:
    CMsgQueue();
    virtual ~CMsgQueue();
    void Put(tMsg* msg);
    void PutBack(tMsg* msg);
    tMsg* Get(void);
    size_t Size() {return msgqueue.size();}
    void Clear() {
        Lock();
        for (auto & m : msgqueue) {
            delete m;
        }
        msgqueue.clear();
        Unlock();
    }
    //
    //  This is the queue checker for the event base class.
    virtual bool CheckCondition();
private:
    std::list<tMsg*>     msgqueue;
};

#endif // MSGQUEUE_H
