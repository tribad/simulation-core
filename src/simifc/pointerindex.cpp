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

#include "pointerindex.h"
#include "ptrnode.h"
#include "msgqueue.h"
#include "sharedmsgqueue.h"
#include "coreifc.h"
#include "simobjfactory.h"

#include "sqlrecordset.h"

#include "ptrreleasethread.h"
#include "ptrroot.h"
#include "ptrleafnode.h"

#include <tSigObjectLoadReq.h>
//
//
extern CCoreIfc* __coreIfc;
//
//
CPtrReleaseThread* CPointerIndex::mPtrRelease = nullptr;
//
//  With the contructor we create the release thread as well
//  This is needed for the on-demand loading mechanism to release
//  unused objects.
CPointerIndex::CPointerIndex()
{
    if (mPtrRelease == nullptr) {
        mPtrRelease = new CPtrReleaseThread;

        mPtrRelease->Create();
        while (!mPtrRelease->Running) ;
    }
}

CPointerIndex::~CPointerIndex() {
    if (mPtrRelease != nullptr) {
        //
        //  The default behaviour is to delete the thread object at exit.
        mPtrRelease->Stop();
    }
}

tNodeEntry* CPointerIndex::Add(tSimObj *obj, uint64_t vers) {
    tNodeEntry* retval;

    if (obj != nullptr) {
        //
        //  Lock the map with the root objects.
        CPtrRoot::mRootMap.Lock();

        std::map<uint64_t, CPtrRoot>::iterator root = CPtrRoot::mRootMap.Get().find(vers);
        //
        //  If we do not have a root element for this version we create one.
        if (root == CPtrRoot::mRootMap.Get().end()) {
            CPtrRoot::mRootMap.Get().insert(std::pair<uint64_t, CPtrRoot>(vers, CPtrRoot(new CLeafNode(vers))));
        }
        //
        //  After this point all locking is done on the individual root objects.
        //  So we can unlock the map.
        CPtrRoot::mRootMap.Unlock();
        //
        //  Make sure the root really has a root.
        CPtrRoot::mRootMap[vers].ValidateRoot(vers);
        //
        //  As we do now have a root object we can allocate a
        //  new id if needed.
        if (obj->objid == INVALID_OBJID) {
            //
            //  As the parameter to Allocate is a non-const reference
            //  the new object id will be put into the object.
            obj->objid = CPtrRoot::mRootMap[vers].Allocate();
        }
        //
        //  The root maybe moved if new layers gets added.
        retval     = CPtrRoot::mRootMap[vers].Insert(obj->objid, obj);
    } else {
        retval = nullptr;
    }
    return retval;
}

void CPointerIndex::Remove(tSimObj *obj, uint64_t vers) {
    //
    //  Dont work on nullptr
    if (obj != nullptr) {
        if (obj->objid != INVALID_OBJECTID) {
            //  If we do not have a root element for this version we
            //  assume it is already deleted.
            if (CPtrRoot::mRootMap.Check(vers)) {
                CPtrRoot::mRootMap[vers].Remove(obj->objid);
                delete obj;
            }
        }
    }
}

uint64_t CPointerIndex::Reserve(uint64_t id, uint64_t vers) {
    //
    //  Lock the map with the root objects.
    CPtrRoot::mRootMap.Lock();

    std::map<uint64_t, CPtrRoot>::iterator root = CPtrRoot::mRootMap.Get().find(vers);
    //
    //  If we do not have a root element for this version we create one.
    if (root == CPtrRoot::mRootMap.Get().end()) {
        CPtrRoot::mRootMap.Get().insert(std::pair<uint64_t, CPtrRoot>(vers, CPtrRoot(new CLeafNode(vers))));
    }
    //
    //  After this point all locking is done on the individual root objects.
    //  So we can unlock the map.
    CPtrRoot::mRootMap.Unlock();
    //
    //  Make sure the root really has a root.
    CPtrRoot::mRootMap[vers].ValidateRoot(vers);

    if (id == INVALID_OBJID) {
        id = CPtrRoot::mRootMap[vers].Allocate();
    } else {
        (void)CPtrRoot::mRootMap[vers].Insert(id, nullptr);
    }
    return id;
}

