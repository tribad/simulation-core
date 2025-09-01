// *************************************************************************************************************
//
//  Modul-Name     : tMsgSQLExecStringReq.h
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
#ifndef TMSGSQLEXECSTRINGREQ_INC
#define TMSGSQLEXECSTRINGREQ_INC

#define IDM_SQLEXECSTRINGREQ (0x4b421d5d3ebb97a9)

struct tMsgSQLExecStringReq : public tMsgDB {
    tMsgSQLExecStringReq() {id = IDM_SQLEXECSTRINGREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL; SQL = nullptr;}
    tMsgSQLExecStringReq(tJSON* j)  {(void)j; id = IDM_SQLEXECSTRINGREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL; SQL = nullptr;}
    virtual ~tMsgSQLExecStringReq() {
        if (SQL != nullptr) {
            free(SQL);
        }
    }
    char*       SQL;
};

using tMsgSQLExecStringReqPtr = std::shared_ptr<tMsgSQLExecStringReq>;

#endif  // TMSGSQLEXECSTRINGREQ_INC
