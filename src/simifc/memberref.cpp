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
// Modul-Name        : memberref.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     :
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#include "memberref.h"
#include <variant.h>

void stdb_set(uint8_t attrtype, uint64_t objid, uint64_t valueid, uint64_t index, const tVariant&);
void stdb_del(uint8_t attrtype, uint64_t objid, uint64_t valueid, uint64_t index);

tMemberRef& tMemberRef::operator =(const tObjectRef& newref) {
    uint64_t oldref = *this;
    uint64_t newrefvalue = newref;


    if (oldref != newrefvalue) {
        (*(tObjectRef*)(this)) = newref;
        stdb_set((uint8_t)eMemberValueType::Reference, objectid, valueid, valueidx, (uint64_t)(*this));
    }
    return *this;
}

void tMemberRef::Remove() {
    (*(tObjectRef*)(this)) = tObjectRef();
    stdb_del((uint8_t)eMemberValueType::Reference, objectid, valueid, valueidx);
}
