// **************************************************************************
//
// Modul-Name        : tSignalStartCycle
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 01.05.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#pragma once
#ifndef TSIGNALSTARTCYCLE_H
#define TSIGNALSTARTCYCLE_H

#define IDS_STARTCYCLE (0x81ad60dcc094755f)

struct tSignalStartCycle : public tSig {
    tSignalStartCycle() {id = IDS_STARTCYCLE;type = MSG_TYPE_SIGNAL | MSG_TYPE_INTERNAL;}
    tSignalStartCycle(tJSON* j)  {(void)j; id = IDS_STARTCYCLE;type = MSG_TYPE_SIGNAL | MSG_TYPE_INTERNAL;}
    virtual ~tSignalStartCycle() {}
    uint64_t    Cycle;
};

using tSignalStartCyclePtr = std::shared_ptr<tSignalStartCycle>;

#endif // TSIGNALSTARTCYCLE_H
