#ifndef CJSONCONNECTION_H
#define CJSONCONNECTION_H

#include "json.h"
#include "simifc.h"

#include "tcpconnection.h"

class CJsonConnection : public CTcpConnection
{
public:
    CJsonConnection(CLogger& aLogger);
    CJsonConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger);
    virtual ~CJsonConnection();
private:
};

#endif // CJSONCONNECTION_H
