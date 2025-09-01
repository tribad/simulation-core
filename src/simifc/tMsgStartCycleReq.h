// *************************************************************************************************************
//
//  Modul-Name     : tMsgStartCycleReq.h
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
#ifndef TMSGSTARTCYCLEREQ_INC
#define TMSGSTARTCYCLEREQ_INC

#define IDM_STARTCYCLEREQ (0x9fee11478aa1599e)

struct tMsgStartCycleReq : public tMsg {
    tMsgStartCycleReq() {id = IDM_STARTCYCLEREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;}
    tMsgStartCycleReq(tJSON* j)  {(void)j; id = IDM_STARTCYCLEREQ;type = MSG_TYPE_REQUEST | MSG_TYPE_INTERNAL;}
    virtual ~tMsgStartCycleReq() {}
    uint64_t    Cycle;
};

using tMsgStartCycleReqPtr = std::shared_ptr<tMsgStartCycleReq>;

#endif  // TMSGSTARTCYCLEREQ_INC
