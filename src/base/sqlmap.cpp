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

#include "sqlmap.h"

CSQLMap::CSQLMap(uint64_t aStart)
{
    pthread_spin_init(&sqllock, PTHREAD_PROCESS_PRIVATE);
    newindex = aStart;
}

uint64_t CSQLMap::Add(tMsgPtr aMsg) {

    pthread_spin_lock(&sqllock);
    uint64_t retval = ++newindex;

    SQLMap[newindex] = aMsg;

    pthread_spin_unlock(&sqllock);

    return retval;
}

tMsgPtr CSQLMap::Remove(uint64_t aId) {
    tMsgPtr retval;

    pthread_spin_lock(&sqllock);
    std::map<uint64_t, tMsgPtr>::iterator si = SQLMap.find(aId);

    if (si != SQLMap.end()) {
        retval = si->second;
        SQLMap.erase(si);
    }
    pthread_spin_unlock(&sqllock);
    return retval;
}

void CSQLMap::Clear() {
    pthread_spin_lock(&sqllock);
    SQLMap.clear();
    pthread_spin_unlock(&sqllock);
}
