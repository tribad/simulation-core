#include <stdint.h>
#include <stdlib.h>
#include "netpack.h"
#include "simifc.h"
#include "logger.h"
#include "ifcthread.h"
#include "phpsconnection.h"
#include "phpsifcthread.h"

CPHPsIfcThread::CPHPsIfcThread( ) : CSSLIfcThread(ModulId::PhpSSLServer, "PHPs-Ifc")
{

}

std::shared_ptr<CConnection> CPHPsIfcThread::NewConnection(int aHandle, uint64_t aConnectionId) {
    return (std::make_shared<CPHPsConnection>(aHandle, aConnectionId, ifcLogger));
}
