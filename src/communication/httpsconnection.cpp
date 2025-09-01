#include <stdint.h>
#include <stdlib.h>
#include <gnutls/gnutls.h>
#include "netpack.h"
#include "simifc.h"
#include "logger.h"
#include "commthread.h"
#include "connection.h"
#include "httpsconnection.h"
#include "httpdomainspec.h"
#include "http-proto.h"

CHTTPSConnection::CHTTPSConnection(CLogger& aLogger) : CSSLConnection (aLogger)
{
}

CHTTPSConnection::CHTTPSConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger) : CSSLConnection (aHandle, aConnectionId, aLogger, new CHTTPProtocol(aConnectionId,aLogger)) {
}

CHTTPSConnection::~CHTTPSConnection() {
}
int CHTTPSConnection::Process(std::shared_ptr<tMsg> aMsg) {
    int                       retval = 0;
    std::shared_ptr<tNetPack> packet;

    if (aMsg) {
        switch (aMsg->id) {
        default:
            if (Protocol != nullptr) {
                if (aMsg->id == IDM_TMSGWSDISCONNECTREQ) {
                    SendReplyAfterClose = true;
                }
                packet = Protocol->Process(aMsg);
                if (packet) {
                    retval = Write(packet);
                }
            }
            break;
        }
    }
    return retval;
}

int CHTTPSConnection::Process(std::shared_ptr<tNetPack> aPacket) {
    int                       retval = 0;
    std::shared_ptr<tNetPack> packet;

    if (aPacket) {
        if (Protocol != nullptr) {
            packet = Protocol->Process(aPacket);
            if (packet) {
                retval = Write(packet);
            }
        }
    }
    return retval;
}

int CHTTPSConnection::Process(tTimerEvent* aTimerId) {
    int retval = 0;

    switch (aTimerId->what) {
    case FIRSTDATA_TIMERID:
        if ((InByteCount == 0) && (InPacketCount == 0)){
            std::ostringstream oss;
            oss << "Got no data in time. Aborting: " << Handle;
            Log.Write(LogLevel::Info, oss);
            Close();
            retval = -1;
        }
        break;
    case PING_TIMERID:
        break;
    default:
        break;

    }

    return retval;
}

