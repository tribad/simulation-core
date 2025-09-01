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
#ifndef TMSGWSDISCONNECTREQ_INC
#define TMSGWSDISCONNECTREQ_INC

#define IDM_TMSGWSDISCONNECTREQ (0xfbf5704b5f407545)

struct tMsgWSDisconnectReq : public tMsg {
    tMsgWSDisconnectReq() {id = IDM_TMSGWSDISCONNECTREQ;type = MSG_TYPE_REQUEST|MSG_TYPE_INTERNAL;}
    tMsgWSDisconnectReq(tJSON* j)  {(void)j; id = IDM_TMSGWSDISCONNECTREQ;type = MSG_TYPE_REQUEST|MSG_TYPE_INTERNAL;}
    virtual ~tMsgWSDisconnectReq() {}
};

using tMsgWSDisconnectReqPtr = std::shared_ptr<tMsgWSDisconnectReq>;

#endif  // TMSGWSDISCONNECTREQ_INC
