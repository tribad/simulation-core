#ifndef IDTOOBJECT_H
#define IDTOOBJECT_H

#include <pthread.h>
#include <stdint.h>
#include <map>
#include <list>
#include <simobj.h>

class CIdToObject {
public:
    CIdToObject();
    ~CIdToObject();
    bool Add(const uint64_t aId, tSimObj* aUser);
    void Remove(const uint64_t aId);
    tSimObj* Find(const uint64_t aId);
    std::list<tSimObj*> SubSet(const uint64_t aKey, uint64_t aStart, uint64_t aMaxCount);
    std::list<tSimObj*> SubSet(const uint64_t aFrom, uint64_t aTo, uint64_t aStart, uint64_t aMaxCount);
    uint64_t Size();
private:
    pthread_spinlock_t           MapLock;
    std::map<uint64_t, tSimObj*> ToObject;    
};


#endif
