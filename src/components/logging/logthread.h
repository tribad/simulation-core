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

#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include <map>
#include "msgqueue.h"
#include "CSystemTime.h"
#include "CThread.h"

#define MEDIATYPE_CONSOLE 0x01
#define MEDIATYPE_NCURSES 0x02
#define MEDIATYPE_SYSLOG  0x04

struct CMsgWriteLog : public tMsg {
    CMsgWriteLog() : tMsg(0x0f3d8672ebe0ab8e) { }
    CSystemTime stamp;
    uint64_t    modul;
    uint64_t    level;
    std::string msg;
};

struct CMsgEnableModul : public tMsg {
    CMsgEnableModul() : tMsg(0x7f2bf08135d803af) { }
    uint64_t    modul;
};

struct CMsgDisableModul : public tMsg {
    CMsgDisableModul() : tMsg(0x4872c94302313910) { }
    uint64_t    modul;
};

class CLogThread : public CThread
{
public:
    CLogThread();
    virtual ~CLogThread();
    virtual bool InitInstance();
    virtual long Run();
    void RegisterModul(uint64_t aModul, std::string aName);
    void Put(tMsg* aMsg) {
        if (aMsg->id == 0x0f3d8672ebe0ab8e) {
            CMsgWriteLog* m = (CMsgWriteLog*)(aMsg);
            if (m->modul & Modul) {
                q.Put(aMsg);
            } else {
                delete aMsg;
            }
        } else {
            q.Put(aMsg);
        }
    }
private:
    void Process(CMsgWriteLog* aMsg);
    void Process(CMsgEnableModul* aMsg);
    void Process(CMsgDisableModul* aMsg);
    void ConsoleOutput(CMsgWriteLog* aMsg);
    void NcursesOutput(CMsgWriteLog* aMsg);
    void SyslogOutput(CMsgWriteLog* aMsg);
public:
    static CLogThread Instance;
private:
    uint64_t  Modul;
    uint8_t   Medium;
    CMsgQueue q;
    std::map<uint64_t, std::string> ModulMap;
};


#endif // LOGTHREAD_H
