#include <algorithm>
#include "CStringToObject.h"

CStringToObject::CStringToObject() {
   pthread_spin_init(&MapLock, PTHREAD_PROCESS_PRIVATE);
}

CStringToObject::~CStringToObject() {
}

bool CStringToObject::Add(const std::string& aString, tSimObj* aUser) {
    bool                                      retval = false;
    std::string                               lower;
    std::map<std::string, tSimObj*>::iterator ui;
    /*
     * We do only add strings that have a length.
     */
    if (!aString.empty()) {
        lower.reserve(aString.size()+1);
        lower.resize(aString.size());
        std::transform(aString.begin(), aString.end(), lower.begin(), ::tolower);

        pthread_spin_lock(&MapLock);
        ui = ToObject.find(lower);
        if (ui == ToObject.end()) {
            retval = true;
            ToObject.insert(std::pair<std::string, tSimObj*>(lower, aUser));
        }
        pthread_spin_unlock(&MapLock);
    }
    return retval;
}

void CStringToObject::Remove(const std::string& aString) {
    std::string lower;
    /*
     * We only try to remove data that have a length.
     */
    if (!aString.empty()) {
        lower.reserve(aString.size()+1);
        lower.resize(aString.size());
        std::transform(aString.begin(), aString.end(), lower.begin(), ::tolower);

        pthread_spin_lock(&MapLock);
        ToObject.erase(lower);
        pthread_spin_unlock(&MapLock);
    }
}

tSimObj* CStringToObject::Find(const std::string& aString) {
    tSimObj*                                  retval = 0;
    std::map<std::string, tSimObj*>::iterator ui;
    std::string                               lower;
    /*
     *  We only search for data with a length.
     */
    if (!aString.empty()) {
        lower.reserve(aString.size()+1);
        lower.resize(aString.size());
        std::transform(aString.begin(), aString.end(), lower.begin(), ::tolower);

        pthread_spin_lock(&MapLock);
        ui = ToObject.find(lower);
        if (ui != ToObject.end()) {
            retval = ui->second;
        }
        pthread_spin_unlock(&MapLock);
    }
    return retval;
}

std::list<tSimObj*> CStringToObject::SubSet(const std::string &aKey, uint64_t aStart, uint64_t aMaxCount) {
    std::list<tSimObj*>                       retval;
    std::map<std::string, tSimObj*>::iterator i;
    /*
     * Skip a number of entries.
     */
    pthread_spin_lock(&MapLock);
    for (i = ToObject.upper_bound(aKey); (i != ToObject.end()) && (aStart > 0); ++i, aStart--);
    /*
     * If we are before the end start creation of the list.
     */
    if (i != ToObject.end()) {
        for (; (i!= ToObject.end()) && (aMaxCount>0); ++i, aMaxCount--) {
            retval.push_back(i->second);
        }
    }
    pthread_spin_unlock(&MapLock);

    return retval;
}

std::list<tSimObj*> CStringToObject::SubSet(const std::string &aFrom, const std::string& aTo, uint64_t aStart, uint64_t aMaxCount) {
    std::list<tSimObj*>                       retval;
    std::map<std::string, tSimObj*>::iterator i;
    /*
     * Skip a number of entries.
     */
    pthread_spin_lock(&MapLock);
    for (i = ToObject.upper_bound(aFrom); (i != ToObject.end()) && (aStart > 0); ++i, aStart--)
    {
        /*
         * Early out because we skip beyond the aTo-key
         */
        if (i->first >= aTo) {
            i = ToObject.end();
            break;
        }
    }
    /*
     * If we are before the end start creation of the list.
     */
    if (i != ToObject.end()) {
        for (; (i!= ToObject.end()) && (aMaxCount>0); ++i, aMaxCount--) {
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

uint64_t CStringToObject::Size() {
    uint64_t retval;

    pthread_spin_lock(&MapLock);
    retval = ToObject.size();
    pthread_spin_unlock(&MapLock);

    return retval;
}
