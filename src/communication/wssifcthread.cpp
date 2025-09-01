#include "wssconnection.h"
#include "logger.h"
#include "ifcthread.h"
#include "sslifcthread.h"
#include "wssifcthread.h"

CWSSIfcThread::CWSSIfcThread(): CSSLIfcThread(ModulId::WsSSLServer, "WSs-Ifc")
{

}

std::shared_ptr<CConnection> CWSSIfcThread::NewConnection(int aHandle, uint64_t aConnectionId) {
    return (std::make_shared<CWSSConnection>(aHandle, aConnectionId, ifcLogger));
}
