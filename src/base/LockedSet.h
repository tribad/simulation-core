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

#ifndef LOCKEDSET_H
#define LOCKEDSET_H

#include <pthread.h>
#include <cstdint>

#include <set>

template <class K>
class LockedSet {
    pthread_spinlock_t __lock;
    std::set<K>        __set;
public:
    LockedSet() {pthread_spin_init(&__lock, PTHREAD_PROCESS_PRIVATE);}
    ~LockedSet() {pthread_spin_destroy(&__lock);}
    bool Add(const K& aKey) {
        bool retval = false;

        pthread_spin_lock(&__lock);
        auto i = __set.find(aKey);

        if (i == __set.end()) {
            retval = true;
            __set.insert(aKey);
        }
        pthread_spin_unlock(&__lock);
        return retval;
    }
    void Remove(const K& aKey) {
        pthread_spin_lock(&__lock);
        __set.erase(aKey);
        pthread_spin_unlock(&__lock);
    }
    bool Check(const K& aKey) {
        bool retval;

        pthread_spin_lock(&__lock);
        auto i = __set.find(aKey);

        retval = (i != __set.end());

        pthread_spin_unlock(&__lock);
        return retval;
    }
    uint64_t Size() {
        uint64_t retval = 0ul;

        pthread_spin_lock(&__lock);
        retval = __set.size();
        pthread_spin_unlock(&__lock);
        return retval;
    }


    std::set<K> &Get() {return __set;}
    void Lock() {pthread_spin_lock(&__lock);}
    void Unlock() {pthread_spin_unlock(&__lock);}
};

#endif //  LOCKEDSET_H
