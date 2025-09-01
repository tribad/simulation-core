// *************************************************************************************************************
//
//  Modul-Name     : tMsgWSConnectReq.h
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
#ifndef TMSGWSCONNECTREQ_INC
#define TMSGWSCONNECTREQ_INC

#include <stdint.h>
#include <string>
#include <vector>

#define IDM_TMSGWSCONNECTREQ (0x010a9edabfcf3b35)


struct tMsgWSConnectReq : public tMsg {
    tMsgWSConnectReq() {id = IDM_TMSGWSCONNECTREQ;type = MSG_TYPE_REQUEST;}
    tMsgWSConnectReq(tJSON* j)  {(void)j; id = IDM_TMSGWSCONNECTREQ;type = MSG_TYPE_REQUEST;}
    virtual ~tMsgWSConnectReq() {}
    uint32_t    client;
    std::string uri;
    std::string host;
    std::string origin;
    std::string cookie;
    std::vector < std::string > proto;
};

using tMsgWSConnectReqPtr = std::shared_ptr<tMsgWSConnectReq>;

#endif  // TMSGWSCONNECTREQ_INC
