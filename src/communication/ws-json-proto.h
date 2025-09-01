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

#ifndef WS_JSON_PROTO_H
#define WS_JSON_PROTO_H

#include "json.h"
#include "protocol.h"
#include "httprequest.h"

#include "tMsgWSConnectReq.h"
#include "tMsgWSConnectReply.h"
#include "tMsgWSDisconnectReq.h"
#include "tSigWSDisconnectIndication.h"
#include "wsmessage.h"

/*
 * The timeout values in milliseconds.
 */
#define FIRSTDATA_TIMEOUT 1000
#define PING_TIME         30000
/*
 * The timer ids to use
 */
#define FIRSTDATA_TIMERID 0x12
#define PING_TIMERID      0x23

#define IDM_WSPINGREQ (0x4ab71f8055ef6a63)

struct tMsgWSPingReq : public tMsg {
    tMsgWSPingReq() {id = IDM_WSPINGREQ;type = MSG_TYPE_REQUEST|MSG_TYPE_INTERNAL;}
    virtual ~tMsgWSPingReq() {}
};


class CWSJSONProtocol : public CProtocol {
public:
    CWSJSONProtocol(uint64_t aConnId, CLogger& aLogger);
    virtual ~CWSJSONProtocol();
    //
    //  This method handles incoming packages
    virtual std::list<std::shared_ptr<tNetPack> > Handle(std::shared_ptr<tNetPack> aPacket);
    //
    //  This method processes messages and signal that are
    //  send to the protocol.
    virtual std::shared_ptr<tNetPack> Process(std::shared_ptr<tMsg> aMsg);
    //
    //  This method processes outgoing packages
    virtual std::shared_ptr<tNetPack> Process(std::shared_ptr<tNetPack> aPacket);
protected:
    int ProcessHttp(std::shared_ptr<tNetPack> aPacket);
    std::shared_ptr<tNetPack> Process(std::shared_ptr<tMsgWSConnectReply> aMsg);
    std::shared_ptr<tNetPack> Process(std::shared_ptr<tMsgWSDisconnectReq> aMsg);
    std::shared_ptr<tNetPack> Process(std::shared_ptr<tSigWSDisconnectIndication> aMsg);
    std::shared_ptr<tNetPack> Process(std::shared_ptr<tMsgWSPingReq> aMsg);
    std::shared_ptr<tNetPack> ProcessDefault(std::shared_ptr<tMsg> aMsg);

    std::shared_ptr<tNetPack> Pack(const std::string& aContent);

    std::shared_ptr<tNetPack> CloseFrame(int errorcode);
    std::shared_ptr<tNetPack> PongFrame();
    std::shared_ptr<tNetPack> PingFrame();

    std::shared_ptr<tNetPack> Running(std::shared_ptr<tNetPack> aPacket, size_t & aPos) ;

    void ResetMessage();
private:
    enum WSState { eWaitHttp, eWaitSimulation, eRunning };
    WSState       wsstate;
    tHttpRequest* http;
    tWSMessage    message;
    uint8_t*      wspayload;
    tJSON*        json;
    uint64_t      PingCount;
};

#endif
