#include "phpconnection.h"
#include "logger.h"
#include "phpifcthread.h"

CPHPIfcThread::CPHPIfcThread() : CIfcThread(ModulId::PhpIfcServer, "PHP-Ifc")
{

}

std::shared_ptr<CConnection> CPHPIfcThread::NewConnection(int aHandle, uint64_t aConnectionId) {
    return (std::make_shared<CPHPConnection>(aHandle, aConnectionId, ifcLogger));
}
