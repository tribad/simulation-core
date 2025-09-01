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
#ifndef __TIMER_INC__
#define __TIMER_INC__

#include <stdint.h>
#include <time.h>

struct tTimerEvent {
    tTimerEvent(uint64_t awho, uint64_t aid) {who = awho, what = aid;};
    uint64_t who;    // something
    uint64_t what;   // timer id.
};

typedef struct tagTimer {
    uint64_t        start;      // in milliseconds
    uint64_t        delay;      // in milliseconds
    struct timespec end;        // as timespec
    int             sig;        // signal to use
    uint64_t        who;        // identifier
    uint64_t        id;         // timer id
} tTimer;

void starttimerthread(void);
void addtimer(uint64_t when, uint64_t who, int what, uint64_t timerid);
void removetimer(tTimer *);

#endif
