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
#ifndef TSIGWSDISCONNECTINDICATION_H
#define TSIGWSDISCONNECTINDICATION_H

#define IDS_TSIGWSDISCONNECTINDICATION (0xf9c819c3c6a2701b)

struct tSigWSDisconnectIndication : public tMsg {
    tSigWSDisconnectIndication() {id = IDS_TSIGWSDISCONNECTINDICATION;type = MSG_TYPE_INDICATION|MSG_TYPE_INTERNAL;}
    tSigWSDisconnectIndication(tJSON* j)  {(void)j; id = IDS_TSIGWSDISCONNECTINDICATION;type = MSG_TYPE_INDICATION|MSG_TYPE_INTERNAL;}
    virtual ~tSigWSDisconnectIndication() {}
};

using tSigWSDisconnectIndicationPtr = std::shared_ptr<tSigWSDisconnectIndication>;

#endif // TSIGWSDISCONNECTINDICATION_H
