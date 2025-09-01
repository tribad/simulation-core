#include <set>
#include "httpdomainspec.h"
#include "httpsconnection.h"
#include "logger.h"
#include "ifcthread.h"
#include "sslifcthread.h"
#include "httpsifcthread.h"

CHTTPSIfcThread::CHTTPSIfcThread() : CSSLIfcThread(ModulId::HttpSSLServer, "HTTPS-Ifc")
{
    std::set<CHttpDomainSpec*> specs;
    //
    //  Lock the domain map access
    CHTTPProtocol::domains_lock.lock();
    for (auto d : CHTTPProtocol::domains) {
        if (specs.find(d.second) == specs.end()) {
            //d.second->InitNotifier(aWatchList);
        }
    }
    CHTTPProtocol::domains_lock.unlock();

}

std::shared_ptr<CConnection> CHTTPSIfcThread::NewConnection(int aHandle, uint64_t aConnectionId) {
    return (std::make_shared<CHTTPSConnection>(aHandle, aConnectionId, ifcLogger));
}
