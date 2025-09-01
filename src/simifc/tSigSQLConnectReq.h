// *************************************************************************************************************
//
//  Modul-Name     : tSigSQLConnectReq.h
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
#ifndef TSIGSQLCONNECTREQ_INC
#define TSIGSQLCONNECTREQ_INC

#include <string>

#define IDS_SQLCONNECTREQ (0xc73f17b2d4a87a2e)

struct tSigSQLConnectReq : public tMsg {
    tSigSQLConnectReq() {id = IDS_SQLCONNECTREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;}
    tSigSQLConnectReq(tJSON* j)  {(void)j; id = IDS_SQLCONNECTREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;}
    virtual ~tSigSQLConnectReq() {}
    std::string name;
    std::string host;
    std::string port;
    std::string user;
    std::string passwd;
};

using tSigSQLConnectReqPtr = std::shared_ptr<tSigSQLConnectReq>;

#endif  // TSIGSQLCONNECTREQ_INC
