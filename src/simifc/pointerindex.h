// MIT License
//
// Copyright (c) 2018 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#ifndef POINTERINDEX_H
#define POINTERINDEX_H

#include <map>
#include <simobj.h>

class CPtrNode;
class CLeafNode;
class tNodeEntry;
class CPtrReleaseThread;

class CPointerIndex
{
#if defined(APPCORE_SIMIFC_POINTERINDEX_TEST_FRIEND)
    APPCORE_SIMIFC_POINTERINDEX_TEST_FRIEND();
#endif
public:
    CPointerIndex();
    ~CPointerIndex();
    //
    //  Adding an object. If the object has an INVALIDINDEX set as
    //  object id it gets a new index allocated.
    //  In any other case the pointer for the object id gets set/updated.
    tNodeEntry* Add(tSimObj* obj, uint64_t vers = 0u);
    //
    //  Removing the Pointer and freeing the index that this objects has
    //  been using.
    void Remove(tSimObj* obj, uint64_t vers = 0u);
    //
    //  Only mark the index as reserved.
    uint64_t Reserve(uint64_t id = INVALID_OBJID, uint64_t vers = 0u);
    //
    //  Release the id and return true if succeeded. If there has been a pointer
    //  allocated to the id, do not release the entry but return false as a result.
    bool Release(uint64_t id, uint64_t vers = 0u);
    //
    //  Find the pointer to the id.
    tNodeEntry* Find(uint64_t id, uint64_t vers = 0u);
    //
    //  Get the pointer to the id means loading it from the DB if needed.
    tNodeEntry* Get(uint64_t aId, uint64_t vers = 0u);
    //
    //  WindUp the objects droptimer.
    void WindUpTimer(tNodeEntry* aNode, uint64_t aDropTimeOut);
    //
    //  Singleton access.
    static CPointerIndex& instance() {
        static CPointerIndex ptr;

        return ptr;
    }
private:
    static CPtrReleaseThread*           mPtrRelease;
};

#endif // POINTERINDEX_H
