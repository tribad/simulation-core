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
