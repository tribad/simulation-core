#ifndef CJSONIFCTHREAD_H
#define CJSONIFCTHREAD_H

#include "ifcthread.h"

class CJsonIfcThread : public CIfcThread
{
public:
    CJsonIfcThread();
    virtual std::shared_ptr<CConnection> NewConnection(int aHandle, uint64_t aConnectionId);
};

#endif // CJSONIFCTHREAD_H
