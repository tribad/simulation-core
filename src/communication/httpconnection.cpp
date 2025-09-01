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
