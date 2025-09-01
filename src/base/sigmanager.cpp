#include "sigmanager.h"

CSigManager SigManager;

CSigManager::CSigManager()
{
    pthread_spin_init(&PoolLock, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_lock(&PoolLock);
    for (int i=SIGRTMIN; i <=SIGRTMAX; ++i) {
        Pool.insert(i);
    }
    pthread_spin_unlock(&PoolLock);
}

int CSigManager::Get() {
    std::set<int>::iterator si;


    pthread_spin_lock(&PoolLock);
    si=Pool.begin();
    if (si != Pool.end()) {
        int i=*si;

        Pool.erase(*si);
        pthread_spin_unlock(&PoolLock);
        return (i);
    } else {
    }
    pthread_spin_unlock(&PoolLock);
    return -1;
}

void CSigManager::Release(int aSignal) {
    if ((aSignal >= SIGRTMIN) && (aSignal <= SIGRTMAX)) {
        pthread_spin_lock(&PoolLock);
        Pool.insert(aSignal);
        pthread_spin_unlock(&PoolLock);
    }
}
