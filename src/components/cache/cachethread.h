// **************************************************************************
//
// Modul-Name        : cachethread.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 20.08.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef CACHETHREAD_H
#define CACHETHREAD_H
//
//  System header files.
#include <sys/signalfd.h>
//
//  C++ header
#include <map>
#include <memory>
//
//  Simulated-Universe header
#include <CThread.h>
#include <cacheentry.h>
#include <logger.h>

class CCacheThread : public CThread {
public:
    CCacheThread();
    bool InitInstance() override;
    long Run() override;
    static CCacheThread& instance() {
        static CCacheThread cache;

        return cache;
    }
    std::shared_ptr<CCacheEntry> Get(std::string aPath);

private:
    void OnEvent(int aWatch, uint32_t aMask, const char* aName, uint32_t aCookie);
    void RenewWatch(const std::string& aName);
private:
    CLogger*                                              cachelog      = nullptr;
    int                                                   NotifyHandle;
    std::map < int, std::shared_ptr<CCacheEntry> >        WatchList;
    std::map <std::string, std::shared_ptr<CCacheEntry> > NameList;
    std::shared_ptr<CCacheEntry>                          LRUStart;
    std::shared_ptr<CCacheEntry>                          LRUEnd;
};

#endif // CACHETHREAD_H
