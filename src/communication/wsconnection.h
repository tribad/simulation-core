#ifndef CWSCONNECTION_H
#define CWSCONNECTION_H

#include "simifc.h"
#include "ws-json-proto.h"
#include "tcpconnection.h"

class CWSConnection : public CTcpConnection
{
public:
    CWSConnection(CLogger& aLogger);
    CWSConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger);
    virtual ~CWSConnection();
    virtual int Process(std::shared_ptr<tMsg> aMsg);
    virtual int Process(std::shared_ptr<tNetPack> aPacket);
    virtual int Process(std::shared_ptr<tTimerEvent> aTimer);
private:

};

#endif // CWSCONNECTION_H
