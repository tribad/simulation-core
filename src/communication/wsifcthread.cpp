#include "wsconnection.h"
#include "logger.h"
#include "wsifcthread.h"

CWSIfcThread::CWSIfcThread( ) : CIfcThread(ModulId::WsIfcServer, "WS-Ifc")
{

}

std::shared_ptr<CConnection> CWSIfcThread::NewConnection(int aHandle, uint64_t aConnectionId) {
    return (std::make_shared<CWSConnection>(aHandle, aConnectionId, ifcLogger));
}
