// **************************************************************************
//
// Modul-Name        : objectref
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 30.04.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef OBJECTREF_H
#define OBJECTREF_H

#include <stdint.h>
#include <future>

#ifndef INVALID_OBJECTID
#define INVALID_OBJECTID UINT64_MAX
#endif

#include "ptrnode.h"

class tNodeEntry;
class tSimObj;

class tObjectRef {
public:
#if defined(APPCORE_BASE_OBJECTREF_TEST_FRIEND)
    APPCORE_BASE_OBJECTREF_TEST_FRIEND();
#endif
    //
    //  Copy and assignment
    tObjectRef(const tObjectRef& other);
    tObjectRef& operator=(const tObjectRef& other);

    tObjectRef() {id = INVALID_OBJECTID; mPtr = nullptr;}
    tObjectRef(uint64_t aId, tSimObj* aPtr, uint64_t vers = 0) ;
    tObjectRef(uint64_t aId, uint64_t vers = 0);
    ~tObjectRef();

    tSimObj* Raw() const {return (tSimObj*)(mPtr->Get());}

    bool valid() const {
        if (mPtr != nullptr) {
            return true;
        } else if (id != INVALID_OBJECTID) {
            return Check(id);
        } else {
            return false;
        }
    }
    tSimObj* operator->() const;
    tSimObj* operator*() {return Raw();}
    operator uint64_t () const {return id;}
    operator void*() const {return mPtr;}
    bool operator!=(const tObjectRef &a_other) const {return (id != (uint64_t)a_other);};

    void Delete();
public:
    static uint64_t Allocate(uint64_t vers= 0u) {return Reserve(INVALID_OBJECTID, vers);}
    static tObjectRef Add(tSimObj* obj, uint64_t vers= 0u);
    static uint64_t Reserve(uint64_t objid = INVALID_OBJECTID, uint64_t vers = 0u);
    static void Release(uint64_t objid, uint64_t vers = 0u);
    static tSimObj* Find(uint64_t aId, uint64_t vers = 0u);
    static bool Check(uint64_t aId, uint64_t vers = 0u);
private:
    uint64_t               id;
    mutable tNodeEntry*    mPtr;
};


#endif // OBJECTREF_H
