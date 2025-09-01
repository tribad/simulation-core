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
