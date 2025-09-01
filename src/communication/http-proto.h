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

#ifndef HTTP_PROTO_H
#define HTTP_PROTO_H

#include <mutex>
#include "protocol.h"

#include "tMsgWSConnectReq.h"
#include "tMsgWSConnectReply.h"
#include "tMsgWSDisconnectReq.h"
#include "tSigWSDisconnectIndication.h"
/*
 * Forwards
 */
class tHttpRequest;
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

class CHttpDomainSpec;

class CHTTPProtocol : public CProtocol {
public:
    CHTTPProtocol(uint64_t aConnId, CLogger& aLogger);
    virtual ~CHTTPProtocol();
    //
    //  This method handles incoming packages
    virtual  std::list<std::shared_ptr<tNetPack> > Handle(std::shared_ptr<tNetPack> aPacket);
    //
    //  This method processes messages and signal that are
    //  send to the protocol.
    virtual std::shared_ptr<tNetPack> Process(std::shared_ptr<tMsg> aMsg);
    //
    //  This method processes outgoing packages
    virtual std::shared_ptr<tNetPack> Process(std::shared_ptr<tNetPack> aPacket);
    //
    //  This is for the handling of content libraries.
    static std::map<std::string, std::string>      ext2mime;
    //
    //  This is for the handling of the domains
    static std::mutex                              domains_lock; //  Locking the domains map
    static std::map<std::string, CHttpDomainSpec*> domains;      //  Mapping from domainname
private:
    const char* GetMimeType(const std::string& extension);
protected:
    int ProcessHttp(std::shared_ptr<tNetPack> aPacket);
private:
    tHttpRequest*                                  http;
    uint64_t                                       PingCount;
};

#endif
