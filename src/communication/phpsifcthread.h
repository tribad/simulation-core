#ifndef CPHPSIFCTHREAD_H
#define CPHPSIFCTHREAD_H

#include "sslifcthread.h"

class CPHPsIfcThread : public CSSLIfcThread
{
public:
    CPHPsIfcThread();
    virtual std::shared_ptr<CConnection> NewConnection(int aHandle, uint64_t aConnectionId);
};

#endif // CPHPIFCTHREAD_H
