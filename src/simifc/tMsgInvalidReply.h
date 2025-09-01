// MIT License
//
// Copyright (c) 2020 Hans-Juergen Lange<hjl@simulated-universe.de>
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
#ifndef TMSGINVALIDREPLY_INC
#define TMSGINVALIDREPLY_INC

#define IDM_INVALIDREPLY (0x6c3cc791349b97e1)

#include <string>
#include <msg.h>

struct tMsgInvalidReply : public tMsg {
    tMsgInvalidReply() {id = IDM_INVALIDREPLY;type = MSG_TYPE_REPLY;}
    tMsgInvalidReply(tJSON* j)  {(void)j; id = IDM_INVALIDREPLY;type = MSG_TYPE_REPLY;}
    virtual ~tMsgInvalidReply() {}
    virtual std::ostream& json(std::ostream& output) {
        output << "\"MsgId\": \"InvalidReply\"";
        output <<  ", \"MsgName\": \"" << this->MsgName << "\"";

        return output;
    }

    std::string MsgName;
};

using tMsgInvalidReplyPtr = std::shared_ptr<tMsgInvalidReply>;

#endif  // TMSGINVALIDREPLY_INC
