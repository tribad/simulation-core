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
