// MIT License
//
// Copyright (c) 2020 Hans-Juergen Lange<hjl@simulated-universe.de>
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

// **************************************************************************
//
// Modul-Name        : websocket
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 11.07.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#include <iostream>
#include <sstream>
#include <map>

#include <cstring>
#include <ctime>
#include "socketthread.h"
#include "websocket.h"

#define MAX_WEBSOCKET_HEADER (16U)

namespace client {
/*
 * Function for a base64encode.
 */
#if 0
    static const char basis_64[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    static std::string base64encode(unsigned char *str, int len) {
        int i;
        std::string retval;

        for (i = 0; i < len - 2; i += 3) {
            retval.push_back(basis_64[(str[i] >> 2) & 0x3F]);
            retval.push_back(basis_64[((str[i] & 0x3) << 4) | ((int) (str[i + 1] & 0xF0) >> 4)]);
            retval.push_back(basis_64[((str[i + 1] & 0xF) << 2) | ((int) (str[i + 2] & 0xC0) >> 6)]);
            retval.push_back(basis_64[str[i + 2] & 0x3F]);
        }
        if (i < len) {
            retval.push_back(basis_64[(str[i] >> 2) & 0x3F]);
            if (i == (len - 1)) {
                retval.push_back(basis_64[((str[i] & 0x3) << 4)]);
                retval.push_back('=');
            } else {
                retval.push_back(basis_64[((str[i] & 0x3) << 4) | ((int) (str[i + 1] & 0xF0) >> 4)]);
                retval.push_back(basis_64[((str[i + 1] & 0xF) << 2)]);
            }
            retval.push_back('=');
        }

        return (retval);
    }

#endif

    CWebSocket::CWebSocket(std::string aIP, int aPort, std::string aURL, std::string aProtocol) : CTcpSocket(aIP, aPort) {
        mURL = aURL;
        mProtocol = aProtocol;
    }

    CWebSocket::~CWebSocket() {
        if (m_socket_buffer) {
            delete[] m_socket_buffer;
            m_socket_buffer = nullptr;
        }
    }


    void CWebSocket::on_close() {
        //
        //  Reset the state of the socket early.
        //  This is needed as the callback later may start a reconnect.
        //  On the timeline the reconnect must happen after the state of the internal
        //  variables has been reset.
        m_state    = State::init;
        m_writable = false;
        CSocketThread::instance().detach(this);
        //
        // Do the cleanup of the socket implementation internals.
        CTcpSocket::close();
        if (m_socket_buffer) {
            delete[] m_socket_buffer;
            m_socket_buffer = nullptr;
            m_socket_buffer_size = 0;
            if (m_on_close) {
                m_on_close(-1);
            }
        }
    }

    void CWebSocket::on_read() {
        int error_code = 0;
        //
        //  Read as much as possible from the socket. As the amount of data in a socket
        //  is system level limited we requested the limit after connecting to the server
        //  and always try to retrieve all data from the sockets internal buffer.
        error_code = CTcpSocket::Receive(m_socket_buffer, m_socket_buffer_size);
        //
        //  Check for graceful shutdown.
        if (error_code == 0) {
            m_writable = false;
            on_close();
        } else if ((error_code == -1) && (errno != EWOULDBLOCK) && (errno != EAGAIN)) {
            //
            //  Some error occured. Report it and close the communication.
            std::cerr << "Error on reading the socket: " << strerror(errno) << std::endl;
        } else {
            //
            //  error_code now contains the number of bytes read.
            //
            //  Setup the start of for later parsing the content of the buffer.
            //  The buffer may contain more than a single message/response.
            char *s = m_socket_buffer;
            //
            //  We process the buffer until a processing method returns a nullptr
            //  indicating the processing of the complete buffer.
            while (s != nullptr) {
                //
                //  Do processing the buffer content
                //  What we do if we get a read event depends on the state
                switch (m_state) {
                    case State::init:
                        //
                        //  We await the HTTP response on our request to switch protocols.
                        s = process_http_response(s, error_code);
                        break;
                    case State::running:
                        s = process_ws_message(s, error_code);
                        break;
                    default:
                        std::cerr << "Some weird state has been reached in the WebSocket communication:"
                                  << (uint16_t) m_state
                                  << std::endl;
                        break;
                }
            }
        }
    }

    void CWebSocket::on_write() {
        //
        //  Its always to that we are writable now.
        //std::cerr << "on_write: Writeable : " << m_writable << std::endl;
        m_writable = true;
        //
        // All processing is done under lock to prevent data corruption.
        std::lock_guard<std::mutex> lck(m_q_lock);
        do {
            //
            //  Check if we have any left-overs to process.
            if (!m_to_send.first) {
                if (!m_queue.empty()) {
                    m_to_send = m_queue.front();
                    m_queue.pop_front();
                }
            }
            if (m_to_send.first) {
                //
                //  If there is any data to send the m_to_send contains the needed info.
                //  We can now try to send data through the socket.
                int success;

                success = CTcpSocket::Send(m_to_send.first + m_already_send, m_to_send.second - m_already_send);
                //
                //  Graceful shutdown of the socket.
                if (success == 0) {
                    on_close();
                    m_queue.clear();
                    m_writable = false;
                } else if (success == -1) {
                    if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
                        //
                        //  We are not writable anymore.
                        m_writable = false;
                        //
                        //  break out of the queue checking loop.
                        break;
                    } else {
                        //
                        //  Any other error is treated as real error.
                        //  So we cleanup the communication.
                        on_close();
                        m_queue.clear();
                        m_writable = false;
                    }
                } else {
                    //
                    //  Some positive amount of data has been send. Check if all got out.
                    if (success < (int)(m_to_send.second - m_already_send)) {
                        //
                        // We need to know how much has been send.
                        m_already_send += success;
                        //
                        //  We are not writable anymore.
                        m_writable = false;
                        //
                        //
                    } else {
                        //
                        //  All gone. Cleanup m_to_send
                        delete[] m_to_send.first;
                        m_to_send.first = nullptr;
                        m_to_send.second = 0;
                    }
                }
            }
        } while (!m_queue.empty());
    }

