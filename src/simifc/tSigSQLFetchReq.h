// *************************************************************************************************************
//
//  Modul-Name     : tSigSQLFetchReq.h
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
#ifndef TSIGSQLFETCHREQ_INC
#define TSIGSQLFETCHREQ_INC

#include <string>

#define IDS_SQLFETCHREQ (0xebf7fad57aa76a56)

#include <msgdb.h>

struct tSigSQLFetchReq : public tMsgDB {
    tSigSQLFetchReq() {id = IDS_SQLFETCHREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;Limit = 1000u; cmdType = 0x00;}
    tSigSQLFetchReq(tJSON* j)  {(void)j; id = IDS_SQLFETCHREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;Limit = 1000u; cmdType = 0x00;}
    virtual ~tSigSQLFetchReq() {}
public:
    uint64_t cmdType;
    uint32_t Limit;
};

using tSigSQLFetchReqPtr = std::shared_ptr<tSigSQLFetchReq>;

#endif  // TSIGSQLFETCHREQ_INC
