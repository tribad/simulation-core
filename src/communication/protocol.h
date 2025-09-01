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
