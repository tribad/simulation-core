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

#ifndef CWEBSOCKET_H
#define CWEBSOCKET_H

#include <vector>
#include <string>
#include <list>
#include <mutex>
#include <functional>
#include <tcpsocket.h>
#include "wsmessage.h"
#include "http-proto-response.h"

namespace client {

    class CWebSocket : public CTcpSocket {
    public:
        CWebSocket() = default;
        CWebSocket(std::string aIP, int aPort, std::string aURL, std::string aProtocol = "");
        virtual ~CWebSocket();
        void on_close() override;
        void on_read() override;
        void on_write() override;
        //
        // This is the interface for the application
        void on_close(std::function<void(int)> a_close_fnc) { m_on_close = a_close_fnc; };
        void on_message(std::function<void(std::string &&)> a_message_fnc) { m_on_receive = a_message_fnc; };
        void on_connect(std::function<void(bool)> a_connect_fnc) { m_on_connect = a_connect_fnc; }
        bool connect();
        int connect(std::string aIP, int aPort, std::string aURL, std::string aProtocol = "");
        void close();
        int send(const char *a_buffer, size_t a_size);
    private:
        void switch_protocol_req();
        char *process_http_response(char *a_buffer, int a_size);
        char *process_ws_message(char *a_buffer, int a_size);
        void send_pong();
    private:
        enum class State : uint16_t {
            init,
            running
        };
        std::string                          mURL;
        std::string                          mProtocol;
        client::tWSMessage                   m_message;
        State                                m_state = State::init;
        std::mutex                           m_q_lock;
        std::list<std::pair<char *, size_t>> m_queue;
        std::pair<char *, size_t>            m_to_send{0, 0};
        int                                  m_already_send = 0;
        size_t                               m_socket_buffer_size;
        char*                                m_socket_buffer = nullptr;
        HttpProtoResponse                    m_http_response;
        std::function<void(std::string &&)>  m_on_receive;
        std::function<void(int)>             m_on_close;
        std::function<void(bool)>            m_on_connect;
    };

}

#endif // CWEBSOCKET_H
