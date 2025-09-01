#ifndef PACKETTYPE_H
#define PACKETTYPE_H

#include "netpack.h"
#include "timer.h"

enum class ePackageType : uint16_t {
    Message,
    NetPack,
    Timer,
    None
};

struct tPackageType {
    tPackageType() {type = ePackageType::None; connid = 0;};
    tPackageType(ePackageType aType, uint64_t aConnectionId) {type = aType;
                                                              connid = aConnectionId;
                                                             }
    virtual ~tPackageType() { }

    ePackageType type;
    uint64_t     connid;
};

struct tMsgPackage : public tPackageType {
    tMsgPackage(uint64_t aConnectionId, std::shared_ptr<tMsg> aMsg) : tPackageType (ePackageType::Message, aConnectionId) {msg = aMsg;}

    virtual ~tMsgPackage() { }

    std::shared_ptr<tMsg> msg;
};

struct tNetPackage : public tPackageType {
    tNetPackage(uint64_t aConnectionId, std::shared_ptr<tNetPack> aPack) : tPackageType (ePackageType::NetPack, aConnectionId), mPack(aPack) {}

    virtual ~tNetPackage()  = default;

    std::shared_ptr<tNetPack> mPack;
};

struct tTimerPackage : public tPackageType {
    tTimerPackage(uint64_t aConnectionId, std::shared_ptr<tTimerEvent>  aEvent) : tPackageType (ePackageType::Timer, aConnectionId),  event(aEvent) {}

    virtual ~tTimerPackage() = default;

    std::shared_ptr<tTimerEvent>  event;
};


#endif
