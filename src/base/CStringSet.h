#ifndef STRINGSET_H
#define STRINGSET_H

#include <cstdint>
#include <pthread.h>
#include <set>
#include <string>

class CStringSet {
public:
    CStringSet();
    ~CStringSet();
    bool Add(const std::string& aSting);
    void Remove(const std::string& aString);
    bool Check(const std::string& aString);
    uint64_t Size();
private:
    pthread_spinlock_t    SetLock;
    std::set<std::string> TheSet;
};

#endif
