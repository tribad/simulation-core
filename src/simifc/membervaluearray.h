#ifndef MEMBERVALUEARRAY_H
#define MEMBERVALUEARRAY_H

#include <set>
#include <vector>
#include <LockedMap.h>
#include <membervalue.h>

class tMemberValueArray : public tMemberBase, public LockedMap<uint64_t, tMemberValue>
{
public:
#if defined(APPCORE_SIMIFC_MEMBERVALUEARRAY_TEST_FRIEND)
    APPCORE_SIMIFC_MEMBERVALUEARRAY_TEST_FRIEND();
#endif
    tMemberValueArray(eMemberValueType aType, uint64_t aOID, uint64_t aVID, bool aShared=false);
    tMemberValueArray(bool aShared=false);
    tMemberValue& operator [](uint64_t aIdx);
    void Remove(const uint64_t aIdx);
    void Remove(const std::set<uint64_t>& aToClear);
    void RemoveMissing(const std::set<uint64_t>& aToKeep);
    void RemoveMissing(const std::vector<uint64_t>& aToKeep);
    void Drop(const uint64_t aIdx);
    void Drop(const std::set<uint64_t>& aToClear);
    void DropMissing(const std::set<uint64_t>& aToKeep);
    void DropMissing(const std::vector<uint64_t>& aToKeep);
private:
    bool  shared;
};

#endif // MEMBERVALUEARRAY_H
