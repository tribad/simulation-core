// **************************************************************************
//
// Modul-Name        : CFileName.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 20.02.2011
// Modification-Date : 20.02.2011 09:40:03
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef __EXTENSIONLIBS_FILESYSTEM_LINUXFILE_CFILENAME_INC
#define __EXTENSIONLIBS_FILESYSTEM_LINUXFILE_CFILENAME_INC
// **************************************************************************
#define PATH_DELIMITER '/'

#define PATH_DELIMITER_STRING "/"
#define PATH_THIS_DIR         "./"
#define PATH_UPPER_DIR        "../"

#include <vector>
#include <string>

// **************************************************************************
//                             F o r w a r d s
// **************************************************************************
// **************************************************************************
//                  C l a s s    d e c l a r a t i o n
// **************************************************************************
class CFileName {
public:
    CFileName();
    CFileName(const std::string aPath);
    ~CFileName();
    void Setup(const std::string aPath);
    void SetBase(const std::string aBase);
    void SetExtension(const std::string aExt);
    size_t DirDepth();
    void ReduceDepth();
    std::string Directory();
    std::string Base();
    std::string Extension();
    operator std::string();
    std::string operator=(const std::string& aPath);
    std::string operator=(const char* aPath);
    
    CFileName operator+(const std::string aMore);
    CFileName operator+(const char c);
    bool operator==(const char* aB);
private:
    std::string BaseName;
    std::string Ext;
    std::vector<std::string> Dirs;
};
#endif // __EXTENSIONLIBS_FILESYSTEM_LINUXFILE_CFILENAME_INC
