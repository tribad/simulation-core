// **************************************************************************
//
// Modul-Name        : cachethread.cpp
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 20.08.2020
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
//
//  System headers.
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <sys/inotify.h>
//
//  C++ headers
#include <cstring>
//
//  Module headers.
#include "cachethread.h"
//
//  Simulated-Universe Header
#include <CFile.h>
//
//  These are the events we need to see.
#define IN_CACHE_EVENTS ( IN_CLOSE_WRITE | IN_CLOSE_NOWRITE )
//    | IN_MOVED_FROM  | IN_MOVED_TO | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF)

CCacheThread::CCacheThread() : CThread("Cache"){

}

bool CCacheThread::InitInstance() {
    bool retval = CThread::InitInstance();


    if (retval == true) {
        cachelog = new CLogger(ModulId::Cache, "FileCache");
        NotifyHandle = inotify_init();
        if (NotifyHandle == -1) {
            cachelog->Write(LogLevel::Critical, "Cannot initialize notifier interface for the cache.");
        }
    }
    return retval;
}

long CCacheThread::Run() {
    /*
     * Notifier initialized.
     */
    int                     dataread;
    uint8_t                 buffer[sizeof(struct inotify_event) + NAME_MAX + 1];
    struct inotify_event*   event;

    Running = true;
    /*
     * This is the processing loop of incoming signals. These maybe incomming connections
     * or incomming data.
     */
    for (;Running;) {
        dataread=read(NotifyHandle, buffer, sizeof(buffer));
        if (dataread >= 0) {
            event = (struct inotify_event*)(buffer);
            if (event->len > 0u) {
                OnEvent(event->wd, event->mask, event->name, event->cookie);
            } else {
                OnEvent(event->wd, event->mask, nullptr, event->cookie);
            }
        } else {
            cachelog->Write(LogLevel::Debug, "Some error occured: %s", strerror(errno));
        }

    }
    return (0);
}

std::shared_ptr<CCacheEntry> CCacheThread::Get(std::string aPath) {
    std::shared_ptr<CCacheEntry> retval;
    CFileName                    fname(aPath);
    CFile                        f;
    //
    //  only monitoring existings files.
    if (f.Exists(fname)) {
        //
        //  First processing the directory.
        std::string fdir = fname.Directory();
        //
        //  Check if we have the directory already setup for monitoring.
        auto searchName = NameList.find(fdir);
        //
        //  Only need action if the directory is not in the namelist.
        if (searchName == NameList.end()) {
            //
            //  Setup the file info block for the directory.
            CFileInfo dirinfo(fdir);
            //
            //  If the file info is valid continue.
            if (dirinfo.IsValid()) {
                int w = inotify_add_watch(NotifyHandle, fdir.c_str(), IN_CACHE_EVENTS);
                //
                //  Have a watch.
                if (w != -1) {
                    //
                    //  Create a cache entry with the loaded data.
                    retval = std::make_shared<CCacheEntry>(w, dirinfo);
                    //
                    //  Fill the lists.
                    WatchList.insert(std::pair<int, std::shared_ptr<CCacheEntry> >(w, retval));
                    NameList.insert (std::pair<std::string, std::shared_ptr<CCacheEntry> >(fdir, retval));
                } else {
                    cachelog->Write(LogLevel::Warn, "Cannot setup watch for %s", fdir.c_str());
                }
            } else {
                cachelog->Write(LogLevel::Error, "Cannot obtain the directory info for %s", fdir.c_str());
            }
        } else {
            cachelog->Write(LogLevel::Trace, "Directory already has a watch");
        }
        //
        //  Processing the full path.
        searchName = NameList.find(fname);
        //
        //  check if new
        if (searchName == NameList.end()) {
            std::string fullname = fname;

            int w = inotify_add_watch(NotifyHandle, fullname.c_str(), IN_CACHE_EVENTS);
            //
            //  Have a watch.
            if (w != -1) {
                //
                //  Create a cache entry with the loaded data.
                retval = std::make_shared<CCacheEntry>(w, f.GetInfo());
                //
                //  Fill the lists.
                WatchList.insert(std::pair<int, std::shared_ptr<CCacheEntry> >(w, retval));
                NameList.insert (std::pair<std::string, std::shared_ptr<CCacheEntry> >(fullname, retval));
                //
                //  check if we have any Item to monitor.
                if (!LRUStart) {
                    LRUStart = LRUEnd = retval;
                } else {
                    retval->prev = LRUEnd;
                    LRUEnd       = retval;
                }
            } else {
                cachelog->Write(LogLevel::Warn, "Cannot setup watch for %s", ((std::string)fname).c_str());
            }
        } else {
            retval = searchName->second;
            searchName->second->MoveUp();
        }
    }

    return retval;
}

