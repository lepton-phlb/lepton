/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting  the provisions above and replace
them with the notice and other provisions required by the [eCos GPL] License.
If you do not delete the provisions above, a recipient may use your version of this file under
either the MPL or the [eCos GPL] License."
*/


/*===========================================
Includes
=============================================*/
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"

#include "kernel/core/time.h"
#include "kernel/core/systime.h"



/*===========================================
Global Declaration
=============================================*/
#define SECS_PER_HOUR 3600L
#define SECS_PER_DAY  86400L


static const unsigned short int __mon_lengths[2][12] =
{
   /* Normal years.  */
   { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
   /* Leap years.  */
   { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:settimeofday
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __settimeofday(struct __timeval *tp, void *tzp){
   settimeofday_t settimeofday_dt;

   settimeofday_dt.tv=tp;
   settimeofday_dt.tz=tzp;

   __mk_syscall(_SYSCALL_SETTIMEOFDAY,settimeofday_dt);

   return settimeofday_dt.ret;
}

/*-------------------------------------------
| Name:gettimeofday
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int __gettimeofday(struct __timeval *tp, void *tzp){
   gettimeofday_t gettimeofday_dt;

   gettimeofday_dt.tv=tp;
   gettimeofday_dt.tz=tzp;

   __mk_syscall(_SYSCALL_GETTIMEOFDAY,gettimeofday_dt);

   return gettimeofday_dt.ret;
}

/*-------------------------------------------
| Name:stime
| Description:
| Parameters:
| Return Type:
| Comments: SVr4, SVID, X/OPEN compliance.
| See:
---------------------------------------------*/
int __stime(time_t *when)
{
   struct timeval tv;
   tv.tv_sec = *when;
   tv.tv_usec = 0;
   return __settimeofday(&tv, (struct timezone *)0);
}

/*-------------------------------------------
| Name:time
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
time_t __time(register time_t *tloc)
{
   struct __timeval tv;
   register struct __timeval *p = &tv;

   gettimeofday(p, NULL);               /* This should never fail... */

   if (tloc) {
      *tloc = p->tv_sec;
   }

   return p->tv_sec;
}

/*-------------------------------------------
| Name:__tm_conv
| Description:
| Parameters:
| Return Type:
| Comments:This is adapted from glibc
|          Copyright (C) 1991, 1993 Free Software Foundation, Inc
| See:
---------------------------------------------*/
void __tm_conv(struct tm *tmbuf,const time_t *t,time_t offset)
{
   long days, rem;
   register long y;
   register unsigned short int *ip;
   ldiv_t ldiv_r;

   ldiv_r = ldiv(*t,SECS_PER_DAY);
   //instead of
   //days = *t / SECS_PER_DAY;
   //rem = *t % SECS_PER_DAY;
   days   = ldiv_r.quot;
   rem    = ldiv_r.rem;

   rem += offset;
   while (rem < 0)
   {
      rem += SECS_PER_DAY;
      --days;
   }
   while (rem >= SECS_PER_DAY)
   {
      rem -= SECS_PER_DAY;
      ++days;
   }
   ldiv_r = ldiv(rem, SECS_PER_HOUR);
   //instead of
   //tmbuf->tm_hour = rem / SECS_PER_HOUR;
   tmbuf->tm_hour = ldiv_r.quot;

   ldiv_r = ldiv(rem, SECS_PER_HOUR);
   rem = ldiv_r.rem;
   //instead of
   //rem %= SECS_PER_HOUR;

   tmbuf->tm_min = rem / 60;
   tmbuf->tm_sec = rem % 60;
   /* January 1, 1970 was a Thursday.  */
   tmbuf->tm_wday = (4 + days) % 7;
   if (tmbuf->tm_wday < 0)
      tmbuf->tm_wday += 7;
   y = 1970;
   while (days >= (rem = __isleap(y) ? 366 : 365))
   {
      ++y;
      days -= rem;
   }
   while (days < 0)
   {
      --y;
      days += __isleap(y) ? 366 : 365;
   }
   tmbuf->tm_year = y - 1900;
   tmbuf->tm_yday = days;
   ip = (unsigned short int *)__mon_lengths[__isleap(y)];
   for (y = 0; days >= ip[y]; ++y)
      days -= ip[y];
   tmbuf->tm_mon = y;
   tmbuf->tm_mday = days + 1;
   tmbuf->tm_isdst = -1;
}

/*-------------------------------------------
| Name:hit
| Description:
| Parameters:
| Return Type:
| Comments:Internal ascii conversion routine, avoid use of printf, it's a bit big!
| See:
---------------------------------------------*/
static void hit(char * buf, int val){
   *buf = '0' + val%10;
}

/*-------------------------------------------
| Name:__asctime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void __asctime2(register char * buffer, struct tm * ptm)
{
   static const char days[] = "SunMonTueWedThuFriSat";
   static const char mons[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
   int year;

   /*              012345678901234567890123456 */
   strcpy(buffer, "Err Err .. ..:..:.. ....\n");
   if( (ptm->tm_wday >= 0) && (ptm->tm_wday <= 6) )
      memcpy(buffer, days+3*(ptm->tm_wday), 3);

   if( (ptm->tm_mon >= 0) && (ptm->tm_mon <= 11) )
      memcpy(buffer+4, mons+3*(ptm->tm_mon), 3);


   hit(buffer+ 8, ptm->tm_mday/10);
   hit(buffer+ 9, ptm->tm_mday   );
   hit(buffer+11, ptm->tm_hour/10);
   hit(buffer+12, ptm->tm_hour   );
   hit(buffer+14, ptm->tm_min/10);
   hit(buffer+15, ptm->tm_min   );
   hit(buffer+17, ptm->tm_sec/10);
   hit(buffer+18, ptm->tm_sec   );

   year = ptm->tm_year + 1900;
   hit(buffer+20, year/1000);
   hit(buffer+21, year/100);
   hit(buffer+22, year/10);
   hit(buffer+23, year);
}

/*-------------------------------------------
| Name:asctime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char * __asctime(const struct tm * timeptr){
   static char timebuf[26];
   if( timeptr == 0 ) return 0;
   __asctime2(timebuf,(struct tm*)timeptr);
   return timebuf;
}

/*-------------------------------------------
| Name:asctime_r
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char * __asctime_r(const struct tm *timeptr, char *timebuf){
   if( timeptr == 0 ) return 0;
   __asctime2(timebuf,(struct tm*)timeptr);
   return timebuf;
}


/*-------------------------------------------
| Name:ctime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char * __ctime(const time_t * timep){
   static char cbuf[26];
   struct tm tmb;
   struct timezone tz;
   time_t offt;

   gettimeofday((void*)0, &tz);

   offt = (time_t)(-tz.tz_minuteswest*60L);

   /* tmb.tm_isdst = ? */
   __tm_conv(&tmb, timep, offt);

   __asctime2(cbuf, &tmb);

   return cbuf;
}

