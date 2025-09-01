// MIT License
//
// Copyright (c) 2024 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#include <iostream>

#include "socketthread.h"
#include "websocket.h"
#include <cstring>

int connectionstate = 0;

int main() {
    client::CWebSocket ws("127.0.0.1", 8082, "/",  "daemon2mgr");

    ws.on_close([](int a_error) {
        connectionstate = 1;
        std::cerr << "Closed : " << a_error << std::endl;
    }
    );

    ws.on_connect([](bool a_done) {
                     connectionstate = 2;
                     std::cerr << "Connected : " << a_done << std::endl;
                 }
    );

    ws.on_message([](std::string&& a_message) {
                           std::cerr << "Got Message : " << a_message << std::endl;
                       }
    );

    ws.connect();

    int runcounter = 0;
    for (;runcounter < 50000;) {
        sleep(2);
        if (connectionstate == 1) {
            ws.connect();
        } else if (connectionstate == 2) {
            const char* msg = "{\"MsgId\" : \"GetAllProjectsReq\"}";
            ws.send(msg, strlen(msg));
        }
        runcounter++;
    }
    client::CSocketThread::instance().stop();
    sleep(2);
    return 0;
}
