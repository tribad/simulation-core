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
#ifndef TSIGSQLEXECSTRINGREQ_INC
#define TSIGSQLEXECSTRINGREQ_INC

#define IDS_SQLEXECSTRINGREQ (0x7745df4a45b8043f)

#include <string>
#include <msgdb.h>

struct tSigSQLExecStringReq : public tMsgDB {
    tSigSQLExecStringReq() {id = IDS_SQLEXECSTRINGREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;Limit = (uint32_t)-1;}
    tSigSQLExecStringReq(tJSON* j)  {(void)j; id = IDS_SQLEXECSTRINGREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;Limit = (uint32_t)-1;}
    virtual ~tSigSQLExecStringReq() {}
    uint64_t    CmdType;    //  This is some sort of id to allow the sender to identify the reply.
    uint32_t    Limit;      //  This is the number of max records to get in a single reply.
    std::string SQL;
};

using tSigSQLExecStringReqPtr = std::shared_ptr<tSigSQLExecStringReq>;

#endif  // TSIGSQLEXECSTRINGREQ_INC
