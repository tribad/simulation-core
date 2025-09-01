#include "logger.h"
#include "commthread.h"
#include "httpsimulationifc.h"
#include <httpdomainspec.h>

CHttpSimulationIfc::CHttpSimulationIfc(CLogger& aLog, CHttpDomainSpec* aParent) : Log(aLog)
{
    Parent = aParent;
}

void CHttpSimulationIfc::SendMsg(uint64_t a_connid, tMsg *msg) {
    CCommThread::instance().Put(a_connid, msg);
}

void CHttpSimulationIfc::SendSig(uint64_t a_connid, tSig *sig) {
    CCommThread::instance().Put(a_connid, sig);
}

void CHttpSimulationIfc::SendCoreMsg(uint64_t a_connid, tMsg *msg) {
    CCommThread::instance().Put(a_connid, msg);
}

void CHttpSimulationIfc::SendCoreSig(uint64_t a_connid, tSig *sig) {
    CCommThread::instance().Put(a_connid, sig);
}

std::shared_ptr<CCacheEntry> CHttpSimulationIfc::GetHtml(const std::string& aFileName) {
    std::shared_ptr<CCacheEntry> retval;

    if (Parent != nullptr) {
        std::string html  = Parent->HtmlDir;
        std::string aPath = html + "/" + aFileName;

        retval = CCacheThread::instance().Get(aPath);
    }

    return retval;
}
std::shared_ptr<CCacheEntry> CHttpSimulationIfc::GetContent(const std::string& aFileName) {
    std::shared_ptr<CCacheEntry> retval;

    if (Parent != nullptr) {
        std::string content  = Parent->ContentDir;
        std::string aPath = content + "/" + aFileName;

        retval = CCacheThread::instance().Get(aPath);
    }
    return retval;
}
std::shared_ptr<CCacheEntry> CHttpSimulationIfc::GetData(const std::string& aFileName) {
    std::shared_ptr<CCacheEntry> retval;

    if (Parent != nullptr) {
        std::string data  = Parent->DataDir;
        std::string aPath = data + "/" + aFileName;

        retval = CCacheThread::instance().Get(aPath);
    }
    return retval;
}
