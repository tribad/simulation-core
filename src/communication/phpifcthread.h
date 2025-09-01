#ifndef CPHPIFCTHREAD_H
#define CPHPIFCTHREAD_H

#include "ifcthread.h"

class CPHPIfcThread : public CIfcThread
{
public:
    CPHPIfcThread();
    virtual std::shared_ptr<CConnection> NewConnection(int aHandle, uint64_t aConnectionId);
};

#endif // CPHPIFCTHREAD_H
