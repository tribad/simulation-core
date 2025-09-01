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
// Modul-Name        : CFile.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 26.02.2011
// Modification-Date : 28.04.2011 12:05:08
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
// **************************************************************************
//                   E x t r a   I n c l u d e   L i s t
// **************************************************************************
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <string>
#include <stddef.h>
#include <stdint.h>
#include <pwd.h>
#include <vector>
// **************************************************************************
//                   F i r s t   I n c l u d e   L i s t
// **************************************************************************
#include "CSystemTime.h"
#include "CFileInfo.h"
#include "CFileName.h"
#include "CFile.h"
// **************************************************************************
//                   L a s t   I n c l u d e   L i s t
// **************************************************************************
// **************************************************************************
//
//  Method-Name       : CFile()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 30.03.2011
//  Modification-Date : 12.04.2011 08:29:21
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CFile::CFile() {
    Handle=-1;
    bEof=true;
    bError=false;
}
// **************************************************************************
//
//  Method-Name       : CFile()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 30.03.2011
//  Modification-Date : 12.04.2011 08:29:36
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CFile::CFile(const std::string aPath) {
    Name=aPath;
    Handle=-1;
    FileInfo.Update(Name);
    bEof=true;
    bError=false;
}
// **************************************************************************
//
//  Method-Name       : ~CFile()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 30.03.2011
//  Modification-Date : 30.03.2011 15:54:47
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CFile::~CFile() {
    if (Handle != -1) {
        close(Handle);
        Handle=-1;
    } else {
    }
}
// **************************************************************************
//
//  Method-Name       : Create()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 30.03.2011
//  Modification-Date : 28.04.2011 12:02:28
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
int CFile::Create(const std::string aPath) {
    Name=aPath;
    
    return (Create());
}
// **************************************************************************
//
//  Method-Name       : Open()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 30.03.2011
//  Modification-Date : 28.04.2011 12:05:49
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
int CFile::Open(const std::string aPath, int aMode) {
    Name=aPath;
    
    return (Open(aMode));
}
// **************************************************************************
//
//  Method-Name       : Exists()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 30.03.2011
//  Modification-Date : 30.03.2011 18:01:47
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool CFile::Exists(const std::string aPath) {
    int         errorcode=-1;
    struct stat s;
    
    Name=aPath;
    errorcode=stat( ((std::string)(Name)).c_str(), &s);
    if (errorcode == 0) {
        if ((s.st_mode & S_IFREG)==0) {
            return (false);
        } else {
            FileInfo.Update(aPath);
            return (true);
        }
    } else {
        return (false);
    }
}
// **************************************************************************
//
//  Method-Name       : Link()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 
//  Modification-Date : 07.01.2011 15:12:40
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CFile::Link() {
    
}
// **************************************************************************
//
//  Method-Name       : Close()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 30.03.2011
//  Modification-Date : 30.03.2011 15:54:26
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CFile::Close() {
    if (Handle != -1) {
        close(Handle);
        Handle=-1;
    } else {
    }
}
// **************************************************************************
//
//  Method-Name       : Read()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 30.03.2011
//  Modification-Date : 12.04.2011 08:35:10
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
int CFile::Read(void* aBuffer, const uint64_t aSize) {
    int errorcode=-1;
    
    if (Handle!=-1) {
        errorcode=read(Handle, aBuffer, aSize);
        if (errorcode >=0) {
            if ((aSize>0) && (errorcode==0)) {
                bEof=true;
            } else {
                bEof=false;
            }
        } else {
            bError=true;
            bEof=false;
            errorcode=0;
        }
    } else {
        bError=true;
        bEof=true;
    }
    return (errorcode);
}
// **************************************************************************
//
//  Method-Name       : Write()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 30.03.2011
//  Modification-Date : 01.05.2011 08:47:56
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
int CFile::Write(const void* aBuffer, const uint64_t aSize) {
    int errorcode=-1;
    
    if (Handle!=-1) {
        errorcode=write(Handle, aBuffer, aSize);
        if (errorcode <=0) {
            bError=true;
        } else {
            bError=false;
        }
    } else {
        bError=true;
        bEof=true;
    }
    return (errorcode);
}
// **************************************************************************
//
//  Method-Name       : GetInfo()
//  Author            : 
//  Creation-Date     : 26.02.2011
//  Modification-Date : 26.02.2011 18:30:16
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CFileInfo& CFile::GetInfo() {
    return (FileInfo);
}
// **************************************************************************
//
//  Method-Name       : Seek()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 30.03.2011
//  Modification-Date : 01.05.2011 08:53:54
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool CFile::Seek(int64_t aPosition, const int aWhence) {
    if (Handle != -1) {
        lseek(Handle, aPosition, aWhence);
    } else {
        return (false);
    }
    return (true);
}
// **************************************************************************
//
//  Method-Name       : IsOpen()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 30.03.2011
//  Modification-Date : 01.05.2011 08:53:58
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool CFile::IsOpen() {
    if (Handle==-1) {
        return (false);
    } else {
        return (true);
    }
}
// **************************************************************************
//
//  Method-Name       : GetLastError()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 06.04.2011
//  Modification-Date : 01.05.2011 08:54:03
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
int CFile::GetLastError() {
    return (errno);
}
// **************************************************************************
//
//  Method-Name       : Eof()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 07.04.2011
//  Modification-Date : 01.05.2011 08:54:07
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool CFile::Eof() {
    return (bEof);
}
// **************************************************************************
//
//  Method-Name       : GetLine()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 07.04.2011
//  Modification-Date : 01.05.2011 08:54:11
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
std::string& CFile::GetLine() {
    uint8_t buffer[512];
    int     dataread;
    
    Line.erase();
    do {
        dataread=Read(buffer, sizeof(buffer));
        if (dataread > 0) {
            for (int i=0;i<dataread;i++) {
                if (buffer[i]!='\r') {
                    Line.push_back(buffer[i]);
                } else {
                    i+=2;
                    Seek(-(dataread-i), SEEK_CUR);
                    return (Line);
                }
            }
        } else {
        }
    } while (!Eof());
    return (Line);
}
// **************************************************************************
//
//  Method-Name       : SetName()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 26.02.2011
//  Modification-Date : 01.05.2011 08:54:14
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CFile::SetName(std::string aName) {
    Name=aName;
    FileInfo.Update((std::string)(Name));
}
// **************************************************************************
//
//  Method-Name       : Create()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 30.03.2011
//  Modification-Date : 28.04.2011 12:02:21
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
int CFile::Create() {
    int errorcode=-1;
    
    if (Handle == -1) {
        Handle=open( ((std::string)(Name)).c_str(), O_RDWR|O_CREAT);
        if (Handle == -1) {
            errorcode=errno;
            bError=true;
            bEof=true;
        } else {
            FileInfo.Update(((std::string)(Name)).c_str());
            errorcode=0;
            bError=false;
            bEof=true;
        }
    } else {
        errorcode=0;
    }
    return (errorcode);
}
// **************************************************************************
//
//  Method-Name       : Open()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 30.03.2011
//  Modification-Date : 28.04.2011 12:08:30
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
int CFile::Open(int aMode) {
    int errorcode=-1;
    
    if (Handle == -1) {
        Handle=open( ((std::string)(Name)).c_str(), aMode);
        if (Handle == -1) {
            errorcode=errno;
            bError=true;
            bEof=true;
        } else {
            FileInfo.Update(((std::string)(Name)).c_str());
            errorcode=0;
            bError=false;
            bEof=false;
        }
    } else {
        errorcode=0;
    }
    return (errorcode);
}
