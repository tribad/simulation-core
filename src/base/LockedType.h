// **************************************************************************
//
// Modul-Name        :
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     :
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef LOCKEDTYPE_H
#define LOCKEDTYPE_H



template <class I>
class LockedType {
    pthread_spinlock_t __lock;
    I                  __value;
public:
    LockedType() {pthread_spin_init(&__lock, PTHREAD_PROCESS_PRIVATE);}
    LockedType(const I& v) : __value(v) {
        pthread_spin_init(&__lock, PTHREAD_PROCESS_PRIVATE);
    }
    ~LockedType() {pthread_spin_destroy(&__lock);}

    void Lock() {pthread_spin_lock(&__lock);}
    void Unlock() {pthread_spin_unlock(&__lock);}

    operator I () {
        I retval;
        pthread_spin_lock(&__lock);
        retval = __value;
        pthread_spin_unlock(&__lock);
        return retval;
    }
    const I& operator=(const I & v) {
        pthread_spin_lock(&__lock);
        __value = v;
        pthread_spin_unlock(&__lock);
        return this;
    }


};


#endif // LOCKEDTYPE_H
