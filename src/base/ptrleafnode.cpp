// **************************************************************************
//
// Modul-Name        : ptrleafnode.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 23.11.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#include "ptrnode.h"
#include "ptrleafnode.h"
#include "ptrroot.h"

CLeafNode::CLeafNode(uint64_t aVersion) {
    mPrev    = mNext = nullptr;
    mVersion = aVersion;
    pthread_spin_init(&mLock, PTHREAD_PROCESS_PRIVATE);
}

CLeafNode::~CLeafNode() {
}

void CLeafNode::Allocate(uint8_t* id) {
    uint16_t pos = getUnused();

    *id =  pos;

    mEntry[*id].Set(nullptr);
    //
    //  Reset the free bit.
    uint8_t index = (*id) >> 6u;
    uint8_t bit   = (*id) & (0x3f);

    uint64_t mask = ~(1ul << bit);
    unused[index] &= mask;
    //
    //  Check if we have any unused left.
    //  if not set the full flag.
    if (getUnused() == UINT16_MAX) {
        full = true;
    }
}

tNodeEntry* CLeafNode::Insert(uint8_t *id, void *obj) {
    tNodeEntry* retval = nullptr;
    //
    //  No we are in the leaf. Insert the obj at position
    mEntry[*id].Set(obj);
    retval = mEntry + (*id);
    //
    //  Reset the free bit.
    uint8_t index = (*id) >> 6u;
    uint8_t bit   = (*id) & (0x3f);

    uint64_t mask = ~(1ul << bit);
    unused[index] &= mask;
    //
    //  Check if we have any unused left.
    //  if not set the full flag.
    if (getUnused() == UINT16_MAX) {
        full = true;
    }
    return retval;
}

tNodeEntry* CLeafNode::Find(uint8_t *id) {
    tNodeEntry* retval = nullptr;
    if ((mEntry[*id].mState == eNodeState::InUse) || (mEntry[*id].mState == eNodeState::Deleted)) {
        retval =  mEntry + (*id);
    }
    return retval;
}

bool CLeafNode::Remove(uint8_t *id) {
    //
    //  Remove prepares the removal.
    //  But if the nodeentry is still referenced the removal must
    //  be deffered.
    bool retval = full;

    mEntry[*id].Free();
    //
    //  Set the free bit.
    uint8_t index = (*id) >> 6u;
    uint8_t bit   = (*id) & (0x3f);

    uint64_t mask = (1ul << bit);

    unused[index] |= mask;
    full = false;

    return retval;
}
