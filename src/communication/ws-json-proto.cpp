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

#include <memory.h>
#include <gnutls/gnutls.h>
#include <iostream>
#include <iomanip>
#include "ws-json-proto.h"
#include "logger.h"
#include "commthread.h"
#include "connection.h"

extern uint64_t loglevel;

static const char basis_64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
 * Function for a base64encode.
 */
static std::string base64encode(unsigned char *str, int len) {
    int i;
    std::string retval;

    for (i = 0; i < len - 2; i += 3) {
        retval.push_back( basis_64[(str[i] >> 2) & 0x3F]);
        retval.push_back( basis_64[((str[i] & 0x3) << 4) | ((int) (str[i + 1] & 0xF0) >> 4)]);
        retval.push_back( basis_64[((str[i + 1] & 0xF) << 2) |((int) (str[i + 2] & 0xC0) >> 6)]);
        retval.push_back( basis_64[str[i + 2] & 0x3F]);
    }
    if (i < len) {
        retval.push_back( basis_64[(str[i] >> 2) & 0x3F]);
        if (i == (len - 1)) {
            retval.push_back( basis_64[((str[i] & 0x3) << 4)]);
            retval.push_back( '=' );
        } else {
            retval.push_back( basis_64[((str[i] & 0x3) << 4) | ((int) (str[i + 1] & 0xF0) >> 4)]);
            retval.push_back( basis_64[((str[i + 1] & 0xF) << 2)]);
        }
        retval.push_back('=');
    }

    return (retval);
}

CWSJSONProtocol::CWSJSONProtocol(uint64_t aConnId, CLogger& aLogger) : CProtocol(aLogger) {
    wsstate      = eWaitHttp;
    http         = new tHttpRequest(aConnId);
    json         = 0;
    wspayload    = 0;
    PingCount    = 0;
    ConnectionId = aConnId;
    memset(&message, 0, sizeof(message));
    addtimer(FIRSTDATA_TIMEOUT, aConnId, (aConnId >> CONNECTIONID_BITS), FIRSTDATA_TIMERID);
}

CWSJSONProtocol::~CWSJSONProtocol() {
    if (http != 0) {
        delete http;
    }
    if (json != 0) {
        delete json;
    }
    if (wspayload != 0) {
        delete [] wspayload;
    }
}

void CWSJSONProtocol::ResetMessage() {
    if (wspayload != nullptr) {
        /*
         * The payload is no longer needed.
         */
        delete [] wspayload;
        /*
         * Prevent a double free in the connection destructor.
         */
        wspayload = nullptr;
        /*
         * Reset the statemachine of the WS message.
         */
    }
    memset(&message, 0, sizeof(message));
}

