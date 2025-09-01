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
// Modul-Name        : ptrtreenode.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 23.11.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef PTRTREENODE_H
#define PTRTREENODE_H

#include <ptrnode.h>

class CTreeNode : public CPtrNode {
#if defined(APPCORE_BASE_PTRNODE_TEST_FRIEND)
    APPCORE_BASE_PTRNODE_TEST_FRIEND();
#endif
public:
    CTreeNode(uint64_t aVersion = 0ul);
    CTreeNode(CPtrNode* down);
    virtual ~CTreeNode() ;

    void        Allocate(uint8_t* id)          override;
    tNodeEntry* Insert(uint8_t* id, void* obj) override;
    tNodeEntry* Find(uint8_t* id)              override;
    bool        Remove(uint8_t* id)            override;
private:
    CPtrNode*   mEntry[NODESIZE_COUNT]; //  The new node entries.
};


#endif // PTRTREENODE_H
