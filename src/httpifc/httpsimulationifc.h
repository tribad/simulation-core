#ifndef HTTPSIMULATIONIFC_H
#define HTTPSIMULATIONIFC_H

#include <cstdint>
#include <msg.h>
#include <logger.h>
#include <cachethread.h>

class tSQLRecordSet;
class CHttpDomainSpec;
class IFileCacheNotifier;

class CHttpSimulationIfc
{
public:
    CHttpSimulationIfc(CLogger& aLog, CHttpDomainSpec* aParent);
    //
    //  DB-Access
    int DBConnect(const char* dbname, const char *user, const char *password, const char* port, const char* host);
    tSQLRecordSet* DBExec(int handle, const char* cmd);
    void DBClose(int handle);
    //
    //  In-Core messaging
    void SendMsg(uint64_t a_connid, tMsg* msg);
    void SendSig(uint64_t a_connid, tSig* sig);
    void SendCoreMsg(uint64_t a_connid, tMsg* msg);
    void SendCoreSig(uint64_t a_connid, tSig* sig);
    //
    //  File Cache
    std::shared_ptr<CCacheEntry> GetHtml(const std::string& aFileName);
    std::shared_ptr<CCacheEntry> GetContent(const std::string& aFileName);
    std::shared_ptr<CCacheEntry> GetData(const std::string& aFileName);
    //
    //  Logging
    CLogger& Log;
private:
    CHttpDomainSpec* Parent;
};

#endif // HTTPSIMULATIONIFC_H
