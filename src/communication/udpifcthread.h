// **************************************************************************
//
// Modul-Name        : udpifcthread.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 17.02.2021
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef CUDPINTERFACE_H
#define CUDPINTERFACE_H

class CUdpInterface
{
public:
    CUdpInterface(ModulId aModulId, const std::string& aModulName);

    virtual ~CUdpInterface() = default;
    virtual bool InitInstance(void);

    int RunRead();
    int RunWrite();

    void Create(std::string aIfc, int aPort, std::string aName);
public:
    bool         mRunning = false;
private:
    std::string  mName;
    std::string  mIfc;
    int          mPort;
    pthread_t    mReadIfcId;
    pthread_t    mWriteIfcId;
    CLogger      mLogger;
    /*
     * Socket specifics.
     */
    int          mHandle;
};

#endif // CUDPINTERFACE_H
