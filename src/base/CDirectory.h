// **************************************************************************
//
// Modul-Name        : CDirectory.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 20.02.2011
// Modification-Date : 10.04.2011 07:17:46
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef __EXTENSIONLIBS_FILESYSTEM_LINUXFILE_CDIRECTORY_INC
#define __EXTENSIONLIBS_FILESYSTEM_LINUXFILE_CDIRECTORY_INC
#include <string>
#include <dirent.h>
#include "CFileName.h"
// **************************************************************************
//                             F o r w a r d s
// **************************************************************************
class CFileName;
// **************************************************************************
//                  C l a s s    d e c l a r a t i o n
// **************************************************************************
class CDirectory {
public:
    CDirectory();
    CDirectory(const std::string aPath);
    virtual ~CDirectory();
    virtual int Create(const std::string& aPath);
    virtual int Open(const std::string& aPath);
    virtual bool Exists(const std::string aPath);
    virtual int FindFirst(const std::string& aFileSpec);
    virtual int FindNext();
    virtual void OpenFile();
    virtual void CreateFile();
    virtual void ExistsFile();
    char* GetFindName();
    virtual int Open();

    std::string operator+(const std::string& newpart);
    operator std::string();
private:
    DIR* Handle;
    
    CFileName Name;
    std::string FoundName;
    std::string Expression;
};

#endif // __EXTENSIONLIBS_FILESYSTEM_LINUXFILE_CDIRECTORY_INC
