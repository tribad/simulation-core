// *************************************************************************************************************
//
//  Modul-Name     : tSigObjectLoadReq.h
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
