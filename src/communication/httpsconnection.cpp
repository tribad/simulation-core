// MIT License
//
// Copyright (c) 2018 Hans-Juergen Lange<hjl@simulated-universe.de>
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

