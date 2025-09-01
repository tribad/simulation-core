#ifndef CWSSCONNECTION_H
#define CWSSCONNECTION_H

#include "sslconnection.h"

class CWSSConnection : public CSSLConnection
{
public:
    CWSSConnection(CLogger& aLogger);
    CWSSConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger);
    virtual ~CWSSConnection();
    virtual int Process(std::shared_ptr<tMsg> aMsg);
    virtual int Process(std::shared_ptr<tNetPack> aPacket);
    virtual int Process(std::shared_ptr<tTimerEvent> aTimer);
};

#endif // CWSCONNECTION_H
