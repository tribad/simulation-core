#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/signalfd.h>
#include <poll.h>
#include <memory.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "logger.h"
#include "netpack.h"

#include "simifc.h"
#include "ifcthread.h"
#include "connection.h"
#include "sigmanager.h"
#include "tcpconnection.h"

#define IFC_MAINTENANCE_TIMER (0x78c4f00117c3c61d)

extern int mainpid;

int CIfcThread::Count = 0;

static void *ifcrun(void*parameter) {
    CIfcThread* thread=(CIfcThread*)(parameter);
    long        retval = 0;

    if (thread->InitInstance()) {
        retval = thread->Run();
    }

    return ((void*)retval);
}

CIfcThread::CIfcThread(ModulId aModulId, const std::string& aModulName) : ifcLogger(aModulId, aModulName)
{
    Index           = Count;
    SignalNo        = SigManager.Get();
    Running         = true;
    ListenHandle    = -1;
    SignalFile      = -1;
    Port            = -1;
    Count++;
    ConnectionCount = 0;
    MaintenanceTime = 3;
}

void CIfcThread::Create(std::string aIfc, int aPort) {
    pthread_attr_t attr;
    /*
     * Store the interface spec.
     */
    Port = aPort;
    Ifc  = aIfc;
    /*
     * Start the thread now
     */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&IfcId, &attr, ifcrun, this);
    ifcLogger.Write(LogLevel::Unbreakable, "Started interface thread");

}

bool CIfcThread::InitInstance() {
    bool               retval  = false;
    int                err     = 0;
    sigset_t           signals;
    struct sockaddr_in addr;
    socklen_t          addrlen;
    /*
     * First we must block all signals that we want to handle here.
     * Therefor we create a set of signals.
     */
    sigemptyset(&signals);
    for (int i=SIGRTMIN;i<SIGRTMAX;++i) {
        sigaddset(&signals, i);
    }
    sigaddset(&signals, SIGHUP);
    sigaddset(&signals, SIGPIPE);
    sigaddset(&signals, SIGURG );
    sigaddset(&signals, SIGIO);

    err=sigprocmask(SIG_BLOCK , &signals, NULL);
    /*
     * If blocking the signals succeeded we create a signalfile
     * from where we can read the signals later.
     */
    if (err==0) {
        sigemptyset(&signals);
        sigaddset(&signals, SignalNo);
        sigaddset(&signals, SIGHUP);
        sigaddset(&signals, SIGPIPE);
        sigaddset(&signals, SIGURG );
        sigaddset(&signals, SIGIO);
        SignalFile=signalfd(-1, &signals, 0);

        if (SignalFile > -1) {
            /*
             * Initialize the listener.
             */
            ListenHandle = socket(AF_INET, SOCK_STREAM, 0) ;
            if (ListenHandle > -1) {
                /*
                 * Set the listener to reuse address.
                 */
                int optval = 1;
                setsockopt(ListenHandle, SOL_SOCKET, SO_REUSEADDR, (void *) &optval, sizeof(int));
                /*
                 * Setup the address structure for the interface binding of the listen socket.
                 */
                memset(&addr, 0, sizeof(addr));
                addr.sin_family      = AF_INET;
                addr.sin_addr.s_addr = inet_addr(Ifc.c_str());
                addr.sin_port        = htons(Port);
                addrlen              = sizeof(addr);

                while ((err=bind(ListenHandle, (struct sockaddr *) &addr, addrlen)) < 0) {
                    if ((errno==EINVAL) || (errno==EADDRINUSE )) {
                        struct timeval to={REBIND_TIMEOUT, 0};
                        ifcLogger.Write(LogLevel::Warn, "Interface already bound. Waiting");
                        select(0, 0, 0, 0, &to);
                    } else {
                        ifcLogger.Write(LogLevel::Error, "Got error on bind to address: %s", strerror(errno));
                        break ;
                    }
                }
                if (err == 0) {
                    /*
                     * The binding to the interface succeeded and we now start the listener.
                     * The listener generates a signal if a new connection is available. This
                     * way we can wait on signals.
                     */
                    optval=fcntl(ListenHandle, F_GETFL);
                    /*
                     * Check if getting the flags succeeded.
                     */
                    if (optval != -1) {
                        /*
                         * Setting additional flags.
                         */
                        err=fcntl(ListenHandle, F_SETFL, optval | O_ASYNC | O_NONBLOCK | O_RDWR);
                        if (err==0) {
                            /*
                             * Set the owning process of the handle so that the thread may get the signals.
                             */
                            err=fcntl(ListenHandle, F_SETOWN, mainpid);
                            if (err==0) {
                                /*
                                 * Set the signal to use on socket events.
                                 */
                                err=fcntl(ListenHandle, F_SETSIG, SignalNo);
                                if (err == 0) {
                                    err=listen(ListenHandle, 64);
                                    if (err == 0) {
                                        retval = true;
                                        ifcLogger.Write(LogLevel::Info, "Listener startet on : %s : %i", Ifc.c_str(), Port);
                                        addtimer(MaintenanceTime*1000ul, 0, SignalNo, IFC_MAINTENANCE_TIMER);
                                    } else {
                                        ifcLogger.Write(LogLevel::Error, "Error on start listening: %s", strerror(errno));
                                    }
                                } else {
                                    ifcLogger.Write(LogLevel::Error, "Error on set signal for listener: %s", strerror(errno));
                                }
                            } else {
                                ifcLogger.Write(LogLevel::Error, "Error on setting the owner for the listener: %s", strerror(errno));
                            }
                        } else {
                            ifcLogger.Write(LogLevel::Error, "Error on setting async flags on listener: %s", strerror(errno));
                        }
                    } else {
                        ifcLogger.Write(LogLevel::Error, "Error on getting flags from listener socket: %s", strerror(errno));
                    }
                }
            } else {
                ifcLogger.Write(LogLevel::Error, "Error on create the listen socket: %s", strerror(errno));
            }
        } else {
            ifcLogger.Write(LogLevel::Error, "Error on opening signal fd: %s", strerror(errno));
        }
    } else {
        ifcLogger.Write(LogLevel::Error, "Error on blocking signals from thread: %s", strerror(errno));
    }
    return retval;
}

