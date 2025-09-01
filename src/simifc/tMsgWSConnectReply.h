// *************************************************************************************************************
//
//  Modul-Name     : tMsgWSConnectReply.h
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
#ifndef TMSGWSCONNECTREPLY_INC
#define TMSGWSCONNECTREPLY_INC

#include <simobj.h>
#include <string>
#include <stdint.h>

#define IDM_TMSGWSCONNECTREPLY (0xdb3b10b05ec2fd37)

struct tMsgWSConnectReply : public tMsg {
    tMsgWSConnectReply() {id = IDM_TMSGWSCONNECTREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;}
    tMsgWSConnectReply(const tCommTarget& aDst, const tCommTarget& aSrc) {
        id = IDM_TMSGWSCONNECTREPLY;
        type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;
        src = aSrc;
        dst = aDst;
    }
    tMsgWSConnectReply(tJSON* j)  {(void)j; id = IDM_TMSGWSCONNECTREPLY;type = MSG_TYPE_REPLY | MSG_TYPE_INTERNAL;}
    virtual ~tMsgWSConnectReply() {}
    std::string proto;
    int         error;
};

using tMsgWSConnectReplyPtr = std::shared_ptr<tMsgWSConnectReply>;

#endif  // TMSGWSCONNECTREQ_INC
