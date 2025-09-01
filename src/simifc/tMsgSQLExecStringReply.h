// *************************************************************************************************************
//
//  Modul-Name     : tMsgSQLExecStringReply.h
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
#ifndef TMSGSQLEXECSTRINGREPLY_INC
#define TMSGSQLEXECSTRINGREPLY_INC

#define IDM_SQLEXECSTRINGREPLY (0x66a9f8cb720051eb)

#include "sqlrecordset.h"

struct tMsgSQLExecStringReply : public tMsg {
    tMsgSQLExecStringReply() {id = IDM_SQLEXECSTRINGREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;}
    tMsgSQLExecStringReply(tJSON* j)  {(void)j; id = IDM_SQLEXECSTRINGREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;}
    virtual ~tMsgSQLExecStringReply() {}
    uint64_t       Error;
    tSQLRecordSet* Records;
};

using tMsgSQLExecStringReplyPtr = std::shared_ptr<tMsgSQLExecStringReply>;

#endif  // TMSGSQLEXECSTRINGREPLY_INC
