#ifndef CCONNECTION_H
#define CCONNECTION_H

#include <string>
#include <list>
#include <map>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "msg.h"
#include "packettype.h"
#include "protocol.h"

#include <logger.h>

#define CONNECTIONID_MASK (0x00ffffffffffffff)
#define SIGNAL_MASK       (0xff00000000000000)

#define CONNECTIONID_BITS 56
#define SIGNAL_BITS        8
#define CLOSEWAIT_TIME    10000
#define CLOSEWAIT_TIMERID 0xa5

class CConnection
{
public:
    CConnection(CLogger& aLogger);
    CConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger, CProtocol* aProto);
    virtual ~CConnection();
    /*
     * Here is a collection of processing function that normaly
     * gets called from the interface
     */
    virtual int OnRead();
    virtual int OnWrite();
    virtual int OnClose();
    virtual int OnQueue(tPackageType* aPacket);
public:
    int QueueOutgoing(std::shared_ptr<tNetPack> aPacket);
    virtual int Read(void) = 0;
    virtual int Write(std::shared_ptr<tNetPack> aPacket = std::shared_ptr<tNetPack>()) = 0;
    virtual void Close(void);

    virtual int Process(std::shared_ptr<tNetPack> aPacket);
    virtual int Process(std::shared_ptr<tMsg> aMsg);
    virtual int Process(std::shared_ptr<tTimerEvent> aTimer);

private:
    int Process(tMsgPackage*   aMsg);
    int Process(tNetPackage*   aPack);
    int Process(tTimerPackage* aEvent);
public:
    /*
     * Personality info
     */
    int                  Handle = -1;
    std::string          Name;
    /*
     * Incoming attributes
     */
    std::shared_ptr<tNetPack>            Packet;
    /*
     * Outgoing attributes.
     */
    bool                                  Writable = true;
    std::shared_ptr<tNetPack>             ToSend;
    std::list<std::shared_ptr<tNetPack> > OutQueue;
    /*
     * Connection Management attributes
     */
    bool                 CloseAfterSend      = false;
    bool                 SendReplyAfterClose = false;
    uint64_t             ConnectionId;

    CProtocol*           Protocol            = nullptr;
    /*
     * Counters
     */
    uint64_t             InByteCount    = 0U;
    uint64_t             InPacketCount  = 0U;
    uint64_t             OutByteCount   = 0U;
    uint64_t             OutPacketCount = 0U;
    //
    //  Overall counters
    static uint64_t      AllInByteCount;
    static uint64_t      AllInPacketCount;
    static uint64_t      AllOutByteCount;
    static uint64_t      AllOutPacketCount;
    //
    //  Logging
    CLogger& Log;
};

#endif // CCONNECTION_H
