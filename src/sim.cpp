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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <sys/select.h>
#include <dirent.h>
#include <ctype.h>
#include <stdint.h>
#include <signal.h>
#include <libpq-fe.h>
#include <arpa/inet.h>
#include <dlfcn.h>

#include <netpack.h>
#include <msg.h>
#include <simobj.h>
#include <db.h>
#include <sim.h>
#include <simworker.h>
#include <logger.h>

extern bool    persistent;
extern bool    cycleoutput;

extern std::string dbname;
extern std::string dbtype;
extern std::string dbhost;
extern std::string dbport;
extern std::string dbuser;
extern std::string dbpasswd;


extern uint64_t maxcycle;

static int      number_of_workers   = 2;
static uint64_t workers_start_value = -1&(~3);
/*
 * The simulation can be stopped through the running variable.
 */
extern bool running;
/*
 * This is the initial value for the stepsize of the simulation.
 */
#define STEPSIZE 1000ull
/*
 * This is the simulation main structure.
 */
tSimCore core;
/*
 * This is the simulation interface with its function pointers.
 */
CCoreIfc coreIfc;
/*
 * Loading the configuration of the simulation.
 * This maybe pathes cycle times and the like.
 */
static void loadconfig(void) {

}

void loadsimulation(tSimCore *simcore, CLogger& aLog) {
    simcore->simulation.initsimlib = nullptr;
    if (!simcore->objlibpath.empty()) {
        simcore->simulation.handle=dlopen(simcore->objlibpath.c_str(), RTLD_NOW);
        if (simcore->simulation.handle != nullptr) {
            simcore->simulation.path       = simcore->objlibpath;
            simcore->simulation.initsimlib = (SimInitFnc)(dlsym(simcore->simulation.handle, "createsimlibifc"));
        }
        if (simcore->simulation.initsimlib != nullptr ) {
            simcore->simulation.ifc = simcore->simulation.initsimlib(&coreIfc);
        } else {
            simcore->simulation.ifc = 0;
            aLog.Write(LogLevel::Warn, "Could not load simulation: %s", dlerror());
        }
    } else {
        aLog.Write(LogLevel::Info, "No simulation set.");
    }
}
/*
 * Initialization of the simulation and loading of configuration
 * and the simulation data. All data is read from the DB.
 */
bool init_simulation(const std::string& simpath, CLogger& aLog) {
    /*
     * These are the initializations for the simulation loop and the stepper
     * mechanism.
     */
    pthread_mutex_init(&(core.waitmutex), 0);
    pthread_cond_init(&(core.wait), 0);
    core.objlibpath  = simpath;
    pthread_spin_init(&(core.runninglock), PTHREAD_PROCESS_PRIVATE);
    /*
     * These are the initializations for the update chunk handling.
     */
    pthread_spin_init(&(core.updatelock), PTHREAD_PROCESS_PRIVATE);
    core.update_first   = nullptr;
    core.update_end     = nullptr;
    core.update         = nullptr;
    core.updatesrunning = 0u;
    /*
     * These are the initializations for the message chunk handling.
     */
    pthread_spin_init(&(core.msglock), PTHREAD_PROCESS_PRIVATE);
    core.msg_end = nullptr;
    core.msg     = nullptr;
    /*
     *  If we have some configuration data to load we do that before loading the
     *  simulation library.
     */
    loadconfig();
    /*
     *  Load the simulation library and exchange the interface objects.
     */
    loadsimulation(&core, aLog);
    /*
     * Only if we could load the simulation lib we continue
     */
    if (core.simulation.ifc != nullptr) {
        /*
         *  Take the starting time before loading from the DB
         */
        return (true);
    }
    return (false);
}
/*
 * A worker calls this function to get a new work package.
 * If there is no more work todo the worker gets a NULL pointer.
 */
tSimUpdateChunk *getupdatework(void) {
    tSimUpdateChunk *u;

    pthread_spin_lock(&(core.updatelock));
    u = core.update;
    if (u != nullptr) {
        core.update = u->next;
    } else {
    }
    pthread_spin_unlock(&(core.updatelock));
    return (u);
}
/*
 * A worker calls this function to get a new chunk of
 * signals to be delivered to the objects.
 * If there are no more signals to send the worker gets
 * a NULL pointer.
 */
