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