/*-------------------------------------------
| Name:ctime_r
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char *__ctime_r(const time_t *timep, char *cbuf){
   struct tm tmb;
   struct timezone tz;
   time_t offt;

   gettimeofday((void*)0, &tz);

   offt = (time_t)(-tz.tz_minuteswest*60L);

   /* tmb.tm_isdst = ? */
   __tm_conv(&tmb, timep, offt);

   __asctime2(cbuf, &tmb);

   return cbuf;
}

/*-------------------------------------------
| Name:gmtime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
struct tm * __gmtime(const time_t * timep){
   static struct tm tmb;

   __tm_conv(&tmb, timep, 0L);

   return &tmb;
}

/*-------------------------------------------
| Name:gmtime_r
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
struct tm *__gmtime_r(const time_t *timep, struct tm *tmb){
   __tm_conv(tmb, timep, 0L);
   return tmb;
}

/*-------------------------------------------
| Name:localtime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
struct tm * __localtime(const time_t * timep){
   static struct tm tmb;
   struct timezone tz;
   time_t offt;

   gettimeofday((void*)0, &tz);

   offt = (time_t)(-tz.tz_minuteswest*60L);

   /* tmb.tm_isdst = ? */
   __tm_conv(&tmb, timep, offt);

   return &tmb;
}

