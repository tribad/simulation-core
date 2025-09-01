// MIT License
//
// Copyright (c) 2020 Hans-Juergen Lange<hjl@simulated-universe.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// **************************************************************************
//
// Modul-Name        : ptrnode.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 11.01.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef PTRNODE_H
#define PTRNODE_H

#include <pthread.h>

#include <cstdint>
#include <cstring>

#include "LockedSet.h"
//
//  More flexible with the nodesize
//  But only supporting multiples of 8 bits.
#define NODESIZE_BITS      8u
#define NODESIZE_COUNT     (1u << NODESIZE_BITS)

enum class eNodeState {
    InUse,   //  Its in Use. NodeEntry is blocked from removal.
    Free,    //  Its free. NodeEntry can be used for a new object.
    Deleted  //  There are still references to the object existing.
};
//
//  This is one entry in the Nodes list.
//  We need a spinlock and a expiration value
class tNodeEntry {
#if defined(APPCORE_BASE_NODEENTRY_TEST_FRIEND)
    APPCORE_BASE_NODEENTRY_TEST_FRIEND();
#endif
public:
    tNodeEntry() {
        pthread_spin_init(&mAccessLock, PTHREAD_PROCESS_PRIVATE);
    }
    tNodeEntry(void* aPtr, uint64_t aExpiration = UINT64_MAX) : mPtr(aPtr), mExpiration(aExpiration) {
        pthread_spin_init(&mAccessLock, PTHREAD_PROCESS_PRIVATE);
    }
    ~tNodeEntry();
    void Lock() {
        pthread_spin_lock(&mAccessLock);
    }
    void Unlock() {
        pthread_spin_unlock(&mAccessLock);
    }
    bool isLocked() {
                  bool retval = true;
                     if (pthread_spin_trylock(&mAccessLock) == 0) {
                         pthread_spin_unlock(&mAccessLock);
                         retval = false;
                     }
                     return retval;
                 }
    //
    //  This method is used to drop the object from memory. Not freeing it from the system.
    //
    //  As we do not know what object, if any, is stored in the node,
    //  we return the pointer and the caller is responsible to free the object
    //  the node has stored so far.
    void* Drop(uint64_t aTimeStamp) {
        void* retval = nullptr;

        Lock();
        if  ((mState == eNodeState::InUse) && (aTimeStamp > mExpiration)) {
            retval = mPtr;
            mPtr   = nullptr;
        }
        Unlock();
        return retval;
    }
    //
    //  Completely free up the nodeentry if no reference is hold on
    //  the object.
    void* Free() {
        void* retval = nullptr;

        Lock();
        if  ((mState == eNodeState::Deleted) && (mRefCount == 0)) {
            retval = mPtr;

            mPtr        = nullptr;
            mState      = eNodeState::Free;
            mExpiration = UINT64_MAX;
        }
        Unlock();
        return retval;
    }

    void* Get(uint64_t aExpiration) {
        Set(aExpiration);
        return mPtr;
    }

    void* Get() {
        return mPtr;
    }

    void Set(void* aPtr, uint64_t aExpiration = UINT64_MAX) {
        Lock();
        mPtr        = aPtr;
        mExpiration = aExpiration;
        mState      = eNodeState::InUse;
        Unlock();
    }
    void Set(uint64_t aExpiration = UINT64_MAX) {
        Lock();
        mExpiration = aExpiration;
        Unlock();
    }
    uint64_t RefCount() {
        return mRefCount;
    }
    uint64_t IncrementReferences() {
        uint64_t retval;

        Lock();
        //
        //  if nodestate is deleted it should not possible to increment the reference count.
        if (((mState == eNodeState::InUse)  || (mState == eNodeState::Deleted)) && (mRefCount < UINT64_MAX)) {
            mRefCount++;
        }
        retval = mRefCount;
        Unlock();
        return retval;
    }
    uint64_t DecrementReferences() {
        uint64_t retval;

        Lock();
        //
        // Counting down
        if (((mState == eNodeState::InUse) || (mState == eNodeState::Deleted)) && (mRefCount > 0u)) {
            mRefCount--;
        }
        retval = mRefCount;
        Unlock();
        return retval;
    }
    void Delete() {
        Lock();
        if (mState != eNodeState::Free) {
            mState = eNodeState::Deleted;
        }
        Unlock();
    }
public:
    eNodeState         mState      = eNodeState::Free;
private:
    void*              mPtr        = nullptr;
    uint64_t           mExpiration = UINT64_MAX;
    uint64_t           mRefCount   = 0;
    pthread_spinlock_t mAccessLock;
};

class CPtrNode
{
#if defined(APPCORE_BASE_PTRNODE_TEST_FRIEND)
    APPCORE_BASE_PTRNODE_TEST_FRIEND();
#endif
public:
    CPtrNode() {
        //
        // Not full.
        full  = 0;
    }
    virtual ~CPtrNode() = default;
    bool isFull() {
        return full;
    }
    uint16_t getUnused() {
        uint16_t pos;
        //  if there is a single 1 bit we get the index+1 to it.
        if ((pos=__builtin_ffsl(unused[0]))) {
            return (pos-1);
        } else if ((pos=__builtin_ffsl(unused[1]))) {
            return (63+pos);
        } else if ((pos=__builtin_ffsl(unused[2]))) {
            return (127+pos);
        } else if ((pos=__builtin_ffsl(unused[3]))) {
            return (191+pos);
        } else {
            return (UINT16_MAX);
        }
    }

    CPtrNode* getRoot() {
        if (up) {
            return up->getRoot();
        } else {
            return this;
        }
    }

    CPtrNode*           Allocate(uint64_t & id);

    tNodeEntry*         Insert(uint64_t id, void* obj);
    void                Remove(uint64_t id);
    tNodeEntry*         Find(uint64_t id);

    virtual void        Allocate(uint8_t* id) = 0;

    virtual tNodeEntry* Insert(uint8_t* id, void* obj) = 0;
    virtual tNodeEntry* Find(uint8_t* id) = 0;
    //
    //  Returning full flag before removal
    virtual bool        Remove(uint8_t* id) = 0;
public:
    CPtrNode*       up        = nullptr;                                            // Parent
    uint64_t        unused[4] = {UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX};   // Bitmap for usage map
    uint64_t        mVersion  = 0u;                                                 // Version the node belongs to
    uint8_t         layer     = 0u;                                                 // layer starts with zero the LSB
    uint8_t         full:1;
};



#endif // PTRNODE_H
