// **************************************************************************
//
// Modul-Name        : httpcontentlib.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 25.12.2019
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#include <dlfcn.h>

#include <string>
#include <sstream>
#include <list>

#include <httpsimulationifc.h>
#include <httpcontentifc.h>
#include <httpcontentlib.h>
#include <CDirectory.h>

CHttpContentLib::~CHttpContentLib() {
    if (mContentHandle != nullptr) {
        dlclose(mContentHandle);
    }
    mContentHandle = nullptr;
}

CLogger& CHttpContentLib::GetLogger() {
    static CLogger contentLibLogger(ModulId::ContentLib, "ContentLib");

    return contentLibLogger;
}

int CHttpContentLib::Load(const std::string& aContentDir, const std::string& aContentLibName, xmlNode* aParameter) {
    int                    retval    = 0;
    int                    finderror;
    std::string            libname;
    std::list<std::string> libnames;
    CDirectory             path;
    /*
     * store some values.
     */
    mContentDir = aContentDir;
    /*
     *  Search all content libs that match the contentlib name.
     */
    path = mContentDir+"/";
    finderror = path.FindFirst(aContentLibName+"*");
    while (finderror == 0) {
        if (path.GetFindName() > libname) {
            libnames.push_back(path.GetFindName());
        }
        finderror = path.FindNext();
    }
    //
    //  Now search the most recent lib by its version number
    int         v = 0; // version
    int         r = 0; // revision
    int         b = 0; // build
    size_t      pos; // position of dots.
    std::string openpath;
    //
    //  We roll out the extraction of the version number.
    for (auto l : libnames) {
        pos = l.find(".so");
        if (pos != std::string::npos) {
            pos = pos+4;
        } else {
            break;
        }
        //
        //  if we have no version info we set all to zero
        if (pos < l.size()) {
            std::istringstream iss(l.substr(pos));
            int                nv = 0; // new version
            int                nr = 0; // new revision
            int                nb = 0; // new build

            iss >> nv;
            //
            //  Skip the dot.
            iss.seekg(1, std::ios_base::cur);
            iss >> nr;
            //
            //  Skip the dot.
            iss.seekg(1, std::ios_base::cur);
            iss >> nb;

            if ((nv > v) || ((nv >= v) && (nr > r)) || ((nv >= v) && (nr >= r) && (nb > b))) {
                v = nv;
                r = nr;
                b = nb;
                mContentFileName = l;
                openpath         = path + l;
            }
        } else {
            if ((v == 0) && (r == 0) && (b == 0)) {
                mContentFileName = l;
                openpath         = path + l;
            }
        }
    }
    //
    //  Check if we got any lib to load.
    if (!openpath.empty()) {
        mContentHandle=dlopen(openpath.c_str(), RTLD_NOW);
        if (mContentHandle!=0) {
            httpcontentifc_op ifc;

            GetLogger().Write(LogLevel::Info, "Loading shared library: %s : %s\n", openpath.c_str(), dlerror());
            //
            //  get the entry point to the library.
            ifc = (httpcontentifc_op)(dlsym(mContentHandle, "getcontentifc"));
            if (ifc != 0) {
                mSimulationIfc = new CHttpSimulationIfc(mLogger, mParent);
                mContentIfc    = ifc(aParameter, mSimulationIfc);
                //
                //  Give an error if we do not get a content ifc back.
                if (mContentIfc == nullptr) {
                    GetLogger().Write(LogLevel::Error, "Loading shared library: %s : Did not get a content interface\n", openpath.c_str());
                }
            } else {
                GetLogger().Write(LogLevel::Error, "Loading shared library: %s : could not find entry point\n", openpath.c_str());
                //
                //  make sure we have not operation available.
                ifc = nullptr;
            }
        } else {
            GetLogger().Write(LogLevel::Error, "Loading shared library: %s : %s\n", openpath.c_str(), dlerror());
        }
    }  else {
        GetLogger().Write(LogLevel::Error, "Loading shared library failed: no file name given\n");
    }
    return retval;
}