/*-------------------------------------------
| Name:localtime_r
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
struct tm *__localtime_r(const time_t *timep, struct tm *tmb){
   struct timezone tz;
   time_t offt;

   gettimeofday((void*)0, &tz);

   offt = (time_t)(-tz.tz_minuteswest*60L);

   /* tmb.tm_isdst = ? */
   __tm_conv(tmb, timep, offt);

   return tmb;
}

/*-------------------------------------------
| Name:__mktime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
time_t __mktime(struct tm *timeptr)
{
static const unsigned char vals[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, /* non-leap */29,};
   long days, secs;
   time_t t;
   struct tm x;
   /* 0:sec  1:min  2:hour  3:mday  4:mon  5:year  6:wday  7:yday  8:isdst */
   register int *p = (int *) &x;
   register const unsigned char *s;
   int d;

   //_sys_tzset();

   memcpy(p, timeptr, sizeof(struct tm));

   d = 400;
   p[5] = (p[5] - ((p[6] = p[5]/d) * d)) + (p[7] = p[4]/12);
   if ((p[4] -= 12 * p[7]) < 0) {
      p[4] += 12;
      --p[5];
   }

   s = vals;
   d = (p[5] += 1900);                          /* Correct year.  Now between 1900 and 2300. */
   if (__isleap(d)) {
      s += 11;
   }

   p[7] = 0;
   d = p[4];
   while (d) {
      p[7] += *s;
      if (*s == 29) {
         s -= 11;                                       /* Backup to non-leap Feb. */
      }
      ++s;
      --d;
   }

   d = p[5] - 1;
   days = -719163L + ((long)d)*365 + ((d/4) - (d/100) + (d/400) + p[3] + p[7]);
   secs = p[0] + 60*( p[1] + 60*((long)(p[2])) )
          + _time_tzinfo[timeptr->tm_isdst > 0].gmt_offset;
   if (secs < 0) {
      secs += 120009600L;
      days -= 1389;
   }
   if ( ((unsigned long)(days + secs/86400L)) > 49710L) {
      return (time_t)-1;
   }
   secs += (days * 86400L);

   t = secs;

   return t;
}

/*-------------------------------------------
| Name:difftime
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
#include <float.h>

#if FLT_RADIX != 2
   #error difftime implementation assumptions violated for you arch!
#endif

double __difftime(time_t time1, time_t time0)
{
#if defined(__IAR_SYSTEMS_ICC__) || defined(__ARMCC_VERSION) 
   return ((double) time1) - time0;
#else
   #if ((LONG_MAX >> DBL_MANT_DIG) == 0)

   /* time_t fits in the mantissa of a double. */
   return ((double) time1) - time0;

   #elif ((LONG_MAX >> DBL_MANT_DIG) >> DBL_MANT_DIG) == 0

   /* time_t can overflow the mantissa of a double. */
   time_t t1, t0, d;

   d = ((time_t) 1) << DBL_MANT_DIG;
   t1 = time1 / d;
   time1 -= (t1 * d);
   t0 = time0 / d;
   time0 -= (t0*d);

   /* Since FLT_RADIX==2 and d is a power of 2, the only possible
    * rounding error in the expression below would occur from the
    * addition. */
   return (((double) t1) - t0) * d + (((double) time1) - time0);

   #else
      #error difftime needs special implementation on your arch.
   #endif
#endif
}

