// **************************************************************************
//
// Modul-Name        : tSigRecvRawIndication.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 08.02.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef TSIGRECVRAWINDICATION_H
#define TSIGRECVRAWINDICATION_H

#define IDS_RECVRAWINDICATION (0xc217e3e41e4f7b5e)

#include <iostream>

struct tSigRecvRawIndication : public tMsg {
    tSigRecvRawIndication() {id = IDS_RECVRAWINDICATION;type = MSG_TYPE_INDICATION | MSG_TYPE_INTERNAL;}
    tSigRecvRawIndication(tJSON* j)  {(void)j; id = IDS_RECVRAWINDICATION;type = MSG_TYPE_INDICATION | MSG_TYPE_INTERNAL;}
    virtual ~tSigRecvRawIndication() {}
    virtual std::ostream& json(std::ostream& output) {
        output << "\"SignalId\": \"RecvRawIndication\"";
        return output;
    }

};

using tSigRecvRawIndicationPtr = std::shared_ptr<tSigRecvRawIndication>;

#endif // TSIGRECVRAWINDICATION_H
