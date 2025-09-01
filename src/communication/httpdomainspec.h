// MIT License
//
// Copyright (c) 2014 Hans-Juergen Lange<hjl@simulated-universe.de>
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
//  Modul-Name     : httpdomainspec.h
//  Author(s)      : Hans-Juergen Lange
//
//  Copyrights 2014 by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef HTTPDOMAINSPEC_H
#define HTTPDOMAINSPEC_H
//
//  C++ header
#include <string>
#include <mutex>
//
//  Simulated-Universe header
#include <helper.h>
#include <logger.h>
class CHttpContentIfc;
class CHttpContentLib;


class CHttpDomainSpec
{
public:
    CHttpDomainSpec(std::string aName, std::string aHtmlDir, std::string aContentDir, std::string aDataDir, std::string aContentLib, std::string aMaxAge, CLogger* log);
    ~CHttpDomainSpec() = default;

    void Init(int aMainPid, int aSignalNo, CLogger* aLogger);
    bool SwitchContent();
    void TryUnload();
    int  GetNotifierHandle() {return NotifierHandle;}
    void HandleEvent(int aWatch, uint32_t aMask, const char* aName);
public:
    std::string        Name;
    std::string        HtmlDir;
    std::string        ContentDir;
    std::string        DataDir;
    std::string        ContentLib;
    std::string        MaxAge;
public:
    CLogger*           domainLog = nullptr;
    std::mutex         ContentSwitchLock;
    /*
     * Actual Content
     */
    CHttpContentLib*   mActual;
    /*
     * Old content if still in use
     */
    CHttpContentLib*   mOld;
    /*
     * Parameter from the simulation configuration
     */
    xmlNode*           Parameter;
    /*
     * Watcher for new content library.
     */
    int                NotifierHandle;
    int                Watch;
};

#endif // HTTPDOMAINSPEC_H
