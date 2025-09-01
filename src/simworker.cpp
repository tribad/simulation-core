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
#include <malloc.h>
#include <stdint.h>
#include <signal.h>
#include <algorithm>
#include <netpack.h>
#include <msg.h>
#include <simobj.h>
#include <sim.h>
#include <simworker.h>

static int        workercount=0;
static tSimWorker workers[NUMBER_OF_WORKERS];
static uint64_t   cycle;

static void init(tSimWorker *aWorker) {
    pthread_mutex_init(&(aWorker->startmutex), 0);
    pthread_cond_init(&(aWorker->start), 0);
    aWorker->run=0;
}

static void *simworker_run(void* ptr) {
    tSimUpdateChunk  *updates;
    tSimProcessChunk *processes;
    tSimWorker       *worker=(tSimWorker*)ptr;
    sigset_t         signals;

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
     * Endless loop
     */
    for(;;) {
        /*
         * Wait for event.
         */
        pthread_mutex_lock(&(worker->startmutex));
        if (worker->run==0) {
            pthread_cond_wait(&(worker->start), &(worker->startmutex));
        } else {
        }
        //Log.Error("Cycle: %lu\n", cycle);
        worker->run=0;
        pthread_mutex_unlock(&(worker->startmutex));
        /*
         * Ask simulation for work. Updates first.
         */
        while ((updates=getupdatework())!=0) {
            //Log.Error("Chunk: %p\n", updates);
            for (size_t i = 0; i < updates->object.size();) {
                auto & o = updates->object[i];
                /*
                 * If update returns false no further update of this object needed.
                 * Then we remove it from the update list and go on.
                 * Because we stay at the actual position we must check again if the update function
                 * has been already removed.
                 */
                if (o->update(cycle) == false) {
                    /*
                     * not the last element?
                     */
                    if (o != (updates->object.back())) {
                        //
                        //  copy from end to position
                        o = updates->object.back();
                    }
                    //
                    // Always pop from the end.
                    updates->object.pop_back();
                } else {
                    ++i;
                }
            }
            //Log.Error("%i Objects in Chunk: %p\n", updates->count, updates);
        }
        //
        //  Sync on updates.
        simupdatesdone(worker->number);
        /*
         * Wait for event.
         */
        pthread_mutex_lock(&(worker->startmutex));
        if (worker->run == 0) {
            pthread_cond_wait(&(worker->start), &(worker->startmutex));
        } else {
        }
        worker->run = 0;
        pthread_mutex_unlock(&(worker->startmutex));

        while ((processes=getprocesswork())!=NULL) {
            for (size_t i = 0; i < processes->msg.size();++i) {
                std::shared_ptr<tMsg> tomsg = processes->msg[i];
                //
                //  Call process on object targets.
                tReference ref = std::get<tReference>(tomsg->dst);

                tObjectRef obj(ref.m_id, (tSimObj*)ref.m_ptr);
                if (obj.Raw() != nullptr) {
                    obj->Process(tomsg);
                }
            }
            free (processes);
        }
        simworkdone(worker->number);
    }
    return (0);
}

void startworkers(uint64_t nextcycle) {
    int  i;
    cycle=nextcycle;

    for (i=0; i<workercount; i++) {
        pthread_mutex_lock(&((workers+i)->startmutex));
        workers[i].run=1;
        pthread_cond_broadcast(&((workers+i)->start));
        pthread_mutex_unlock(&((workers+i)->startmutex));
    }
}

void simworker(int number) {
    int i;
    pthread_attr_t attr;
    /*
     * Start the workers
     */
    workercount = (std::min(number, NUMBER_OF_WORKERS));
    for (i=0; i< workercount; i++) {
        tSimWorker::Log().Write (LogLevel::Error, "Starting worker number %i", i);
        /*
         * Do init.
         */
        init(workers+i);
        workers[i].number=i;
        /*
         * Start the thread now
         */
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&((workers+i)->id), &attr, simworker_run, workers+i);
    }
}
