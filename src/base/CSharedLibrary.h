// **************************************************************************
//
// Modul-Name        : CSharedLibrary.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 21.01.2011
// Modification-Date : 03.11.2018
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef __EXTENSIONLIBS_SHAREDLIBRARY_SHAREDLIBS_CSHAREDLIBRARY_INC
#define __EXTENSIONLIBS_SHAREDLIBRARY_SHAREDLIBS_CSHAREDLIBRARY_INC
// **************************************************************************
//           G l o b a l    m a c r o    d e f i n i t i o n s
// **************************************************************************
#define SHAREDLIBRARYEXTENSION "so"
//
//
#include <iostream>
// **************************************************************************
//                             F o r w a r d s
// **************************************************************************
// **************************************************************************
//                  C l a s s    d e c l a r a t i o n
// **************************************************************************
class CSharedLibrary {
public:
    CSharedLibrary();
    CSharedLibrary(const std::string aPath, std::ostream& msg = std::cerr);
    void* GetFunction(const std::string aName);
    void* GetFunction(const int aIndex);
    bool IsLoaded() {return (Handle != nullptr);}
    ~CSharedLibrary();
private:
    std::string Path;
    void* Handle;
};
#endif // __EXTENSIONLIBS_SHAREDLIBRARY_SHAREDLIBS_CSHAREDLIBRARY_INC
