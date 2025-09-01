// MIT License
//
// Copyright (c) 2016 Hans-Juergen Lange<hjl@simulated-universe.de>
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
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>

#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include <map>

#include "msg.h"
#include "packettype.h"
#include "timer.h"

static pid_t           pid;
static pthread_mutex_t timer_mutex;
static pthread_cond_t  timer_event;
static int timercount = 0;

static  std::multimap<uint64_t, tTimer *> timer_list;

bool gTimerRunning = false;

bool operator<(struct timespec & left, struct timespec& right) {
    if ((left.tv_sec < right.tv_sec) || ((left.tv_sec == right.tv_sec) && (left.tv_nsec < right.tv_nsec))) {
        return true;
    }
    return false;
}
static void *do_timer(void *parameter) {
    //
    //
    (void)parameter;
    //
    int err = 0;
    /*
     * Set the pid to use it in all sigqueue calls.
     */
    pid = getpid();
    //
    // All work is done with the mutex locked.
    // it gets unlocked if we wait for the next event
    // and is locked again if the event-wait function returns.
    for (gTimerRunning = true;gTimerRunning;) {
        pthread_mutex_lock(&timer_mutex);
        //
        //  Check if list is empty.
        //  With an empty list we only wait for an event.
        if (timer_list.empty()) {
            pthread_cond_wait(&timer_event, &timer_mutex);
        } else {
            //
            //  We extract the end timespec from the first element in the list.
            struct timespec end = timer_list.begin()->second->end;
            //
            //  Wait until the timeout reached or someone wakes us.
            err=pthread_cond_timedwait(&timer_event, &timer_mutex, &end);
            //
            //  On a timeout we signal the threads and remove the timers.
            if (err == ETIMEDOUT) {
                /*
                 * Get the time we have now. As we should have waited until that absolut time
                 * We can signal all timers that have an end-time smaler
                 */
                clock_gettime(CLOCK_REALTIME, & (end));
                auto ti = timer_list.begin();
                //
                //  Go along the running timer to find all timers that must be signaled.
                while ((ti != timer_list.end()) && (ti->second->end < end)) {
                    //
                    //  Using a local makes it more readable.
                    auto bt = ti->second;
                    //
                    //  Create an event and a package to signal-queue
                    std::shared_ptr<tTimerEvent> ev     = std::make_shared<tTimerEvent>(bt->who, bt->id);
                    //
                    //  We cannot use shared_ptrs for the signal queue elements.
                    //  But for the data they transport.
                    tTimerPackage* packet = new tTimerPackage(ev->who, ev);

                    union sigval val;
                    val.sival_ptr = (void*)(packet);
                    //
                    //  Queue the signal for our process.
                    sigqueue(pid, bt->sig, val);
                    //
                    //  Remove the timer from the timer list. Do the cleanup.
                    timercount--;
                    timer_list.erase(ti);
                    ti = timer_list.begin();
                    delete bt;
                }
            } else {
            }
        }
        pthread_mutex_unlock(&timer_mutex);
    }
    return nullptr;
}

void starttimerthread(void) {
    /*
     * We start another thread to handle timer event by signals.
     */
    pthread_attr_t wsattr;
    pthread_t      ws_id;
    /*
     * Init event and mutex to handle the event.
     */
    pthread_cond_init(&timer_event, 0);
    pthread_mutex_init(&timer_mutex,0);
    /*
     * Start the thread now
     */
    pthread_attr_init(&wsattr);
    pthread_attr_setdetachstate(&wsattr, PTHREAD_CREATE_DETACHED);
    pthread_create(&ws_id, &wsattr, do_timer, 0);
}

void addtimer(uint64_t when, uint64_t who, int what, uint64_t timerid) {
    tTimer *nt=new tTimer;

    nt->id    = timerid;
    nt->who   = who;
    nt->sig   = what;
    nt->delay = when;
    /*
     * Get the time we have now.
     */
    clock_gettime(CLOCK_REALTIME, & (nt->end));
    /*
     * We need the end value as a key into the map.
     * So we calculate the start time as milliseconds.
     */
    nt->start = nt->end.tv_nsec / 1000000ul;
    nt->start+= nt->end.tv_sec * 1000ul;
    /*
     * Because we give the delay in milliseconds and the timespec has
     * seconds an nanoseconds we take the modulo an make it nanoseconds
     * to be added to the now value.
     */
    nt->end.tv_nsec+=((when%1000ul)*1000000ul);
    /*
     * If come across a second we add a seconds to the tv_sec and reduce the
     * nanosecond part according.
     */
    if (nt->end.tv_nsec >= 1000000000l) {
        nt->end.tv_sec++;
        nt->end.tv_nsec -= 1000000000ul;
    }
    /*
     * At the end we add the seconds to the now value.
     */
    nt->end.tv_sec += (when/1000ul);
    pthread_mutex_lock(&timer_mutex);
    /*
     * check if list is empty
     */
    if (timer_list.empty()) {
        /*
         * Now add the timer to the list.
         */
        timer_list.insert(std::pair<uint64_t, tTimer*>(nt->start+nt->delay, nt));
        /*
         * This is simple. Signal the event.
         */
        pthread_cond_signal(&timer_event);
    } else {
        /*
         * Now add the timer to the list.
         */
        timer_list.insert(std::pair<uint64_t, tTimer*>(nt->start+nt->delay, nt));
        /*
         * Check if the new end time is smaller than the running timer.
         * Than we must wake up the thread to take the new time.
         */
        if ((nt->start + nt->delay) < timer_list.begin()->first) {
            pthread_cond_signal(&timer_event);
        } else {
            /*  Let it go. */
        }
    }
    pthread_mutex_unlock(&timer_mutex);
}



