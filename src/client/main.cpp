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
