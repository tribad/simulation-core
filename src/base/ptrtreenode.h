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
