// **************************************************************************
//
// Modul-Name        : tcpsocket
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 11.07.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#include "tcpsocket.h"

namespace client {
    CTcpSocket::CTcpSocket(std::string aIP, int aPort) : CSocket(aIP, aPort) {

    }

    void CTcpSocket::on_close() {

    }

    void CTcpSocket::on_read() {

    }

    void CTcpSocket::on_write() {

    }
}