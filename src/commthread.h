// MIT License
//
// Copyright (c) 2016 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#ifndef __COMMTHREAD_INC__
#define __COMMTHREAD_INC__

#include <sharedmsgthread.h>

class tJSON;
//
//  From here on we define a new comm thread.
class CCommThread : public CSharedMsgThread {
public:
    CCommThread();
    virtual ~CCommThread() {}
    void Process(std::shared_ptr<tMsg> aMsg) override;

    void Put (uint64_t aConnId, tJSON* aJson);
    void Put (uint64_t aConnId, tMsg*  aMsg);
public:  //  static methods here.
    static void Start();
    static CCommThread& instance() {
        static CCommThread comm;

        return comm;
    }
public:
    CLogger             commlogger;
};

#endif