std::shared_ptr<tNetPack> CWSJSONProtocol::Running(std::shared_ptr<tNetPack> aPacket, size_t & aPos) {
    std::shared_ptr<tNetPack> retval;
    uint8_t*                  s    = aPacket->buffer + aPos;

    /*
     * Check that the position is within the content before processing.
     */
    if (aPos < aPacket->len) {
        PingCount        = 0;

        do {
            switch (message.state) {
            case 0:
                message.fin    = ((*s) & 0x80)==0x80;
                message.rsv1   = ((*s) & 0x40)==0x40;
                message.rsv2   = ((*s) & 0x20)==0x20;
                message.rsv3   = ((*s) & 0x10)==0x10;
                message.opcode = ((*s) & 0x0f);
                message.hdrbytecount++;
                message.state = 1;
                if ((message.opcode != 0x00) && (message.opcode != 0x01) && (message.opcode != 0x02) &&
                    (message.opcode != 0x08) && (message.opcode != 0x09) && (message.opcode != 0x0a)) {
                        Log.Write(LogLevel::Critical, "Got invalid opcode %0x on connection %0lx", message.opcode, aPacket->connid);
                }
                break;
            case 1:
                message.mask = (((*s) &0x80) == 0x80);
                /*
                 * We are a server. Clients must mask their data. Doing not
                 * is an error.
                 */
                if (!message.mask) {
                    retval = CloseFrame(1002);
                    /*
                     * Set the position to the end. Even the packet gets deleted.
                     * Helps on debugging.
                     */
                    aPos   = aPacket->len;
                    Log.Write(LogLevel::Critical,"No Message Mask. Invalid for client %0lx", aPacket->connid);
                    ResetMessage();
                    return retval;
                }
                message.payloadlen=((*s) & 0x7f);
                switch (message.payloadlen) {
                case 126:
                    message.state = 2;
                    break;
                case 127:
                    message.state = 3;
                    break;
                default:
                    message.state = 4;
                    break;
                }
                message.hdrbytecount++;
                break;
            case 2:  //  ReadIn 16 Bit payload len.
                if (message.hdrbytecount==2) {
                    message.payloadlen = (uint64_t)((*s)) << 8;
                } else {
                    message.payloadlen |= ((uint64_t)((*s)) & 0x00000000000000ff);
                    message.state = 4;

                }
                message.hdrbytecount++;
                break;
            case 3:  //  ReadIn 64 Bit Payload len
                switch (message.hdrbytecount) {
                case 2:
                    message.payloadlen = (uint64_t)((*s)) << 56;
                    break;
                case 3:
                    message.payloadlen |= (uint64_t)(*s) << 48;
                    break;
                case 4:
                    message.payloadlen |= (uint64_t)((*s)) << 40;
                    break;
                case 5:
                    message.payloadlen |= (uint64_t)((*s)) << 32;
                    break;
                case 6:
                    message.payloadlen |= (uint64_t)((*s)) << 24;
                    break;
                case 7:
                    message.payloadlen |= (uint64_t)((*s)) << 16;
                    break;
                case 8:
                    message.payloadlen |= (uint64_t)((*s)) << 8;
                    break;
                case 9:
                    message.payloadlen |= (uint64_t)((*s));
                    message.state = 4;
                    break;
                default:
                    break;
                }
                message.hdrbytecount++;
                break;
            case 4: // Mask byte 1
                message.state = 5;
                message.maskkey[0] = (*s);
                message.hdrbytecount++;
                break;
            case 5: // Mask byte 2
                message.state = 6;
                message.maskkey[1] = (*s);
                message.hdrbytecount++;
                break;
            case 6: // Mask byte 3
                message.state = 7;
                message.maskkey[2] = (*s);
                message.hdrbytecount++;
                break;
            case 7: // Mask byte 4
                message.maskkey[3] = (*s);
                message.hdrbytecount++;
                /*
                 * restart recv counter
                 */
                message.recv = 0;
                /*
                 * We do not create a payload data area for empty payloads.
                 */
                if (message.payloadlen > 0) {
                    message.state = 8;
                    /*
                     * Create the payload buffer.
                     * We must copy the data to get them encoded.
                     */
                    wspayload = new uint8_t[message.payloadlen];
                } else {
                    wspayload     = nullptr;
                    message.state = 9;
                }
                break;
            case 8:
                wspayload[message.recv]=(*s) ^ message.maskkey[message.recv % 4];
                message.recv++;
                break;
            default:
                break;
            }
            s++;
        } while (((s-aPacket->buffer) < (int)(aPacket->len)) && ((message.state < 8) || (message.recv < message.payloadlen)));
        /*
         * Update the position parameter.
         */
        aPos = s - aPacket->buffer;
        /*
         * Processed all bytes for a frame.
         * send it into the simulation.
         */
        if ((message.recv >= message.payloadlen) && (message.state >= 8)) {
            switch (message.opcode) {
            case 0x00:
            case 0x01: /* Text frame. JSON expected */
                /*
                 * We may not have a payload
                 */
                if (wspayload != nullptr) {
                    /*
                     * Parse the packet content. We expect JSON formated messages here.
                     */
                    json=parse(json, wspayload, message.payloadlen);
                    if (completed(json)) {
                        CCommThread::instance().Put(aPacket->connid, json);
                        json = nullptr;
                    } else {
                    }
                } else {
                    Log.Write(LogLevel::Warn, "Expected payload got none");
                }
                ResetMessage();
                break;
            case 0x02: /* Binary frame. Binary serialized format expected. */
            {
                std::ostringstream oss;

                oss << "We do not process binary frames on " << ConnectionId;
                Log.Write(LogLevel::Info, oss);
                ResetMessage();
                break;
            }
            case 0x08:
                /*
                 * Got a close frame and do a reply to that.
                 */
                retval = CloseFrame(1002);
                if (loglevel & 0x1000) {
                    std::ostringstream oss;
                    oss << "Got close frame from " << ConnectionId;

                    Log.Write(LogLevel::Trace, oss);
                }
                /*
                 * As the close frame may contain a payload we reset the message structure and
                 * free the payload if one has been send.
                 */
                ResetMessage();
                /*
                 * This is a quick exit to come around the situation to have a double free.
                 * After getting a close we do not process any data that may still be in the
                 * aPacket.
                 */
                return retval;
                break;
            case 0x09:
                /*
                 * The incoming ping frame may have a payload we need to copy.
                 * So we create the answer right before anything else.
                 */
                retval = PongFrame();
                if (loglevel & 0x1000) {
                    std::ostringstream oss;

                    oss << "Got ping frame from " << ConnectionId;
                    Log.Write(LogLevel::Trace, oss);
                }
                ResetMessage();
                break;
            case 0x0a:
                if (loglevel & 0x1000) {
                    std::ostringstream oss;

                    oss << "Pong   : " << std::hex << std::setw(16) << std::setfill('0') << ConnectionId << std::dec;
                    Log.Write(LogLevel::Trace, oss);
                }
                if (PingCount!=0) {
                    PingCount--;
                }
                ResetMessage();
                break;
            default:
                Log.Write(LogLevel::Critical, "Invalid opcode");
                ResetMessage();
                break;
            }
        }
    }
    return retval;
}


