// MIT License
//
// Copyright (c) 2018 Hans-Juergen Lange<hjl@simulated-universe.de>
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

#include <errno.h>
#include <sys/inotify.h>
#include <fcntl.h>
#include <unistd.h>

#include <Pattern.h>
#include "httpdomainspec.h"
#include "httpconnection.h"
#include "logger.h"
#include "httpifcthread.h"

extern int mainpid;

CHTTPIfcThread::CHTTPIfcThread() : CIfcThread(ModulId::HttpIfcServer, "HTTP-Ifc")
{
#if 0
    int optval;
    int err;
    //
    //  Create a new watch list.
    ContentLibNotifier  = inotify_init();
    ContentLoadNotifier = -1;
    if (ContentLibNotifier != -1) {
        /*
         * We created a new watchlist.
         */
        optval = fcntl(ContentLibNotifier, F_GETFL);
        /*
         * Check if getting the flags succeeded.
         */
        if (optval != -1) {
            /*
             * Setting additional flags.
             */
            err=fcntl(ContentLibNotifier, F_SETFL, optval | O_ASYNC | O_NONBLOCK | O_RDWR);
            if (err==0) {
                /*
                 * Set the owning process of the handle so that the thread may get the signals.
                 */
                err=fcntl(ContentLibNotifier, F_SETOWN, mainpid);
                if (err==0) {
                    /*
                     * Set the signal to use on socket events.
                     */
                    err=fcntl(ContentLibNotifier, F_SETSIG, SignalNo);
                    if (err == 0) {
                        ifcLogger.Write(LogLevel::Info, "Watchlist started");
                    } else {
                        ifcLogger.Write(LogLevel::Error, "Error on set signal for watchlist: %s", strerror(errno));
                    }
                } else {
                    ifcLogger.Write(LogLevel::Error, "Error on setting the owner for the watchlist: %s", strerror(errno));
                }
            } else {
                ifcLogger.Write(LogLevel::Error, "Error on setting async flags on watchlist: %s", strerror(errno));
            }
        } else {
            ifcLogger.Write(LogLevel::Error, "Error on getting flags from watchlist handle: %s", strerror(errno));
        }
    }
#endif
    //
    //  The domains have been setup already.
    //  But now we need to setup the notifier and some other stuff that
    //  has not been available as the domainspecs have been created.
    std::set<CHttpDomainSpec*> specs;
    //
    //  Lock the domain map access
    CHTTPProtocol::domains_lock.lock();
    for (auto d : CHTTPProtocol::domains) {
        if (specs.find(d.second) == specs.end()) {
            d.second->Init(mainpid, SignalNo, &ifcLogger);
            DomainSpecs.insert(std::make_pair(d.second->GetNotifierHandle(), d.second));
            specs.insert(d.second);
        }
    }
    CHTTPProtocol::domains_lock.unlock();
}

std::shared_ptr<CConnection> CHTTPIfcThread::NewConnection(int aHandle, uint64_t aConnectionId) {
    return (std::make_shared<CHTTPConnection>(aHandle, aConnectionId, ifcLogger));
}


