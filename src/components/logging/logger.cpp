#include <stdarg.h>
#include <cstdio>
#include <set>
#include <msg.h>
#include <CEvent.h>
#include <logger.h>
#include <logthread.h>
//
//  Zeroing the pointers.
std::set<CLogger*> logger;
//
//  Dont use it for global instances of any kind.
//  It cannot be guaranteed that the LogThread has been constructed already.
CLogger::CLogger(ModulId aModul, std::string aName)
{
    logger.insert(this);
    modul = (1u << (uint8_t)aModul);
    //
    //  Log everything
    level = UINT64_MAX;
    CLogThread::Instance.RegisterModul(modul, aName);
}

CLogger::~CLogger() {
    logger.erase(this);
}
//
//  The size is an estimate
int CLogger::Write(LogLevel aLevel, const char *aFormat, ...) {
    int retval = 0;
    if (level >= (uint64_t)aLevel) {
        va_list pa;
        va_start(pa, aFormat);
        retval = vsnprintf(buffer, MAX_LOGMESSAGE-1, aFormat, pa);
        CMsgWriteLog* msg = new CMsgWriteLog;
        msg->level = (uint64_t)aLevel;
        msg->modul = modul;
        msg->msg   = buffer;

        CLogThread::Instance.Put(msg);

        return retval;
    }
    return 0;
}

int CLogger::Write(LogLevel aLevel, const std::string& aMsg) {
    if (level >= (uint64_t)aLevel) {
        CMsgWriteLog* msg = new CMsgWriteLog;
        msg->level = (uint64_t)aLevel;
        msg->modul = modul;
        msg->msg   = aMsg;

        CLogThread::Instance.Put(msg);

        return aMsg.size();
    }
    return 0;
}

int CLogger::Write(LogLevel aLevel, const std::ostringstream& aStream) {
    if (level >= (uint64_t)aLevel) {
        CMsgWriteLog* msg = new CMsgWriteLog;
        msg->level = (uint64_t)aLevel;
        msg->modul = modul;
        msg->msg   = aStream.str();

        CLogThread::Instance.Put(msg);

        return aStream.str().size();
    }
    return 0;
}

void CLogger::SetLogLevel(LogLevel aLevel) {
    level = (1 << (uint64_t)aLevel);
}

void CLogger::Enable(ModulId aModul) {
    CMsgEnableModul* msg = new CMsgEnableModul;

    msg->modul = (1u << (uint8_t)aModul);

    CLogThread::Instance.Put(msg);
}

void CLogger::Disable(ModulId aModul) {
    CMsgDisableModul* msg = new CMsgDisableModul;

    msg->modul = (1u << (uint8_t)aModul);

    CLogThread::Instance.Put(msg);

}
