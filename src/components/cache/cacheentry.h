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
