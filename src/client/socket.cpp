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
// Modul-Name        : socket
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 11.07.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#include <iostream>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "socket.h"

#include <cstring>

namespace client {

    std::atomic<uint64_t> CSocket::m_lastid {0};

    CSocket::CSocket(std::string aIP, int aPort, int aType) {
        m_lastid += 1;
        m_id     = m_lastid;
        mIP      = aIP;
        mPort    = aPort;
        mType    = aType;
        m_handle = -1;
    }

    CSocket::~CSocket() {
        close();
    }

    void CSocket::on_close() {

    }

    void CSocket::on_read() {

    }

    void CSocket::on_write() {

    }

    int CSocket::connect(std::string aIP, int aPort, int aType) {
        m_lastid += 1;
        m_id     = m_lastid;
        mIP      = aIP;
        mPort    = aPort;
        mType    = aType;
        m_handle = -1;
        return connect();
    }

    int CSocket::connect() {
        int retval = -1;
        //
        //  If we have already a socket here we close it before we
        //  re-initiate the connection.
        if (m_handle == -1) {
            //
            //  Create a new socket.
            m_handle = socket(AF_INET, mType, 0);
        } else {
            std::cerr << "Already have a handle. Using existing socket handle.\n";
        }
        //
        //  Check for errors and complain.
        if (m_handle != -1) {
            sockaddr sAddress;
            struct sockaddr_in *inaddr;
            struct hostent *host;

            host = gethostbyname(mIP.c_str());

            inaddr = (struct sockaddr_in *) (&sAddress);

            if ((host != nullptr) && (host->h_length == 4)) {
                memcpy(&(inaddr->sin_addr), host->h_addr_list[0], host->h_length);
                inaddr->sin_port = htons(mPort);
                inaddr->sin_family = PF_INET;

                retval = ::connect(m_handle, &sAddress, sizeof(sAddress));
                //
                //  As this is a synchronous call we should always come up with an ok or error.
                if (retval) {
                    retval = -errno;
                    std::cerr << "Cannot connect to host : " << mIP << ":" << mPort << ":" << strerror(errno)
                              << std::endl;
                }
            } else {
            }
        } else {
            std::cerr << "Cannot create socket : " << strerror(errno) << std::endl;
            retval = -errno;
        }
        return retval;
    }

    int CSocket::shutdown() {
        int retval = -1;

        retval = ::shutdown(m_handle, SHUT_RDWR);

        return retval;
    }

    int CSocket::close() {
        int retval = -1;

        if (m_handle != -1) {
            retval = ::close(m_handle);
            m_handle = -1;
        }
        return retval;
    }

    int CSocket::Send(const char *aBuffer) {
        int retval = -1;

        if (m_handle != -1) {
            size_t len = strlen(aBuffer);

            retval = send(m_handle, aBuffer, len, 0);
        }
        return retval;
    }

    int CSocket::Send(const char *aBuffer, size_t aSize) {
        int retval = -1;

        if (m_handle != -1) {
            retval = send(m_handle, aBuffer, aSize, 0);
        }

        return retval;
    }

    int CSocket::SendTo(const char *aBuffer, size_t aSize, std::string aIP, int aPort) {
        int retval = -1;

        if (m_handle != -1) {
            sockaddr sAddress;
            struct sockaddr_in *inaddr;
            struct hostent *host;

            host = gethostbyname(aIP.c_str());

            inaddr = (struct sockaddr_in *) (&sAddress);

            if ((host != nullptr) && (host->h_length == 4)) {
                memcpy(&(inaddr->sin_addr), host->h_addr_list[0], host->h_length);
                inaddr->sin_port = htons(aPort);
                inaddr->sin_family = PF_INET;

                retval = sendto(m_handle, aBuffer, aSize, 0, &sAddress, sizeof(sAddress));
            } else {
            }
        }

        return retval;
    }

    int CSocket::Receive(char *aBuffer, size_t aSize) {
        int retval = -1;

        if (m_handle != -1) {
            retval = recv(m_handle, aBuffer, aSize, 0);
        }

        return retval;
    }

    int CSocket::ReceiveFrom(char *aBuffer, size_t aSize, std::string &aIP, int &aPort) {
        int retval = -1;

        if (m_handle != -1) {
            sockaddr sAddress;
            socklen_t sAddressLen = sizeof(sAddress);
            char sAddressBuffer[128];

            retval = recvfrom(m_handle, aBuffer, aSize, 0, &sAddress, &sAddressLen);

            const char *s = inet_ntop(AF_INET, &sAddress, sAddressBuffer, sizeof(sAddressBuffer));

            if (s != nullptr) {
                struct sockaddr_in *inaddr = (struct sockaddr_in *) (&sAddress);

                aIP = s;
                aPort = ntohs(inaddr->sin_port);
            } else {
                aIP.clear();
                aPort = -1;
            }
        }

        return retval;
    }

    int CSocket::SetSignal(int aSignal) {
        int err = -1;
        int optval;

        optval = fcntl(m_handle, F_GETFL);
        /*
         * Check if getting the flags succeeded.
         */
        if (optval != -1) {
            /*
             * Setting additional flags.
             */
            err = fcntl(m_handle, F_SETFL, optval | O_ASYNC | O_NONBLOCK | O_RDWR);
            if (err == 0) {
                /*
                 * Set the owning process of the handle so that the thread may get the signals.
                 */
                err = fcntl(m_handle, F_SETOWN, getpid());
                if (err == 0) {
                    /*
                     * Set the signal to use on socket events.
                     */
                    err = fcntl(m_handle, F_SETSIG, aSignal);
                }
            }
        }

        return err;
    }

    int CSocket::SetAsync() {
        int err = -1;
        int optval;

        optval = fcntl(m_handle, F_GETFL);
        /*
         * Check if getting the flags succeeded.
         */
        if (optval != -1) {
            /*
             * Setting additional flags.
             */
            err = fcntl(m_handle, F_SETFL, optval | O_RDWR | O_ASYNC | O_NONBLOCK);
        }
        return err;
    }

    int CSocket::SetSync() {
        int err = -1;
        int optval;

        optval = fcntl(m_handle, F_GETFL);
        /*
         * Check if getting the flags succeeded.
         */
        if (optval != -1) {
            /*
             * Setting additional flags.
             */
            err = fcntl(m_handle, F_SETFL, (optval | O_RDWR) & ~(O_ASYNC | O_NONBLOCK));
        }
        return err;
    }

    size_t CSocket::get_socket_size() {
        int buffer_size;

        socklen_t m = sizeof(buffer_size);
        int error_code = getsockopt(m_handle, SOL_SOCKET, SO_RCVBUF, (void *) &buffer_size, &m);
        if (error_code) {
            buffer_size = 0U;
            std::cerr << "Cannot get socket buffer size: " << strerror(errno) << std::endl;
        }
        return buffer_size;
    }

}