/*-------------------------------------------
| Name:strftime
| Description:
| Parameters:
| Return Type:
| Comments: very simple implementation
| See:
---------------------------------------------*/
static const char sweekdays [7] [4] = {
   "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static const char weekdays [7] [10] = {
   "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};
static const char smonths [12] [4] = {
   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static const char*  months [12] = {
   "January", "February", "March", "April", smonths[5-1], "June",
   "July", "August", "September", "October", "November", "December"
};
static const char ampm [4] [3] = {
   "am", "pm",
   "AM", "PM"
};

static int  i2a ( char* dest,unsigned int x )
{
   int div = 10;
   *dest++ = x/div + '0';
   *dest++ = x%div + '0';
   *dest++ = '\0';
   return 2;
}

size_t  __strftime ( char* dst, size_t max, const char* format, const struct tm* tm )
{
   char*         p = dst;
   const char*   src;
   unsigned int no;
   char buf [5];


   for (; *format != '\0'; format++ ) {
      if (*format == '%') {
         if (*++format == '%') {
            *p++ = '%';
         }
         else
again:
            switch (*format) {
//          case '%': *p++ = '%';                                break;			// reduce size of jump table
            case 'n': *p++ = '\n';                               break;
            case 't': *p++ = '\t';                               break;
            case 'O': case 'E': ++format; goto again;
            case 'c': src = "%b %a %d %k:%M:%S %Z %Y";           goto _strf;
            case 'r': src = "%I:%M:%S %p";                       goto _strf;
            case 'R': src = "%H:%M";                             goto _strf;
            case 'x': src = "%b %a %d";                          goto _strf;
            case 'X': src = "%k:%M:%S";                          goto _strf;
            case 'D': src = "%m/%d/%y";                          goto _strf;
            case 'T': src = "%H:%M:%S";
_strf: p  += strftime (p, (size_t)(dst+max-p), src, tm);          break;
            case 'a': src = sweekdays [tm->tm_wday];             goto _str;
            case 'A': src = weekdays  [tm->tm_wday];             goto _str;
            case 'h':
            case 'b': src = smonths   [tm->tm_mon];              goto _str;
            case 'B': src = months    [tm->tm_mon];              goto _str;
            case 'p': src = ampm [tm->tm_hour > 12 ? 3 : 2]; goto _str;
            case 'P': src = ampm [tm->tm_hour > 12 ? 1 : 0]; goto _str;
            case 'C': no  = tm->tm_year/100 + 19;                goto _no;
            case 'd': no  = tm->tm_mday;                         goto _no;
            case 'e': no  = tm->tm_mday;                         goto _nos;
            case 'H': no  = tm->tm_hour;                         goto _no;
            case 'I': no  = tm->tm_hour % 12;                    goto _no;
            case 'j': no  = tm->tm_yday;                         goto _no;
            case 'k': no  = tm->tm_hour;                         goto _nos;
            case 'l': no  = tm->tm_hour % 12;                    goto _nos;
            case 'm': no  = tm->tm_mon + 1;                      goto _no;
            case 'M': no  = tm->tm_min;                          goto _no;
            case 'S': no  = tm->tm_sec;                          goto _no;
            case 'u': no  = tm->tm_wday ? tm->tm_wday : 7;       goto _no;
            case 'w': no  = tm->tm_wday;                         goto _no;
            case 'U': no  = (tm->tm_yday - tm->tm_wday + 7) / 7; goto _no;
            case 'W': no  = (tm->tm_yday - (tm->tm_wday - 1 + 7) % 7 + 7) / 7; goto _no;
            case 'Z':
#ifdef WANT_TZFILE_PARSER
               tzset(); src = tzname[0];
#else
               src = "";       //"[unknown timezone]";
#endif
               goto _str;
            case 'Y': i2a ( buf+0, (unsigned int)(tm->tm_year / 100 + 19) );
               i2a ( buf+2, (unsigned int)(tm->tm_year % 100) );
               src = buf;
               goto _str;
            case 'y': no  = tm->tm_year % 100;                   goto _no;
_no: i2a ( buf, no );                                            /* append number 'no' */
               src = buf;
               goto _str;
_nos: i2a ( buf, no );                                           /* the same, but '0'->' ' */
               if (buf[0] == '0')
                  buf[0] = ' ';
               src = buf;
_str: while (*src  &&  p < dst+max)                              /* append string */
                  *p++ = *src++;
               break;
            };
      } else {
         *p++ = *format;
      }

      if (p >= dst+max)
         break;
   }

   *p = '\0';
   return p - dst;
}


/*===========================================
End of Sourcestatvfs.c
=============================================*/
