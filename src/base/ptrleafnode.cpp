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
