// **************************************************************************
//
// Modul-Name        : CDirectory.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 20.02.2011
// Modification-Date : 10.04.2011 07:17:46
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
// **************************************************************************
//                   E x t r a   I n c l u d e   L i s t
// **************************************************************************
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <string>
#include <stddef.h>
#include <stdint.h>
#include <vector>
// **************************************************************************
//                   F i r s t   I n c l u d e   L i s t
// **************************************************************************
#include "CFileName.h"
#include "CDirectory.h"
#include "Pattern.h"
// **************************************************************************
//                   L a s t   I n c l u d e   L i s t
// **************************************************************************
// **************************************************************************
//
//  Method-Name       : CDirectory()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 08.04.2011 13:05:42
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CDirectory::CDirectory() {
    Handle=0;
}
// **************************************************************************
//
//  Method-Name       : CDirectory()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 08.04.2011 14:52:10
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CDirectory::CDirectory(const std::string aPath) {
    Name=aPath;
    Handle=0;
}
// **************************************************************************
//
//  Method-Name       : ~CDirectory()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 10.04.2011 07:24:09
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CDirectory::~CDirectory() {
    if (Handle != 0) {
        closedir(Handle);
        Handle=0;
    }
}
// **************************************************************************
//
//  Method-Name       : Create()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 08.04.2011 13:12:51
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
int CDirectory::Create(const std::string& aPath) {
    int errorcode=-1;
    
    Name=aPath;
    errorcode=mkdir(((std::string)(Name)).c_str(), 0777);
    return (errorcode);
}
// **************************************************************************
//
//  Method-Name       : Open()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 08.04.2011 13:16:45
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
int CDirectory::Open(const std::string& aPath) {
    Name=aPath;
    return (Open());
}
// **************************************************************************
//
//  Method-Name       : Exists()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 08.04.2011 13:14:33
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
bool CDirectory::Exists(const std::string aPath) {
    int         errorcode=-1;
    struct stat s;
    
    Name=aPath;
    errorcode=stat( ((std::string)(Name)).c_str(), &s);
    if (errorcode == 0) {
        if ((s.st_mode & S_IFDIR)==0) {
            return (false);
        } else {
            return (true);
        }
    } else {
        return (false);
    }
}
// **************************************************************************
//
//  Method-Name       : FindFirst()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 10.04.2011 08:32:02
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
int CDirectory::FindFirst(const std::string& aFileSpec) {
    struct dirent *result;
    int           errorcode=-1;
    bool          matchcode;
    
    Expression=aFileSpec;
    if (Handle==0) {
        errorcode=Open();
    } else {
    }
    if (Handle!=0) {
        rewinddir(Handle);
        do {
            result = readdir(Handle);
            if (result==0) {
                errorcode=-1;
            } else {
                matchcode=PatternMatch(Expression, result->d_name);
            }
        } while ((errorcode==0) && (matchcode==false));
        if ((errorcode==0) && (matchcode==true)) {
            FoundName=result->d_name;
        } else {
            FoundName.clear();
        }
    } else {
        FoundName.clear();
    }
    return (errorcode);
}
// **************************************************************************
//
//  Method-Name       : FindNext()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 10.04.2011 08:32:30
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
int CDirectory::FindNext() {
    struct dirent *result;
    int           errorcode=0;
    bool          matchcode;
    
    do {
        result=readdir(Handle);
        if (result==0) {
            errorcode=-1;
        } else {
            matchcode=PatternMatch(Expression, result->d_name);
        }
    } while ((errorcode==0) && (matchcode==false));
    if ((errorcode==0) && (matchcode==true)) {
        FoundName=result->d_name;
    } else {
        FoundName.clear();
    }
    return (errorcode);
}
// **************************************************************************
//
//  Method-Name       : OpenFile()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 12:59:36
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CDirectory::OpenFile() {
    
}
// **************************************************************************
//
//  Method-Name       : CreateFile()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 12:59:39
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CDirectory::CreateFile() {
    
}
// **************************************************************************
//
//  Method-Name       : ExistsFile()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 20.02.2011 12:59:42
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void CDirectory::ExistsFile() {
    
}
// **************************************************************************
//
//  Method-Name       : GetFindName()
//  Author            : 
//  Creation-Date     : 21.01.2011
//  Modification-Date : 08.04.2011 15:07:33
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
char* CDirectory::GetFindName() {
    return ((char*)(FoundName.c_str()));
}
// **************************************************************************
//
//  Method-Name       : Open()
//  Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
//  Creation-Date     : 20.02.2011
//  Modification-Date : 08.04.2011 13:15:38
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
int CDirectory::Open() {
    int errorcode=0;
    Handle=opendir(((std::string)(Name)).c_str());
    if (Handle == 0) {
        errorcode=-1;
    }
    return (errorcode);
}

std::string CDirectory::operator+(const std::string& newpart) {
    std::string retval;

    retval=Name+newpart;

    return retval;
}
