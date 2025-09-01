// **************************************************************************
//
// Modul-Name        :
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     :
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef MEMBERREF_H
#define MEMBERREF_H

#include <memberbase.h>
#include <objectref.h>

class tMemberRef : public tMemberBase, public tObjectRef
{
public:
    tMemberRef() = default;
    tMemberRef(uint64_t oid, uint64_t vid) : tMemberBase(eMemberValueType::Reference, oid, vid) {}
    tMemberRef& operator=(const tObjectRef& newvalue);
    tMemberRef& SetIdx(uint64_t aIdx) {valueidx = aIdx; return *this;}

    void Remove();
};

#endif // MEMBERREF_H
