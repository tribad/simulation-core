// **************************************************************************
//
// Modul-Name        : socketthread
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 11.07.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#include <iostream>
#include <cstring>
#include <sys/epoll.h>

#include "socketthread.h"

#define MAX_SOCKET_COUNT (100)
namespace client {
    std::atomic<uint64_t>        CSocketThread::m_request_counter {0};
    CSocketThread::CSocketThread() {
        m_thread = std::thread([this]() {
                                   this->run();
                               }
        );

    }

    CSocketThread::~CSocketThread() {

    }

    long CSocketThread::run() {
        int pollcount = 0;
        int i;
        struct epoll_event poll_list;
        CSocket* connection;
        uint64_t connid;

        m_poll_handle = epoll_create(MAX_SOCKET_COUNT);
        m_running = true;
        for (; m_running;) {
            memset(&poll_list, 0, sizeof(poll_list));
            pollcount = epoll_wait(m_poll_handle, &poll_list, 1, 1000);
            if (pollcount > 0) {
                for (i = 0; i < pollcount; i++) {
                    //
                    //  Retrieve the connection id from the poll event structure.
                    //  We do not redirect through the handle.
                    connid = poll_list.data.u64;
                    //
                    //  We do not make a find on the connid but use the array operator instead.
                    //  This may create a new uninitialized element in the map. But we need to
                    //  check the pointer returned anyway. Defensive programming.
                    {
                        //
                        //  Access the map under lock to prevent corruption of the map.
                        std::lock_guard<std::mutex> m_lock(m_idmap_lock);
                        connection = m_idmap[connid];
                    }
                    if (connection) {
                        //
                        //  If we got an error or hangup event we break the loop after processing
                        //  the event.
                        if (poll_list.events & EPOLLERR) {
                            connection->on_close();
                            //std::cerr << "Close Event\n";
                            break;
                        }
                        if (poll_list.events & EPOLLHUP) {
                            connection->on_close();
                            //std::cerr << "Hangup Event\n";
                            break;
                        }
                        //
                        //  As multiple events may be reported we process the read first
                        if (poll_list.events & EPOLLIN) {
                            connection->on_read();
                            //std::cerr << "Read Event\n";
                        }
                        if (poll_list.events & EPOLLOUT) {
                            connection->on_write();
                            //std::cerr << "Write Event\n";
                        }
                    }
                }
            }
        }
        return 0;
    }

    int CSocketThread::attach(CSocket* a_socket) {
        int error = -1;
        struct epoll_event newpoll;

        std::lock_guard<std::mutex> m_lock(m_idmap_lock);

        if (m_idmap.find(a_socket->get_id()) == m_idmap.end()) {
            m_idmap.emplace(a_socket->get_id(), a_socket);
        }

        if (a_socket) {
            newpoll.events = EPOLLET | EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR;
            newpoll.data.u64 = a_socket->get_id();
            error = epoll_ctl(m_poll_handle, EPOLL_CTL_ADD, a_socket->get_handle(), &newpoll);
            if (error) {
                error = -errno;
                std::cerr << "Cannot set epoll for reading : " << strerror(errno) << std::endl;
            }
        } else {
        }

        return error;
    }

    int CSocketThread::detach(CSocket* a_socket) {
        int errorcode = 0;
        struct epoll_event newpoll;

        std::shared_ptr<CSocket> retval;
        std::lock_guard<std::mutex> m_lock(m_idmap_lock);
        auto item = m_idmap.find(a_socket->get_id());

        if (item != m_idmap.end()) {
            m_idmap.erase(a_socket->get_id());
            if (item->second) {
                errorcode = epoll_ctl(m_poll_handle, EPOLL_CTL_DEL, item->second->get_handle(), &newpoll);
                if (errorcode) {
                    if (errno == ENOENT) {
                        //
                        //  Its ok if the epoll does not have the socket for monitoring. Not nice but ok.
                    } else {
                        std::cerr << "Cannot remove socket from epoll : " << strerror(errno) << std::endl;
                    }
                }
            } else {
                errorcode = -1;
            }
        } else {
            errorcode = -1;
        }

        return errorcode;
    }


    uint64_t CSocketThread::store_request(std::promise<std::shared_ptr<tMsg>> a_promise) {
        std::unique_lock<std::mutex> lck(m_promise_lock);
        uint64_t request_id = ++m_request_counter;

        m_promise_map.emplace(request_id, std::move(a_promise));

        return request_id;
    }

    int CSocketThread::signal_request(std::shared_ptr<tMsg> a_msg) {
        std::unique_lock<std::mutex> lck(m_promise_lock);
        uint64_t reqid = a_msg->seq;

        auto p = m_promise_map.find(reqid);

        if (p != m_promise_map.end()) {
            p->second.set_value(a_msg);
            m_promise_map.erase(reqid);
        }
        return 0;
    }

    void CSocketThread::cleanup_requests() {
        std::unique_lock<std::mutex> lck(m_promise_lock);

        for (auto & p : m_promise_map) {
            p.second.set_value(nullptr);
        }
        m_promise_map.clear();
    }

}