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
#ifndef TSIGSQLEXECSTRINGREPLY_INC
#define TSIGSQLEXECSTRINGREPLY_INC

#define IDS_SQLEXECSTRINGREPLY (0x0a0d62ec02f52f0c)

#include "sqlrecordset.h"

struct tSigSQLExecStringReply : public tMsg {
    tSigSQLExecStringReply() {id = IDS_SQLEXECSTRINGREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL | MSG_TYPE_SIGNAL;RecordSet = 0;}
    tSigSQLExecStringReply(tJSON* j)  {(void)j; id = IDS_SQLEXECSTRINGREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL | MSG_TYPE_SIGNAL;RecordSet = 0;}
    //
    //   !!!!   CAUTION   !!!!
    //
    //   If the application needs the content of the Recordset past the livetime
    //   of the signal it must save the pointer to the recordset and set it to
    //   NULL in the signal before deleting the signal itself.
    virtual ~tSigSQLExecStringReply() {if (RecordSet != 0) delete RecordSet;}
    uint64_t       CmdType;
    uint64_t       Error;
    tSQLRecordSet* RecordSet;
};

using tSigSQLExecStringReplyPtr = std::shared_ptr<tSigSQLExecStringReply>;

#endif  // TSIGSQLEXECSTRINGREPLY_INC
