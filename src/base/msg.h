// MIT License
//
// Copyright (c) 2014 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#ifndef MSG_H
#define MSG_H

#define MSG_TYPE_UNSPECIFIED (0x0000000000000000)
#define MSG_TYPE_REQUEST     (0x0000000000000001)
#define MSG_TYPE_REPLY       (0x0000000000000002)
#define MSG_TYPE_INDICATION  (0x0000000000000004)
#define MSG_TYPE_CONFIRM     (0x0000000000000008)
#define MSG_TYPE_INTERNAL    (0x1000000000000000)
#define MSG_TYPE_SIGNAL      (0x2000000000000000)

#include <iostream>
#include <sstream>
#include <json.h>
#include <memory>
#include <commtarget.h>
#include <atomic>

#ifndef INVALID_OBJECTID
#define INVALID_OBJECTID UINT64_MAX
#endif

enum class SerializerType : uint8_t {
    eNone,
    eJSON,
    eBinary
};

/*
 * The message type.
 */
struct tMsg {
    tMsg(uint64_t aId = 0u, uint64_t aType = MSG_TYPE_UNSPECIFIED) {
        id = aId;
        type = aType;
        if (aType & MSG_TYPE_REQUEST) {
            ++gRequestSequence;
        } else if (aType & MSG_TYPE_INDICATION) {
            ++gIndicationSequence;
        }
        if (seq == 0) {
            seq = ++gMsgCounter;
        }
    }
    tMsg(tJSON* aJson) {
        id = 0;
        type = MSG_TYPE_INTERNAL;
        (void)aJson;
    }
    virtual ~tMsg() {}
    virtual std::ostringstream& pack_json() {
        return oss;
    }
    virtual std::ostream& pack_json(std::ostream& a_stream) {
        return a_stream;
    }

    virtual void unpack_json(tJSON* a_json) {
        (void)a_json;
    };

    virtual char* pack(char* ptr, SerializerType type = SerializerType::eJSON) {
        switch (type) {
            case SerializerType::eJSON:
                break;
            default:
                break;
        }
        return ptr;
    };
    virtual char* unpack(char* ptr, SerializerType type = SerializerType::eJSON) {
        switch (type) {
            case SerializerType::eJSON:
                break;
            default:
                break;
        }
        return ptr;
    };
    virtual size_t size(SerializerType type = SerializerType::eJSON) {
        switch (type) {
            case SerializerType::eJSON:
                break;
            default:
                break;
        }
        return 0;
    };

public:  //
    uint64_t                     id;
    uint64_t                     type;
    tCommTarget                  src;
    tCommTarget                  dst;
    uint64_t                     seq = 0UL;
    std::ostringstream           oss;
    static std::atomic<uint64_t> gRequestSequence;
    static std::atomic<uint64_t> gIndicationSequence;
    static std::atomic<uint64_t> gMsgCounter;
};

using tSig = tMsg;

using tMsgPtr = std::shared_ptr<tMsg>;
using tSigPtr = std::shared_ptr<tSig>;

#define newPtr(msgclass) std::make_shared<msgclass>()

#endif // MSG_H
