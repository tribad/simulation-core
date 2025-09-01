/*
 * simworker.h
 *
 *  Created on: Jun 13, 2014
 *      Author: ltoo
 */

#ifndef SIMWORKER_H_
#define SIMWORKER_H_

#include <pthread.h>
#include <logger.h>
/*
 * This is the maximum number of workers we can start.
 * Its a constant to not allow someone to be extreme.
 * And it is bound to the 64-Bit unsigned bitmask about active
 * workers.
 */
#define NUMBER_OF_WORKERS 64

struct tSimWorker {
    int             number;
    pthread_t       id;
    pthread_mutex_t startmutex;
    pthread_cond_t  start;
    int             run;
    static CLogger& Log() {static CLogger  Log(ModulId::SimWorker, "Simulation-Worker"); return Log;}
};

void simworker(int number) ;
void startworkers(uint64_t nextcycle);

#endif /* SIMWORKER_H_ */
