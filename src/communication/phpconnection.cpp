#include <stdint.h>
#include <stdlib.h>
#include "netpack.h"
#include "simifc.h"
#include "logger.h"
#include "commthread.h"
#include "connection.h"
#include "phpconnection.h"
#include "none-json-proto.h"


CPHPConnection::CPHPConnection(CLogger& aLogger) : CTcpConnection(aLogger)
{
    CloseAfterSend = true;
}

CPHPConnection::CPHPConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger) : CTcpConnection (aHandle, aConnectionId, aLogger, new CNoneJSONProtocol(aLogger)) {
    CloseAfterSend = true;
}

CPHPConnection::~CPHPConnection() {
}
