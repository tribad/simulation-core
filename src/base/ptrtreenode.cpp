// **************************************************************************
//
// Modul-Name        : ptrtreenode.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 23.11.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#include "ptrnode.h"
#include "ptrtreenode.h"
#include "ptrleafnode.h"

CTreeNode::CTreeNode(uint64_t aVersion) {
    mVersion = aVersion;
    std::memset(mEntry, 0, sizeof(mEntry));
}

CTreeNode::CTreeNode(CPtrNode *down) : CTreeNode () {
    //
    //  Setup the layer id for the new layer.
    layer       = down->layer + 1;
    mEntry[0]   = down;
    mVersion    = down->mVersion;
    //
    //  Only if the old root is already full we set the unused bit.
    if (down->isFull()) {
        unused[0] = ~(1ul);
    }
    down->up = this;
}

CTreeNode::~CTreeNode() {
    for (unsigned i = 0; i < NODESIZE_COUNT; ++i) {
        if (mEntry[i] != nullptr) {
            delete mEntry[i];
        }
    }
}

void CTreeNode::Allocate(uint8_t* id) {
    CPtrNode* node   = nullptr;
    //
    //  Getting the id where we may find some slots
    //  to allocate new ids from.
    *id = getUnused();
    //
    //  This maybe the node.
    node = mEntry[*id];
    //
    //  Check if the entry already has a node
    if (node == nullptr) {
        //
        //  Attach a new node
        if (layer > 1u) {
            node = mEntry[*id] = new CTreeNode(mVersion);
        } else {
            node = mEntry[*id] = new CLeafNode(mVersion);
        }
        node->up    = this;
        node->layer = layer - 1u;
    }

    node->Allocate(id - 1);
    //
    //  We need to check the space in the node down before
    //  allocating a new id.
    if (node->full) {
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
}

tNodeEntry* CTreeNode::Insert(uint8_t *id, void *obj) {
    tNodeEntry* retval = nullptr;
    CPtrNode*   node   = mEntry[*id];
    //
    //  Need new node here
    if (node == nullptr) {
        //
        //  As we create a node one down we check if we are on a
        //  node some layer above the leave nodes.
        //  In layer 1 we create leaf node
        if (layer > 1u) {
            node = mEntry[*id] = new CTreeNode(mVersion);
        } else {
            node = mEntry[*id] = new CLeafNode(mVersion);
            ((CLeafNode*)node)->Chain();
        }
        node->up    = this;
        node->layer = layer - 1u;
    }
    retval = node->Insert(id-1, obj);
    //
    //  Check the full flag after insertion.
    //  If the node is full we update the unused map to speedup search for
    //  free ids.
    if (node->full) {
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
    return retval;
}

tNodeEntry* CTreeNode::Find(uint8_t *id) {
    tNodeEntry* retval = nullptr;

    if (mEntry[*id] != nullptr) {
        retval = mEntry[*id]->Find(id-1);
    }
    return retval;
}

bool CTreeNode::Remove(uint8_t *id) {
    bool      retval = full;
    CPtrNode*      r = mEntry[*id];

    if (r != nullptr) {
        //
        //  As the remove method returns the full flag before removal
        //  We can reduce code.
        if (r->Remove(id-1)) {
            //
            //  Set the free bit.
            uint8_t index = (*id) >> 6u;
            uint8_t bit   = (*id) & (0x3f);

            uint64_t mask = (1ul << bit);

            unused[index] |= mask;
            //
            // checking before resetting needs longer that
            // setting anyways.
            full = false;
        }
    }
    return retval;
}