bool CPointerIndex::Release(uint64_t id, uint64_t vers) {
    bool retval = true;

    if (id != INVALID_OBJECTID) {
        //
        //  If we do not have a root element for this version we
        //  think its released. ;)
        if (CPtrRoot::mRootMap.Check(vers)) {
            CPtrRoot::mRootMap[vers].Remove(id);
        } else {
            retval = false;
        }
    } else {
        retval = false;
    }
    return retval;
}

tNodeEntry* CPointerIndex::Find(uint64_t id, uint64_t vers) {
    tNodeEntry* retval = nullptr;
    //
    //  If we do not have a root element we return 0
    if (CPtrRoot::mRootMap.Check(vers)) {
        retval = CPtrRoot::mRootMap[vers].Find(id);
    }
    return retval;
}

tNodeEntry* CPointerIndex::Get(uint64_t aId, uint64_t vers) {
    tNodeEntry* retval = nullptr;

    retval = Find(aId, vers);
    //
    //  If the Id could not be found a demand loading cannot be done.
    if (retval != nullptr) {
        //
        //  Check if the object is available.
        if (retval->Get() == nullptr) {
            //
            //  Here we create a communication channel between the simulation core and this specific threads waiting point.
            std::shared_ptr<tSigObjectLoadReq>    loadreq(new tSigObjectLoadReq);
            //
            //  Creation of the two endpoints.
            std::future<tSQLRecordSet*> loadchannel;
            std::promise<tSQLRecordSet*> promise;

            loadchannel = promise.get_future();
            //
            //  The promise we send with the message.
            loadreq->loadchannel = std::move(promise);
            loadreq->objectid    = aId;
            loadreq->handle      = __coreIfc->stdbhandle;

            if (__coreIfc->stdb != nullptr) {
                __coreIfc->stdb->Put(loadreq);
                //
                //  Wait until the message gets answered.
                tSQLRecordSet* data = loadchannel.get();

                if (data != nullptr) {
                    //
                    //  Get the typeid
                    uint8_t  done     = 0u;
                    uint64_t tid      = 0ul;
                    uint64_t droptime = UINT64_MAX;

                    for (size_t i = 0ul; (i < data->Count) && (done != 0x03); ++i) {
                        //
                        //  The valueid zero is for the type to be transfered.
                        if (((*data)[i][0] == 0ul) && ((*data)[i][1] == 0ul)) {
                            tid = (*data)[i][2];
                            done |= 0x01;
                        }
                        //
                        //  Check for the droptime at valueid == 1ul
                        if (((*data)[i][0] == 0ul) && ((*data)[i][1] == 1ul)) {
                            droptime = (*data)[i][2];
                            done |= 0x02;
                        }
                    }
                    if (tid != 0ul) {
                        //
                        //  Typeid first. Its different to any other place
                        tSimObj* newobj = factory_createobj(tid, aId, droptime);
                        if (newobj != nullptr) {

                            for (size_t i = 0; i < data->Count; ++i) {
                                if ((*data)[i][0] != 0) {
                                    newobj->InitMember((*data)[i][0], (*data)[i][1], (*data)[i][2]);
                                }
                            }
                        }
                        if (CPtrRoot::mRootMap.Check(vers)) {
                            retval = CPtrRoot::mRootMap[vers].Insert(aId, newobj);
                        }
                        //
                        //  Release the recordset.
                        delete data;
                        //
                        //  Set the next timeout time.
                        //  if we have something not unlimited.
                        if (newobj->droptime != UINT64_MAX) {
                            retval->Set(newobj->droptime+__coreIfc->relCycle);
                        }
                    }
                }
            } else {
                retval = nullptr;
            }
        } else {
            //
            //  Set the next timeout time.
            if (((tSimObj*)retval->Get())->droptime != UINT64_MAX) {
                retval->Set(((tSimObj*)retval->Get())->droptime+__coreIfc->relCycle);
            }
        }
    }
    return retval;
}

void CPointerIndex::WindUpTimer(tNodeEntry *aNode, uint64_t aDropTimeOut) {
    if (aNode != nullptr) {
        aNode->Set(__coreIfc->relCycle + aDropTimeOut);
    }
}
