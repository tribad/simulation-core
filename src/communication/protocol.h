#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <list>

#include "msg.h"
#include "netpack.h"

#include <logger.h>

class CProtocol {
public:
    CProtocol(CLogger& aLogger) : Log(aLogger) {}
    virtual ~CProtocol() = default;
    virtual std::list<std::shared_ptr<tNetPack> > Handle(std::shared_ptr<tNetPack> aPacket);    //  This is the packet handler that implements the incoming data part of the protocol.
    virtual std::shared_ptr<tNetPack> Process(std::shared_ptr<tMsg> aMsg);                      //  This is the dispatcher function that handles messages and signals to the protocol.
    virtual std::shared_ptr<tNetPack> Process(std::shared_ptr<tNetPack> aPacket);               //  This is the packet handler that implements the outgoing data part of the protocol.
protected:
    uint64_t ConnectionId;
    CLogger&  Log;
};

#endif
