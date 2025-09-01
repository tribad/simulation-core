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
