// **************************************************************************
//
// Modul-Name        :
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     :
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#include "memberrefarray.h"

void stdb_del(uint8_t attrtype, uint64_t objid, uint64_t valueid, uint64_t index);

tMemberRef& tMemberRefArray::operator [](uint64_t aIdx) {
    tMemberRef& retval = (*((LockedMap<uint64_t, tMemberRef>*) (this)))[aIdx];

    (tMemberBase&)(retval) = *this;

    retval.SetIdx(aIdx);

    return retval;
}


//
//  This removes the value from the DB as well.
void tMemberRefArray::Remove(const uint64_t aIdx) {
    (*((LockedMap<uint64_t, tMemberRef>*) (this))).Remove(aIdx);
    stdb_del((uint8_t)eMemberValueType::Reference, objectid, valueid, aIdx);
}
//
//  This drops the value from the map, not the DB
void tMemberRefArray::Drop(const uint64_t aIdx) {
    (*((LockedMap<uint64_t, tMemberRef>*) (this))).Remove(aIdx);
}
