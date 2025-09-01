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
#ifndef TSIGOBJECTLOADREQ_INC
#define TSIGOBJECTLOADREQ_INC

#define IDS_OBJECTLOADREQ (0x0a3d16530b0cf6d2)

#include <string>
#include <msgdb.h>
#include <future>

struct tSQLRecordSet;

struct tSigObjectLoadReq : tMsgDB {
    tSigObjectLoadReq() {id = IDS_OBJECTLOADREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;}
    tSigObjectLoadReq(tJSON* j)  {(void)j; id = IDS_OBJECTLOADREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;}
    virtual ~tSigObjectLoadReq() {}
    uint64_t                     objectid    = INVALID_OBJECTID; //  Object Id to load
    std::promise<tSQLRecordSet*> loadchannel;                    //  The one way channel of communication to
                                                                 //  use at the end of the operartion
};

using tSigObjectLoadReqPtr = std::shared_ptr<tSigObjectLoadReq>;

#endif  // TSIGOBJECTLOADREQ_INC
