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

#ifndef LOCKEDVECTOR_H
#define LOCKEDVECTOR_H

#include <pthread.h>
#include <cstdint>

#include <stdexcept>
#include <vector>

template <class I>
class LockedVector {
    pthread_spinlock_t __lock;
    std::vector<I>     __vector;
public:
    LockedVector() {pthread_spin_init(&__lock, PTHREAD_PROCESS_PRIVATE);}
    ~LockedVector() {pthread_spin_destroy(&__lock);}
    void Append(const I& aItem) {

        pthread_spin_lock(&__lock);
        __vector.push_back(aItem);
        pthread_spin_unlock(&__lock);
    }

    void Prepend(const I& aItem) {

        pthread_spin_lock(&__lock);
        __vector.push_front(aItem);
        pthread_spin_unlock(&__lock);
    }

    uint64_t Size() {
        uint64_t retval = 0ul;

        pthread_spin_lock(&__lock);
        retval = __vector.size();
        pthread_spin_unlock(&__lock);
        return retval;
    }

    I& operator[](size_t pos) {
        I retval;

        pthread_spin_lock(&__lock);
        if ((pos >= __vector.size()) || (__vector.size() == 0)) {
            throw std::out_of_range();
        }
        retval = __vector[pos];

        pthread_spin_unlock(&__lock);

        return retval;

    }

    std::vector<I> &Get() {return __vector;}
    void Lock() {pthread_spin_lock(&__lock);}
    void Unlock() {pthread_spin_unlock(&__lock);}

};

#endif //  LOCKEDVECTOR_H
