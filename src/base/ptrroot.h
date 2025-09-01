// **************************************************************************
//
// Modul-Name        : ptrroot.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 23.11.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef CPTRROOT_H
#define CPTRROOT_H

#include <cstdint>
#include <memory>
#include <LockedMap.h>

#include <ptrnode.h>

class CPtrNode;
class CLeafNode;

class CPtrRoot {
#if defined(APPCORE_BASE_PTRROOT_TEST_FRIEND)
    APPCORE_BASE_PTRROOT_TEST_FRIEND();
#endif
public:
    CPtrRoot() ;
    CPtrRoot(const CPtrRoot& aOther) = default;
    CPtrRoot & operator=(const CPtrRoot& aOther) = default;
    CPtrRoot(CPtrRoot&& aOther) ;
    CPtrRoot & operator=(CPtrRoot&& aOther);
    CPtrRoot(CLeafNode* aRoot);
    ~CPtrRoot();

    bool ValidateRoot(uint64_t aVersion);

    CLeafNode*          GetStart() {
        CLeafNode* retval;
        pthread_mutex_lock(&mLock);
        retval = mStart;
        pthread_mutex_unlock(&mLock);
        return retval;
    }

    void                Chain(CLeafNode* aNode);
    uint64_t            LeafCount();

    uint64_t            Allocate(void);
    tNodeEntry*         Insert(uint64_t id, void* obj);
    void                Remove(uint64_t id);
    tNodeEntry*         Find(uint64_t id);
public:
    static LockedMap<uint64_t, CPtrRoot> mRootMap;
private:
    CPtrNode*       mRoot;
    CLeafNode*      mStart   = nullptr;
    CLeafNode*      mEnd     = nullptr;
    pthread_mutex_t mLock    = PTHREAD_MUTEX_INITIALIZER;
    uint64_t        mVersion = 0ul;
};

#endif // CPTRROOT_H
