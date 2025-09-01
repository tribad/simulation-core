#ifndef CHTTPCONNECTION_H
#define CHTTPCONNECTION_H

#include "simifc.h"
#include "http-proto.h"
#include "tcpconnection.h"

class CHTTPConnection : public CTcpConnection
{
public:
    CHTTPConnection(CLogger& aLogger);
    CHTTPConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger);
    virtual ~CHTTPConnection();
    virtual int Process(std::shared_ptr<tMsg> aMsg);
    virtual int Process(std::shared_ptr<tNetPack> aPacket);
    virtual int Process(std::shared_ptr<tTimerEvent> aTimer);
private:

};

#endif // CHTTPCONNECTION_H
