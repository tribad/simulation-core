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

#include "CIdToObject.h"

CIdToObject::CIdToObject() {
   pthread_spin_init(&MapLock, PTHREAD_PROCESS_PRIVATE);
}

CIdToObject::~CIdToObject() {
}

bool CIdToObject::Add(const uint64_t aId, tSimObj* aUser) {
    bool retval = false;
    std::map<uint64_t, tSimObj*>::iterator ui;

    pthread_spin_lock(&MapLock);
    ui = ToObject.find(aId);
    if (ui == ToObject.end()) {
        retval = true;
        ToObject.insert(std::pair<uint64_t, tSimObj*>(aId, aUser));
    }
    pthread_spin_unlock(&MapLock);

    return retval;
}

void CIdToObject::Remove(const uint64_t aId) {
    pthread_spin_lock(&MapLock);
    ToObject.erase(aId);
    pthread_spin_unlock(&MapLock);
}

tSimObj* CIdToObject::Find(const uint64_t aId) {
    tSimObj* retval = 0;
    std::map<uint64_t, tSimObj*>::iterator ui;   
    
    pthread_spin_lock(&MapLock);
    ui = ToObject.find(aId);
    if (ui != ToObject.end()) {
        retval = ui->second;
    }
    pthread_spin_unlock(&MapLock);

    return retval;
}

std::list<tSimObj*> CIdToObject::SubSet(const uint64_t aKey, uint64_t aStart, uint64_t aMaxCount) {
    std::list<tSimObj*>                    retval;
    std::map<uint64_t, tSimObj*>::iterator i;
    /*
     * Skip a number of entries.
     */
    pthread_spin_lock(&MapLock);
    for (i = ToObject.upper_bound(aKey); (i != ToObject.end()) && (aStart > 0); ++i, aStart--);
    /*
     * If we are before the end start creation of the list.
     */
    if (i != ToObject.end()) {
        for (; (i!= ToObject.end()) && (aMaxCount>0); ++i, aMaxCount--) {
            retval.push_back(i->second);
        }
    }
    pthread_spin_unlock(&MapLock);

    return retval;
}

std::list<tSimObj*> CIdToObject::SubSet(const uint64_t aFrom, const uint64_t aTo, uint64_t aStart, uint64_t aMaxCount) {
    std::list<tSimObj*>                    retval;
    std::map<uint64_t, tSimObj*>::iterator i;
    /*
     * Skip a number of entries.
     */
    pthread_spin_lock(&MapLock);
    for (i = ToObject.upper_bound(aFrom); (i != ToObject.end()) && (aStart > 0); ++i, aStart--)
    {
        /*
         * Early out because we skip beyond the aTo-key
         */
        if (i->first >= aTo) {
            i = ToObject.end();
            break;
        }
    }
    /*
     * If we are before the end start creation of the list.
     */
    if (i != ToObject.end()) {
        for (; (i!= ToObject.end()) && (aMaxCount>0); ++i, aMaxCount--) {
            if (i->first < aTo) {
                retval.push_back(i->second);
            } else {
                break;
            }
        }
    }
    pthread_spin_unlock(&MapLock);

    return retval;
}

uint64_t CIdToObject::Size() {
    uint64_t retval;

    pthread_spin_lock(&MapLock);
    retval = ToObject.size();
    pthread_spin_unlock(&MapLock);

    return retval;
}