void CIfcThread::ExtendedSetup(std::shared_ptr<CConnection> aConnection) {
    //
    //
    static_cast<void>(aConnection);
    //

}

void CIfcThread::OnBrokenPipe(int aSocket, int aState) {
    //
    //
    static_cast<void>(aSocket);
    static_cast<void>(aState);
    //
}

void CIfcThread::OnHangUp(int aSocket, int aState) {
    //
    //
    static_cast<void>(aSocket);
    static_cast<void>(aState);
    //
}

void CIfcThread::OnUrgentData(int aSocket, int aState) {
    //
    //
    static_cast<void>(aSocket);
    static_cast<void>(aState);
    //
}

void CIfcThread::OnEvent(int aSocket, int aState) {
    auto ci = ConnectionMap.find(aSocket);


    if (ci != ConnectionMap.end()) {
        /*
         * Keep a hold on the connection.
         */
        std::shared_ptr<CConnection> runningCon = ci->second;

        if ((aState & POLLIN) || (aState & POLLRDNORM)) {
            aState &= (~(POLLIN | POLLRDNORM));
            /*
             * OnRead should do anything that is related to the transport
             * if the OnRead fails.
             */
            if (runningCon->OnRead() == -EBADF) {
                ifcLogger.Write(LogLevel::Trace, "Close connection: %s with id: %lu after read", runningCon->Name.c_str(), runningCon->ConnectionId);
                CleanupConnection(aSocket);
                //
                //  returning here prevents us from doing the cleanup again.
                //  It would not harm to try it, but its a little speedup not to try.
                return;
            } else {
            }
        }
        if (aState & (POLLOUT | POLLWRNORM )) {
            aState &= (~POLLOUT | POLLWRNORM);
            if (runningCon->OnWrite() == -EBADF) {
                ifcLogger.Write(LogLevel::Trace, "Close connection: %s with id: %lu after write", runningCon->Name.c_str(), runningCon->ConnectionId);
                CleanupConnection(aSocket);
                //
                //  returning here prevents us from doing the cleanup again.
                //  It would not harm to try it, but its a little speedup not to try.
                return;
            }
        }
        if (aState & POLLHUP) {
            aState &= (~POLLHUP);
            runningCon->OnClose();
            ifcLogger.Write(LogLevel::Trace, "Close connection: %s with id: %lu\n", runningCon->Name.c_str(), runningCon->ConnectionId);
            CleanupConnection(aSocket);
            //
            //  returning here prevents us from doing the cleanup again.
            //  It would not harm to try it, but its a little speedup not to try.
            return;
        }
        //
        //  remove all known but not used flags here.
        aState &= (~(POLLRDBAND| POLLWRBAND));
        //
        //  If anything is leftover close the connection.
        if (aState != 0) {
            //
            //  At this point something strange happend. None of the above events happened
            ifcLogger.Write(LogLevel::Info, "Unknown Event: %4.4x on socket: %d", aState, aSocket);
            ifcLogger.Write(LogLevel::Info, "Closing ... %d", aSocket);
            runningCon->OnClose();
            CleanupConnection(aSocket);
        }
    } else {
        //
        //  We may handle other file descriptors as well.
        //  So we let check the FD and if nothing is done we close the fd.
        if (OnFDEvent(aSocket, aState) == false) {
            ifcLogger.Write(LogLevel::Warn, "Could not find connection for socket: %d", aSocket);
            ifcLogger.Write(LogLevel::Info, "Call close on handle to make sure the connection has been removed from system as well.");
            close(aSocket);
        }
    }
}

