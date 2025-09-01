#ifndef LOCKEDSET_H
#define LOCKEDSET_H

#include <pthread.h>
#include <cstdint>

#include <set>

template <class K>
class LockedSet {
    pthread_spinlock_t __lock;
    std::set<K>        __set;
public:
    LockedSet() {pthread_spin_init(&__lock, PTHREAD_PROCESS_PRIVATE);}
    ~LockedSet() {pthread_spin_destroy(&__lock);}
    bool Add(const K& aKey) {
        bool retval = false;

        pthread_spin_lock(&__lock);
        auto i = __set.find(aKey);

        if (i == __set.end()) {
            retval = true;
            __set.insert(aKey);
        }
        pthread_spin_unlock(&__lock);
        return retval;
    }
    void Remove(const K& aKey) {
        pthread_spin_lock(&__lock);
        __set.erase(aKey);
        pthread_spin_unlock(&__lock);
    }
    bool Check(const K& aKey) {
        bool retval;

        pthread_spin_lock(&__lock);
        auto i = __set.find(aKey);

        retval = (i != __set.end());

        pthread_spin_unlock(&__lock);
        return retval;
    }
    uint64_t Size() {
        uint64_t retval = 0ul;

        pthread_spin_lock(&__lock);
        retval = __set.size();
        pthread_spin_unlock(&__lock);
        return retval;
    }


    std::set<K> &Get() {return __set;}
    void Lock() {pthread_spin_lock(&__lock);}
    void Unlock() {pthread_spin_unlock(&__lock);}
};

#endif //  LOCKEDSET_H
