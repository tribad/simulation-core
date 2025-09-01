// MIT License
//
// Copyright (c) 2017 Hans-Juergen Lange<hjl@simulated-universe.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