    int CWebSocket::connect(std::string aIP, int aPort, std::string aURL, std::string aProtocol) {
        mURL = aURL;
        mProtocol = aProtocol;

        int error_code = CTcpSocket::connect(aIP, aPort);

        if (!error_code) {
            //
            //  If the connect succeeded we ask for the internal socket buffer size.
            //  On reading we always read all data available.
            m_socket_buffer_size = CTcpSocket::get_socket_size();
            if ((m_socket_buffer_size > 0) && (m_socket_buffer == nullptr)) {
                m_socket_buffer = new char[m_socket_buffer_size];
            }
            //
            //  To make epoll work correctly.
            CTcpSocket::SetAsync();

            CSocketThread::instance().attach(this);
            //
            // We send out the HTTP request for switching the protocol to WebSockets.
            switch_protocol_req();
        }
        return error_code;
    }


    bool CWebSocket::connect() {
        bool success = false;

        auto error_code = CTcpSocket::connect();

        if (!error_code) {
            //
            //  If the connect succeeded we ask for the internal socket buffer size.
            //  On reading we always read all data available.
            m_socket_buffer_size = CTcpSocket::get_socket_size();
            if ((m_socket_buffer_size > 0) && (m_socket_buffer == nullptr)) {
                m_socket_buffer = new char[m_socket_buffer_size];
            }
            //
            //  To make epoll work correctly.
            CTcpSocket::SetAsync();

            CSocketThread::instance().attach(this);
            //
            // We send out the HTTP request for switching the protocol to WebSockets.
            switch_protocol_req();

            success = true;
        }
        return success;
    }

