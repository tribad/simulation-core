#include "CIdToObject.h"

CIdToObject::CIdToObject() {
   pthread_spin_init(&MapLock, PTHREAD_PROCESS_PRIVATE);
}

CIdToObject::~CIdToObject() {
}

bool CIdToObject::Add(const uint64_t aId, tSimObj* aUser) {
    bool retval = false;
    std::map<uint64_t, tSimObj*>::iterator ui;

    pthread_spin_lock(&MapLock);
    ui = ToObject.find(aId);
    if (ui == ToObject.end()) {
        retval = true;
        ToObject.insert(std::pair<uint64_t, tSimObj*>(aId, aUser));
    }
    pthread_spin_unlock(&MapLock);

    return retval;
}

void CIdToObject::Remove(const uint64_t aId) {
    pthread_spin_lock(&MapLock);
    ToObject.erase(aId);
    pthread_spin_unlock(&MapLock);
}

tSimObj* CIdToObject::Find(const uint64_t aId) {
    tSimObj* retval = 0;
    std::map<uint64_t, tSimObj*>::iterator ui;   
    
    pthread_spin_lock(&MapLock);
    ui = ToObject.find(aId);
    if (ui != ToObject.end()) {
        retval = ui->second;
    }
    pthread_spin_unlock(&MapLock);

    return retval;
}

std::list<tSimObj*> CIdToObject::SubSet(const uint64_t aKey, uint64_t aStart, uint64_t aMaxCount) {
    std::list<tSimObj*>                    retval;
    std::map<uint64_t, tSimObj*>::iterator i;
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

std::list<tSimObj*> CIdToObject::SubSet(const uint64_t aFrom, const uint64_t aTo, uint64_t aStart, uint64_t aMaxCount) {
    std::list<tSimObj*>                    retval;
    std::map<uint64_t, tSimObj*>::iterator i;
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

uint64_t CIdToObject::Size() {
    uint64_t retval;

    pthread_spin_lock(&MapLock);
    retval = ToObject.size();
    pthread_spin_unlock(&MapLock);

    return retval;
}
