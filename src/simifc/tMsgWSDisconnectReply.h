// *************************************************************************************************************
//
//  Modul-Name     : tMsgWSDisconnectReq.h
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
#ifndef TMSGWSDISCONNECTREPLY_INC
#define TMSGWSDISCONNECTRELPY_INC

#define IDM_TMSGWSDISCONNECTREPLY (0x4200e2a4b761c481)

struct tMsgWSDisconnectReply : public tMsg {
    tMsgWSDisconnectReply() {id = IDM_TMSGWSDISCONNECTREPLY;type = MSG_TYPE_REPLY|MSG_TYPE_INTERNAL;}
    tMsgWSDisconnectReply(tJSON* j)  {(void)j; id = IDM_TMSGWSDISCONNECTREPLY;type = MSG_TYPE_REPLY|MSG_TYPE_INTERNAL;}
    virtual ~tMsgWSDisconnectReply() {}
};

using tMsgWSDisconnectReplyPtr = std::shared_ptr<tMsgWSDisconnectReply>;

#endif  // TMSGWSDISCONNECTREPLY_INC
