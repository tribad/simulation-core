#include <stdint.h>
#include <stdlib.h>
#include <gnutls/gnutls.h>
#include <sstream>
#include <iomanip>
#include "netpack.h"
#include "simifc.h"
#include "logger.h"
#include "commthread.h"
#include "connection.h"
#include "wssconnection.h"
#include "ws-json-proto.h"

extern long loglevel;

CWSSConnection::CWSSConnection(CLogger& aLogger) : CSSLConnection (aLogger)
{
}

CWSSConnection::CWSSConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger) : CSSLConnection (aHandle, aConnectionId, aLogger, new CWSJSONProtocol(aConnectionId, aLogger)) {
}

CWSSConnection::~CWSSConnection() {
}
int CWSSConnection::Process(std::shared_ptr<tMsg> aMsg) {
    int                       retval = 0;
    std::shared_ptr<tNetPack> packet;

    if (aMsg != 0) {
        if (aMsg->type & MSG_TYPE_INTERNAL) {
            switch (aMsg->id) {
            default:
                if (Protocol != 0) {
                    if (aMsg->id == IDM_TMSGWSDISCONNECTREQ) {
                        SendReplyAfterClose = true;
                    }
                    packet = Protocol->Process(aMsg);
                    if (packet != 0) {
                        retval = Write(packet);
                    }
                }
                break;
            }
        } else {
                std::ostringstream oss;
                oss << "{";
                aMsg->pack_json(oss);
                oss << "}";

                auto conn = std::get<tConnection>(aMsg->dst);

                std::shared_ptr<tNetPack> packet = std::make_shared<tNetPack>(oss.str().size());

                memcpy(packet->buffer, oss.str().c_str(), oss.str().size());
                packet->len    = oss.str().size();
                packet->connid = conn.id;

                if (loglevel & 0x1000) {
                    std::ostringstream lss;
                    lss << "WSS-Data:" << std::hex << std::setw(16) << std::setfill('0') << packet->connid << "::" << std::dec << oss.str().size() << "::" << oss.str();

                    Log.Write(LogLevel::Trace, lss);
                }

                retval = Process(packet);
        }
    }
    return retval;
}

int CWSSConnection::Process(std::shared_ptr<tNetPack> aPacket) {
    int       retval = 0;
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

int CWSSConnection::Process(std::shared_ptr<tTimerEvent> aTimerId) {
    int retval = 0;

    switch (aTimerId->what) {
    case FIRSTDATA_TIMERID:
        if ((InByteCount == 0) && (InPacketCount == 0)) {
            std::ostringstream oss;

            oss << "Got no data in time. Aborting: " << Handle;
            Log.Write(LogLevel::Info, oss);

            retval = -1;
        }
        break;
    case PING_TIMERID: {
        std::shared_ptr<tMsg>     pingreq = std::make_shared<tMsgWSPingReq>();
        std::shared_ptr<tNetPack> packet;

        if (Protocol != nullptr) {
            packet = Protocol->Process(pingreq);
            if (packet) {
                retval = Write(packet);
            }
        }
        break;
        }
    case CLOSEWAIT_TIMERID:
    {
        std::ostringstream oss;

        oss << "Close wait time reached before socket event: " << Handle;
        Log.Write(LogLevel::Info, oss);
        retval = -1;
        break;
    }
    default:
        break;

    }

    return retval;
}

