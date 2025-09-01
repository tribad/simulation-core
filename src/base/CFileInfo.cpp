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
// **************************************************************************
//
// Modul-Name        : CFileInfo.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 13.01.2011
// Modification-Date : 07.04.2011 11:05:15
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
// **************************************************************************
//                   E x t r a   I n c l u d e   L i s t
// **************************************************************************
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <string>
#include <stddef.h>
#include <stdint.h>
// **************************************************************************
//                   F i r s t   I n c l u d e   L i s t
// **************************************************************************
#include "CSystemTime.h"
#include "CFileInfo.h"
// **************************************************************************
//                   L a s t   I n c l u d e   L i s t
// **************************************************************************
// **************************************************************************
//                   N a m e s p a c e   L i s t
// **************************************************************************
// **************************************************************************
//
//  Method-Name       : GetSize()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 23.02.2011
//  Modification-Date : 23.02.2011 10:57:56
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
//uint64_t CFileInfo::GetSize() {
//    return (Size);
//}
// **************************************************************************
//
//  Method-Name       : GetCreationDate()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 23.02.2011
//  Modification-Date : 23.02.2011 11:02:03
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CSystemTime& CFileInfo::GetCreationDate() {
    return (CreationDate);
}
// **************************************************************************
//
//  Method-Name       : GetModificationDate()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 23.02.2011
//  Modification-Date : 23.02.2011 11:02:07
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CSystemTime& CFileInfo::GetModificationDate() {
    return (ModificationDate);
}
// **************************************************************************
//
//  Method-Name       : GetLastAccessDate()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 23.02.2011
//  Modification-Date : 23.02.2011 11:02:22
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CSystemTime& CFileInfo::GetLastAccessDate() {
    return (AccessDate);
}
// **************************************************************************
//
//  Method-Name       : CFileInfo()
//  Author            : 
//  Creation-Date     : 30.03.2011
//  Modification-Date : 07.04.2011 11:04:26
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CFileInfo::CFileInfo(std::string aPath) {
    Name=aPath;
    Valid=false;
    Update();
}
// **************************************************************************
//
//  Method-Name       : Update()
//  Author            : 
//  Creation-Date     : 30.03.2011
//  Modification-Date : 30.03.2011 17:19:09
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CFileInfo::Update(std::string aPath) {
    Name=aPath;
    Update();
}
// **************************************************************************
//
//  Method-Name       : Update()
//  Author            : 
//  Creation-Date     : 30.03.2011
//  Modification-Date : 07.04.2011 11:04:45
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CFileInfo::Update() {
    int         errorcode=-1;
    struct stat s;
    
    errorcode=stat( ((std::string)(Name)).c_str(), &s);
    if (errorcode == 0) {
        Mode=s.st_mode;
        CreationDate.Set(s.st_ctime);
        ModificationDate.Set(s.st_mtime);
        AccessDate.Set(s.st_atime);
        Size=s.st_size;
        Valid=true;
    } else {
        Valid=false;
    }
    
}
// **************************************************************************
//
//  Method-Name       : IsValid()
//  Author            : 
//  Creation-Date     : 07.04.2011
//  Modification-Date : 07.04.2011 11:04:13
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool CFileInfo::IsValid() {
    return (Valid);
}
// **************************************************************************
//
//  Method-Name       : CFileInfo()
//  Author            : 
//  Creation-Date     : 
//  Modification-Date : 
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CFileInfo::CFileInfo() {
    
}

bool CFileInfo::IsFile() const {
    return ((Mode & S_IFMT) == S_IFREG);
}

bool CFileInfo::IsDirectory() const {
    return ((Mode & S_IFMT) == S_IFDIR);
}
