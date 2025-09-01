// MIT License
//
// Copyright (c) 2021 Hans-Juergen Lange<hjl@simulated-universe.de>
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

// **************************************************************************
//
// Modul-Name        : udpframeblk.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 17.02.2021
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef UDPFRAMEBLK_H
#define UDPFRAMEBLK_H

#include "udpframe.h"

#define SENDERMEM_BITS   10
#define RECEIVERMEM_BITS 10

#define SENDERMEM_MASK     (unsigned long)((1 << SENDERMEM_BITS)-1)
#define RECEIVERMEM_MASK   (unsigned long)((1 << RECEIVERMEM_BITS)-1)

using tUdpSenderBlk   = tUdpFrame[1024];
using tUdpReceiverBlk = tUdpFrame[128];

class tUdpMemory {
    static tUdpMemory& instance() {
        static tUdpMemory mem;

        return mem;
    }
private:
    tUdpSenderBlk   SenderMem  [1 << (SENDERMEM_BITS-1)];
    tUdpReceiverBlk ReceiverMem[1 << (RECEIVERMEM_BITS -1)];
};

#endif // UDPFRAMEBLK_H
