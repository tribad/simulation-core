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
