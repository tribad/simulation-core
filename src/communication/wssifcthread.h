#ifndef CWSSIFCTHREAD_H
#define CWSSIFCTHREAD_H

#include "sslifcthread.h"

class CWSSIfcThread : public CSSLIfcThread
{
public:
    CWSSIfcThread();
    virtual std::shared_ptr<CConnection> NewConnection(int aHandle, uint64_t aConnectionId);
};

#endif // CWSSIFCTHREAD_H
