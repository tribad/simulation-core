#ifndef CHTTPIFCTHREAD_H
#define CHTTPIFCTHREAD_H

#include <set>
#include "ifcthread.h"

class CHttpDomainSpec;

class CHTTPIfcThread : public CIfcThread
{
public:
    CHTTPIfcThread();
    virtual std::shared_ptr<CConnection> NewConnection(int aHandle, uint64_t aConnectionId);
    virtual bool OnFDEvent(int aSocket, int aState);
    virtual void OnMaintenanceTime(uint64_t aTimerId);
private:
    std::map<int , CHttpDomainSpec*> DomainSpecs;
};

#endif // CHTTPIFCTHREAD_H
