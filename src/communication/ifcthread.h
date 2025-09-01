// MIT License
//
// Copyright (c) 2017 Hans-Juergen Lange<hjl@simulated-universe.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef CIFCTHREAD_H
#define CIFCTHREAD_H

#include <string>
#include <map>
#include <memory>
#include <pthread.h>
#include <logger.h>

enum class eIfcType {
    TcpPHP,
    TcpPHPs,
    WS,
    WSS,
    Bin,
    BinS,
    Udp,
    Max
};

#define REBIND_TIMEOUT        30          // Seconds
#define SIGNALS_AT_ONCE       16          // Number of signals to read at once.

class CConnection;

class CIfcThread
{
public:
    CIfcThread(ModulId aModulId, const std::string& aModulName );
    virtual ~CIfcThread() = default;
    void Create(std::string aIfc, int aPort);
    virtual bool InitInstance(void);
    virtual int Run(void);
    virtual void ExtendedSetup(std::shared_ptr<CConnection> aConnection);
protected:
    virtual void OnInterfaceTimer(uint64_t aTimerId);
    virtual void OnMaintenanceTime(uint64_t aTimerId);
private:
    virtual void OnHangUp(int aSocket, int aState);
    virtual void OnUrgentData(int aSocket, int aState);
    virtual void OnBrokenPipe(int aSocket, int aState);
    virtual void OnEvent(int aSocket, int aState);
    virtual bool OnFDEvent(int aSocket, int aState);
    virtual void OnConnect(int aSocket, int aState);
    virtual void OnQueue(void* aData);
    virtual void OnIO(struct signalfd_siginfo* aSigInfo);
    virtual std::shared_ptr<CConnection> NewConnection(int aHandle, uint64_t aConnectionId);
    virtual void CleanupConnection(int aHandle);
private:
    static int                                       Count;
    int                                              Index;
    int                                              SignalFile;
    bool                                             Running;
    std::string                                      Ifc;
    int                                              Port;
    int                                              ListenHandle;
    pthread_t                                        IfcId;
    int                                              MaintenanceTime;
protected:
    int                                              SignalNo;
    std::map<int, std::shared_ptr<CConnection> >     ConnectionMap;    //  This map uses the socket handle as the key.
    uint64_t                                         ConnectionCount;  //  This is the connection counter.
    std::map<uint64_t, std::shared_ptr<CConnection>> Connection;       //  This map uses the connection id as the key.
    CLogger                                          ifcLogger;
};

#endif // CIFCTHREAD_H