bool CHTTPIfcThread::OnFDEvent(int aSocket, int aState) {
    //
    //
    (void)aState;
    //
    bool                  retval = true;
    char                  buffer[sizeof(struct inotify_event) + NAME_MAX + 1];
    int                   data_read;
    struct inotify_event* event;

    auto ds = DomainSpecs.find(aSocket);
    //
    // Events are handled per domain spec.
    if (ds != DomainSpecs.end()) {
        for(;;) {
            data_read = read(ds->first, buffer, sizeof (buffer));
            if ((data_read == -1) && (errno != EAGAIN)) {
                ifcLogger.Write(LogLevel::Error, "Error while reading events from watchlist: %s", strerror(errno));
                break;
            }
            //
            //  Ok. No error but no data as well. End of events for that domain spec.
            if (data_read <= 0) {
                break;
            }

            for (char* ptr = buffer; ptr < (buffer + data_read); ptr += sizeof(struct inotify_event) + event->len) {
                //
                //  Now we have an event.
                event = (struct inotify_event *) ptr;
                //
                //
                ifcLogger.Write(LogLevel::Trace, "Before processing");
                if (event->len != 0u) {
                    ds->second->HandleEvent(event->wd, event->mask, event->name);
                } else {
                    ds->second->HandleEvent(event->wd, event->mask, nullptr);
                }
                ifcLogger.Write(LogLevel::Trace, "After processing");

            }
        }
    } else {
        ifcLogger.Write(LogLevel::Error, "Got FD event for unknown handle: %i", aSocket);
    }
    return retval;

#if 0
    if (aSocket == ContentLibNotifier) {
        //
        //  We break the loop later. Reading must be done at least once.
        for(;;) {
            data_read = read(ContentLibNotifier, buffer, sizeof buffer);
            if ((data_read == -1) && (errno != EAGAIN)) {
                ifcLogger.Write(LogLevel::Error, "Error while reading events from watchlist: %s", strerror(errno));
                break;
            }
            //
            //  Ok. No error but no data as well.
            if (data_read <= 0) {
                break;
            }

            for (char* ptr = buffer; ptr < (buffer + data_read); ptr += sizeof(struct inotify_event) + event->len) {
                //
                //  Now we have an event.
                event = (struct inotify_event *) ptr;
                //
                //  Extract the mask
                uint32_t mask = event->mask;
                //
                //  Check for file creation. A new content library?
                if (mask & IN_CREATE) {
                    //
                    //  Check if name matches one of the ContentDir name of the domains.
                    for (auto n : ContentLibNames) {
                        bool matchcode=PatternMatch(n.first+"?*", event->name);
                        if (matchcode) {
                            if (ContentLoadNotifier == -1) {
                                std::string name = n.second->ContentDir+"/"+event->name;
                                ContentLoadNotifier = inotify_add_watch(ContentLibNotifier, name.c_str(), IN_CLOSE_WRITE);
                                if (ContentLoadNotifier == -1) {
                                    ifcLogger.Write(LogLevel::Critical, "Could not set close watch on %s", name.c_str());
                                }
                            }
                            ifcLogger.Write(LogLevel::Info, "Created: %s", event->name);
                        }
                    }
                    mask &= (~IN_CREATE);
                }
                if (mask & IN_CLOSE_WRITE) {
                    for (auto n : ContentLibNames) {
                        std::string name = n.second->ContentDir+"/"+event->name;

                        if (n.second->SwitchContent(event->name)) {
                        } else {
                            ifcLogger.Write(LogLevel::Critical, "Could not load %s after close", name.c_str());
                        }
                        if (ContentLoadNotifier != -1) {
                            inotify_rm_watch(ContentLibNotifier, ContentLoadNotifier);
                            ContentLoadNotifier = -1;
                        }
                    }
                    ifcLogger.Write(LogLevel::Info, "Closed after Write: %s", event->name);
                    mask &= (~IN_CLOSE_WRITE);
                }
                if (mask & IN_DELETE) {
                    ifcLogger.Write(LogLevel::Info, "Deleted: %s", event->name);
                    mask &= (~IN_DELETE);
                }
                if (mask & IN_MODIFY) {
                    ifcLogger.Write(LogLevel::Info, "Modified: %s", event->name);
                    mask &= (~IN_MODIFY);
                }
                if (mask & IN_MOVED_FROM) {
                    ifcLogger.Write(LogLevel::Info, "Moved from: %s", event->name);
                    mask &= (~IN_MOVED_FROM);
                }
                if (mask & IN_MOVED_TO) {
                    ifcLogger.Write(LogLevel::Info, "Moved to: %s", event->name);
                    mask &= (~IN_MOVED_TO);
                }
                if (mask != 0) {
                    ifcLogger.Write(LogLevel::Info, "Unhandled event: %s", event->name);
                }

             }
         }

        retval = true;
    }
#endif
}

void CHTTPIfcThread::OnMaintenanceTime(uint64_t aTimerId) {
    for (auto n : DomainSpecs) {
        n.second->TryUnload();
    }
    //
    //  The default implementation does restart the timer.
    CIfcThread::OnMaintenanceTime(aTimerId);
}
