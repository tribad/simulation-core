#ifndef CSIGMANAGER_H
#define CSIGMANAGER_H

#include <set>
#include <sys/signal.h>
#include <pthread.h>

class CSigManager
{
public:
    CSigManager();
    int Get();
    void Release(int aSignal);
private:
    pthread_spinlock_t PoolLock;
    std::set<int>      Pool;
};

extern CSigManager SigManager;

#endif // CSIGMANAGER_H
