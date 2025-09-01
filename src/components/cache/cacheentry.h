// **************************************************************************
//
// Modul-Name        : cacheentry.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 27.01.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef CACHEENTRY_H
#define CACHEENTRY_H
//
//  C++ headers
#include <cstdint>
#include <list>
#include <mutex>
#include <memory>
//
//  Simulated-Universe headers.
#include <CFileInfo.h>

class CCacheEntry
{
public:
    CCacheEntry();
    CCacheEntry(int aId, const CFileInfo & aInfo);
    ~CCacheEntry();
    void Load();
    void MoveUp();
    bool operator==(const std::shared_ptr<CCacheEntry> & other);
private:
    void InternalLoad();
public:
    std::mutex  Lock;
    bool        LoadIgnore = false;
    CFileInfo   Info;
    int32_t     Id;
    uint8_t*    Hash   = nullptr;
    uint8_t*    Buffer = nullptr;
public:
    //
    //  These are used for the LRU mechanism.
    std::shared_ptr<CCacheEntry> prev;
    std::shared_ptr<CCacheEntry> next;
};

#endif // CACHEENTRY_H
