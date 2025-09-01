#ifndef LOCKEDMAP_H
#define LOCKEDMAP_H

#include <pthread.h>
#include <cstdint>

#include <map>

template <class K, class I>
class LockedMap {
    pthread_spinlock_t __lock;
    std::map<K, I>     __map;
public:
    LockedMap() {pthread_spin_init(&__lock, PTHREAD_PROCESS_PRIVATE);}
    ~LockedMap() {pthread_spin_destroy(&__lock);}
    bool Add(const K& aKey, const I& aItem) {
        bool retval = false;

        pthread_spin_lock(&__lock);
        auto i = __map.find(aKey);

        if (i == __map.end()) {
            retval = true;
            __map.insert(std::pair<K, I>(aKey, aItem));
        }
        pthread_spin_unlock(&__lock);
        return retval;
    }
    void Remove(const K& aKey) {
        pthread_spin_lock(&__lock);
        __map.erase(aKey);
        pthread_spin_unlock(&__lock);
    }
    bool Check(const K& aKey) {
        bool retval;

        pthread_spin_lock(&__lock);
        auto i = __map.find(aKey);

        retval = (i != __map.end());

        pthread_spin_unlock(&__lock);
        return retval;
    }

    I Get(const K& aKey) {
        I retval;

        pthread_spin_lock(&__lock);
        auto i = __map.find(aKey);

        if (i != __map.end()) {
            retval = i->second;
        }

        pthread_spin_unlock(&__lock);
        return retval;
    }

    //
    //  Gives the data at a specific position in the map.
    I At(const K& aKey) {
        I retval;

        pthread_spin_lock(&__lock);
        retval = __map[aKey];
        pthread_spin_unlock(&__lock);

        return retval;
    }
    //
    //  Gives the reference to the specific element in the map.
    I& operator[](const K& aKey) {
        pthread_spin_lock(&__lock);
        I& retval (__map[aKey]);
        pthread_spin_unlock(&__lock);

        return retval;
    }

    uint64_t Size() {
        uint64_t retval = 0ul;

        pthread_spin_lock(&__lock);
        retval = __map.size();
        pthread_spin_unlock(&__lock);
        return retval;
    }

    std::map<K, I> &Get() {return __map;}
    bool Empty() {return (Size() == 0);}
    void Lock() {pthread_spin_lock(&__lock);}
    void Unlock() {pthread_spin_unlock(&__lock);}

};

#endif //  LOCKEDMAP_H
