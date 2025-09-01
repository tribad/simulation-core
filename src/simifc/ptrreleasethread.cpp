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

#include <unistd.h>
#include "ptrreleasethread.h"
#include "pointerindex.h"
#include "coreifc.h"
#include "ptrnode.h"
#include "ptrleafnode.h"

#include "ptrroot.h"

#define RELEASE_CYCLE 1u //  This is the release cycle in seconds.

#ifdef PTRRELEASETHREAD_NO_LOOP
#define RUNVERSION() releasecounter+=RunVersion(versions[x]);
#else
#define RUNVERSION() (void)RunVersion(versions[x]);
#endif

extern CCoreIfc* __coreIfc;

CPtrReleaseThread::CPtrReleaseThread()
{

}

CPtrReleaseThread::~CPtrReleaseThread () {

}
//
//  This is the Run-Loop of the thread.
long CPtrReleaseThread::Run() {
    Running = true;

#ifndef PTRRELEASETHREAD_NO_LOOP
    for(;Running;) {
        sleep(RELEASE_CYCLE);
#else
    size_t releasecounter = 0ul;
#endif
        //
        //  Need an array for the version numbers
        int    versions[CPtrRoot::mRootMap.Size()];
        size_t vcount = 0ul;
        //
        //  Get the version numbers under lock.
        CPtrRoot::mRootMap.Lock();
        for (std::map<uint64_t, CPtrRoot>::iterator i = CPtrRoot::mRootMap.Get().begin();
             i != CPtrRoot::mRootMap.Get().end(); ++i) {
            versions[vcount++] = i->first;
        }
        CPtrRoot::mRootMap.Unlock();
        //
        //  No we can check the versions for objects to release.
        for (size_t x = 0ul; x < vcount; ++x) {
            RUNVERSION();
        }
#ifndef PTRRELEASETHREAD_NO_LOOP
    }
    return 0;
#else
        return releasecounter;
#endif
}

size_t CPtrReleaseThread::RunVersion(uint64_t aVersion) {
    CLeafNode* leaf = CPtrRoot::mRootMap[aVersion].GetStart();
    size_t     releasecounter = 0ul;

    while (leaf != nullptr) {
        for (size_t i = 0ul; i < NODESIZE_COUNT; ++i) {
            tSimObj* drop = (tSimObj*)(leaf->Get(i)->Drop(__coreIfc->relCycle));
            if (drop != nullptr) {
                delete drop;
                releasecounter++;
            } else {
                drop =(tSimObj*)(leaf->Get(i)->Free());
                if (drop != nullptr) {
                    delete drop;
                    releasecounter++;
                }
            }
        }
        leaf = leaf->GetNext();
    }
    return releasecounter;
}
