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

