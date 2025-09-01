// **************************************************************************
//
// Modul-Name        : ptrroot.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 23.11.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#include "ptrroot.h"
#include "ptrnode.h"
#include "ptrleafnode.h"
//
//  This is a map for the different versions we have.
LockedMap<uint64_t, CPtrRoot > CPtrRoot::mRootMap;

CPtrRoot::CPtrRoot() {
    mStart = mEnd = nullptr; //new CLeafNode;
    mRoot  = mStart;
}

CPtrRoot::CPtrRoot(CLeafNode* aRoot)
{
    mRoot    = aRoot;
    mStart   = mEnd = aRoot;
    mVersion = aRoot->mVersion;
}

CPtrRoot::CPtrRoot(CPtrRoot&& aOther) {
    mRoot = aOther.mRoot;
    aOther.mRoot = nullptr;

    mStart = aOther.mStart;
    mEnd   = aOther.mEnd;
    mVersion = mRoot->mVersion;
}

CPtrRoot& CPtrRoot::operator = (CPtrRoot&& aOther) {
    mRoot = aOther.mRoot;
    aOther.mRoot = nullptr;

    mStart = aOther.mStart;
    mEnd   = aOther.mEnd;

    mVersion = mRoot->mVersion;
    return *this;
}

CPtrRoot::~CPtrRoot() {
    if (mRoot != nullptr) {
        delete mRoot;
        mRoot = nullptr;
    }
}

uint64_t CPtrRoot::Allocate(void) {
    uint64_t retval = 0ul;

    pthread_mutex_lock(&mLock);

    mRoot = mRoot->Allocate(retval);

    pthread_mutex_unlock(&mLock);

    return retval;
}

tNodeEntry* CPtrRoot::Insert(uint64_t id, void* obj) {
    tNodeEntry* retval = nullptr;

    pthread_mutex_lock(&mLock);

    retval = mRoot->Insert(id, obj);
    //
    //  Allign the root
    mRoot = mRoot->getRoot();

    pthread_mutex_unlock(&mLock);
    return retval;
}

void CPtrRoot::Remove(uint64_t id) {
    pthread_mutex_lock(&mLock);

    mRoot->Remove(id);

    pthread_mutex_unlock(&mLock);
}

tNodeEntry* CPtrRoot::Find(uint64_t id) {
    tNodeEntry* retval;

    pthread_mutex_lock(&mLock);

    retval = mRoot->Find(id);

    pthread_mutex_unlock(&mLock);
    return retval;
}

void CPtrRoot::Chain(CLeafNode *aNode) {
    if (mRoot == nullptr) {
        mRoot = aNode;
    }
    if (mStart == nullptr) {
        mEnd = mStart = aNode;
    } else {
        mEnd->SetNext(aNode);
        aNode->SetPrev(mEnd);
        mEnd = aNode;
    }
}

uint64_t CPtrRoot::LeafCount() {
    size_t leafcounter = 0ul;

    CLeafNode* node = mStart;

    while (node != nullptr) {
        leafcounter++;
        node = node->GetNext();
    }
    return leafcounter;
}

bool CPtrRoot::ValidateRoot(uint64_t aVersion) {
    bool retval = false;

    if (mRoot == nullptr) {
        Chain(new CLeafNode(aVersion));
        retval = true;
    }
    return retval;
}