tSimProcessChunk *getprocesswork(void) {
    tSimProcessChunk *u;

    pthread_spin_lock(&(core.msglock));
    u = core.msg;
    if (u != nullptr) {
        core.msg = u->next;
        if (core.msg == nullptr) {
            core.msg_end = nullptr;
        } else {
        }
    } else {
    }
    pthread_spin_unlock(&(core.msglock));
    return (u);
}
/*
 * add a new object to the update list.
 */
void simaddobject(const tObjectRef& newobject) {
    pthread_spin_lock(&(core.updatelock));
    /*
     * Check if list is empty.
     */
    if (core.update_end == nullptr) {
        /*
         * The first chunk of objects gets set at both ends of the list.
         */
        core.update_end            = core.update_first = new tSimUpdateChunk;
        core.update_end->object.reserve(MAX_UPDATE_FNC);

    } else {
        /*
         * Check if the chunk is full.
         */
        if (core.update_end->object.size() == MAX_UPDATE_FNC) {
            /*
             * Create a new chunk at the end of the list.
             */
            core.update_end->next   = new tSimUpdateChunk;
            core.update_end         = core.update_end->next;
            core.update_end->object.reserve(MAX_UPDATE_FNC);
        }
    }
    /*
     * Add object to the end of the last chunk of the list.
     */
    core.update_end->object.push_back(newobject);
    pthread_spin_unlock(&(core.updatelock));
}

/*
 * Add an object to the list of objects to be delete at end of the next cycle.
 */
void simdelobject(const tObjectRef& aObject) {
    static_cast<void>(aObject);
}
void simaddmsg(std::shared_ptr<tMsg> msg) {
    pthread_spin_lock(&(core.msglock));
    /*
     * Check if list is empty.
     */
    if (core.msg_end == nullptr) {
        /*
         * The first chunk of signals gets set at both ends of the list.
         */
        core.msg = core.msg_end = new tSimProcessChunk;
        core.msg->msg.reserve(MAX_MESSAGE);
    } else {
        /*
         * Check if the chunk is full.
         */
        if (core.msg_end->msg.size() == MAX_MESSAGE) {
            /*
             * Create a new chunk at the end of the list.
             */
            core.msg_end->next = new tSimProcessChunk;
            core.msg_end       = core.msg_end->next;
            core.msg_end->msg.reserve(MAX_MESSAGE);

        }
    }
    /*
     * Add signal to the end of the last chunk of the list.
     */
    core.msg_end->msg.push_back(msg);
    pthread_spin_unlock(&(core.msglock));
}

/*
 * This is the simulation manager entry point.
 * The manager is running in an own thread.
 */
