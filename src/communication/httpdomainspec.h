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
