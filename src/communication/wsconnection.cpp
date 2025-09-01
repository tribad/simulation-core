// MIT License
//
// Copyright (c) 2017 Hans-Juergen Lange<hjl@simulated-universe.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdint.h>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include "netpack.h"
#include "simifc.h"
#include "logger.h"
#include "commthread.h"
#include "wsconnection.h"
extern long loglevel;
CWSConnection::CWSConnection(CLogger& aLogger) : CTcpConnection(aLogger)
{
}

CWSConnection::CWSConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger) : CTcpConnection (aHandle, aConnectionId, aLogger, new CWSJSONProtocol(aConnectionId, aLogger)) {
    Log.Write(LogLevel::Trace, "Got a new WebSocket connection");
}

CWSConnection::~CWSConnection() {
}

int CWSConnection::Process(std::shared_ptr<tMsg> aMsg) {
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

int CWSConnection::Process(std::shared_ptr<tNetPack> aPacket) {
    int                       retval = 0;
    std::shared_ptr<tNetPack> packet;

    if (aPacket != 0) {
        if (Protocol != 0) {
            packet = Protocol->Process(aPacket);
            if (packet != 0) {
                retval = Write(packet);
            }
        }
    }
    return retval;
}
int CWSConnection::Process(std::shared_ptr<tTimerEvent> aTimerId) {
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