void CIfcThread::OnConnect(int aSocket, int aState) {
    //
    //
    (void)aSocket;
    (void)aState;
    //
    int                handle;
    struct sockaddr_in addr;
    socklen_t          addrlen = sizeof(addr);

    handle=accept(aSocket, (struct sockaddr*)(&addr), &addrlen);
    if (handle!=-1) {
        /*
         * Using the connection id mask to make wrap-around on bit 56.
         * This prevents a manipulation of the signal offset field in the connection id.
         */
        uint64_t connectid = (++ConnectionCount & CONNECTIONID_MASK)+((uint64_t)(SignalNo) << CONNECTIONID_BITS);
        /*
         * Initialize the newconnection local variable with some values.
         */
        std::shared_ptr<CConnection> newconnection=NewConnection(handle, connectid);
        if ((newconnection != nullptr) && (newconnection->Handle != -1)) {
            //
            //  Do an extended Setup, like the Session in an gnutls connection.
            ExtendedSetup(newconnection);
            //
            //  Prepare anything that is needed to handle all connections in a consistent way.
            ConnectionMap.insert(std::pair<int, std::shared_ptr<CConnection> >(handle, newconnection));
            Connection.insert(std::pair<uint64_t, std::shared_ptr<CConnection> >(connectid, newconnection));
            //
            //  Check if some data may already have appeared.
            int retval = newconnection->Read();
            //
            //  If the reading gives us some sort of error we cleanup all work.
            if (retval == -EBADF) {
                newconnection->Close();
                //
                //  We may not find the connection in the Connection(maps) but a thread
                //  like the SSLIfcThread may use it to handle other stuff.
                CleanupConnection(handle);
            } else {
                //
                //  Dont try to optimize the connection maps.
                //  The content of the maps is needed on cleanup.
            }
        }
    } else {
        ifcLogger.Write(LogLevel::Error, "Error on accepting a new connection: %s", strerror(errno));
    }
}
//
//  Processing packages that are comming through the queue.
void CIfcThread::OnQueue(void* aData) {
    int retval         = 0;
    tPackageType* pack = (tPackageType*)aData;
    //
    //  Connid == 0 is a thread specific event. Mostly timers.
    if (pack->connid != 0) {
        auto ci = Connection.find(pack->connid);

        if (ci != Connection.end()) {
            /*
             * Keep a hold on the connection.
             */
            std::shared_ptr<CConnection> connection = ci->second;
            //
            //  The pack got deleted in the connection.
            retval = connection->OnQueue(pack);
            //
            //  If we got a non zero return value we remove the connection directly
            //  from the connection maps.
            if ((retval != 0) && (retval != -EWOULDBLOCK)) {
                int handle = connection->Handle;
                //
                //  To be sure nothing is left we call the close method again.
                connection->Close();
                //
                //  Cleanup after closing.
                CleanupConnection(handle);
            }
        } else {
            delete pack;
        }
    } else {
        //
        //  Thread specific event handling.
        std::shared_ptr<tTimerEvent> ev;

        switch (pack->type) {
        case ePackageType::Timer:
            ev = ((tTimerPackage*)(pack))->event;
            if (ev) {
                OnInterfaceTimer(ev->what);
            }
            break;
        default:
            break;
        }
        delete pack;
    }
}

