// **************************************************************************
//
// Modul-Name        : memberref.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     :
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#include "memberref.h"
#include <variant.h>

void stdb_set(uint8_t attrtype, uint64_t objid, uint64_t valueid, uint64_t index, const tVariant&);
void stdb_del(uint8_t attrtype, uint64_t objid, uint64_t valueid, uint64_t index);

tMemberRef& tMemberRef::operator =(const tObjectRef& newref) {
    uint64_t oldref = *this;
    uint64_t newrefvalue = newref;


    if (oldref != newrefvalue) {
        (*(tObjectRef*)(this)) = newref;
        stdb_set((uint8_t)eMemberValueType::Reference, objectid, valueid, valueidx, (uint64_t)(*this));
    }
    return *this;
}

void tMemberRef::Remove() {
    (*(tObjectRef*)(this)) = tObjectRef();
    stdb_del((uint8_t)eMemberValueType::Reference, objectid, valueid, valueidx);
}
