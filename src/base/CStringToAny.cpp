#include <algorithm>
#include "CStringToAny.h"

CStringToAny::CStringToAny() {
   pthread_spin_init(&MapLock, PTHREAD_PROCESS_PRIVATE);
}

CStringToAny::~CStringToAny() {
}

bool CStringToAny::Add(const std::string& aString, void* aUser) {
    bool                                      retval = false;
    std::string                               lower;
    std::map<std::string, void*>::iterator ui;
    /*
     * We do only add strings that have a length.
     */
    if (!aString.empty()) {
        lower.reserve(aString.size()+1);
        lower.resize(aString.size());
        std::transform(aString.begin(), aString.end(), lower.begin(), ::tolower);

        pthread_spin_lock(&MapLock);
        ui = To.find(lower);
        if (ui == To.end()) {
            retval = true;
            To.insert(std::pair<std::string, void*>(lower, aUser));
        }
        pthread_spin_unlock(&MapLock);
    }
    return retval;
}

void CStringToAny::Remove(const std::string& aString) {
    std::string lower;
    /*
     * We only try to remove data that have a length.
     */
    if (!aString.empty()) {
        lower.reserve(aString.size()+1);
        lower.resize(aString.size());
        std::transform(aString.begin(), aString.end(), lower.begin(), ::tolower);

        pthread_spin_lock(&MapLock);
        To.erase(lower);
        pthread_spin_unlock(&MapLock);
    }
}

void* CStringToAny::Find(const std::string& aString) {
    void*                                  retval = 0;
    std::map<std::string, void*>::iterator ui;
    std::string                            lower;
    /*
     *  We only search for data with a length.
     */
    if (!aString.empty()) {
        lower.reserve(aString.size()+1);
        lower.resize(aString.size());
        std::transform(aString.begin(), aString.end(), lower.begin(), ::tolower);

        pthread_spin_lock(&MapLock);
        ui = To.find(lower);
        if (ui != To.end()) {
            retval = ui->second;
        }
        pthread_spin_unlock(&MapLock);
    }
    return retval;
}

std::list<void*> CStringToAny::SubSet(const std::string &aKey, uint64_t aStart, uint64_t aMaxCount) {
    std::list<void*>                       retval;
    std::map<std::string, void*>::iterator i;
    /*
     * Skip a number of entries.
     */
    pthread_spin_lock(&MapLock);
    for (i = To.upper_bound(aKey); (i != To.end()) && (aStart > 0); ++i, aStart--);
    /*
     * If we are before the end start creation of the list.
     */
    if (i != To.end()) {
        for (; (i!= To.end()) && (aMaxCount>0); ++i, aMaxCount--) {
            retval.push_back(i->second);
        }
    }
    pthread_spin_unlock(&MapLock);
    return retval;
}

std::list<void*> CStringToAny::SubSet(const std::string &aFrom, const std::string& aTo, uint64_t aStart, uint64_t aMaxCount) {
    std::list<void*>                       retval;
    std::map<std::string, void*>::iterator i;
    /*
     * Skip a number of entries.
     */
    pthread_spin_lock(&MapLock);
    for (i = To.upper_bound(aFrom); (i != To.end()) && (aStart > 0); ++i, aStart--)
    {
        /*
         * Early out because we skip beyond the aTo-key
         */
        if (i->first >= aTo) {
            i = To.end();
            break;
        }
    }
    /*
     * If we are before the end start creation of the list.
     */
    if (i != To.end()) {
        for (; (i!= To.end()) && (aMaxCount>0); ++i, aMaxCount--) {
            if (i->first < aTo) {
                retval.push_back(i->second);
            } else {
                break;
            }
        }
    }
    pthread_spin_unlock(&MapLock);

    return retval;
}

uint64_t CStringToAny::Size() {
    uint64_t retval;

    pthread_spin_lock(&MapLock);
    retval = To.size();
    pthread_spin_unlock(&MapLock);

    return retval;
}
