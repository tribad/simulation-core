// **************************************************************************
//
//  Modul-Name     : httpdomainspec.cpp
//  Author(s)      : Hans-Juergen Lange
//
//  Copyrights 2014 by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
//
//  system headers
#include <sys/inotify.h>
#include <fcntl.h>
//
//  C++ headers
#include <sstream>
//
//  Simulated-Universe headers.
#include <CDirectory.h>
#include <httpcontentifc.h>
#include "httpdomainspec.h"
#include <httpcontentlib.h>
#include <logger.h>

CHttpDomainSpec::CHttpDomainSpec(std::string aName, std::string aHtmlDir, std::string aContentDir, std::string aDataDir, std::string aContentLib, std::string aMaxAge, CLogger* log)
{
    Name             = aName;
    HtmlDir          = aHtmlDir;
    ContentDir       = aContentDir;
    DataDir          = aDataDir;
    ContentLib       = aContentLib;
    MaxAge           = aMaxAge;
    mOld             = nullptr;
    Parameter        = nullptr;
    domainLog        = log;
    NotifierHandle   = -1;  //  File descriptors are better initialized with -1
    //
    mActual          = new CHttpContentLib(this);
}

void CHttpDomainSpec::Init(int aMainPid, int aSignalNo, CLogger* aLogger) {
    //
    //  We do not free the pointer
    //  Loggers a borrowed from others.
    domainLog = aLogger;
    //
    //  Init the notifier and make it async IO.
    int optval;
    int err;
    //
    //  Create a new watch list.
    NotifierHandle  = inotify_init();
    if (NotifierHandle != -1) {
        /*
         * We created a new watchlist.
         */
        optval = fcntl(NotifierHandle, F_GETFL);
        /*
         * Check if getting the flags succeeded.
         */
        if (optval != -1) {
            /*
             * Setting additional flags.
             */
            err=fcntl(NotifierHandle, F_SETFL, optval | O_ASYNC | O_NONBLOCK | O_RDWR);
            if (err==0) {
                /*
                 * Set the owning process of the handle so that the thread may get the signals.
                 */
                err=fcntl(NotifierHandle, F_SETOWN, aMainPid);
                if (err==0) {
                    /*
                     * Set the signal to use on socket events.
                     */
                    err=fcntl(NotifierHandle, F_SETSIG, aSignalNo);
                    if (err == 0) {
                        domainLog->Write(LogLevel::Info, "Watchlist started");
                    } else {
                        domainLog->Write(LogLevel::Error, "Error on set signal for watchlist: %s", strerror(errno));
                    }
                } else {
                    domainLog->Write(LogLevel::Error, "Error on setting the owner for the watchlist: %s", strerror(errno));
                }
            } else {
                domainLog->Write(LogLevel::Error, "Error on setting async flags on watchlist: %s", strerror(errno));
            }
        } else {
            domainLog->Write(LogLevel::Error, "Error on getting flags from watchlist handle: %s", strerror(errno));
        }
    }
    //
    //  Not doing more levels of ifs as this is another part of the overall initialization.
    //  Setup the watch on the ContentLib dir
    if (err == 0) {
        Watch = inotify_add_watch(NotifierHandle, ContentDir.c_str(), IN_CLOSE_WRITE);
    }

    mActual->Load(ContentDir, ContentLib, Parameter);
}

bool CHttpDomainSpec::SwitchContent() {
    bool retval = false;
    //
    //  Lock against other threads that try to let handle requests through the content lib.
    ContentSwitchLock.lock();
    //
    //  Check if there is still one old library left over.
    if (mOld == nullptr) {
        mOld    = mActual;
        mActual = new CHttpContentLib(this);

        mActual->Load(ContentDir, ContentLib, Parameter);
        /*
         * Roll-back if not loaded
         */
        if (!mActual->isLoaded()) {
            delete mActual;

            mActual = mOld;

            mOld = nullptr;
        }
    }

    TryUnload();
    ContentSwitchLock.unlock();
    return retval;
}

void CHttpDomainSpec::TryUnload() {
    //
    // Check if there are any responses outstanding.
    if ((mOld != nullptr) && (mOld->isReadyToRelease())) {
        delete mOld;

        mOld = nullptr;
    }
}

void CHttpDomainSpec::HandleEvent(int aWatch, uint32_t aMask, const char *aName) {
    //
    //  Consistency Check.
    if (aWatch == Watch) {
        if (aMask & IN_CLOSE_WRITE) {
            if (aName != nullptr) {
                std::string fullname = ContentDir + "/" + aName;
                if (memcmp(ContentLib.c_str(), aName, ContentLib.size()) == 0) {
                    domainLog->Write(LogLevel::Info, "Closed after Write: %s",  fullname.c_str());
                    SwitchContent();
                }
            }
            aMask &= (~IN_CLOSE_WRITE);
        }
        if (aMask & IN_CLOSE_NOWRITE) {
            if (aName != nullptr) {
                std::string fullname = ContentDir + "/" + aName;

                domainLog->Write(LogLevel::Info, "Closed after non-Write: %s",  fullname.c_str());
            }
            aMask &= (~IN_CLOSE_NOWRITE);
        }
        if (aMask != 0u) {
            domainLog->Write(LogLevel::Error, "Cannot find");
        }
    }
}