std::list<std::shared_ptr<tNetPack> > CWSJSONProtocol::Handle(std::shared_ptr<tNetPack> aPack) {
    std::list<std::shared_ptr<tNetPack> > retval;

    switch (wsstate) {
    case eWaitHttp:
        /*
         * The processing of the HTTP request is a state machine.
         * This way we can process the request step by step
         */
        ProcessHttp(aPack);
        /*
         * If the http request has been completed we send a message to the
         * Simulation and wait for an answer.
         */
        if (http->state == -1) {
            /*
             * Create a request message.
             */
            tMsgWSConnectReq* req = new tMsgWSConnectReq;
            req->client           = aPack->host;
            if (http->uri != nullptr) {
                req->uri              = http->uri;
            }
            if (http->host != nullptr) {
                req->host = http->host;
            }
            if (http->origin != nullptr) {
                req->origin = http->origin;
            }
            if (http->cookie != nullptr) {
                req->cookie = http->cookie;
            }
            if (http->proto != nullptr) {
                std::string allproto = http->proto;
                std::string proto;
                size_t      start    = 0;
                size_t      end      = 0;

                do {
                    end=allproto.find_first_of(" ,", start);
                    proto=allproto.substr(start, end-start);
                    /*
                     * trim front
                     */
                    while (proto[0]==' ') proto.erase(0, 1);
                    /*
                     * if proto has a size after trimming we can add it to the
                     * list of protos.
                     */
                    if (proto.size()>0) {
                        req->proto.push_back(proto);
                    }
                    /*
                     * if we are not at end of search we skip a character.
                     * At end we do nothing and let the loop condition stop
                     * the loop.
                     */
                    if (end == std::string::npos) {
                        start = end;
                    } else {
                        start = end + 1;
                    }
                } while (end!=std::string::npos);
            }
            CCommThread::instance().Put(aPack->connid, req);

            wsstate = eWaitSimulation;
        }
        break;
    case eWaitSimulation:

        break;
    case eRunning:
        {
            size_t                    position = 0u;
            std::shared_ptr<tNetPack> answer;
            /*
             * Maybe we need to introduce the dump of the incoming package again.
             * But for now we leave it by a more quiet version.
             */

            do {
                answer = Running(aPack, position);
                retval.push_back(answer);
            } while (position < aPack->len);
        }
        break;
    default:
        break;
    }
    return retval;
}

