// MIT License
//
// Copyright (c) 2017 Hans-Juergen Lange<hjl@simulated-universe.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
#include "tcpconnection.h"

extern int mainpid;
extern long loglevel;

CTcpConnection::CTcpConnection(CLogger& aLogger) : CConnection(aLogger)
{
}

CTcpConnection::CTcpConnection(int aHandle, uint64_t aConnectionId, CLogger& aLogger, CProtocol* aProto) : CConnection(aHandle, aConnectionId, aLogger, aProto) {
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

CTcpConnection::~CTcpConnection() {
}

int CTcpConnection::Read() {
    int       retval   = 0;
    ssize_t   dataread = -EBADF;
    /*
     * We do it in an pseudo endless loop.
     * breaking out is easier to handle than a flag.
     */
    for (;Handle != -1;) {
        /*
         * We always create a new NetPack as we do not know if the old one is still
         * flowing around.
         */
        Packet = std::make_shared<tNetPack>();
        /*
         * Read in as much as the Packet allows.
         */
        dataread = recv(Handle, Packet->buffer, Packet->size, MSG_DONTWAIT);
        /*
         * Check for a shutdown of the connection.
         * If it happens, close the connection and remove the connection info from
         * the connectionmap
         */
        if (dataread == 0) {
            retval = -EBADF;
            break;
        } else {
            /*
             * If we got an error scheck if it is a wouldblock error.
             * Any other error than wouldblock, is a real error.
             * Close the connection than and remove the connection info from
             * the connectionmap
             */
            if (dataread < 0) {
                if (errno==EWOULDBLOCK) {
                    retval = -EWOULDBLOCK;
                } else {
                    retval = -EBADF;
                }
                break;
            } else {
                /*
                 * We got some data from the socket.
                 * Prepare the packet with the size and call the simulation.
                 */
                if (dataread > 0) {
                    InByteCount  += dataread;
                    InPacketCount++;
                    /*
                     * The answer must not be on stack. Sending maybe split into several steps.
                     */
                    if (Protocol != nullptr) {
                        std::list<std::shared_ptr<tNetPack> > answer;

                        Packet->connid = ConnectionId;
                        Packet->len    = dataread;
                        Packet->port   = Port;
                        Packet->host   = Address;
                        answer = Protocol->Handle(Packet);

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
    }
    return retval;
}

int CTcpConnection::Write(std::shared_ptr<tNetPack> aPacket) {
    int     retval = 0;
    ssize_t datasend;
    //
    //  We ignore the return value as we look directly on the queue
    QueueOutgoing(aPacket);
    /*
     * Loop until all packets are send or writing has been suspended.
     */
    while ((ToSend != nullptr) || (!OutQueue.empty())) {
        if (ToSend == nullptr) {
            ToSend      = OutQueue.front();
            AlreadySend = 0;
            OutQueue.pop_front();
        } else {
        }
        /*
         * Send as much data as possible. Maybe the complete.
         */
        datasend=send(Handle, ToSend->buffer+AlreadySend, ToSend->len-AlreadySend, 0);
        if ((datasend  > 0) && (loglevel & 0x1000)) {
            std::ostringstream oss;

            oss << "TCP-Out :" << std::hex << std::setw(16) << std::setfill('0') << ToSend->connid << "::" << std::dec << datasend << std::endl;
            Log.Write(LogLevel::Trace, oss.str());
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
                ToSend      = nullptr;
                AlreadySend = 0;
            }
        } else {
            /*
             * Check for an error.
             * If we got a would block error the sending maybe completed later.
             *
             * datasend == 0 is a gracefull shutdown.
             */
            if (datasend <= 0) {
                retval = -errno;
                if (errno == EWOULDBLOCK) {
                    Writable = false;
                    break;
                } else {
                    /*
                     * Close connection, remove connection info and delete packet.
                     *
                     * Here we have a serious error or a gracefull shutdown.
                     * The shutdown call clears the condition for the loop.
                     */
                    if (datasend == 0) {
                        Log.Write(LogLevel::Trace, "Got a gracefull shutdown on %08.8x", ToSend->connid);
                    } else {
                        Log.Write(LogLevel::Error, "Got a real error on %08.8x : %s", ToSend->connid, strerror(errno));
                    }
                    Shutdown();
                    retval = -EBADF;
                }
            } else {
                OutByteCount  += datasend;
                OutPacketCount++;
                /*
                 * We did a partial send. Mark the socket as not writable.
                 * Store the data already send and save the message for later use.
                 */
                Writable    = false;
                AlreadySend += (size_t)(datasend);
                retval      = -EWOULDBLOCK;
                /*
                 * Break the loop as we are not writeable anymore.
                 */
                break;
            }
        }
    }
    return retval;
}

int CTcpConnection::Process(std::shared_ptr<tMsg> aMsg) {
    int retval = 0;

    if (!(aMsg->type & MSG_TYPE_INTERNAL)) {
        if (std:: holds_alternative<tConnection>(aMsg->dst)) {
            auto conn = std::get<tConnection>(aMsg->dst);

            if (conn.format == eCommFormat::JSON) {
                std::ostringstream oss;
                oss << "{";
                aMsg->pack_json(oss);
                oss << "}";
                /*
                 * Out of memory will lead to an excpetion anyway.
                 * So no pointer checking.
                 */
                std::shared_ptr<tNetPack> packet = std::make_shared<tNetPack>(oss.str().size());

                memcpy(packet->buffer, oss.str().c_str(), oss.str().size());
                packet->len = packet->size;
                packet->connid = conn.id;

                if (loglevel & 0x1000) {
                    std::ostringstream lss;

                    lss << "TCP-Data:" << std::hex << std::setw(16) << std::setfill('0') << packet->connid << "::"
                        << std::dec << oss.str().size() << "::" << oss.str();
                    Log.Write(LogLevel::Trace, lss);
                }

                retval = Process(packet);
            }
        }
    }
    return retval;
}

void CTcpConnection::Shutdown() {
    shutdown(Handle, SHUT_RDWR);
    /*
     * Communication has been shutdown. So all sending can be aborted.
     */
    AlreadySend = 0;
}

int CTcpConnection::Process(std::shared_ptr<tNetPack> aPacket) {
    int retval = 0;

    retval = Write(aPacket);
    return retval;

}
