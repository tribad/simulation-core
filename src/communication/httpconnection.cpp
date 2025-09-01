#include <stdint.h>
#include <stdlib.h>
#include "netpack.h"
#include "simifc.h"
#include "logger.h"
#include "commthread.h"
#include "httpconnection.h"

CHTTPConnection::CHTTPConnection(CLogger& aLogger) : CTcpConnection(aLogger)
{
}

CHTTPConnection::CHTTPConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger) : CTcpConnection (aHandle, aConnectionId, aLogger, new CHTTPProtocol(aConnectionId, aLogger)) {
}

CHTTPConnection::~CHTTPConnection() {
}

int CHTTPConnection::Process(std::shared_ptr<tMsg> aMsg) {
    int       retval = 0;

    if (aMsg != 0) {
        switch (aMsg->id) {
        default:
            if (Protocol != nullptr) {
                std::shared_ptr<tNetPack> packet = Protocol->Process(aMsg);
                if (packet != nullptr) {
                    retval = Write(packet);
                }
            }
            break;
        }
    }
    return retval;
}

int CHTTPConnection::Process(std::shared_ptr<tNetPack> aPacket) {
    int                       retval = 0;
    std::shared_ptr<tNetPack> packet;

    if (aPacket) {
        if (Protocol != nullptr) {
            packet = Protocol->Process(aPacket);
            if (packet != 0) {
                retval = Write(packet);
            }
        }
    }
    return retval;
}
int CHTTPConnection::Process(std::shared_ptr<tTimerEvent> aTimerId) {
    //
    //
    (void)aTimerId;
    //
    return 0;
}
