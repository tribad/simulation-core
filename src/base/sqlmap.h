#ifndef SQLMAP_H
#define SQLMAP_H

#include <pthread.h>
#include <cstdint>
#include <msg.h>

class CSQLMap
{
public:
    CSQLMap(uint64_t start = 0x01);
    uint64_t Add(tMsgPtr aMsg);
    tMsgPtr  Remove(uint64_t aId);
    size_t   Size() {return SQLMap.size();}
    bool     Empty() {return SQLMap.empty();}
    void     Clear() ;
private:
    uint64_t                                   newindex;
    pthread_spinlock_t                         sqllock;
    std::map<uint64_t, tMsgPtr > SQLMap;
};

#endif // SQLMAP_H
