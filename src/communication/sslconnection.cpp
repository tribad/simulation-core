#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdint.h>
#include <sstream>
#include <iomanip>
#include "netpack.h"
#include "simifc.h"
#include "sim.h"
#include "connection.h"
#include "sslconnection.h"

extern int mainpid;
extern long loglevel;

CSSLConnection::CSSLConnection(CLogger& aLogger) : CConnection(aLogger)
{
    AlreadySend    = 0;
}

CSSLConnection::CSSLConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger, CProtocol* aProto)
    : CConnection(aHandle, aConnectionId, aLogger, aProto), mX509(nullptr) {
    AlreadySend     = 0;
    ConnectionState = eSSLState::SSLHandshake;

    struct sockaddr_in addr;
    socklen_t          addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(aHandle, (struct sockaddr *)&addr, &addr_size);

    if (res == 0) {
        Name    = inet_ntoa(addr.sin_addr);
        Address = ntohl(addr.sin_addr.s_addr);
        Port    = ntohs(addr.sin_port);
    } else {
        Address = 0;
        Port    = -1;
    }
}

CSSLConnection::~CSSLConnection() noexcept {
    gnutls_deinit(Session);
}

void CSSLConnection::Setup(std::shared_ptr<CX509> x509) {
    int                gtlserror;

    gtlserror = gnutls_init(&Session, GNUTLS_SERVER | GNUTLS_NONBLOCK);
    gtlserror = gnutls_priority_set(Session, x509->priority_cache);
    gtlserror = gnutls_credentials_set(Session, GNUTLS_CRD_CERTIFICATE, x509->credentials);
    if (gtlserror != GNUTLS_E_SUCCESS) {
        std::ostringstream oss;
        oss << "Error on SSL Session setup: " << gnutls_strerror(gtlserror);

        Log.Write(LogLevel::Error, oss);
    }
    /*
     * Make sure that the credentials are removed after all connections have ended the usage.
     */
    mX509 = x509;
    gnutls_certificate_server_set_request(Session, GNUTLS_CERT_IGNORE);
    gnutls_handshake_set_timeout(Session, GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);
    gnutls_transport_set_int(Session, Handle);
}

