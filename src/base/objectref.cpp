// **************************************************************************
//
// Modul-Name        : objectref
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 30.04.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#include <exception>
#include <future>
#include "objectref.h"
#include "pointerindex.h"

tObjectRef::tObjectRef(const tObjectRef &other) {
    id = other.id;
    mPtr = other.mPtr;
    if (mPtr != nullptr) {
        mPtr->IncrementReferences();
    }
}

tObjectRef& tObjectRef::operator=(const tObjectRef& other) {
    id = other.id;
    mPtr = other.mPtr;
    if (mPtr != nullptr) {
        mPtr->IncrementReferences();
    }
    return *this;
}

tObjectRef::tObjectRef(uint64_t aId, uint64_t vers) {
    id  = aId;
    mPtr = CPointerIndex::instance().Find(aId, vers);
    if (mPtr != nullptr) {
        mPtr->IncrementReferences();
    }
}

tObjectRef::tObjectRef(uint64_t aId, tSimObj* aPtr, uint64_t vers) {
    //
    //
    //  This constructor is a special purpose constructor not an all purpose one.
    //  Its used while do the initialization from the database. So it does a bit
    //  more as only setup the object for usage.
    id = aId;
    mPtr = CPointerIndex::instance().Add(aPtr, vers);
    if (mPtr != nullptr) {
        mPtr->IncrementReferences();
    }
}

tObjectRef::~tObjectRef() {
    if (mPtr != nullptr) {
        mPtr->DecrementReferences();
    }
}

uint64_t tObjectRef::Reserve(uint64_t objid, uint64_t vers) {
    uint64_t retval = CPointerIndex::instance().Reserve(objid, vers);

    return retval;
}

void tObjectRef::Release(uint64_t objid, uint64_t vers) {
    CPointerIndex::instance().Release(objid, vers);
}

tObjectRef tObjectRef::Add(tSimObj *obj, uint64_t vers) {
    return tObjectRef(obj->objid, obj, vers);
}

tSimObj* tObjectRef::Find(uint64_t objid, uint64_t vers) {
    tSimObj*    retval;
    tNodeEntry* entry =CPointerIndex::instance().Find(objid, vers);

    if (entry != nullptr) {
        retval = (tSimObj*)entry->Get();
    } else {
        retval = nullptr;
    }
    return retval;
}

bool tObjectRef::Check(uint64_t objid, uint64_t vers) {
    return (CPointerIndex::instance().Find(objid, vers) != nullptr);
}


tSimObj* tObjectRef::operator->() const {
    tSimObj* retval = nullptr;

    if (id != INVALID_OBJECTID) {
        if (mPtr == nullptr) {
            mPtr = CPointerIndex::instance().Get(id);
            //
            //  Doing some more checks after loading.
            if (mPtr != nullptr) {
                //
                // Inc reference, as this tObjectRef holds a ref now.
                mPtr->IncrementReferences();
                retval = (tSimObj*)(mPtr->Get());
            } else {
                throw std::bad_alloc();
            }
        } else {
            retval = (tSimObj*)(mPtr->Get());
            if (retval->droptime != UINT64_MAX) {
                CPointerIndex::instance().WindUpTimer(mPtr, retval->droptime);
            }
        }
        if (retval != nullptr) {
            //
            // Lazy checking if object already loaded.
            retval = (tSimObj*)(mPtr->Get());
            if  (retval->objid != id) {
                throw std::bad_alloc();
            }

        }
    }
    return retval;
}
//
//  Remove the object inclusive its pointer and free the id.s
void tObjectRef::Delete() {
    if (mPtr != nullptr) {
        mPtr->Delete();
    }
}
