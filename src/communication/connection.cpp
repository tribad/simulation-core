#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdint.h>
#include <logger.h>
#include <commthread.h>
#include <netpack.h>
#include <simifc.h>
#include <sim.h>
#include <connection.h>
#include <tMsgWSDisconnectReply.h>
#include <tSigWSDisconnectIndication.h>

extern int mainpid;

uint64_t      CConnection::AllInByteCount    = 0;
uint64_t      CConnection::AllInPacketCount  = 0;
uint64_t      CConnection::AllOutByteCount   = 0;
uint64_t      CConnection::AllOutPacketCount = 0;

CConnection::CConnection(CLogger& aLogger) :Log(aLogger)
{
}

CConnection::CConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger, CProtocol* aProto)
    : Handle(aHandle),
      ConnectionId(aConnectionId),
      Protocol(aProto),
      Log(aLogger)
{
    /*
     * Do the intializing of the attributes of the connection.
     */
    Handle              = aHandle;
    ConnectionId        = aConnectionId;
    Protocol            = aProto;
    /*
     * Now make it non-blocking
     */
    int mypid  = getpid();
    /*
     * The Type bits must not be set. This way we need no Masking of the top bits.
     */
    int signal = (ConnectionId >> CONNECTIONID_BITS) & CONNECTIONID_MASK;
    /*
     * Now we make the socket asynchron and set the signal so we can process
     * incoming events.
     */
    int err;
    int flags;
    /*
     * Read the actual flags for the new socket.
     */
    flags=fcntl(Handle, F_GETFL);
    if (flags != -1) {
        /*
         * Add the async flags and set them.
         */
        err=fcntl(Handle, F_SETFL, flags | O_ASYNC | O_NONBLOCK | O_RDWR);
        if (err==0) {
            /*
             * Set the owner of the connection.
             */
            err=fcntl(Handle, F_SETOWN, mypid);
            if (err==0) {
                /*
                 * Set the signal to use for eventhandling.
                 */
                err=fcntl(Handle, F_SETSIG, signal);
                if (err != 0) {
                    Log.Write(LogLevel::Error, "Error on setting signal for event handline: %s\n", strerror(errno));
                }
            } else {
                Log.Write(LogLevel::Error, "Error on set owner of connection: %s\n", strerror(errno));
            }
        } else {
            Log.Write(LogLevel::Error, "Error on setting async flags on socket: %s\n", strerror(errno));
        }
    } else {
        Log.Write(LogLevel::Error, "Error on getting flags from socket: %s\n", strerror(errno));
    }
}
/*
 * The connection destructor releases all data already queued for processing.
 */
CConnection::~CConnection() {

    OutQueue.clear();

    if (Protocol != nullptr) {
        delete Protocol;
    }
}


int CConnection::OnWrite() {
    int retval = -EBADF;

    retval = Write();

    if (retval == -EBADF) {
        Close();
    }
    return (retval);
}

int CConnection::OnRead() {
    int retval = -EBADF;

    retval = Read();

    if (retval == -EBADF) {
        Close();
    }
    return (retval);
}

int CConnection::OnQueue(tPackageType* aPacket) {
    int retval = 0;

    if (aPacket) {
        switch (aPacket->type) {
        case ePackageType::Message:
            retval = Process(static_cast<tMsgPackage*>(aPacket));
            break;
        case ePackageType::NetPack:
            retval = Process(static_cast<tNetPackage*>(aPacket));
            break;
        case ePackageType::Timer:
            retval = Process(static_cast<tTimerPackage*>(aPacket));
            break;
        default:
            Log.Write(LogLevel::Critical, "Invalid packet-type %04.4x found", aPacket->type);
            break;
        }
    }
    return (retval);
}

int CConnection::OnClose() {
    int retval = -EBADF;

    if (retval == -EBADF) {
        Close();
    }
    return (retval);
}
/*
 * This processes a msg that has been send through the signal queue.
 */
int CConnection::Process(tMsgPackage *aMsg) {
    return Process(aMsg->msg);
}
/*
 * This processes a package that has been send through the signal queue
 */
int CConnection::Process(tNetPackage *aPack) {
    return (Process(aPack->mPack));
}
/*
 * This processes a timer event that has been send through the signal queue.
 */
int CConnection::Process(tTimerPackage *aEvent) {
    return Process(aEvent->event);
}
/*
 * The default implementation warns about not processing the package and
 * frees the memory.
 */
int CConnection::Process(std::shared_ptr<tNetPack> aPacket) {
    if (aPacket != nullptr) {
        Log.Write(LogLevel::Warn, "Unprocessed NetPack detected");
    }
    return 0;
}


int CConnection::Process(std::shared_ptr<tMsg> aMsg) {
    if (aMsg != 0) {
        Log.Write(LogLevel::Warn, "Unprocessed Message/Signal detected");
    }
    return 0;
}

int CConnection::Process(std::shared_ptr<tTimerEvent> aTimerId) {
    if (aTimerId) {
        Log.Write(LogLevel::Warn, "Unprocessed timer event %08.8x detected", aTimerId->what);
    }
    return 0;
}

int CConnection::QueueOutgoing(std::shared_ptr<tNetPack> aPacket) {
    int     retval = 0;
    /*
     * Check the connection id and the pointer to the packet.
     */
    if ((aPacket) && (aPacket->connid == ConnectionId)) {
        if (!ToSend) {
            ToSend = aPacket;
        } else {
            OutQueue.push_back(aPacket);
        }
    } else {
        if (aPacket == nullptr) {
            retval = -1;
        } else {
            Log.Write(LogLevel::Error, "Invalid ConnectionId : %lu : in NetPack to Write ....  dropping the packet\n", aPacket->connid);

            retval = -EBADF;
        }
    }
    return (retval);
}

void CConnection::Close() {
    if (close(Handle) == 0) {
        Handle = -1;

        OutQueue.clear();

        tMsg* msg;

        if (SendReplyAfterClose) {
            msg = (tMsg*)new tMsgWSDisconnectReply;
        } else {
            msg = (tMsg*)new tSigWSDisconnectIndication;
        }

        CCommThread::instance().Put(ConnectionId, msg);
    } else {
        Log.Write(LogLevel::Error, "Close failed for %i : %s", Handle, strerror(errno));
    }
}
