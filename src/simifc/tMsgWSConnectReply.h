// MIT License
//
// Copyright (c) 2016 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#pragma once
#ifndef TMSGWSCONNECTREPLY_INC
#define TMSGWSCONNECTREPLY_INC

#include <simobj.h>
#include <string>
#include <stdint.h>

#define IDM_TMSGWSCONNECTREPLY (0xdb3b10b05ec2fd37)

struct tMsgWSConnectReply : public tMsg {
    tMsgWSConnectReply() {id = IDM_TMSGWSCONNECTREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;}
    tMsgWSConnectReply(const tCommTarget& aDst, const tCommTarget& aSrc) {
        id = IDM_TMSGWSCONNECTREPLY;
        type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;
        src = aSrc;
        dst = aDst;
    }
    tMsgWSConnectReply(tJSON* j)  {(void)j; id = IDM_TMSGWSCONNECTREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;}
    virtual ~tMsgWSConnectReply() {}
    std::string proto;
    int         error;
};

using tMsgWSConnectReplyPtr = std::shared_ptr<tMsgWSConnectReply>;

#endif  // TMSGWSCONNECTREQ_INC
