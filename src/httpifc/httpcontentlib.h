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
