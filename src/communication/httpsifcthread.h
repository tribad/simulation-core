#ifndef CHTTPSIFCTHREAD_H
#define CHTTPSIFCTHREAD_H

#include "sslifcthread.h"

class CHTTPSIfcThread : public CSSLIfcThread
{
public:
    CHTTPSIfcThread();
    virtual std::shared_ptr<CConnection> NewConnection(int aHandle, uint64_t aConnectionId);
};

#endif // CHTTPSIFCTHREAD_H
