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
