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
// // **************************************************************************
//
// Modul-Name        : CFileName.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 20.02.2011
// Modification-Date : 20.02.2011 09:40:03
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
// **************************************************************************
//                   E x t r a   I n c l u d e   L i s t
// **************************************************************************
#ifdef __linux__
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif // __linux__
#include <string>
#include <stddef.h>
#include <stdint.h>
#include <vector>
// **************************************************************************
//                   F i r s t   I n c l u d e   L i s t
// **************************************************************************
#ifdef __linux__
#include "CFileName.h"
#endif // __linux__
// **************************************************************************
//                   L a s t   I n c l u d e   L i s t
// **************************************************************************
// **************************************************************************
//                   N a m e s p a c e   L i s t
// **************************************************************************
// **************************************************************************
//
//  Method-Name       : CFileName()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 09:32:18
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CFileName::CFileName() {
    
}
// **************************************************************************
//
//  Method-Name       : CFileName()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 09:56:31
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CFileName::CFileName(const std::string aPath) {
    Setup(aPath);
}
// **************************************************************************
//
//  Method-Name       : ~CFileName()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 09:32:32
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CFileName::~CFileName() {
    
}
// **************************************************************************
//
//  Method-Name       : Setup()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 11:09:59
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CFileName::Setup(const std::string aPath) {
    size_t startpos;
    size_t endpos;
    std::string subber;
    char   buffer[4096];
    int    result;
    struct passwd pwentry;
    struct passwd *pwresult;
    std::string path=aPath;
    //
    BaseName.clear();
    Ext.clear();
    Dirs.clear();
    //
    //  replace '/' with '\'
    //
    while (path.find('\\')!=std::string::npos) {
        path.replace(path.find('\\'), 1, 1,'/');
    }
    //  Re-interpret the leading ~
    if (aPath[0]=='~') {
        result=getpwuid_r(geteuid(), &pwentry, buffer, sizeof(buffer), &pwresult);
        if ((result==0) && (pwresult!=0)) {
            path.replace(0,1,pwresult->pw_dir);
        } else {
            path.replace(0,1,"/tmp/");
        }
    } else {
    }
    //
    //  Create a list of dirs.
    startpos=0;
    while ((endpos=path.find_first_of(PATH_DELIMITER, startpos)) != std::string::npos) {
        subber=path.substr(startpos, (endpos-startpos)+1);
        if (subber != PATH_THIS_DIR) {
            if ((subber == PATH_UPPER_DIR) && (Dirs.size()>1)) {
                Dirs.pop_back();
            } else if ((subber == PATH_DELIMITER_STRING) && (Dirs.size() > 0)) {
            } else {
                Dirs.push_back(subber);
            }
        }
        startpos=endpos+1;
    }
    //
    //  Check if the path only consists of directories.
    if (!path.empty()) {
        //
        // Now we do not need to check if this is a valid expression.
        //  Check the last character
        if (*path.rbegin() != PATH_DELIMITER) {
            //
            //  Have a filename. Create the base and the extension.
            subber=path.substr(startpos);
            endpos=subber.find_last_of('.');
            if (endpos != std::string::npos) {
                BaseName=subber.substr(0, (endpos));
                Ext=subber.substr(endpos+1);
            } else {
                BaseName=path.substr(startpos);
            }
        }
    }
}
// **************************************************************************
//
//  Method-Name       : SetBase()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 09:52:07
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CFileName::SetBase(const std::string aBase) {
    BaseName=aBase;
}
// **************************************************************************
//
//  Method-Name       : SetExtension()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 09:53:26
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CFileName::SetExtension(const std::string aExt) {
    Ext=aExt;
}
// **************************************************************************
//
//  Method-Name       : DirDepth()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 09:58:59
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
size_t CFileName::DirDepth() {
    return (Dirs.size());
}
// **************************************************************************
//
//  Method-Name       : ReduceDepth()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 09:59:08
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CFileName::ReduceDepth() {
    if (Dirs.size()>1) {
        Dirs.pop_back();
    } else {
    }
    
}
// **************************************************************************
//
//  Method-Name       : Directory()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 09:59:34
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
std::string CFileName::Directory() {
    std::vector<std::string>::iterator i;
    std::string result;
    
    i=Dirs.begin();
    while (i!=Dirs.end()) {
        result=result+(*i);
        i++;
    }
    return (result);
    
}
// **************************************************************************
//
//  Method-Name       : Base()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 09:53:18
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
std::string CFileName::Base() {
    return (BaseName);
}
// **************************************************************************
//
//  Method-Name       : Extension()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 09:54:07
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
std::string CFileName::Extension() {
    return (Ext);
}
// **************************************************************************
//
//  Method-Name       : operator std::string()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 10:00:04
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CFileName::operator std::string() {
    std::string re;
    std::vector<std::string>::iterator i;
    
    i=Dirs.begin();
    while (i != Dirs.end()) {
        re=re+(*i);
        i++;
    }
    if (BaseName.size()>0) {
        re=re+BaseName;
    }
    if (Ext.size()>0) {
        re=re+".";
        re=re+Ext;
    }
    return (re);
    
}
// **************************************************************************
//
//  Method-Name       : operator=()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 09:55:22
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
std::string CFileName::operator=(const std::string& aPath) {
    Setup(aPath);
    return ((std::string)(*this));
}
// **************************************************************************
//
//  Method-Name       : operator=()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 09:55:08
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
std::string CFileName::operator=(const char* aPath) {
    Setup(aPath);
    return ((std::string)(*this));
}
// **************************************************************************
//
//  Method-Name       : operator+()
//  Author            : 
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 10:04:38
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CFileName CFileName::operator+(const std::string aMore) {
    std::string newpath=(std::string)(*this)+aMore;
    return (CFileName(newpath));
}

CFileName CFileName::operator+(const char aMore) {
    std::string newpath=(std::string)(*this)+aMore;
    return (CFileName(newpath));
}
// **************************************************************************
//
//  Method-Name       : operator==()
//  Author            : 
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 09:33:32
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool CFileName::operator==(const char* aB) {
    std::string s=(std::string)(*this);
    
    return (s==aB);
}
