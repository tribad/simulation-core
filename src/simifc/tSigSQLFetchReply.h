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

#pragma once
#ifndef TSIGSQLFETCHREPLY_INC
#define TSIGSQLFETCHREPLY_INC

#define IDS_SQLFETCHREPLY (0xaaaaad7c8c1aedaf)

#include "sqlrecordset.h"

struct tSigSQLFetchReply : public tMsg {
    tSigSQLFetchReply() {id = IDS_SQLFETCHREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL | MSG_TYPE_SIGNAL;}
    tSigSQLFetchReply(tJSON* j)  {(void)j; id = IDS_SQLFETCHREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL | MSG_TYPE_SIGNAL;}
    virtual ~tSigSQLFetchReply() {if (RecordSet != 0) delete RecordSet;}
public:
    uint64_t       Error;
    uint64_t       cmdType;
    tSQLRecordSet* RecordSet;
};

using tSigSQLFetchReplyPtr = std::shared_ptr<tSigSQLFetchReply>;

#endif  // TSIGSQLFETCHREPLY_INC
