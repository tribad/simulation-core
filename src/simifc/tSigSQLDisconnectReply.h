// *************************************************************************************************************
//
//  Modul-Name     : tSigSQLDisconnectReply.h
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
#ifndef TSIGSQLDISCONNECTREPLY_INC
#define TSIGSQLDISCONNECTREPLY_INC

#define IDS_SQLDISCONNECTREPLY (0xfdb843e596c875cc)

struct tSigSQLDisconnectReply : public tMsg {
    tSigSQLDisconnectReply() {id = IDS_SQLDISCONNECTREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;}
    tSigSQLDisconnectReply(tJSON* j)  {(void)j; id = IDS_SQLDISCONNECTREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;}
    virtual ~tSigSQLDisconnectReply() {}
    int handle;
};

using tSigSQLDisconnectReplyPtr = std::shared_ptr<tSigSQLDisconnectReply>;

#endif  // TSIGSQLDISCONNECTREPLY_INC
