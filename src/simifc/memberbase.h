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
