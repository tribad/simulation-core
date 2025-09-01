// MIT License
//
// Copyright (c) 2017 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#ifndef COMMTARGET_H
#define COMMTARGET_H

#include <cstdint>
#include <cstring>

#include <variant>

class tObjectRef;
class tSimObj;
class CSharedMsgQueue;

enum class eCommTarget {
    Connection,
    Object,
    Node,
    DB,
    Queue,
    Core,
    Http,
    Unknown
};

enum class eCommFormat : uint16_t {
    JSON,
    Binary,
    Raw,        //  Message/Signal class
    Unknown
};

struct tConnection {
    tConnection(uint64_t a_id,  eCommFormat a_format, uint64_t a_seq = 0) : format(a_format), id(a_id), seqnumber(a_seq) {};
    uint32_t    host = 0u;
    uint16_t    port = 0u;
    eCommFormat format;
    uint64_t    id;
    uint64_t    seqnumber;
};

struct tNode {
    uint64_t nodeid;
};

struct tQueue {
    CSharedMsgQueue* q;
    CSharedMsgQueue* operator->() {return q;}
};

struct tDBQueue {
    CSharedMsgQueue* q;
    CSharedMsgQueue* operator->() {return q;}
};

struct tCoreQueue {
    tCoreQueue(CSharedMsgQueue* a_q) : q(a_q) {}
    CSharedMsgQueue* q;
    CSharedMsgQueue* operator->() {return q;}
};

struct tReference {
    tReference(uint64_t a_id, void* a_ptr = nullptr) : m_id(a_id), m_ptr(a_ptr) {}
    uint64_t m_id = 0;
    void*    m_ptr = nullptr;
};

using tCommTarget = std::variant<std::monostate, tConnection, tNode, tQueue, tDBQueue, tCoreQueue, tReference>;

#if 0

struct tCommTarget {
    tCommTarget(const tCommTarget& other) {
        switch (other.type) {
        case eCommTarget::Connection:
            conn = other.conn;
            break;
        case eCommTarget::Object:
            obj  = other.obj;
            break;
        case eCommTarget::Node:
            node = other.node;
            break;
        case eCommTarget::Core:
        case eCommTarget::DB:
        case eCommTarget::Http:
        case eCommTarget::Queue:
            memcpy(&queue, &other.queue, sizeof(tCommTarget));
            break;
        default:
            break;
        }
        type = other.type;
    }
    tCommTarget& operator=(const tCommTarget& other) {
        switch (other.type) {
        case eCommTarget::Connection:
            conn = other.conn;
            break;
        case eCommTarget::Object:
            obj  = other.obj;
            break;
        case eCommTarget::Node:
            node = other.node;
            break;
        case eCommTarget::Core:
        case eCommTarget::DB:
        case eCommTarget::Http:
        case eCommTarget::Queue:
            memcpy(&queue, &other.queue, sizeof(tCommTarget));
            break;
        default:
            break;
        }
        type = other.type;
        return *this;
    }
    tCommTarget() {type = eCommTarget::Unknown;}
    tCommTarget(const tObjectRef& target);
    tCommTarget(uint64_t aId, tSimObj* aPtr);
    ~tCommTarget() {
        switch (type) {
        case eCommTarget::Connection:
            conn.~tConnection();
            break;
        case eCommTarget::Object:
            obj.~tReference();
            break;
        case eCommTarget::Node:
            node.~tNode();
            break;
        case eCommTarget::Core:
        case eCommTarget::DB:
        case eCommTarget::Http:
        case eCommTarget::Queue:
            queue.~tQueue();
            break;
        default:
            break;
        }
    }
    eCommTarget type;
    union {
        tConnection conn;
        tReference  obj;
        tNode       node;
        tQueue      queue;
    };
    static tCommTarget mainobj;
};

#endif

#endif
