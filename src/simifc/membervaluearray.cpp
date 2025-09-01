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

#include "membervaluearray.h"

void stdb_del(uint8_t attrtype, uint64_t objid, uint64_t valueid, uint64_t index);

tMemberValueArray::tMemberValueArray(eMemberValueType aType, uint64_t aOID, uint64_t aVID, bool aShared) {
    objectid = aOID;
    valueid  = aVID;
    attrtype = aType;
    shared   = aShared;
}

tMemberValueArray::tMemberValueArray(bool aShared)
{
    shared = aShared;
}

tMemberValue& tMemberValueArray::operator [](uint64_t aIdx) {
    tMemberValue& retval = (*((LockedMap<uint64_t, tMemberValue>*) (this)))[aIdx];

    (tMemberBase&)(retval) = *this;

    retval.SetIdx(aIdx);

    return retval;
}

//
//  This removes the value from the DB as well.
void tMemberValueArray::Remove(const uint64_t aIdx) {
    (*((LockedMap<uint64_t, tMemberValue>*) (this))).Remove(aIdx);
    stdb_del((uint8_t)attrtype, objectid, valueid, aIdx);
}

void tMemberValueArray::Remove(const std::set<uint64_t> &aToClear) {
    for (auto e : aToClear) {
        Remove(e);
    }
}

void tMemberValueArray::RemoveMissing(const std::set<uint64_t> &aToKeep) {
    std::set<uint64_t> to_clear;

    Lock();
    for (auto e : Get()) {
        //  Missing e
        if (aToKeep.find(e.first) == aToKeep.end()) {
            to_clear.insert(e.first);
        }
    }
    for (auto c : to_clear) {
        Get().erase(c);
        stdb_del((uint8_t)attrtype, objectid, valueid, c);
    }
    Unlock();
}

void tMemberValueArray::RemoveMissing(const std::vector<uint64_t> &aToKeep) {
    std::set<uint64_t> to_clear;
    std::set<uint64_t> to_keep;

    for (auto v : aToKeep) to_keep.insert(v);

    Lock();
    for (auto e : Get()) {
        //  Missing e
        if (to_keep.find(e.first) == to_keep.end()) {
            to_clear.insert(e.first);
        }
    }
    for (auto c : to_clear) {
        Get().erase(c);
        stdb_del((uint8_t)attrtype, objectid, valueid, c);
    }
    Unlock();
}
//
//  This drops the value from the map, not the DB
void tMemberValueArray::Drop(const uint64_t aIdx) {
    (*((LockedMap<uint64_t, tMemberValue>*) (this))).Remove(aIdx);
}

void tMemberValueArray::Drop(const std::set<uint64_t> &aToClear) {
    for (auto e : aToClear) {
        Drop(e);
    }
}

void tMemberValueArray::DropMissing(const std::set<uint64_t> &aToKeep) {
    std::set<uint64_t> to_clear;

    Lock();
    for (auto e : Get()) {
        //  Missing e
        if (aToKeep.find(e.first) == aToKeep.end()) {
            to_clear.insert(e.first);
        }
    }
    for (auto c : to_clear) {
        Get().erase(c);
    }
    Unlock();
}

void tMemberValueArray::DropMissing(const std::vector<uint64_t> &aToKeep) {
    std::set<uint64_t> to_clear;
    std::set<uint64_t> to_keep;

    for (auto v : aToKeep) to_keep.insert(v);

    Lock();
    for (auto e : Get()) {
        //  Missing e
        if (to_keep.find(e.first) == to_keep.end()) {
            to_clear.insert(e.first);
        }
    }
    for (auto c : to_clear) {
        Get().erase(c);
    }
    Unlock();
}
