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

#ifndef SIM_H_
#define SIM_H_

#include <pthread.h>
#include <stdint.h>
#include <simifc.h>
#include <logger.h>
/*
 * Forwards
 */
class CSimIfc;
class tHttpRequest;
class tWSMessage;
/*
 * Defintion of the Chunk-Sizes.
 */
#define MAX_UPDATE_FNC (core.updatechunksize)
#define MAX_DELETE_OBJ (core.deletechunksize)
#define MAX_MESSAGE    (core.signalchunksize)
/*
 * This is a chunk of objects to run in the simulation.
 * Chunks are simply chained because we are working only forward.
 */
typedef struct tagSimUpdateChunk {
    struct tagSimUpdateChunk *next = nullptr;
    std::vector<tObjectRef> object;   // A dynamic array
} tSimUpdateChunk;
/*
 * This is a chunk of objects to be deleted.
 * Using such blocks of objects is a bit faster and
 * creates less load on memory
 */
typedef struct tagSimDeleteChunk {
    struct tagSimDeleteChunk *next;
    int count;
    tSimObj **object;
}tSimDeleteChunk;
/*
 * This is a chunk of message processing elements.
 */
typedef struct tagSimProcessChunk {
    struct tagSimProcessChunk *next = nullptr;
    std::vector<std::shared_ptr<tMsg> > msg;
}tSimProcessChunk;
/*
 * This typedef describes a single function that gets exported by the simulation library
 * and is used to exchange the interfaces to use for communication between the simulation
 * and the objects representing the simulation logic.9
 */
using SimInitFnc = CSimIfc* (*)(CCoreIfc*);
/*
 *  This structure holds all information needed access the simulation library
 *  and its interface.
 */

typedef struct tagSimulation {
    void*       handle;
    std::string path;
    SimInitFnc  initsimlib;
    CSimIfc*    ifc;
} tSimulation;

struct tSimCore {
    std::string        objlibpath;
    int                freerunning;
    uint64_t           stepsize;
    uint64_t           endcycle;
    size_t             updatechunksize;
    pthread_spinlock_t updatelock;
    tSimUpdateChunk    *update_first;
    tSimUpdateChunk    *update_end;
    tSimUpdateChunk    *update;
    size_t             signalchunksize;
    pthread_spinlock_t msglock;
    tSimProcessChunk   *msg_end;
    tSimProcessChunk   *msg;
    pthread_mutex_t    waitmutex;
    pthread_cond_t     wait;
    pthread_spinlock_t runninglock;
    uint64_t           workersrunning;
    uint64_t           updatesrunning;
    int                simdbhandle = -1;
    /*
     * Here comes the things needed for the simulation storage
     */
    tSimulation        simulation;
};

extern tSimCore core;
extern CCoreIfc coreIfc;

tSimUpdateChunk *getupdatework(void);
tSimProcessChunk *getprocesswork(void);

bool init_simulation(const std::string&, CLogger& aLog);
void simsetnumberofworkers(int number);
void *simrun(void*);
void simaddobject(const tObjectRef& aObject);
void simdelobject(const tObjectRef& aObject);
void simaddmsg(std::shared_ptr<tMsg> msg) ;
void simaddsig(std::shared_ptr<tSig> sig) ;
void transportsig(tSig *sig) ;

tWSMessage *wssimio(uint64_t connid, tWSMessage* msg) ;
const char* wssimcheckconnection(uint64_t, tHttpRequest*);
void wssimcloseconnection(uint64_t id) ;
void wssimout(uint64_t connid, tWSMessage* sig) ;

void simworkdone(int worker) ;
void simupdatesdone(int worker) ;

#endif /* SIM_H_ */
