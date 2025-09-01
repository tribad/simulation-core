// **************************************************************************
//
// Modul-Name        : CSharedLibrary.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 21.01.2011
// Modification-Date : 08.05.2011 09:18:44
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
// **************************************************************************
//                   E x t r a   I n c l u d e   L i s t
// **************************************************************************
#include <iostream>
#include <dlfcn.h>
#include <string>
#include <stddef.h>
#include <stdint.h>
// **************************************************************************
//                   F i r s t   I n c l u d e   L i s t
// **************************************************************************
#include "CSharedLibrary.h"
// **************************************************************************
//                   L a s t   I n c l u d e   L i s t
// **************************************************************************
using namespace std;
// **************************************************************************
//
//  Method-Name       : CSharedLibrary()
//  Author            : 
//  Creation-Date     : 21.01.2011
//  Modification-Date : 05.03.2011 17:07:28
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CSharedLibrary::CSharedLibrary() {
    Handle=0;
}
// **************************************************************************
//
//  Method-Name       : CSharedLibrary()
//  Author            : 
//  Creation-Date     : 21.01.2011
//  Modification-Date : 05.03.2011 17:16:54
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CSharedLibrary::CSharedLibrary(const std::string aPath, std::ostream& msg) {
    Handle=dlopen(aPath.c_str(), RTLD_NOW|RTLD_GLOBAL);
    if (Handle == 0) {
        msg << "Loading shared library:" << aPath.c_str() << ":" << dlerror();
    } else {
        msg << "Loading shared library: " << aPath.c_str() << " succeeded.";
    }
}
// **************************************************************************
//
//  Method-Name       : GetFunction()
//  Author            : 
//  Creation-Date     : 21.01.2011
//  Modification-Date : 10.04.2011 13:49:08
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void* CSharedLibrary::GetFunction(const std::string aName) {
    if (Handle != 0) {
        return (dlsym(Handle, aName.c_str()));
    } else {
        return (0);
    }
}
// **************************************************************************
//
//  Method-Name       : GetFunction()
//  Author            : 
//  Creation-Date     : 21.01.2011
//  Modification-Date : 05.03.2011 17:11:46
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
void* CSharedLibrary::GetFunction(const int aIndex) {
    //
    //
    (void)aIndex;
    //
    return (nullptr);
}
// **************************************************************************
//
//  Method-Name       : ~CSharedLibrary()
//  Author            : 
//  Creation-Date     : 05.03.2011
//  Modification-Date : 05.03.2011 17:12:50
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
CSharedLibrary::~CSharedLibrary() {
    if (Handle != 0) {
        dlclose(Handle);
        Handle=0;
    } else {
    }
}