int CSSLConnection::Read() {
    int       retval   = 0;
    ssize_t   dataread = -EBADF;
    int       tlserror = 0;
    /*
     * We do it in an pseudo endless loop.
     * breaking out is easier to handle than a flag.
     */
    for (;(Handle != -1) && (retval == 0);) {
        /*
         * We are working with a statemachine because we must do the
         * SSL handshake first. And may do it as re-handshake at everytime.
         */
        switch (ConnectionState) {
        case eSSLState::SSLHandshake:
            tlserror = gnutls_handshake(Session);
            if (tlserror == 0) {
                std::ostringstream oss;

                oss << __FILE__ << " : " << __LINE__ << " : " << gnutls_strerror(tlserror);
                Log.Write(LogLevel::Trace, oss);
                ConnectionState = eSSLState::SSLCommunication;
                InPacketCount++;
            } else if (tlserror == GNUTLS_E_AGAIN) {
                std::ostringstream oss;

                oss << __FILE__ << " : " << __LINE__ << " : Short on TLS handshake - AGAIN " << gnutls_strerror(tlserror);
                Log.Write(LogLevel::Info, oss);

                retval = -EWOULDBLOCK;
            } else if (tlserror == GNUTLS_E_INTERRUPTED) {
                std::ostringstream oss;

                oss << __FILE__ << " : " << __LINE__ << " : Short on TLS handshake - INTERRUPTED " << gnutls_strerror(tlserror);
                Log.Write(LogLevel::Info, oss);

                retval = -EWOULDBLOCK;
            } else {
                std::ostringstream oss;

                oss << __FILE__ << " : " << __LINE__ << " : " << gnutls_strerror(tlserror);
                Log.Write(LogLevel::Error, oss);

                retval = -EBADF;
            }
            break;
        case eSSLState::SSLReHandshake:
        {
            std::ostringstream oss;

            oss << __FILE__ << " : " << __LINE__ << " : Re-Handshake requested";
            Log.Write(LogLevel::Info, oss);
            break;
        }
        case eSSLState::SSLCommunication:
            /*
             * We create new packets only if we have none from a earlier
             * run.
             */
            Packet = std::make_shared<tNetPack>();
            dataread=gnutls_record_recv(Session, Packet->buffer, Packet->size);
            /*
             * Check for a shutdown of the connection.
             * If it happens, close the connection and remove the connection info from
             * the connectionmap
             */
            if (dataread == 0) {
                retval = -EBADF;
                std::ostringstream oss;

                oss << "Got a shutdown of the connection:" << Packet->connid;
                Log.Write(LogLevel::Trace, oss);
                break;
            } else {
                /*
                 * If we got an error check if it is a wouldblock error.
                 * Any other error than wouldblock, is a real error.
                 * Close the connection than and remove the connection info from
                 * the connectionmap
                 */
                if (dataread < 0) {
                    if ((dataread == GNUTLS_E_AGAIN) || (dataread == GNUTLS_E_INTERRUPTED)) {
                        retval = -EWOULDBLOCK;
                    } else if ((dataread == GNUTLS_E_WARNING_ALERT_RECEIVED) || (dataread == GNUTLS_E_FATAL_ALERT_RECEIVED)) {
                        gnutls_alert_description_t alert = gnutls_alert_get(Session);
                        std::ostringstream oss;

                        oss << __FILE__ << " : " << __LINE__ << " : " << gnutls_alert_get_name(alert);
                        Log.Write(LogLevel::Error, oss);
                        retval = -EBADF;
                    } else {
                        std::ostringstream oss;
                        oss << __FILE__ << " : " << __LINE__ << " : " << gnutls_strerror(dataread);

                        Log.Write(LogLevel::Error, oss);
                        retval = -EBADF;
                    }
                    break;
                } else {
                    /*
                     * We got some data from the socket.
                     * Prepare the packet with the size and call the simulation.
                     */
                    if (dataread > 0) {
                        InByteCount+=dataread;
                        InPacketCount++;
                        /*
                         * The answer must not be on stack. Sending maybe split into several steps.
                         */
                        if (Protocol != 0) {

                            std::list<std::shared_ptr<tNetPack> > answer;

                            Packet->connid = ConnectionId;
                            Packet->len    = dataread;
                            Packet->port   = Port;
                            Packet->host   = Address;
                            answer         = Protocol->Handle(Packet);

                            for (auto a : answer) {
                                if (a != nullptr) {
                                    a->connid = ConnectionId;
                                    retval = Write(a);
                                }
                            }
                            /*
                             * If there has been something wrong before we break the loop.
                             */
                            if (retval != 0) {
                                break;
                            }
                        }
                    } else {
                        /*
                         * No more elses here.
                         */
                    }
                }
            }
            break;
        default:
            break;
        }

    }
    return retval;
}

