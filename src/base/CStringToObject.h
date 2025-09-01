#ifndef CSTRINGTOOBJECT_H
#define CSTRINGTOOBJECT_H

#include <pthread.h>
#include <string>
#include <map>
#include <list>
#include <simobj.h>

class CStringToObject {
public:
    CStringToObject();
    ~CStringToObject();
    bool Add(const std::string& aString, tSimObj* aUser);
    void Remove(const std::string& aString);
    tSimObj* Find(const std::string& aString);
    std::list<tSimObj*> SubSet(const std::string& aKey, uint64_t aStart, uint64_t aMaxCount);
    std::list<tSimObj*> SubSet(const std::string& aFrom, const std::string& aTo, uint64_t aStart, uint64_t aMaxCount);
    uint64_t Size();
private:
    pthread_spinlock_t              MapLock;
    std::map<std::string, tSimObj*> ToObject;
};

#endif

