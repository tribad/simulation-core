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