    void CWebSocket::close() {

    }
    int CWebSocket::send(const char *a_buffer, size_t a_size) {
        int success = -1;
        char *package = nullptr;
        size_t package_size = 0U;
        //std::cerr << "In send. Writeable : " << m_writable << std::endl;
        //
        //  The on_write event is signaling that sending data is possible now.
        //  This means that we need to wait until the first on_write is called
        //  before we can start to send any data out.
        //
        //  What we do if we get a write event depends on the state
        if (m_state == State::running) {
            package = new char[a_size + MAX_WEBSOCKET_HEADER];
            client::tWSMessage message;

            message.mPayloadLen = a_size;
            message.mMask = true;
            message.mOpCode = 0x1;
            message.mFin = true;
            message.mPayload = (char *) a_buffer;
            time((time_t *) &message.mMaskKey);
            char *endptr = message.pack(package);

            package_size = endptr - package;
        } else {
            package = new char[a_size];
            package_size = a_size;
            memcpy(package, a_buffer, a_size);
        }
        //
        //  Everything done here is done under lock.
        //  As we will have multiple threads running it prevents
        //  Memory corruption and any other sort of problems.
        std::lock_guard<std::mutex> lck(m_q_lock);
        //
        //  First check if the socket is writable or anything else is
        //  already waiting in the send queue.
        //
        //  m_to_send always contains the buffer and size of the data that are going to be send.
        //  So if m_to_send does not contain a buffer, we can directly send out our buffer, or
        //  at least try it.
        //  On the other hand if a buffer is set in m_to_send we need to queue in our data for
        //  sending it later.
        if ((!m_writable) || (m_to_send.first)) {
            //
            //  There is no limit in the number of elements in this list.
            //  This poses the risk of out-of-memory conditions.
            //  But as this code is only intended for low-speed communication
            //  I take the risk
            m_queue.push_back(std::pair<char *, size_t>(package, package_size));
            success = 0;
        } else {
            //
            //  We can try to send our data directly through the socket.
            success = CTcpSocket::Send(package, package_size);
            //
            //  Graceful shutdown of the socket.
            if (success == 0) {
                success = -EBADF;
                m_writable = false;
                on_close();
            } else if (success == -1) {
                if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
                    //
                    //  No send at all. So we store the buffer and size in m_to_send
                    //  As we are in the direct send path we know that the m_to_send is empty.
                    m_already_send = 0;
                    m_to_send.first = package;
                    m_to_send.second = package_size;
                    //
                    //  We are not writable anymore.
                    m_writable = false;
                    //
                    //
                    success = -EWOULDBLOCK;
                } else {
                    //
                    //  Any other error is treated as real error.
                    //  So we cleanup the communication.
                    on_close();
                    m_queue.clear();
                    m_writable = false;
                    success = -errno;
                }
            } else {
                //
                //  Some positive amount of data has been send. Check if all got out.
                if (success < (int)package_size) {
                    //
                    // We need to know how much has been send.
                    m_already_send = success;
                    //
                    //  Partial send. Calculate the rest
                    m_to_send.first = package;
                    m_to_send.second = package_size;
                    //
                    //  We are not writable anymore.
                    m_writable = false;
                    //
                    //
                    success = -EWOULDBLOCK;
                } else {
                    //
                    //  All gone. Cleanup package pointer
                    delete[] package;
                    success = 0;
                }
            }
        }
        return success;
    }

    void CWebSocket::switch_protocol_req() {
        std::ostringstream oss;
        oss << "GET " << mURL << " HTTP/1.1\r\n"
                                 "Host: " << mIP << "\r\n"
                                                    "Upgrade: websocket\r\n"
                                                    "Connection: Upgrade\r\n"
                                                    "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                                                    "Origin: " << mIP << "\r\n"
                                                                         "Sec-WebSocket-Protocol: " << mProtocol
            << "\r\n"
               "Sec-WebSocket-Version: 13\r\n\r\n";
        auto error_code = send(oss.str().c_str(), oss.str().size());
        if (error_code) {
            std::cerr << "Error while sending the protocol switch request." << strerror(errno) << std::endl;
        }
    }

    char *CWebSocket::process_http_response(char *a_buffer, int a_size) {
        char *pos = a_buffer;

        pos = m_http_response.process(a_buffer, a_size);
        //
        //  Check the state. State 6 means we are done with the response.
        if (m_http_response.get_state() == 6) {
            //
            //  Check the response and switch the connection state.
            if (m_http_response.response_code() == "101") {
                m_state = State::running;
                if (m_on_connect) {
                    m_on_connect(true);
                }
            } else {
                if (m_on_connect) {
                    m_on_connect(false);
                }

            }

        } else if (m_http_response.get_state() == -1) {
            //
            //  We got an error and are closing the connection.
            on_close();
        } else {
            //
            // Do nothing and wait for more data.
        }
        return pos;
    }

    char *CWebSocket::process_ws_message(char *a_buffer, int a_size) {
        char *pos = a_buffer;

        int done_bytes = m_message.Process(a_buffer, a_size);

        if (m_message.mDone) {
            switch (m_message.mOpCode) {
                case 9: // Ping frame
                    send_pong();
                    break;
                case 1: // Text frame
                    if (m_on_receive) {
                        //
                        //  Creating a string object and initialize with the payload.
                        std::string payload((char *) m_message.mPayload, m_message.mPayloadLen);
                        m_on_receive(std::move(payload));
                    }
                    break;
                default:
                    break;
            }
        }
        //
        //  It is expected that the processing of the ws message has been completed.
        //  We now reset the internals to allow receiving the next message.
        m_message.Reset();
        //
        //  Update the position for further message processing.
        pos += done_bytes;
        if (done_bytes >= a_size) {
            pos = nullptr;
        }
        return pos;
    }

    void CWebSocket::send_pong() {
        client::tWSMessage pong_frame;

        pong_frame.mFin = true;
        pong_frame.mOpCode = 0xa;
        pong_frame.mPayloadLen = 0;
        pong_frame.mMask = true;
        //
        //  Create some random number.
        pong_frame.mMask = time(nullptr);

        char pong_buffer[16];  // Best guess.
        memset(pong_buffer, 0, sizeof(pong_buffer));

        char *s = pong_frame.pack(pong_buffer);

        CTcpSocket::Send(pong_buffer, s - pong_buffer);
    }

}
