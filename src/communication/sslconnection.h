#ifndef SSLCONNECTION_H
#define SSLCONNECTION_H

#include <string>
#include <list>
#include <map>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <gnutls/gnutls.h>
#include "cx509.h"
#include "msg.h"

#include "connection.h"

class CSSLConnection : public CConnection
{
public:
    CSSLConnection(CLogger& aLogger);
    CSSLConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger, CProtocol* aProto=0);
    virtual ~CSSLConnection() ;

    virtual int Read(void);
    virtual int Write(std::shared_ptr<tNetPack> aPacket = std::shared_ptr<tNetPack>());
    virtual void Close(void);

    virtual int Process(std::shared_ptr<tNetPack> aPacket);
    virtual int Process(std::shared_ptr<tMsg> aMsg);

    void Shutdown(void);
    enum class eSSLState {
        SSLHandshake,
        SSLReHandshake,
        SSLCommunication
    };
    void Setup(std::shared_ptr<CX509> x509);
protected:
    /*
     * Personality info
     */
    uint32_t             Address;
    int                  Port;
    eSSLState            ConnectionState;
private:
    /*
     * Keep a hold on the credentials
     */
    std::shared_ptr<CX509> mX509;
    /*
     * Outgoing attributes.
     */
    size_t               AlreadySend;
    gnutls_session_t     Session;
};

#endif // SSLCONNECTION_H
