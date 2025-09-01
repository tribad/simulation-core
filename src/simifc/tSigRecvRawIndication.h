// MIT License
//
// Copyright (c) 2020 Hans-Juergen Lange<hjl@simulated-universe.de>
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
