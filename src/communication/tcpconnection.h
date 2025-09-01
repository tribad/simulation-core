#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <string>
#include <list>
#include <map>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "msg.h"

#include "connection.h"

class CTcpConnection : public CConnection
{
public:
    CTcpConnection(CLogger& aLogger);
    CTcpConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger, CProtocol* aProto=0);
    virtual ~CTcpConnection();

    virtual int Read(void);
    virtual int Write(std::shared_ptr<tNetPack> aPacket = std::shared_ptr<tNetPack>());

    virtual int Process(std::shared_ptr<tNetPack> aPacket);
    virtual int Process(std::shared_ptr<tMsg> aMsg);
    void Shutdown(void);
protected:
    /*
     * Personality info
     */
    uint32_t             Address;
    int                  Port;
private:
    /*
     * Outgoing attributes.
     */
    size_t               AlreadySend = 0U;
};

#endif // CCONNECTION_H
