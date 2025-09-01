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
