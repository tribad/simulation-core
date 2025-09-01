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
