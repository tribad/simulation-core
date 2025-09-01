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

#include <iostream>
#include "membervalue.h"

void stdb_set(uint64_t objid, uint64_t valueid, uint64_t index, double value);
void stdb_set(uint64_t objid, uint64_t valueid, uint64_t index, int64_t value);
void stdb_set(uint64_t objid, uint64_t valueid, uint64_t index, const std::string& value);
void stdb_set(uint8_t attrtype, uint64_t objid, uint64_t valueid, uint64_t index, const tVariant&);
void stdb_del(uint8_t attrtype, uint64_t objid, uint64_t valueid, uint64_t index);

tMemberValue::tMemberValue()
{

}

tMemberValue::tMemberValue(uint64_t aOID, uint64_t aVID, uint64_t aVIdx, const tVariant &value) {
    objectid   = aOID;
    valueid    = aVID;
    valueidx = aVIdx;

    ((tVariant&)(*this)) = value;
    switch (type) {
    case eVariant::Double:
        attrtype = eMemberValueType::Double;
        break;
    case eVariant::Int:
    case eVariant::Unsigned:
    case eVariant::Boolean:
        attrtype = eMemberValueType::Int;
        break;
    case eVariant::String:
        attrtype = eMemberValueType::String;
        break;
    case eVariant::Reference:
    case eVariant::Null:
        attrtype = eMemberValueType::Invalid;
        break;
    }
}

tMemberValue::tMemberValue(uint64_t aOID, uint64_t aVID, const tVariant &value) {
    objectid   = aOID;
    valueid    = aVID;
    valueidx = 0;

    ((tVariant&)(*this)) = value;
    switch (type) {
    case eVariant::Double:
        attrtype = eMemberValueType::Double;
        break;
    case eVariant::Int:
    case eVariant::Unsigned:
    case eVariant::Boolean:
        attrtype = eMemberValueType::Int;
        break;
    case eVariant::String:
        attrtype = eMemberValueType::String;
        break;
    case eVariant::Reference:
    case eVariant::Null:
        attrtype = eMemberValueType::Invalid;
        break;
    }
}

tMemberValue& tMemberValue::operator =(bool aValue) {
    if ((bool)(*this) != aValue) {
        ((tVariant&)(*this)) = aValue;

        stdb_set((uint8_t)attrtype, objectid, valueid, valueidx, *this);
    }

    return *this;
}

tMemberValue& tMemberValue::operator =(uint64_t aValue) {
    if ((uint64_t)(*this) != aValue) {
        ((tVariant&)(*this)) = aValue;

        stdb_set((uint8_t)attrtype, objectid, valueid, valueidx, *this);
    }

    return *this;
}

tMemberValue& tMemberValue::operator = (double aValue) {
    if ((double)(*this) != aValue) {
        ((tVariant&)(*this)) = aValue;

        stdb_set((uint8_t)attrtype, objectid, valueid, valueidx, *this);
    }

    return *this;
}

tMemberValue& tMemberValue::operator = (const std::string& aValue) {
    if ((std::string)(*this) != aValue) {
        ((tVariant&)(*this)) = aValue.c_str();

        stdb_set((uint8_t)attrtype, objectid, valueid, valueidx, *this);
    }

    return *this;
}

tMemberValue& tMemberValue::operator = (const tVariant& aValue) {
    if ((tVariant&)*this != aValue) {
        ((tVariant&)(*this)) = aValue;

        stdb_set((uint8_t)attrtype, objectid, valueid, valueidx, *this);
    }

    return *this;
}

void tMemberValue::Remove() {
    stdb_del((uint8_t)attrtype, objectid, valueid, valueidx);
}