std::shared_ptr<tNetPack> CWSJSONProtocol::Process(std::shared_ptr<tMsg> aMsg) {
    std::shared_ptr<tNetPack> retval;

    if (aMsg) {
        switch (aMsg->id) {
        case IDM_TMSGWSCONNECTREPLY:
            retval = Process(std::static_pointer_cast<tMsgWSConnectReply>(aMsg));
            break;
        case IDM_TMSGWSDISCONNECTREQ:
            retval = Process(std::static_pointer_cast<tMsgWSDisconnectReq>(aMsg));
            break;
        case IDS_TSIGWSDISCONNECTINDICATION:
            retval = Process(std::static_pointer_cast<tSigWSDisconnectIndication>(aMsg));
            break;
        case IDM_WSPINGREQ:
            retval = Process(std::static_pointer_cast<tMsgWSPingReq>(aMsg));
            break;
        default:
            retval = ProcessDefault(aMsg);
            break;
        }
    }
    return retval;
}

std::shared_ptr<tNetPack> CWSJSONProtocol::Process(std::shared_ptr<tNetPack> aPacket) {
    std::shared_ptr<tNetPack> retval;

    if (aPacket) {
        uint8_t header[16];
        uint8_t *s = header;
        /*
         * clear the header. So we only set bits.
         */
        memset(s, 0, sizeof(header));
        *s |= 0x80;   /* fin => true    */
        *s |= 0x01;   /* opcode => 0x01 */
        s++;
        if (aPacket->len > 125) {
            if (aPacket->len > 65535) {
                *s = 127;
                s++;
                *s = (uint8_t) (aPacket->len >> 56);
                s++;
                *s = (uint8_t) (aPacket->len >> 48);
                s++;
                *s = (uint8_t) (aPacket->len >> 40);
                s++;
                *s = (uint8_t) (aPacket->len >> 32);
                s++;
                *s = (uint8_t) (aPacket->len >> 24);
                s++;
                *s = (uint8_t) (aPacket->len >> 16);
                s++;
                *s = (uint8_t) (aPacket->len >> 8);
                s++;
                *s = (uint8_t) (aPacket->len & 0x00000000000000ff);
                s++;

            } else {
                *s = 126;
                s++;
                *s = aPacket->len >> 8;
                s++;
                *s = aPacket->len & 0x00000000000000ff;
                s++;
            }
        } else {
            *s = aPacket->len;
            s++;
        }
        /*
         * Header now is complete and we can calculate the length
         * of it. The sum of the header length and the payloadlen are
         * the size of the netpack.
         */
        retval = std::make_shared<tNetPack>((s - header) + aPacket->len);
        /*
         * Copy the header
         */
        memcpy(retval->buffer, header, (s - header));
        /*
         * Append the payload
         */
        memcpy(retval->buffer + (s - header), aPacket->buffer, aPacket->len);
        /*
         * Set the length of the netpack.
         */
        retval->len = (s - header) + aPacket->len;
        /*
         * Set the connection id.
         */
        retval->connid = aPacket->connid;

    }
    return retval;
}


