#ifndef CPHPCONNECTION_H
#define CPHPCONNECTION_H

#include "json.h"
#include "simifc.h"

#include "tcpconnection.h"

class CPHPConnection : public CTcpConnection
{
public:
    CPHPConnection(CLogger& aLogger);
    CPHPConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger);
    virtual ~CPHPConnection();
private:
};

#endif // CPHPCONNECTION_H
