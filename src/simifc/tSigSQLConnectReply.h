// *************************************************************************************************************
//
//  Modul-Name     : tSigSQLConnectReply.h
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
#ifndef TSIGSQLCONNECTREPLY_INC
#define TSIGSQLCONNECTREPLY_INC

#include <memory>
#include <msg.h>

#define IDS_SQLCONNECTREPLY (0x3e102499771aec76)

struct tSigSQLConnectReply : public tMsg {
    tSigSQLConnectReply() {id = IDS_SQLCONNECTREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;}
    tSigSQLConnectReply(tJSON* j)  {(void)j; id = IDS_SQLCONNECTREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;}
    virtual ~tSigSQLConnectReply() {}
    int handle;
};

using tSigSQLConnectReplyPtr = std::shared_ptr<tSigSQLConnectReply>;

#endif  // TSIGSQLCONNECTREPLY_INC
