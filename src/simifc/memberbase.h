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

#ifndef MEMBERBASE_H
#define MEMBERBASE_H

#include <cstdint>
#include <objectref.h>

enum class eMemberValueType : uint8_t {
    Double    = 0x01,
    Int       = 0x02,
    String    = 0x04,
    Reference = 0x08,
    Invalid   = 0xff
};

class tMemberBase
{
public:
#if defined(APPCORE_SIMIFC_MEMBERBASE_TEST_FRIEND)
    APPCORE_SIMIFC_MEMBERBASE_TEST_FRIEND();
#endif
    tMemberBase() = default;
    tMemberBase(eMemberValueType aType, uint64_t aOID, uint64_t aVID, uint64_t aVIDX=0ul) {
        attrtype   = aType;
        objectid   = aOID;
        valueid    = aVID;
        valueidx   = aVIDX;
    }
public:
    eMemberValueType attrtype = eMemberValueType::Invalid;
    uint64_t         objectid = INVALID_OBJECTID;
    uint64_t         valueid  = 0ul;
    uint64_t         valueidx = 0ul;
};

#endif // MEMBERBASE_H
