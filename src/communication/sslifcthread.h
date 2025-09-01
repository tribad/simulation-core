#ifndef CSSLIFCTHREAD_H
#define CSSLIFCTHREAD_H

#include <gnutls/gnutls.h>
#include <string>
#include <list>
#include <mutex>
#include "sslconnection.h"

class CSSLIfcThread : public CIfcThread
{
public:
    CSSLIfcThread(ModulId aModulId, const std::string& aModulName );
    void Create(std::string aIfc, int aPort, const std::string& aCertPath,
                const std::string& aKeyPath, const std::string& aCAPath,
                const int aUpdateCert=1);
    virtual bool InitInstance();
    virtual void ExtendedSetup(std::shared_ptr<CConnection> aConnection);
private:
    virtual void OnInterfaceTimer(uint64_t aTimerId);
    virtual std::shared_ptr<CConnection> NewConnection(int aHandle, uint64_t aConnectionId);
private:
    int                    mUpdateCertTime;
    std::shared_ptr<CX509> mX509;
    std::string            mCAPath;
    std::string            mCertPath;
    std::string            mKeyPath;
};

#endif // CSSLIFCTHREAD_H
