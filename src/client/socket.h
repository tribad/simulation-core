// **************************************************************************
//
// Modul-Name        : socket
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 11.07.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef CSOCKET_H
#define CSOCKET_H

#include <atomic>

#include <sys/socket.h>
#include <unistd.h>
#include <string>

namespace client {

    class CSocket {
    public:
        CSocket() = default;
        CSocket(std::string aIP, int aPort, int aType = SOCK_STREAM);
        CSocket(const CSocket &a_other) = delete;
        const CSocket &operator=(const CSocket &a_other) = delete;
        virtual ~CSocket();
        virtual void on_close();
        virtual void on_read();
        virtual void on_write();
        int connect();
        int connect(std::string aIP, int aPort, int aType = SOCK_STREAM);
        int shutdown();
        int close();
        int Send(const char *aBuffer);
        int Send(const char *aBuffer, size_t aSize);
        int SendTo(const char *aBuffer, size_t aSize, std::string aIP, int aPort);
        int Receive(char *aBuffer, size_t aSize);
        int ReceiveFrom(char *aBuffer, size_t aSize, std::string &aIP, int &aPort);
        bool isConnected() { return m_handle != -1; }
        int SetSignal(int signal);
        int SetAsync();
        int SetSync();
        size_t get_socket_size();
        uint64_t get_id() { return (m_id); }
        int get_handle() { return m_handle; }
    protected:
        static std::atomic<uint64_t> m_lastid;
        bool                         m_writable = false;
        std::string                  mIP;
        int                          mPort = 0;
        int                          m_handle = -1;
        int                          mType = SOCK_STREAM;
        uint64_t                     m_id;
    };

}

#endif // CSOCKET_H
