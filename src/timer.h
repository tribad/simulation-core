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
