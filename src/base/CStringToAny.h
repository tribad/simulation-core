#ifndef CSTRINGTOANY_H
#define CSTRINGTOANY_H

#include <cstdint>
#include <pthread.h>
#include <string>
#include <list>
#include <map>

class CStringToAny {
public:
    CStringToAny();
    ~CStringToAny();
    bool Add(const std::string& aString, void* aUser);
    void Remove(const std::string& aString);
    void* Find(const std::string& aString);
    std::list<void*> SubSet(const std::string& aKey, uint64_t aStart, uint64_t aMaxCount);
    std::list<void*> SubSet(const std::string& aFrom, const std::string& aTo, uint64_t aStart, uint64_t aMaxCount);
    uint64_t Size();
private:
    pthread_spinlock_t           MapLock;
    std::map<std::string, void*> To;
};

#endif

