// **************************************************************************
//
// Modul-Name        : ptrreleasethread.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 11.11.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef CPTRRELEASETHREAD_H
#define CPTRRELEASETHREAD_H

#include <cstdint>
#include "CThread.h"

class CPtrReleaseThread : public CThread
{
#if defined(APPCORE_SIMIFC_PTRRELEASETHREAD_TEST_FRIEND)
    APPCORE_SIMIFC_PTRRELEASETHREAD_TEST_FRIEND();
#endif
public:
    CPtrReleaseThread();
    ~CPtrReleaseThread() override;
    long Run() override;
private:
    size_t RunVersion(uint64_t aVersion = 0);
};

#endif // CPTRRELEASETHREAD_H
