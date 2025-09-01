// *************************************************************************************************************
//
//  Modul-Name     : tSigPrepareStorageReply.h
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
#ifndef TSIGPREPARESTORAGEREPLY_INC
#define TSIGPREPARESTORAGEREPLY_INC

#define IDS_PREPARESTORAGEREPLY (0x2ac098e279faa508)

#include "sqlrecordset.h"

struct tSigPrepareStorageReply : public tMsg {
    tSigPrepareStorageReply() {id = IDS_PREPARESTORAGEREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;}
    tSigPrepareStorageReply(tJSON* j)  {(void)j; id = IDS_PREPARESTORAGEREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;}
    virtual ~tSigPrepareStorageReply() {}
public:
    uint64_t       Error;
};

using tSigPrepareStorageReplyPtr = std::shared_ptr<tSigPrepareStorageReply>;

#endif  // TSIGPREPARESTORAGEREPLY_INC
