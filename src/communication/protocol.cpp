#include "protocol.h"


std::list<std::shared_ptr<tNetPack> > CProtocol::Handle(std::shared_ptr<tNetPack> aPacket) {
    std::list<std::shared_ptr<tNetPack> > retval;

    static_cast<void>(aPacket);

    return retval;
}

std::shared_ptr<tNetPack> CProtocol::Process(std::shared_ptr<tMsg> aMsg) {
    std::shared_ptr<tNetPack> retval;

    (void)aMsg;

    return retval;
}

std::shared_ptr<tNetPack> CProtocol::Process(std::shared_ptr<tNetPack> aPacket) {
    std::shared_ptr<tNetPack> retval;

    static_cast<void>(aPacket);
    return retval;
}
