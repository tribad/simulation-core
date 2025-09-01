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

#ifndef MEMBERVALUE_H
#define MEMBERVALUE_H

#include <stdint.h>
#include <variant.h>
#include <memberbase.h>


class tMemberValue : public tMemberBase, public tVariant
{
public:
    tMemberValue();
    tMemberValue(eMemberValueType aType, uint64_t aOID, uint64_t aVID, uint64_t aVIdx) : tMemberBase(aType, aOID, aVID) {
        valueidx = aVIdx;
    }
    tMemberValue(eMemberValueType aType, uint64_t aOID, uint64_t aVID, uint64_t aVIdx, const tVariant& value) : tMemberBase(aType, aOID, aVID) {
        valueidx = aVIdx;
        *this = value;
    }
    tMemberValue(uint64_t aOID, uint64_t aVID, uint64_t aVIdx, const tVariant& value);
    tMemberValue(uint64_t aOID, uint64_t aVID, const tVariant& value);
    tMemberValue& SetIdx(uint64_t aIdx) {valueidx = aIdx; return *this;}

    tMemberValue& operator=(bool value);
    tMemberValue& operator=(uint64_t value);
    tMemberValue& operator=(double value);
    tMemberValue& operator=(int64_t value);
    tMemberValue& operator=(const tVariant& value);
    tMemberValue& operator=(const std::string & value);

    void Remove();
private:
};

#endif // MEMBERVALUE_H
