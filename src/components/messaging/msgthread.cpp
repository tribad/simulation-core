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

#include <CThread.h>
#include <CEvent.h>
#include <msg.h>
#include <msgqueue.h>
#include <msgthread.h>

CMsgThread::CMsgThread()
{

}

long CMsgThread::Run(void) {
    tMsg*  msg;

    Running = true;
    for (;Running;) {
        //
        //  We wait sometime. And if we got a message in time we process the message.
        //  If the wait time expires, we do some special processing.
        if (q.Wait(waittimeout)) {
            msg = q.Get();
            if (msg != 0) {
                Process(msg);
            }
        } else {
            ProcessWaitTimeout();
        }
    }
    return 0;
}
