#ifndef CPHPSCONNECTION_H
#define CPHPSCONNECTION_H

#include "json.h"
#include "simifc.h"

#include "sslconnection.h"

class CPHPsConnection : public CSSLConnection
{
public:
    CPHPsConnection(CLogger& aLogger);
    CPHPsConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger);
    virtual ~CPHPsConnection();
    virtual int Process(std::shared_ptr<tNetPack> aPacket);
private:
    tJSON*   json = nullptr;
};

#endif // CPHPCONNECTION_H
