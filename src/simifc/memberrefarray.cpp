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

// **************************************************************************
//
// Modul-Name        :
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     :
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#include "memberrefarray.h"

void stdb_del(uint8_t attrtype, uint64_t objid, uint64_t valueid, uint64_t index);

tMemberRef& tMemberRefArray::operator [](uint64_t aIdx) {
    tMemberRef& retval = (*((LockedMap<uint64_t, tMemberRef>*) (this)))[aIdx];

    (tMemberBase&)(retval) = *this;

    retval.SetIdx(aIdx);

    return retval;
}


//
//  This removes the value from the DB as well.
void tMemberRefArray::Remove(const uint64_t aIdx) {
    (*((LockedMap<uint64_t, tMemberRef>*) (this))).Remove(aIdx);
    stdb_del((uint8_t)eMemberValueType::Reference, objectid, valueid, aIdx);
}
//
//  This drops the value from the map, not the DB
void tMemberRefArray::Drop(const uint64_t aIdx) {
    (*((LockedMap<uint64_t, tMemberRef>*) (this))).Remove(aIdx);
}
