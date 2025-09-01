// MIT License
//
// Copyright (c) 2019 Hans-Juergen Lange<hjl@simulated-universe.de>
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
// Modul-Name        : httpcontentlib.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 25.12.2019
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************

#ifndef HTTPCONTENTLIB_H
#define HTTPCONTENTLIB_H

#include <string>

#include <logger.h>
#include <helper.h>

class CHttpContentIfc;
class CHttpSimulationIfc;
class CHttpDomainSpec;

class CHttpContentLib
{
public:
    CHttpContentLib(CHttpDomainSpec* aParent = nullptr) : mLogger(ModulId::ContentLib, "Content-Lib") {mContentHandle = nullptr; mContentIfc = nullptr; mParent = aParent;}
    ~CHttpContentLib();
    int Load(const std::string& aContentDir, const std::string& aContentLibName, xmlNode* aParameter = nullptr);
    CLogger& GetLogger();
    bool isLoaded() {return mContentHandle != nullptr;}
    bool isReadyToRelease(){if (mContentIfc != nullptr) {return mContentIfc->ReadyToRelease();} else {return true;}}
public:
    std::string         mContentDir;
    std::string         mContentFileName;
    void*               mContentHandle;
    CHttpContentIfc*    mContentIfc;
    CLogger             mLogger;
    CHttpSimulationIfc* mSimulationIfc;
    CHttpDomainSpec*    mParent;
};

#endif // HTTPCONTENTLIB_H
