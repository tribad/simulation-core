// **************************************************************************
//
// Modul-Name        : tSigSendRawIndication.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 08.02.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef TSIGSENDRAWINDICATION_H
#define TSIGSENDRAWINDICATION_H

#define IDS_SENDRAWINDICATION (0x736832675b5ee36d)

#include <iostream>

struct tSigSendRawIndication : public tMsg {
    tSigSendRawIndication() {id = IDS_SENDRAWINDICATION;type = MSG_TYPE_INDICATION | MSG_TYPE_INTERNAL;}
    tSigSendRawIndication(tJSON* j)  {(void)j; id = IDS_SENDRAWINDICATION;type = MSG_TYPE_INDICATION | MSG_TYPE_INTERNAL;}
    virtual ~tSigSendRawIndication() {}
    virtual std::ostream& json(std::ostream& output) {
        output << "\"SignalId\": \"SendRawIndication\"";
        return output;
    }

};

using tSigSendRawIndicationPtr = std::shared_ptr<tSigSendRawIndication>;

#endif // TSIGSENDRAWINDICATION_H
