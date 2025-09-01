#include <stdint.h>
#include <stdlib.h>
#include "netpack.h"
#include "simifc.h"
#include "logger.h"
#include "commthread.h"
#include "connection.h"
#include "jsonconnection.h"
#include "none-json-proto.h"


CJsonConnection::CJsonConnection(CLogger& aLogger) : CTcpConnection(aLogger)
{
    CloseAfterSend = false;
}

CJsonConnection::CJsonConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger) : CTcpConnection (aHandle, aConnectionId, aLogger, new CNoneJSONProtocol(aLogger)) {
    CloseAfterSend = false;
}

CJsonConnection::~CJsonConnection() {
}
