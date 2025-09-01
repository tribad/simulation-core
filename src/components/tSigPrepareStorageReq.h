// *************************************************************************************************************
//
//  Modul-Name     : tSigPrepareStorageReq.h
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
#ifndef TSIGPREPARESTORAGEREQ_INC
#define TSIGPREPARESTORAGEREQ_INC

#include <string>
#include <msgdb.h>

#define IDS_PREPARESTORAGEREQ (0x415d057c8c93b633)

struct tSigPrepareStorageReq : public tMsgDB {
    tSigPrepareStorageReq()
    {
        id              = IDS_PREPARESTORAGEREQ;
        type            = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;
        history         = false;
        versions        = false;
        objectsondemand = false;
    }
    tSigPrepareStorageReq(tJSON* j) {
        (void)j;
        id              = IDS_PREPARESTORAGEREQ;
        type            = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;
        history         = false;
        versions        = false;
        objectsondemand = false;
    }
    virtual ~tSigPrepareStorageReq() {}
public:
    bool history;
    bool versions;
    bool objectsondemand;
};

using tSigPrepareStorageReqPtr = std::shared_ptr<tSigPrepareStorageReq>;

#endif  // TSIGPREPARESTORAGEREQ_INC