int CWSJSONProtocol::ProcessHttp(std::shared_ptr<tNetPack> aPacket) {
    int   error=0;
    char* s=(char*)(aPacket->buffer);

    do {
        http->buffer[http->len]=*s;
        switch (http->state) {
        case 0:
            if ((*s=='G') || (*s=='g')) {
                http->method=http->buffer;
                http->len++;
                http->state = 1;
            } else if ((*s=='P') || (*s=='p')) {
                http->method=http->buffer;
                http->len++;
                http->state = 11;
            } else {
                http->method = 0;
                error=-1;
            }
            break;
        case 1:
            if ((*s=='E') || (*s=='e')) {
                http->len++;
                http->state = 2;
            } else {
                http->method = 0;
                error=-1;
            }
            break;
        case 2:
            if ((*s=='T') || (*s=='t')) {
                http->len++;
                http->state = 3;
            } else {
                http->method = 0;
                error=-1;
            }
            break;
        case 3:    //  wait until first non blank
            if (*s!=' ') {
                http->uri=http->buffer+http->len;
                http->state = 4;
            } else {
                http->buffer[http->len] = '\0';
            }
            http->len++;
            break;
        case 4:    // Wait until end of URI
            if (*s == ' ') {
                http->buffer[http->len] = '\0';
                http->state = 5;
            }
            http->len++;
            break;
        case 5:    //  wait for next non blank
            if (*s!=' ') {
                http->version=http->buffer+http->len;
                http->state = 6;
            } else {
                http->buffer[http->len] = '\0';
            }
            http->len++;
            break;
        case 6:   //  Wait for end of line. 0x0d
            if (*s != '\x0a') {
                if ((*s == ' ') || (*s == '\x0d')) {
                    http->buffer[http->len] = '\0';
                }
            } else {
                http->buffer[http->len] = '\0';
                http->state = 7;
            }
            http->len++;
            break;
        case 7:   //  Start readin a complete header line
            if (*s=='\x0d') { //  complete.
                http->state = -1;
                error = (s-(char*)(aPacket->buffer))+2;
            } else {
                http->name = http->buffer+http->len;
                http->state = 8;
                http->len++;
            }
            break;
        case 8:  //  wait colon, null space
            if (*s == ' ') {
                http->buffer[http->len] = '\0';
            } else {
                if (*s == ':') {
                    http->buffer[http->len] = '\0';
                    http->state = 9;
                }
            }
            http->len++;
            break;
        case 9:     //  wait first non blank
            if (*s != ' ') {
                http->value = http->buffer+http->len;
                http->state = 10;
            } else {
                http->buffer[http->len] = '\0';
            }
            http->len++;
            break;
        case 10:   //  Wait for end of value
            if (*s == '\x0d') {
                http->buffer[http->len] = '\0';
                if (strcasecmp(http->name, "Host")==0) {
                    http->host = http->value;
                } else if (strcasecmp(http->name, "Origin")==0) {
                    http->origin = http->value;
                } else if (strcasecmp(http->name, "Sec-WebSocket-Key")==0) {
                    http->key = http->value;
                } else if (strcasecmp(http->name, "Sec-WebSocket-Protocol")==0) {
                    http->proto = http->value;
                } else if (strcasecmp(http->name, "Sec-WebSocket-Version")==0) {
                    http->proto_version = http->value;
                } else if (strcasecmp(http->name, "Content-Length")==0) {
                    http->contentlen = strtoul(http->value, 0, 0);
                } else {

                }
                http->headerfields.insert(std::pair<char*, char*>(http->name, http->value));
            } else if (*s == '\x0a') {
                http->buffer[http->len] = '\0';
                http->state = 7;
            }
            http->len++;
            break;
        case 11:
            if ((*s=='O') || (*s=='o')) {
                http->len++;
                http->state = 12;
            } else {
                http->method = 0;
                error=-1;
            }
            break;
        case 12:
            if ((*s=='S') || (*s=='s')) {
                http->len++;
                http->state = 3;
            } else {
                http->method = 0;
                error=-1;
            }
            break;
        default:
            break;
        }
        s++;
    } while ((http->state != -1) && (s-(char*)(aPacket->buffer) < (int)(aPacket->len)) && (error != -1) && (http->len < HTTP_REQUEST_LEN));

    return (error);

}

