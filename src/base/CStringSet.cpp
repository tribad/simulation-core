// MIT License
//
// Copyright (c) 2017 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#include <algorithm>
#include "CStringSet.h"

CStringSet::CStringSet() {
   pthread_spin_init(&SetLock, PTHREAD_PROCESS_PRIVATE);
}

CStringSet::~CStringSet() {
}

bool CStringSet::Add(const std::string& aString) {
    bool                            retval = false;
    std::string                     lower;
    std::set<std::string>::iterator ui;
    /*
     * We do only add strings that have a length.
     */
    if (!aString.empty()) {
        lower.reserve(aString.size()+1);
        lower.resize(aString.size());
        std::transform(aString.begin(), aString.end(), lower.begin(), ::tolower);

        pthread_spin_lock(&SetLock);
        ui = TheSet.find(lower);
        if (ui == TheSet.end()) {
            retval = true;
            TheSet.insert(lower);
        }
        pthread_spin_unlock(&SetLock);
    }
    return retval;
}

void CStringSet::Remove(const std::string& aString) {
    std::string lower;
    /*
     * We only try to remove data that have a length.
     */
    if (!aString.empty()) {
        lower.reserve(aString.size()+1);
        lower.resize(aString.size());
        std::transform(aString.begin(), aString.end(), lower.begin(), ::tolower);

        pthread_spin_lock(&SetLock);
        TheSet.erase(lower);
        pthread_spin_unlock(&SetLock);
    }
}

bool CStringSet::Check(const std::string& aString) {
    bool                            retval = false;
    std::set<std::string>::iterator ui;
    std::string                     lower;
    /*
     *  We only search for data with a length.
     */
    if (!aString.empty()) {
        lower.reserve(aString.size()+1);
        lower.resize(aString.size());
        std::transform(aString.begin(), aString.end(), lower.begin(), ::tolower);

        pthread_spin_lock(&SetLock);
        ui = TheSet.find(lower);
        pthread_spin_unlock(&SetLock);
        if (ui != TheSet.end()) {
            retval = true;
        }
    }
    return retval;
}

uint64_t CStringSet::Size() {
    uint64_t retval;

    pthread_spin_lock(&SetLock);
    retval = TheSet.size();
    pthread_spin_unlock(&SetLock);

    return retval;
}

