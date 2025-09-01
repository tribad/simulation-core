#ifndef CWSIFCTHREAD_H
#define CWSIFCTHREAD_H

#include "ifcthread.h"

class CWSIfcThread : public CIfcThread
{
public:
    CWSIfcThread();
    virtual std::shared_ptr<CConnection> NewConnection(int aHandle, uint64_t aConnectionId);
};

#endif // CWSIFCTHREAD_H
