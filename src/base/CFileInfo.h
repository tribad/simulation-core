// MIT License
//
// Copyright (c) 2011 Hans-Juergen Lange<hjl@simulated-universe.de>
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
//
// **************************************************************************
//
// Modul-Name        : CFileInfo.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 13.01.2011
// Modification-Date : 07.04.2011 11:05:15
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef __EXTENSIONLIBS_FILESYSTEM_LINUXFILE_CFILEINFO_INC
#define __EXTENSIONLIBS_FILESYSTEM_LINUXFILE_CFILEINFO_INC
#include <string>
#include "CSystemTime.h"
// **************************************************************************
//                             F o r w a r d s
// **************************************************************************
class CSystemTime;
// **************************************************************************
//                  C l a s s    d e c l a r a t i o n
// **************************************************************************
class CFileInfo {
public:
    CFileInfo();
    CFileInfo(std::string aPath);

    uint64_t GetSize() const {return Size;}
    CSystemTime& GetCreationDate();
    CSystemTime& GetModificationDate();
    CSystemTime& GetLastAccessDate();
    std::string GetName() const {return Name;}
    void Update(std::string aPath);
    void Update();
    bool IsValid();
    bool IsFile() const;
    bool IsDirectory() const;
private:
    unsigned int Mode;
    CSystemTime  CreationDate;
    CSystemTime  ModificationDate;
    uint64_t     Size;
    CSystemTime  AccessDate;
    std::string  Name;
    bool         Valid;
};

#endif // __EXTENSIONLIBS_FILESYSTEM_LINUXFILE_CFILEINFO_INC
