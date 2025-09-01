#include "jsonconnection.h"
#include "logger.h"
#include "jsonifcthread.h"

CJsonIfcThread::CJsonIfcThread() : CIfcThread(ModulId::JsonIfcServer, "Plain-JSON-Ifc")
{

}

std::shared_ptr<CConnection> CJsonIfcThread::NewConnection(int aHandle, uint64_t aConnectionId) {
    return (std::make_shared<CJsonConnection>(aHandle, aConnectionId, ifcLogger));
}