std::shared_ptr<tNetPack> CWSJSONProtocol::Process(std::shared_ptr<tMsgWSConnectReply> aMsg) {
    std::shared_ptr<tNetPack> packet;
    /*
     * Now do the check
     */
    if (aMsg->error == 0) {
        unsigned char sha1[20];   //  Maximum size of a SHA1 hash
        size_t        sha1length = 20;
        int           tlserr     = 0;

        std::string answerkey=http->key;
        answerkey+="258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        gnutls_datum_t a = {(unsigned char*)(answerkey.c_str()), (unsigned int)(answerkey.size())};

        tlserr = gnutls_fingerprint(GNUTLS_DIG_SHA1, &a, sha1, &sha1length);
        if (tlserr == GNUTLS_E_SUCCESS) {
            packet = std::make_shared<tNetPack>();
            auto conn = std::get<tConnection>(aMsg->dst);

            /*
             * Set the connid. Gets check on write.
             */
            packet->connid = conn.id;
            if (aMsg->proto.empty()) {
                packet->len=sprintf((char*)(packet->buffer), "HTTP/1.1 101 Switching Protocols\r\n"
                                                             "Upgrade: websocket\r\n"
                                                             "Connection: Upgrade\r\n"
                                                             "Sec-WebSocket-Accept: %s\r\n",
                                                             base64encode(sha1, 20).c_str());
            } else {
                packet->len=sprintf((char*)(packet->buffer), "HTTP/1.1 101 Switching Protocols\r\n"
                                                             "Upgrade: websocket\r\n"
                                                             "Connection: Upgrade\r\n"
                                                             "Sec-WebSocket-Accept: %s\r\n"
                                                             "Sec-WebSocket-Protocol: %s\r\n\r\n",
                                                             base64encode(sha1, 20).c_str(), aMsg->proto.c_str());
            }
            wsstate = eRunning;
            addtimer(PING_TIME, ConnectionId, (ConnectionId >> CONNECTIONID_BITS), PING_TIMERID);
        } else {
        }
    } else {
        packet = std::make_shared<tNetPack>();

        packet->len=sprintf((char*)(packet->buffer), "HTTP/1.1 400 Bad Request\r\n\r\n");
    }
    return packet;
}

std::shared_ptr<tNetPack> CWSJSONProtocol::Process(std::shared_ptr<tMsgWSDisconnectReq> aMsg) {
    //
    //  the message get deleted outside
    (void)aMsg;
    //
    return (CloseFrame(1000));
}

std::shared_ptr<tNetPack> CWSJSONProtocol::Process(std::shared_ptr<tSigWSDisconnectIndication> aMsg) {
    //
    //  the message get deleted outside
    (void)aMsg;
    //
    return (CloseFrame(1000));
}

std::shared_ptr<tNetPack> CWSJSONProtocol::Process(std::shared_ptr<tMsgWSPingReq> aMsg) {
    //
    //  the message get deleted outside
    (void)aMsg;
    //
    std::shared_ptr<tNetPack> retval;
    if (PingCount != 0) {
        retval = CloseFrame(1000);
    } else {
        PingCount++;
        addtimer(PING_TIME, ConnectionId, (ConnectionId >> CONNECTIONID_BITS), PING_TIMERID);
        retval = PingFrame();
    }
    return retval;
}
//
//  this needs to create a complete WS packet from the message
std::shared_ptr<tNetPack> CWSJSONProtocol::ProcessDefault(std::shared_ptr<tMsg> aMsg) {
    std::shared_ptr<tNetPack> retval;

    if (!(aMsg->type & MSG_TYPE_INTERNAL)) {
        std::ostringstream oss;
        oss << "{";
        //
        // convert the message to JSON and create a netpack that can hold the result
        // plus the WS header.
        aMsg->pack_json(oss);
        oss << "}";
        retval = Pack(oss.str());
        auto conn = std::get<tConnection>(aMsg->dst);

        /*
         * Set the connection id.
         */
        retval->connid = conn.id;
        retval->host   = conn.host;
        retval->port   = conn.port;
    } else {

    }
    return retval;
}