void CIfcThread::OnIO(signalfd_siginfo *aSigInfo) {
    if ( SignalNo==(int)(aSigInfo->ssi_signo)) {
        if (aSigInfo->ssi_fd == ListenHandle) {
            OnConnect(aSigInfo->ssi_fd, aSigInfo->ssi_band);
        } else {
            if (aSigInfo->ssi_code != SI_QUEUE) {
                OnEvent(aSigInfo->ssi_fd, aSigInfo->ssi_band);
            } else {
                OnQueue((void*)(aSigInfo->ssi_ptr));
            }
        }
    } else {
    }
}

std::shared_ptr<CConnection> CIfcThread::NewConnection(int aHandle, uint64_t aConnectionId) {
    return (std::make_shared<CTcpConnection>(aHandle, aConnectionId, ifcLogger));
}

int CIfcThread::Run(void) {
    /*
     * Listener now running.
     */
    int                     i;
    size_t                  dataread;
    struct signalfd_siginfo siginfo[SIGNALS_AT_ONCE];
    /*
     * This is the processing loop of incoming signals. These maybe incomming connections
     * or incomming data.
     */
    for (;Running;) {
        i=0;
        dataread=read(SignalFile, siginfo, sizeof(siginfo));
        while (dataread >=sizeof(siginfo[0])) {
//            Log.Error("ifcthread: %i::%i::%i::%i\n", siginfo[i].ssi_fd, siginfo[i].ssi_band, siginfo[i].ssi_code, siginfo[i].ssi_signo);
            switch (siginfo[i].ssi_signo) {
            case SIGHUP:
                OnHangUp(siginfo[i].ssi_fd, siginfo[i].ssi_band);
                break;
            case SIGPIPE:
                OnBrokenPipe(siginfo[i].ssi_fd, siginfo[i].ssi_band);
               break;
            case SIGURG:
                OnUrgentData(siginfo[i].ssi_fd, siginfo[i].ssi_band);
                break;
            default:
                OnIO(siginfo+i);

                break;
            }
            dataread-=sizeof(siginfo[0]);
            i++;
        }
    }
    return (0);
}


void CIfcThread::OnInterfaceTimer(uint64_t aTimerId) {
    switch (aTimerId) {
    case IFC_MAINTENANCE_TIMER:
        OnMaintenanceTime(aTimerId);
        break;
    default:
        break;
    }
}

void CIfcThread::OnMaintenanceTime(uint64_t aTimerId) {
    if (aTimerId != IFC_MAINTENANCE_TIMER) {
        std::ostringstream oss;

        oss << "Got Maintenance Timer call with wrong timer id : " << aTimerId;
        ifcLogger.Write(LogLevel::Error, oss);
    }
    addtimer(MaintenanceTime*1000ul, 0, SignalNo, aTimerId);
}

void CIfcThread::CleanupConnection(int aHandle) {
    auto ci=ConnectionMap.find(aHandle);

    if (ci != ConnectionMap.end()) {
        /*
         * Keep a hold on the connection until leaving this block
         */
        std::shared_ptr<CConnection> connection = ci->second;

        Connection.erase(connection->ConnectionId);
        /*
         * Do not use the handle from the connection as this may be already -1
         * So only the parameter holds the truth.
         */
        ConnectionMap.erase(aHandle);
    }
}

bool CIfcThread::OnFDEvent(int aSocket, int aState) {
    //
    //
    static_cast<void>(aSocket);
    static_cast<void>(aState);
    //
    bool retval = false;

    return retval;
}
