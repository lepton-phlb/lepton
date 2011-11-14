/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Philippe Le Boulanger.
Portions created by Philippe Le Boulanger are Copyright (C) 2011 <lepton.phlb@gmail.com>.
All Rights Reserved.

Contributor(s): Jean-Jacques Pitrolle <lepton.jjp@gmail.com>.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license 
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable 
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under 
the MPL, indicate your decision by deleting  the provisions above and replace 
them with the notice and other provisions required by the [eCos GPL] License. 
If you do not delete the provisions above, a recipient may use your version of this file under 
either the MPL or the [eCos GPL] License."
*/
#ifndef _TIME_H
#define _TIME_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/timer.h"
#include "kernel/core/kernel_clock.h"

#include "kernel/core/sys/siginfo.h"
/*===========================================
Declaration
=============================================*/
#define CLOCKS_PER_SEC	   _SC_CLK_TCK
#define CLK_TCK		      _SC_CLK_TCK


/* That must be the same as HZ ???? */
struct __timeval {
	int32_t tv_sec;
	int32_t tv_usec;
};

#define timeval __timeval

//POSIX tm definition:
//    tm_sec   seconds [0,61]
//    tm_min   minutes [0,59]
//    tm_hour  hour [0,23]
//    tm_mday  day of month [1,31]
//    tm_mon   month of year [0,11]
//    tm_year  years since 1900
//    tm_wday  day of week [0,6] (Sunday = 0)
//    tm_yday  day of year [0,365]
//    tm_isdst daylight savings flag

//
struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

//
struct timezone {
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;	/* type of dst correction */
};

#define	__isleap(year)	\
  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))

//extern clock_t	clock(void);

#ifdef __cplusplus
extern "C" {
#endif

int	   __stime(time_t*);
time_t	__time(time_t*);
time_t	__mktime(struct tm *);
char*    __asctime(const struct tm *);
char*    __asctime_r(const struct tm *, char *);
char*    __ctime (const time_t *);
char*    __ctime_r(const time_t *, char *);

struct tm*  __gmtime(const time_t*);
struct tm*  __gmtime_r(const time_t *, struct tm *);
struct tm*	__localtime(const time_t*);
struct tm*  __localtime_r(const time_t *, struct tm *);

double __difftime(time_t,time_t);
int __settimeofday(struct __timeval *tp, void *tzp);
int __gettimeofday(struct __timeval *tp, void *tzp);
size_t  __strftime ( char* dst, size_t max, const char* format, const struct tm* tm );

//timer
int timer_create(clockid_t, struct sigevent *,timer_t *);
int timer_delete(timer_t*);
int timer_gettime(timer_t*, struct itimerspec *);
int timer_getoverrun(timer_t*);
int timer_settime(timer_t*, int, const struct itimerspec *,struct itimerspec *);

#ifdef __cplusplus
}
#endif

#define stime           __stime
#define time            __time
#define mktime          __mktime
#define asctime         __asctime
#define asctime_r       __asctime_r
#define ctime           __ctime
#define ctime_r         __ctime_r
#define gmtime          __gmtime
#define gmtime_r        __gmtime_r
#define localtime       __localtime
#define localtime_r     __localtime_r
#define difftime        __difftime
#define gettimeofday    __gettimeofday
#define strftime        __strftime

//clock
#define clock_gettime(__clk_id__,__tp__) kernel_clock_gettime(__clk_id__,__tp__)
//to do:
//int clock_getres(clockid_t clock_id, struct timespec *res);
//int clock_settime(clockid_t clock_id, const struct timespec *tp);


#endif
