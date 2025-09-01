// MIT License
//
// Copyright (c) 2020 Hans-Juergen Lange<hjl@simulated-universe.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
