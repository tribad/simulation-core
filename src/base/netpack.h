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

#ifndef NETPACK_H
#define NETPACK_H

#include <memory>

#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#ifdef DEBUG
#include <iostream>
#include <memory.h>
#endif
/*
 * This is the network package that has been received or should be send.
 * We use it for both interfaces, TCP and UDP and only for IPv4.
 */
class tNetPack {
public:
    tNetPack(size_t aSize=4096) {
        buffer = new uint8_t[size=aSize];
        len    = 0;
        port   = 0;
        host   = 0;
        connid = 0;
#ifdef DEBUG
        memset(buffer, 0, size);
        pthread_mutex_lock(&tNetPack::counterlock);
        tNetPack::counter++;
        pthread_mutex_unlock(&tNetPack::counterlock);
//        std::cerr << "Allocate Netpack with " << size << " bytes of buffer at " << this << std::endl;
#endif
    };
    ~tNetPack() {
            delete [] buffer;
#ifdef DEBUG
            buffer = 0;
            size   = 0;
            len    = 0;
            host   = 0;
            port   = 0;
            connid = 0;

            pthread_mutex_lock(&tNetPack::counterlock);
            tNetPack::counter--;
            pthread_mutex_unlock(&tNetPack::counterlock);
//            std::cerr << "Freed Netpack at " << this << std::endl;
#endif
        };
    static uint64_t        counter;
    static pthread_mutex_t counterlock;
    uint64_t connid;
    uint32_t host;
    int      port;
    size_t   size;
    uint8_t* buffer;
    size_t   len;
};

using NetPackPtr = std::shared_ptr<tNetPack>;

#endif