std::shared_ptr<tNetPack> CWSJSONProtocol::Pack(const std::string& aContent) {
    std::shared_ptr<tNetPack> retval;
    uint8_t                   header[16];
    uint8_t*                  s = header;
    /*
     * clear the header. So we only set bits.
     */
    memset(s, 0, sizeof(header));
    *s|=0x80u;   /* fin => true    */
    *s|=0x01u;   /* opcode => 0x01 */
    s++;
    if (aContent.size() > 125u) {
        if (aContent.size() > 65535u) {
            *s = 127u;
            s++;
            *s = (uint8_t)(aContent.size() >> 56u);
            s++;
            *s = (uint8_t)(aContent.size() >> 48u);
            s++;
            *s = (uint8_t)(aContent.size() >> 40u);
            s++;
            *s = (uint8_t)(aContent.size() >> 32u);
            s++;
            *s = (uint8_t)(aContent.size() >> 24u);
            s++;
            *s = (uint8_t)(aContent.size() >> 16u);
            s++;
            *s = (uint8_t)(aContent.size() >> 8u);
            s++;
            *s = (uint8_t)(aContent.size() & 0x00000000000000ffu);
            s++;

        } else {
            *s = 126u;
            s++;
            *s = aContent.size() >> 8u;
            s++;
            *s = aContent.size() & 0x00000000000000ffu;
            s++;
        }
    } else {
        *s = aContent.size();
        s++;
    }
    /*
     * Header now is complete and we can calculate the length
     * of it. The sum of the header length and the payloadlen are
     * the size of the netpack.
     */
    retval = std::make_shared<tNetPack>((s-header) + aContent.size());
    /*
     * Copy the header
     */
    memcpy(retval->buffer, header, (s-header));
    /*
     * Append the payload
     */
    memcpy(retval->buffer+(s-header), aContent.c_str(), aContent.size());
    /*
     * Set the length of the netpack.
     */
    retval->len=(s-header) + aContent.size();
    if (loglevel & 0x1000) {
        std::ostringstream oss;

        oss << "WS-out  :" << std::hex << std::setw(16) << std::setfill('0') << ConnectionId << std::dec << "::" <<  message.payloadlen << "::" << aContent << "\nWS-out end:::::";
        Log.Write(LogLevel::Trace, oss);
    }

    return retval;
}


std::shared_ptr<tNetPack> CWSJSONProtocol::CloseFrame(int errorcode) {
    //
    //
    (void)errorcode;
    std::shared_ptr<tNetPack> newpacket = std::make_shared<tNetPack>();
    uint8_t*  s=newpacket->buffer;
    /*
     */
    *s=0x88;
    s++;
    *s=0;
    s++;
    newpacket->len=(s-newpacket->buffer);
    newpacket->connid = ConnectionId;

    addtimer(CLOSEWAIT_TIME, ConnectionId, (ConnectionId >> CONNECTIONID_BITS), CLOSEWAIT_TIMERID);
    return (newpacket);
}

std::shared_ptr<tNetPack> CWSJSONProtocol::PingFrame() {
    std::shared_ptr<tNetPack> newpacket = std::make_shared<tNetPack>();
    uint8_t*  s=newpacket->buffer;
    /*
     */
    *s=0x89;
    s++;
    *s=0;
    s++;
    newpacket->len=(s-newpacket->buffer);
    newpacket->connid = ConnectionId;

    return (newpacket);
}

std::shared_ptr<tNetPack> CWSJSONProtocol::PongFrame() {
    std::shared_ptr<tNetPack> newpacket = std::make_shared<tNetPack>();
    uint8_t*  s=newpacket->buffer;
    /*
     */
    *s=0x8A;
    s++;
    *s=message.payloadlen;
    s++;
    memcpy(s, wspayload, message.recv);
    newpacket->len=(s-newpacket->buffer);

    newpacket->connid = ConnectionId;

    return (newpacket);
}
