#ifndef CHTTPSCONNECTION_H
#define CHTTPSCONNECTION_H

#include "simifc.h"
#include "http-proto.h"
#include "sslconnection.h"

class CHTTPSConnection : public CSSLConnection
{
public:
    CHTTPSConnection(CLogger& aLogger);
    CHTTPSConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger);
    virtual ~CHTTPSConnection();
    virtual int Process(std::shared_ptr<tMsg> aMsg);
    virtual int Process(std::shared_ptr<tNetPack> aPacket);
    virtual int Process(tTimerEvent* aTimer);
};

#endif // CHTTPSCONNECTION_H