int CSSLConnection::Write(std::shared_ptr<tNetPack> aPacket) {
    int     retval = 0;
    ssize_t datasend;

    QueueOutgoing(aPacket);

    while (((ToSend ) || (!OutQueue.empty())) && (retval == 0)) {
        if (ToSend == 0) {
            ToSend      = OutQueue.front();
            AlreadySend = 0;
            OutQueue.pop_front();
        } else {
        }
        /*
         * Send as much data as possible. Maybe the complete.
         */
        datasend=gnutls_record_send(Session, ToSend->buffer+AlreadySend, ToSend->len-AlreadySend);
        if ((datasend  > 0) && (loglevel & 0x1000)) {
            std::ostringstream oss;

            oss << "SSL-Out :" << std::hex << std::setw(16) << std::setfill('0') << ToSend->connid << "::"<< std::dec << datasend;
            Log.Write(LogLevel::Trace, oss);
        }
        /*
         * Could send the complete packet. Nothing left todo later.
         */
        if (datasend == (ssize_t)(ToSend->len-AlreadySend)) {
            OutByteCount  += datasend;
            OutPacketCount++;

            if (CloseAfterSend) {
                /*
                 *  If the sending is complete we close the connection
                 */
                Shutdown();
                retval = -EBADF;
                break;
            } else {
                ToSend      = 0;
                AlreadySend = 0;
            }
        } else {
            /*
             * Check for an error.
             * If we got a would block error the sending maybe completed later.
             */
            if (datasend < 0) {
                if ((datasend == GNUTLS_E_AGAIN) || (datasend == GNUTLS_E_INTERRUPTED)) {
                    retval = -EWOULDBLOCK;
                    if (loglevel & 0x1000) {
                        std::ostringstream oss;
                        oss << "Blocked send to " << Handle;
                        Log.Write(LogLevel::Trace, oss);
                    }
                    Writable = false;
                } else if ((datasend == GNUTLS_E_WARNING_ALERT_RECEIVED) || (datasend == GNUTLS_E_FATAL_ALERT_RECEIVED)) {
                    gnutls_alert_description_t alert = gnutls_alert_get(Session);

                    std::ostringstream oss;
                    oss << __FILE__ << " : " << __LINE__ << " : " << gnutls_alert_get_name(alert);
                    Log.Write(LogLevel::Error, oss);
                    retval = -EBADF;
                } else {
                    std::ostringstream oss;
                    oss << __FILE__ << " : " << __LINE__ << " : " << gnutls_strerror(datasend);

                    Log.Write(LogLevel::Error, oss);
                    retval = -EBADF;
                }
                break;
            } else {
                /*
                 * We did a partial send. Mark the socket as not writable.
                 * Store the data already send and save the message for later use.
                 * On SSL we may send less data because the record length is the limit.
                 * In this case we call the send function again.
                 * If the underlying send function would block we will get an appropriate error
                 * from SSL.
                 */
                OutByteCount+=datasend;
                OutPacketCount++;
                AlreadySend += (size_t)(datasend);
                if (loglevel & 0x1000) {
                    std::ostringstream oss;
                    oss << "Partial Send to " << Handle;

                    Log.Write(LogLevel::Trace, oss);
                }

            }
        }
    }
    return retval;
}

int CSSLConnection::Process(std::shared_ptr<tMsg> aMsg) {
    int retval = 0;

    if (!(aMsg->type & MSG_TYPE_INTERNAL)) {
        auto conn = std::get<tConnection>(aMsg->dst);

        if (conn.format == eCommFormat::JSON) {
            std::ostringstream oss;
            oss << "{";
            aMsg->pack_json(oss);
            oss << "}";
            std::shared_ptr<tNetPack> packet = std::make_shared<tNetPack>(oss.str().size());

            memcpy(packet->buffer, oss.str().c_str(), oss.str().size());
            packet->connid = conn.id;

            if (loglevel & 0x1000) {
                std::ostringstream lss;

                lss << "SSL-Data:" << std::hex << std::setw(16) << std::setfill('0') << packet->connid << "::" << std::dec << oss.str().size() << "::" << oss.str();
                Log.Write(LogLevel::Trace, lss);
            }

            retval = Process(packet);
        }
    }
    return retval;
}


void CSSLConnection::Shutdown() {
    shutdown(Handle, SHUT_RDWR);

    std::ostringstream oss;

    oss << "Shutdown:" << Handle;
    Log.Write(LogLevel::Trace, oss);
}

void CSSLConnection::Close() {
    std::ostringstream oss;

    oss << "Close:" << Handle;
    Log.Write(LogLevel::Trace, oss);

    CConnection::Close();
}

int CSSLConnection::Process(std::shared_ptr<tNetPack> aPacket) {
    int retval;

    retval = Write(aPacket);

    return retval;

}
