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
// Modul-Name        : ptrnode.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 11.01.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#include <iostream>
#include "ptrnode.h"
#include "ptrtreenode.h"
//
//  The nodeentry destructor
tNodeEntry::~tNodeEntry() {
    pthread_spin_destroy(&mAccessLock);
}
//
//

//
// This is the external method to get a new unused id from the system.
CPtrNode* CPtrNode::Allocate(uint64_t &id) {
    CPtrNode* root = this;
    id = 0ul;
    //
    //  If the actual root node is full we create a new node above
    //  and do the initialization of it.
    if (full) {
        root = new CTreeNode(this);
        root->unused[0] = ~(1ul);
    }
    //
    //  Go into the tree
    root->Allocate((uint8_t*)(&id) + root->layer);
    //
    //  The root may change. So we return the actual root to the
    //  caller.
    return root;
}
//
//  This method expect to be started on the root-element.
tNodeEntry* CPtrNode::Insert(uint64_t id, void *obj) {
    CPtrNode*   root   = this;
    tNodeEntry* retval = nullptr;
    uint8_t     l;
    //
    //  Find the needed layercount for the id.
    //  As we wrote this for little endian we search from back to front.
    //
    //  This should be the fastest way to get the layercount.
    if (id) {
        l = 7 - (__builtin_clzl(id)/8);
    } else {
        l = 0;
    }
    //
    //  Here we should have the number of needed layers
    //  in l.
    //  If the number of layers needed is larger than we have already
    //  in the tree we add them.
    //  new layers are always on index 0
    while (l > root->layer) {
        //
        //  Link the old root up.
        root = new CTreeNode(root);
    }
    //
    //  Start insert with the highest not zero byte in the index.
    retval = root->Insert((uint8_t*)(&id) + root->layer, obj);
    return retval;
}
//
//  This method expect to be started on the root-element.
void CPtrNode::Remove(uint64_t id) {
    uint8_t l;
    //
    //  Find the needed layercount for the id.
    //
    //  This should be the fastest way to get the layercount.
    if (id) {
        l = 7 - (__builtin_clzl(id)/8);
    } else {
        l = 0;
    }
    //
    //  Here we should have the number of needed layers
    //  in l
    //  If the id expects more layers than are in the tree we
    //  are done yet. The id cannot be in the tree for removal.
    //  Other way round. If the layer in the root is larger or equal
    //  as needed we can proceed.
    if (layer >= l) {
        Remove((uint8_t*)(&id)+layer);
    }
}
//
//  This method expect to be started on the root-element.
tNodeEntry* CPtrNode::Find(uint64_t id) {
    tNodeEntry*   retval = nullptr;
    uint8_t       l;
    //
    //  Find the needed layercount for the id.
    //
    //  This should be the fastest way to get the layercount.
    if (id) {
        l = 7 - (__builtin_clzl(id)/8);
    } else {
        l = 0;
    }
    //
    //  Here we should have the number of needed layers
    //  in l
    //  If the id expects more layers than are in the tree we
    //  are done yet. The id cannot be in the tree for removal.
    //  Other way round. If the layer in the root is larger or equal
    //  as needed we can proceed.
    if (layer >= l) {
        retval = Find((uint8_t*)(&id) + layer);
    }
    return retval;
}

