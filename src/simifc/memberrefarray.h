// **************************************************************************
//
// Modul-Name        :
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     :
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef MEMBERREFARRAY_H
#define MEMBERREFARRAY_H

#include <memberref.h>
#include <LockedMap.h>

class tMemberRefArray : public tMemberBase, public LockedMap<uint64_t, tMemberRef>
{
public:
#if defined(APPCORE_SIMIFC_MEMBERREFARRAY_TEST_FRIEND)
    APPCORE_SIMIFC_MEMBERREFARRAY_TEST_FRIEND();
#endif
    tMemberRefArray() = default;
    tMemberRefArray(uint64_t oid, uint64_t vid) : tMemberBase(eMemberValueType::Reference, oid, vid) {}

    tMemberRef& operator [](uint64_t aIdx);

    void Remove(const uint64_t aIdx);
    void Drop(const uint64_t aIdx);
};

#endif // MEMBERREFARRAY_H
