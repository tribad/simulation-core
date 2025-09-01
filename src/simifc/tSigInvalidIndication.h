// *************************************************************************************************************
//
//  Modul-Name     : tSigInvalidIndication.h
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
#ifndef TSIGINVALIDINDICATION_INC
#define TSIGINVALIDINDICATION_INC

#define IDS_INVALIDINDICATION (0x56ebbaf0f91675f4)

#include <string>
#include <msg.h>

struct tSigInvalidIndication : public tMsg {
    tSigInvalidIndication() {id = IDS_INVALIDINDICATION;type = MSG_TYPE_INDICATION;}
    tSigInvalidIndication(tJSON* j)  {(void)j; id = IDS_INVALIDINDICATION;type = MSG_TYPE_INDICATION;}
    virtual ~tSigInvalidIndication() {}
    virtual std::ostream& json(std::ostream& output) {
        output << "\"SignalId\": \"InvalidIndication\"";
        output <<  ", \"SignalName\": \"" << this->SignalName << "\"";

        return output;
    }

    std::string SignalName;
};

using tSigInvalidIndicationPtr = std::shared_ptr<tSigInvalidIndication>;

#endif  // TSIGINVALIDINDICATION_INC
