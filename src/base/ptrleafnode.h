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
// Modul-Name        : ptrleafnode.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 23.11.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef PTRLEAFNODE_H
#define PTRLEAFNODE_H

#include <ptrnode.h>
#include <ptrroot.h>

class CLeafNode : public CPtrNode {
#if defined(APPCORE_BASE_PTRNODE_TEST_FRIEND)
    APPCORE_BASE_PTRNODE_TEST_FRIEND();
#endif
public:
    CLeafNode(uint64_t aVersion = 0ul);
    virtual ~CLeafNode() ;

    void        Allocate(uint8_t* id)          override;
    tNodeEntry* Insert(uint8_t* id, void* obj) override;
    tNodeEntry* Find(uint8_t* id)              override;
    bool        Remove(uint8_t* id)            override;

    CLeafNode* GetNext(){
        CLeafNode* retval;

        pthread_spin_lock(&mLock);
        retval = mNext;
        pthread_spin_unlock(&mLock);
        return retval;
    }
    CLeafNode* GetPrev() {
        CLeafNode* retval;

        pthread_spin_lock(&mLock);
        retval = mPrev;
        pthread_spin_unlock(&mLock);
        return retval;
    }
    void SetNext(CLeafNode* aNext) {
        pthread_spin_lock(&mLock);
        mNext = aNext;
        pthread_spin_unlock(&mLock);
    }
    void SetPrev(CLeafNode* aPrev) {
        pthread_spin_lock(&mLock);
        mPrev = aPrev;
        pthread_spin_unlock(&mLock);
    }
    //
    //  This does not sort the new leaf. Its only for the release-thread.
    void Chain() {
        //
        //  We check if the root-block exists. This may be a debugging helper only.
        if (CPtrRoot::mRootMap.Check(mVersion)) {
            CPtrRoot::mRootMap[mVersion].Chain(this);
        }
        pthread_spin_unlock(&mLock);
    }
    //
    //  No check-code here. For internal use only.
    tNodeEntry* Get(size_t aIndex) {
        return mEntry + aIndex;
    }
private:
    tNodeEntry         mEntry[NODESIZE_COUNT]; //  The new node entries.
    pthread_spinlock_t mLock;
    CLeafNode*         mNext;
    CLeafNode*         mPrev;
};



#endif // PTRLEAFNODE_H
