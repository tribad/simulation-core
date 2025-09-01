// *************************************************************************************************************
//
//  Modul-Name     : tSigSQLExecStringReply.h
//
//  Copyrights by Hans-Juergen Lange <hjl@simulated-universe.de>. All rights reserved.
//
// *************************************************************************************************************
// HOC *********************************************************************************************************
//          |         |
// YYYYMMDD | Creator | description
//--------------------------------------------------------------------------------------------------------------
//          |         |
//----------+---------+-----------------------------------------------------------------------------------------
//          |         |
// HOC end *****************************************************************************************************
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
