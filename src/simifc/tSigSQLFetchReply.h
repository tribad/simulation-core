// *************************************************************************************************************
//
//  Modul-Name     : tSigSQLFetchReply.h
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
#ifndef TSIGSQLFETCHREPLY_INC
#define TSIGSQLFETCHREPLY_INC

#define IDS_SQLFETCHREPLY (0xaaaaad7c8c1aedaf)

#include "sqlrecordset.h"

struct tSigSQLFetchReply : public tMsg {
    tSigSQLFetchReply() {id = IDS_SQLFETCHREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL | MSG_TYPE_SIGNAL;}
    tSigSQLFetchReply(tJSON* j)  {(void)j; id = IDS_SQLFETCHREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL | MSG_TYPE_SIGNAL;}
    virtual ~tSigSQLFetchReply() {if (RecordSet != 0) delete RecordSet;}
public:
    uint64_t       Error;
    uint64_t       cmdType;
    tSQLRecordSet* RecordSet;
};

using tSigSQLFetchReplyPtr = std::shared_ptr<tSigSQLFetchReply>;

#endif  // TSIGSQLFETCHREPLY_INC
