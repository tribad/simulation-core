// *************************************************************************************************************
//
//  Modul-Name     : tSigSQLDisconnectReq.h
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
#ifndef TSIGSQLDISCONNECTREQ_INC
#define TSIGSQLDISCONNECTREQ_INC

#define IDS_SQLDISCONNECTREQ (0x3b87c8e69664b035)

#include <msgdb.h>

struct tSigSQLDisconnectReq : public tMsgDB {
    tSigSQLDisconnectReq() {id = IDS_SQLDISCONNECTREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;}
    tSigSQLDisconnectReq(tJSON* j)  {(void)j; id = IDS_SQLDISCONNECTREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;}
    virtual ~tSigSQLDisconnectReq() {}
};

using tSigSQLDisconnectReqPtr = std::shared_ptr<tSigSQLDisconnectReq>;

#endif  // TSIGSQLDISCONNECTREQ_INC
