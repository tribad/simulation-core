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