void CCacheThread::OnEvent(int aWatch, uint32_t aMask, const char *aName, uint32_t aCookie) {
    //
    //  Make clear to the compiler that we do not use this at the moment.
    (void)aCookie;
    //
    //  Find the watch.
    auto wl = WatchList.find(aWatch);
    //
    //  Consistency Check.
    if (wl != WatchList.end()) {
        std::shared_ptr<CCacheEntry> e = wl->second;
        std::string n;

        if (aName != nullptr) {
            n = aName;
        } else {
            n = e->Info.GetName();
        }

        if (aMask & IN_CREATE) {
            if (e->Info.IsDirectory()) {
                if (aName != nullptr) {
                    cachelog->Write(LogLevel::Info, "On Directory Created: %s%s", e->Info.GetName().c_str(), aName);
                } else {
                    cachelog->Write(LogLevel::Info, "On Directory Created: %s", n.c_str());
                }
            }
            aMask &= (~IN_CREATE);

        }
        if (aMask & IN_CLOSE_WRITE) {
            if (e->Info.IsDirectory()) {
                if (aName != nullptr) {
                    std::string fullname = e->Info.GetName() + aName;

                    cachelog->Write(LogLevel::Info, "On Directory Closed after Write: %s%s",  e->Info.GetName().c_str(), aName);
                    auto searchName = NameList.find(fullname);

                    if (searchName != NameList.end()) {
                        searchName->second->Load();
                        RenewWatch(fullname);
                    }
                }
            } else {
                cachelog->Write(LogLevel::Info, "On File Closed after Write: %s",  n.c_str());
                e->Load();
            }
            aMask &= (~IN_CLOSE_WRITE);
        }
        if (aMask & IN_CLOSE_NOWRITE) {
            if (e->Info.IsDirectory()) {
                if (aName != nullptr) {
                    std::string fullname = e->Info.GetName() + aName;

                    cachelog->Write(LogLevel::Info, "On Directory Closed No Write: %s%s",  e->Info.GetName().c_str(), aName);

                    auto searchName = NameList.find(fullname);

                    if (searchName != NameList.end()) {
                        searchName->second->Load();
                        // RenewWatch(fullname);
                    }
                }
            } else {
                cachelog->Write(LogLevel::Info, "On File Closed No Write: %s",  n.c_str());
                e->Load();
            }
            aMask &= (~IN_CLOSE_NOWRITE);
        }
        if (aMask & IN_DELETE) {
            cachelog->Write(LogLevel::Info, "Deleted: %s",  n.c_str());
            aMask &= (~IN_DELETE);
        }
        if (aMask & IN_DELETE_SELF) {
            cachelog->Write(LogLevel::Info, "Deleted self: %s",  n.c_str());
            aMask &= (~IN_DELETE_SELF);
        }
        if (aMask & IN_MOVE_SELF) {
            cachelog->Write(LogLevel::Info, "Move self: %s",  n.c_str());
#if 0
            //
            //  Need to renew the watch.
            WatchList.erase(aWatch);
            inotify_rm_watch(NotifyHandle, aWatch);

            int w = inotify_add_watch(NotifyHandle, n.c_str(), IN_CACHE_EVENTS);

            if (w >= 0) {
                WatchList.insert (std::pair<int, std::shared_ptr<CCacheEntry> >(w, e));
            } else {
                cachelog->Write(LogLevel::Critical, "Cannot add watch on renew");
            }
#endif
            aMask &= (~IN_MOVE_SELF);
        }
        if (aMask & IN_MODIFY) {
            cachelog->Write(LogLevel::Info, "Modified: %s",  n.c_str());
            aMask &= (~IN_MODIFY);
        }
        if (aMask & IN_MOVED_FROM) {
            cachelog->Write(LogLevel::Info, "Moved from: %s%s",  e->Info.GetName().c_str(), aName);
            aMask &= (~IN_MOVED_FROM);
        }
        if (aMask & IN_MOVED_TO) {
            cachelog->Write(LogLevel::Info, "Moved to: %s%s",  e->Info.GetName().c_str(), aName);
#if 0
            auto bn = ByName.find(e->Info.GetName()+aName);

            if (bn != ByName.end()) {
                if (bn->second->FileInScope == false) {
                    bn->second->FileInScope = true;

                    int id = GetNotifierId(bn->second->Info, cNotifierMask);

                    if (id != -1) {
                        bn->second->Id = id;

                        ById.insert(std::make_pair(bn->second->Id, e));
                        cachelog->Write(LogLevel::Info, "Renewed watch %i for %s%s", id, e->Info.GetName().c_str(), aName);
                        Load(bn->second);

                    } else {
                        cachelog->Write(LogLevel::Info, "Cannot renew watch for %s%s", e->Info.GetName().c_str(), aName);
                    }
                }
            }
#endif
            aMask &= (~IN_MOVED_TO);
        }
        if (aMask & IN_IGNORED) {
            cachelog->Write(LogLevel::Info, "Released watch: %i %s", aWatch,  n.c_str());

            WatchList.erase(aWatch);

            aMask &= (~IN_IGNORED);
        }
        if (aMask != 0) {
            cachelog->Write(LogLevel::Info, "Unhandled event: %08.8x on %s", aMask,  n.c_str());
        }
    } else {
        cachelog->Write(LogLevel::Error, "Cannot find watch for %i to handle event : %08.8x", aWatch, aMask);
    }
}

void CCacheThread::RenewWatch(const std::string &aName) {
    int watch;

    auto searchName = NameList.find(aName);

    if (searchName != NameList.end()) {
        watch = searchName->second->Id;

        int err = inotify_rm_watch(NotifyHandle, watch);

        if (err == -1) {
            cachelog->Write(LogLevel::Debug, "Cannot remove watch for %s : %s", aName.c_str(), strerror(errno));
        } else {
            cachelog->Write(LogLevel::Info, "Remove watch for %s : %i ", aName.c_str(), watch);
        }
        auto searchWatch = WatchList.find(watch);

        if (searchWatch != WatchList.end()) {
            WatchList.erase(watch);
            int w = inotify_add_watch(NotifyHandle, aName.c_str(), IN_CACHE_EVENTS);

            if (w != -1) {
                searchName->second->Id = w;
                WatchList.insert(std::pair<int, std::shared_ptr<CCacheEntry> >(w, searchName->second));
            } else {
                cachelog->Write(LogLevel::Error, "Cannot renew watch for %s : %s", aName.c_str(), strerror(errno));
            }
        }
    } else {
        cachelog->Write(LogLevel::Error, "Cannot find watch to renew for %s : %s", aName.c_str(), strerror(errno));
    }
}
