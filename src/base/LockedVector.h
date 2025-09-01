#ifndef LOCKEDVECTOR_H
#define LOCKEDVECTOR_H

#include <pthread.h>
#include <cstdint>

#include <stdexcept>
#include <vector>

template <class I>
class LockedVector {
    pthread_spinlock_t __lock;
    std::vector<I>     __vector;
public:
    LockedVector() {pthread_spin_init(&__lock, PTHREAD_PROCESS_PRIVATE);}
    ~LockedVector() {pthread_spin_destroy(&__lock);}
    void Append(const I& aItem) {

        pthread_spin_lock(&__lock);
        __vector.push_back(aItem);
        pthread_spin_unlock(&__lock);
    }

    void Prepend(const I& aItem) {

        pthread_spin_lock(&__lock);
        __vector.push_front(aItem);
        pthread_spin_unlock(&__lock);
    }

    uint64_t Size() {
        uint64_t retval = 0ul;

        pthread_spin_lock(&__lock);
        retval = __vector.size();
        pthread_spin_unlock(&__lock);
        return retval;
    }

    I& operator[](size_t pos) {
        I retval;

        pthread_spin_lock(&__lock);
        if ((pos >= __vector.size()) || (__vector.size() == 0)) {
            throw std::out_of_range();
        }
        retval = __vector[pos];

        pthread_spin_unlock(&__lock);

        return retval;

    }

    std::vector<I> &Get() {return __vector;}
    void Lock() {pthread_spin_lock(&__lock);}
    void Unlock() {pthread_spin_unlock(&__lock);}

};

#endif //  LOCKEDVECTOR_H
