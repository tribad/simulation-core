#include <stdint.h>
#include <stdlib.h>
#include "netpack.h"
#include "simifc.h"
#include "logger.h"
#include "commthread.h"
#include "connection.h"
#include "phpsconnection.h"

CPHPsConnection::CPHPsConnection(CLogger& aLogger) : CSSLConnection(aLogger)
{
}

CPHPsConnection::CPHPsConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger) : CSSLConnection (aHandle, aConnectionId, aLogger) {
}

CPHPsConnection::~CPHPsConnection() {
    if (json != nullptr) {
        delete json;
    }
}

int CPHPsConnection::Process(std::shared_ptr<tNetPack> aPacket) {

    json=parse(json, aPacket->buffer, aPacket->len);
    if (completed(json)) {
        CCommThread::instance().Put(aPacket->connid, json);
    }
    return 0;
}
