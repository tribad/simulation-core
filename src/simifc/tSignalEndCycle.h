// **************************************************************************
//
// Modul-Name        : tSignalEndCycle
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 01.05.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#pragma once
#ifndef TSIGNALENDCYCLE_H
#define TSIGNALENDCYCLE_H

#define IDS_ENDCYCLE (0x55ff5525b158287d)

struct tSignalEndCycle : public tSig {
    tSignalEndCycle() {id = IDS_ENDCYCLE;type = MSG_TYPE_SIGNAL | MSG_TYPE_INTERNAL;}
    tSignalEndCycle(tJSON* j)  {(void)j; id = IDS_ENDCYCLE;type = MSG_TYPE_SIGNAL | MSG_TYPE_INTERNAL;}
    virtual ~tSignalEndCycle() {}
    uint64_t    Cycle;
};

using tSignalEndCyclePtr = std::shared_ptr<tSignalEndCycle>;

#endif // TSIGNALENDCYCLE_H
