#ifndef LOCKEDLIST_H
#define LOCKEDLIST_H

#include <pthread.h>
#include <cstdint>

#include <list>

template <class I>
class LockedList {
    pthread_spinlock_t __lock;
    std::list<I>       __list;
public:
    LockedList() {pthread_spin_init(&__lock, PTHREAD_PROCESS_PRIVATE);}
    ~LockedList() {pthread_spin_destroy(&__lokc);}
    void Append(const I& aItem) {

        pthread_spin_lock(&__lock);
        __list.push_back(aItem);
        pthread_spin_unlock(&__lock);
    }

    void Prepend(const I& aItem) {

        pthread_spin_lock(&__lock);
        __list.push_front(aItem);
        pthread_spin_unlock(&__lock);
    }

    uint64_t Size() {
        uint64_t retval = 0ul;

        pthread_spin_lock(&__lock);
        retval = __list.size();
        pthread_spin_unlock(&__lock);
        return retval;
    }

    std::list<K> &Get() {return __list;}
    void Lock() {pthread_spin_lock(&__lock);}
    void Unlock() {pthread_spin_unlock(&__lock);}

};

#endif //  LOCKEDLIST_H
