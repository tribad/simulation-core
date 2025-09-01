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

#ifndef IDTOOBJECT_H
#define IDTOOBJECT_H

#include <pthread.h>
#include <stdint.h>
#include <map>
#include <list>
#include <simobj.h>

class CIdToObject {
public:
    CIdToObject();
    ~CIdToObject();
    bool Add(const uint64_t aId, tSimObj* aUser);
    void Remove(const uint64_t aId);
    tSimObj* Find(const uint64_t aId);
    std::list<tSimObj*> SubSet(const uint64_t aKey, uint64_t aStart, uint64_t aMaxCount);
    std::list<tSimObj*> SubSet(const uint64_t aFrom, uint64_t aTo, uint64_t aStart, uint64_t aMaxCount);
    uint64_t Size();
private:
    pthread_spinlock_t           MapLock;
    std::map<uint64_t, tSimObj*> ToObject;    
};


#endif
