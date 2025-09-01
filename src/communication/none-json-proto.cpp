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

#include <iomanip>
#include <sstream>
#include <memory.h>
#include "none-json-proto.h"
#include "logger.h"
#include "commthread.h"

extern long loglevel;

CNoneJSONProtocol::CNoneJSONProtocol(CLogger &aLogger) : CProtocol(aLogger) {
    json = 0;
}

std::list<std::shared_ptr<tNetPack> > CNoneJSONProtocol::Handle(std::shared_ptr<tNetPack> aPacket) {
    std::list<std::shared_ptr<tNetPack> > retval;

    if (loglevel & 0x1000) {
        char buffer[aPacket->len+1];

        memcpy(buffer, aPacket->buffer, aPacket->len);
        buffer[aPacket->len] = '\0';

        std::ostringstream oss;

        oss << "TCP-In  :" << std::hex << std::setw(16) << std::setfill('0') << aPacket->connid << "::" << std::dec << aPacket->len << "::" << buffer;
        Log.Write(LogLevel::Trace, oss);
    }
    /*
     * Parse the packet content. We expect JSON formated messages here.
     */
    json=parse(json, aPacket->buffer, aPacket->len);
    if (completed(json)) {
        CCommThread::instance().Put(aPacket->connid, json);

        json = 0;
    } else {
    }
    /*
     * We do get new packets all the time.
     */
    return retval;
}

