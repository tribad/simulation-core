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

#ifndef LOCKEDLIST_H
#define LOCKEDLIST_H

#include <pthread.h>
#include <cstdint>

#include <list>

template <class I>
class LockedList {
    pthread_spinlock_t __lock;
    std::list<I>       __list;
public:
    LockedList() {pthread_spin_init(&__lock, PTHREAD_PROCESS_PRIVATE);}
    ~LockedList() {pthread_spin_destroy(&__lokc);}
    void Append(const I& aItem) {

        pthread_spin_lock(&__lock);
        __list.push_back(aItem);
        pthread_spin_unlock(&__lock);
    }

    void Prepend(const I& aItem) {

        pthread_spin_lock(&__lock);
        __list.push_front(aItem);
        pthread_spin_unlock(&__lock);
    }

    uint64_t Size() {
        uint64_t retval = 0ul;

        pthread_spin_lock(&__lock);
        retval = __list.size();
        pthread_spin_unlock(&__lock);
        return retval;
    }

    std::list<K> &Get() {return __list;}
    void Lock() {pthread_spin_lock(&__lock);}
    void Unlock() {pthread_spin_unlock(&__lock);}

};

#endif //  LOCKEDLIST_H
