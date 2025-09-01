// **************************************************************************
//
// Modul-Name        : socketthread
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 11.07.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef CSOCKETTHREAD_H
#define CSOCKETTHREAD_H

#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <future>
#include "socket.h"
#include "msg.h"

namespace client {

    class CSocketThread {
    public:
        CSocketThread();
        virtual ~CSocketThread();
        virtual long run(void);
        void stop() {
            m_running = false;
            m_thread.join();
        }
        virtual int attach(CSocket* a_socket);
        virtual int detach(CSocket* a_socket);
        //
        //
        bool isRunning() { return m_running; }
        uint64_t store_request(std::promise<std::shared_ptr<tMsg>> a_promise);
        int signal_request(std::shared_ptr<tMsg>);
        void cleanup_requests();
        //
        //  This is a good way to have a singleton pattern implemented in C++. I think.
        static CSocketThread &instance() {
            static CSocketThread s;

            while (!s.isRunning());
            return s;
        }
    protected:
        static std::atomic<uint64_t>  m_request_counter;
        std::thread                   m_thread;
        bool                          m_running = false;
        std::map<uint64_t, CSocket*>  m_idmap;
        std::mutex                    m_idmap_lock;
        int                           m_poll_handle;
        std::mutex                    m_promise_lock;
        std::map<uint64_t, std::promise<std::shared_ptr<tMsg>>> m_promise_map;
    };

}

#endif // CSOCKETTHREAD_H
