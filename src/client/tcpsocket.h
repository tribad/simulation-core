// **************************************************************************
//
// Modul-Name        : tcpsocket
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 11.07.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef CTCPSOCKET_H
#define CTCPSOCKET_H

#include <socket.h>

namespace client {
    class CTcpSocket : public CSocket {
    public:
        CTcpSocket() = default;
        CTcpSocket(std::string aIP, int aPort);
        virtual ~CTcpSocket() = default;
        void on_close() override;
        void on_read() override;
        void on_write() override;
    };
}

#endif // CTCPSOCKET_H
