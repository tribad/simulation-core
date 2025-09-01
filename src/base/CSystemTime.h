// **************************************************************************
//
// Modul-Name        : CSystemTime.h
// Author            : Hans-Juergen Lange <hjl@simulated-universe.de>
// Creation-Date     : 13.01.2011
// Modification-Date : 05.05.2011 10:13:05
//
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef __EXTENSIONLIBS_TIMERS_LINUXTIMERS_CSYSTEMTIME_INC
#define __EXTENSIONLIBS_TIMERS_LINUXTIMERS_CSYSTEMTIME_INC

#include <string>
// **************************************************************************
//                             F o r w a r d s
// **************************************************************************
// **************************************************************************
//                  C l a s s    d e c l a r a t i o n
// **************************************************************************
class CSystemTime {
public:
    CSystemTime();
    int GetYear();
    int GetMonth();
    int GetDayOfMonth();
    int GetWeekDay();
    int GetHour();
    int GetMinute();
    int GetSecond();
    int GetMilliSecond();
    std::string Format(const char* aFormat);
    std::string Get1123Date();
    void Set(time_t aTime);
    bool operator==(CSystemTime& b);
    bool operator!=(CSystemTime& b);
    bool operator>(CSystemTime& B);
    bool operator<(CSystemTime& B);
private:
    time_t Time;
    int tm_year;
    int tm_mon;
    int tm_wday;
    int tm_mday;
    int tm_hour;
    int tm_min;
    int tm_sec;
    int tm_msec;
    static const char* WeekDay[];
    static const char* Month[];
};
#endif // __EXTENSIONLIBS_TIMERS_LINUXTIMERS_CSYSTEMTIME_INC