void *simrun(void* parameter) {
    //
    //
    (void)parameter;
    //
    struct timeval  timeout;
    struct timespec start;
    struct timespec end;
    int64_t         u_start;
    int64_t         u_end;
    int64_t         u_diff;
    /*
     * Block a bunch of signals from the kernel to not let the thread
     * stop on them.
     */
    sigset_t signals;

    sigemptyset(&signals);
    for (int i=SIGRTMIN;i<SIGRTMAX;++i) {
        sigaddset(&signals, i);
    }
    sigaddset(&signals, SIGHUP);
    sigaddset(&signals, SIGPIPE);
    sigaddset(&signals, SIGURG );
    sigaddset(&signals, SIGIO);

    sigprocmask(SIG_BLOCK , &signals, NULL);
    /*
     * Create the defined number of workers.
     */
    simworker(number_of_workers);
    /*
     * Initialize the update pointer to the first chunk.
     */
    core.update=core.update_first;
    /*
     * Will never stop.
     */
    for (;(running) && (coreIfc.relCycle < maxcycle) ;) {
        /*
         * Get the start time in microseconds. This is needed to calculate the wait-time.
         */
        clock_gettime (CLOCK_MONOTONIC_RAW , &start);
        u_start=(start.tv_sec*1000000ull)+(start.tv_nsec/1000ull);
        /*
         * Set the flags as if all workers are running.
         */
        core.updatesrunning=workers_start_value;
        /*
         * Signal start of cycle to the simulation
         */
        core.simulation.ifc->StartCycle();
        /*
         * Start all workers with the actual cycle number.
         * In this first run all updates are done.
         */
        startworkers(coreIfc.absCycle);
        /*
         * Check if we are running without a fixed step-time.
         */
        if (core.freerunning==0) {
            /*
             * We are running with a fixed step-time.
             *
             * Only wait for event if any worker is still updating the objects.
             */
           pthread_mutex_lock(&(core.waitmutex));
           if (core.updatesrunning != (uint64_t)(-1)) {
               pthread_cond_wait(&(core.wait), &(core.waitmutex));
           } else {
           }
           pthread_mutex_unlock(&(core.waitmutex));
           /*
            * Set the flags as if all workers are running.
            */
           core.workersrunning = workers_start_value;
           /*
            * Start all workers with the actual cycle number for processing signals.
            */
           startworkers(coreIfc.absCycle);
           /*
            * Wait until all signals are done.
            */
           pthread_mutex_lock(&(core.waitmutex));
           if (core.workersrunning!=(uint64_t)(-1)) {
               pthread_cond_wait(&(core.wait), &(core.waitmutex));
           } else {
           }
           pthread_mutex_unlock(&(core.waitmutex));

           core.simulation.ifc->EndCycle();
            clock_gettime (CLOCK_MONOTONIC_RAW , &end);
            u_end  = (end.tv_sec*1000000ull)+(end.tv_nsec/1000ull);
            u_diff = (core.stepsize*1000ull)-(u_end-u_start);
            if (u_diff > 0) {
                timeout.tv_sec=(long)(u_diff/1000000ull);
                timeout.tv_usec=(long)(u_diff%1000000ull);
                if (cycleoutput) {
                    //Log.Error("Cycle: %lu Diff: %lu\n", coreIfc.absCycle, u_diff);
                }

                select(0,0,0,0,&timeout);
            }
            /*
             * re-initialize the update pointer.
             */
            core.update = core.update_first;
            coreIfc.absCycle++;
            coreIfc.relCycle++;
        } else {
            /*
             * We are running free.
             *
             * We wait until the updates are done.
             */
            pthread_mutex_lock(&(core.waitmutex));
            if (core.updatesrunning != (uint64_t)(-1)) {
                pthread_cond_wait(&(core.wait), &(core.waitmutex));
            } else {
            }
            pthread_mutex_unlock(&(core.waitmutex));
            /*
             * Set the flags as if all workers are running.
             */
            core.workersrunning=workers_start_value;
            /*
             * Start all workers with the actual cycle number for processing signals.
             */
            startworkers(coreIfc.absCycle);
            /*
             * Wait until all signals are done.
             */
            pthread_mutex_lock(&(core.waitmutex));
            if (core.workersrunning!=(uint64_t)(-1)) {
                pthread_cond_wait(&(core.wait), &(core.waitmutex));
            } else {
            }
            pthread_mutex_unlock(&(core.waitmutex));

            core.simulation.ifc->EndCycle();
            /*
             * re-initialize the update pointer.
             */
            core.update=core.update_first;
            coreIfc.absCycle++;
            coreIfc.relCycle++;
        }
    }
    exit(0);
    return (0);
}


void simworkdone(int worker) {
    pthread_mutex_lock(&(core.waitmutex));
    core.workersrunning=core.workersrunning|(1<<worker);
    if (core.workersrunning==(uint64_t)(-1)) {
        pthread_cond_broadcast(&(core.wait));
    }
    pthread_mutex_unlock(&(core.waitmutex));

}

void simupdatesdone(int worker) {
    pthread_mutex_lock(&(core.waitmutex));
    core.updatesrunning = core.updatesrunning|(1<<worker);
    if (core.updatesrunning==(uint64_t)(-1)) {
        pthread_cond_broadcast(&(core.wait));
    }
    pthread_mutex_unlock(&(core.waitmutex));

}

void simsetnumberofworkers(int number) {
    if (number<1) {
        number_of_workers=1;
        workers_start_value=-2;
    } else {
        if (number > 63) {
            number_of_workers=64;
            workers_start_value=0;
        } else {
            number_of_workers=number;
            workers_start_value=((uint64_t)(-1) << number);
        }
    }
}
