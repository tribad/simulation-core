// *************************************************************************************************************
//
//  Modul-Name     : tSigSQLExecStringReq.h
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
