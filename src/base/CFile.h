// **************************************************************************
//
// Modul-Name        : CFile.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 26.02.2011
// Modification-Date : 28.04.2011 12:05:08
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef __EXTENSIONLIBS_FILESYSTEM_LINUXFILE_CFILE_INC
#define __EXTENSIONLIBS_FILESYSTEM_LINUXFILE_CFILE_INC
#include <fcntl.h>
#include <string>
#include "CFileName.h"
#include "CFileInfo.h"
// **************************************************************************
//                             F o r w a r d s
// **************************************************************************
class CSystemTime;
class CFileInfo;
class CFileName;
// **************************************************************************
//                  C l a s s    d e c l a r a t i o n
// **************************************************************************
class CFile {
public:
    CFile();
    CFile(const std::string aPath);
    virtual ~CFile();
    int Create(const std::string aPath);
    int Open(const std::string aPath, int aMode=O_RDONLY);
    virtual bool Exists(const std::string aPath);
    virtual void Link();
    virtual void Close();
    virtual int Read(void* aBuffer, const uint64_t aSize);
    virtual int Write(const void* aBuffer, const uint64_t aSize);
    
    CFileInfo& GetInfo();
    bool Seek(int64_t aPosition, const int aWhence);
    bool IsOpen();
    int GetLastError();
    bool Eof();
    std::string& GetLine();
    void SetName(std::string aName);
    std::string GetFileNameExt(void) {return Name.Extension();}
    int Create();
    int Open(int aMode=O_RDONLY);
private:
    CFileInfo   FileInfo;
    int         Handle;
    CFileName   Name;
    std::string Line;
    bool        bEof;
    bool        bError;
};

#endif // __EXTENSIONLIBS_FILESYSTEM_LINUXFILE_CFILE_INC
