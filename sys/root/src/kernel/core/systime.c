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


#include "kernel/core/errno.h"
#include "kernel/core/types.h"
#include "kernel/core/kernel.h"
#include "kernel/core/devio.h"
#include "kernel/core/process.h"
#include "kernel/core/systime.h"
#include "kernel/fs/vfs/vfs.h"

#include "lib/libc/ctype/ctype.h"

#include <stdlib.h>
#include <string.h>

#ifdef __KERNEL_UCORE_ECOS
   #include <ctype.h>
#endif


/*===========================================
Global Declaration
=============================================*/
// this is the structure holding the base time (in UTC, of course)
struct __timeval xtime={0,0};

// ticks updated by the timer interrupt, to be added to the base time
jiff_t jiffies;

// timezone in effect
static struct timezone xzone;


rule_struct _time_tzinfo[2];
static const char vals[] = {
   'T', 'Z', 0,                                 /* 3 */
   'U', 'T', 'C', 0,                            /* 4 */
   25, 60, 60, 1,                               /* 4 */
   '.', 1,                                              /* M */
   5, '.', 1,
   6,  0,  0,                                           /* Note: overloaded for non-M non-J case... */
   0, 1, 0,                                             /* J */
   ',', 'M',      '4', '.', '1', '.', '0',
   ',', 'M', '1', '0', '.', '5', '.', '0', 0
};

#define TZ    vals
#define UTC   (vals + 3)
#define RANGE (vals + 7)
#define RULE  (vals + 11 - 1)
#define DEFAULT_RULES (vals + 22)

/* Initialize to UTC. */
int daylight = 0;
int timezone = 0;
char *tzname[2] = { (char *) UTC, (char *) (UTC-1) };

/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:_sys_settimeofday
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
extern void __tm_conv(struct tm *tmbuf,const time_t *t,time_t offset);
int _sys_settimeofday(struct __timeval *tv, register struct timezone *tz)
{
   desc_t desc;

   //Setting time is a bit tricky, since we don't really keep the time in the xtime
   //structure.  So we need to figure out the offset from the current time and
   //set xtime based on that.
   if (tv != NULL) {
      ldiv_t lr;
      lr = ldiv(__kernel_get_timer_ticks(), HZ);
      jiffies=lr.quot;
      xtime.tv_sec = tv->tv_sec - jiffies;
      xtime.tv_usec = tv->tv_usec - lr.rem*1000L;
   }

   //specific rtc
   //set kernel time to rtc
   if((desc = _vfs_open("/dev/rtc0",O_WRONLY,0))<0) //ST m41t81
      desc = _vfs_open("/dev/rtc1",O_WRONLY,0);


   if(desc>=0) {
      char buf[8]={0};
      struct tm _tm={ 0, 0, 12, 28, 0, 103 }; //init for test

      /* tmb.tm_isdst = ? */
      __tm_conv(&_tm, &tv->tv_sec, 0L);

      //to remove: only for debug:
      buf[0] = _tm.tm_sec;
      buf[1] = _tm.tm_min;
      buf[2] = _tm.tm_hour;
      buf[3] = _tm.tm_mday;
      buf[4] = _tm.tm_mon;
      buf[5] = _tm.tm_year;

      __kernel_dev_settime(desc,buf,8);

      _vfs_close(desc);
      return 0;
   }

   //specific rtt
   //set kernel time to rtt
   if((desc = _vfs_open("/dev/rtt0",O_WRONLY,0))<0)
      return -1;
   if(desc>=0) {
      time_t time=tv->tv_sec;
      _vfs_write(desc,(char*)&time,sizeof(time_t));
      _vfs_close(desc);
      return 0;
   }

   //success
   return 0;
}

/*-------------------------------------------
| Name:sys_gettimeofday
| Description:return the time of day to the user
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_gettimeofday(register struct __timeval *tv, struct timezone *tz)
{
   struct __timeval tmp_tv;
   ldiv_t lr;

   //load the current time into the structures passed
   if (tv != NULL) {
      lr = ldiv(__kernel_get_timer_ticks(), HZ);
      jiffies=lr.quot;
      //printf("jif=%d\n",jiffies);
      tmp_tv.tv_sec = xtime.tv_sec + jiffies;
      tmp_tv.tv_usec = xtime.tv_usec + lr.rem*1000L;
      if (memcpy(tv, &tmp_tv,sizeof(struct __timeval)))
         return -EFAULT;
   }

   if (tz != NULL)
      if (memcpy(tz, &xzone,sizeof(struct timezone)))
         return -EFAULT;

   ///success
   return 0;
}

/*-------------------------------------------
| Name:do_timer
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void do_timer(void){
   (*(jiff_t *)&jiffies)++;
}


/*-------------------------------------------
| Name:getoffset
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static const char *getoffset(register const char *e, int *pn)
{
   register const char *s = RANGE-1;
   int n;
   int f;

   n = 0;
   f = -1;
   do {
      ++s;
      if (isdigit(*e)) {
         f = *e++ - '0';
      }
      if (isdigit(*e)) {
         f = 10 * f + (*e++ - '0');
      }
      if (((unsigned int)f) >= (unsigned)*s) {          //patch (unsigned) for disable warning
         return NULL;
      }
      n = (*s) * n + f;
      f = 0;
      if (*e == ':') {
         ++e;
         --f;
      }
   } while (*s > 1);

   *pn = n;
   return e;
}

/*-------------------------------------------
| Name:getnumber
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static const char *getnumber(register const char *e, int *pn)
{
   /* bcc can optimize the counter if it thinks it is a pointer... */
   register const char *n = (const char *) 3;
   int f;

   f = 0;
   while (n && isdigit(*e)) {
      f = 10 * f + (*e++ - '0');
      --n;
   }

   *pn = f;
   return (n == (const char *) 3) ? NULL : e;
}
/*-------------------------------------------
| Name:_sys_tzset
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _sys_tzset(void)
{
   register const char *e=NULL;
   register char *s;
   int off;
   short *p;
   rule_struct new_rules[2];
   int n, count, f;
   char c;

   //e = getenv(TZ);				/* TZ env var always takes precedence. */


   /* Warning!!!  Since uClibc doesn't do lib locking, the following is
    * potentially unsafe in a multi-threaded program since it is remotely
    * possible that another thread could call setenv() for TZ and overwrite
    * the string being parsed.  So, don't do that... */

   if ((!e /* TZ env var not set... */) || !*e) {
      /* or set to empty string. */
ILLEGAL:                                                /* TODO: Clean up the following... */
      s = _time_tzinfo[0].tzname;
      *s = 'U';
      *++s = 'T';
      *++s = 'C';
      *++s =
         *_time_tzinfo[1].tzname = 0;
      _time_tzinfo[0].gmt_offset = 0;
      goto DONE;
   }

   if (*e == ':') {                             /* Ignore leading ':'. */
      ++e;
   }

   count = 0;
   new_rules[1].tzname[0] = 0;
LOOP:
   /* Get std or dst name. */
   c = 0;
   if (*e == '<') {
      ++e;
      c = '>';
   }

   s = new_rules[count].tzname;
   n = 0;
   while (*e
          && isascii(*e)                        /* SUSv3 requires char in portable char set. */
          && (isalpha(*e)
              || (c && (isalnum(*e) || (*e == '+') || (*e == '-'))))
          ) {
      *s++ = *e++;
      if (++n > TZNAME_MAX) {
         goto ILLEGAL;
      }
   }
   *s = 0;

   if ((n < 3)                                          /* Check for minimum length. */
       || (c && (*e++ != c))            /* Match any quoting '<'. */
       ) {
      goto ILLEGAL;
   }

   /* Get offset */
   s = (char *) e;
   if ((*e != '-') && (*e != '+')) {
      if (count && !isdigit(*e)) {
         off -= 3600;                           /* Default to 1 hour ahead of std. */
         goto SKIP_OFFSET;
      }
      --e;
   }

   ++e;
   if (!(e = getoffset(e, &off))) {
      goto ILLEGAL;
   }

   if (*s == '-') {
      off = -off;                                       /* Save off in case needed for dst default. */
   }
SKIP_OFFSET:
   new_rules[count].gmt_offset = off;

   if (!count) {
      if (*e) {
         ++count;
         goto LOOP;
      }
   } else {                                             /* OK, we have dst, so get some rules. */
      count = 0;
      if (!*e) {                                        /* No rules so default to US rules. */
         e = DEFAULT_RULES;
      }

      do {
         if (*e++ != ',') {
            goto ILLEGAL;
         }

         n = 365;
         s = (char *) RULE;
         if ((c = *e++) == 'M') {
            n = 12;
         } else if (c == 'J') {
            s += 8;
         } else {
            --e;
            c = 0;
            s += 6;
         }

         *(p = &new_rules[count].rule_type) = c;
         if (c != 'M') {
            p -= 2;
         }

         do {
            ++s;
            if (!(e = getnumber(e, &f))
                || (((unsigned int)(f - s[1])) > (unsigned)n)
                || (*s && (*e++ != *s))
                ) {
               goto ILLEGAL;
            }
            *--p = f;
         } while ((n = *(s += 2)) > 0);

         off = 2 * 60 * 60;                     /* Default to 2:00:00 */
         if (*e == '/') {
            ++e;
            if (!(e = getoffset(e, &off))) {
               goto ILLEGAL;
            }
         }
         new_rules[count].dst_offset = off;
      } while (++count < 2);

      if (*e) {
         goto ILLEGAL;
      }
   }

   memcpy(_time_tzinfo, new_rules, sizeof(new_rules));
DONE:
   tzname[0] = _time_tzinfo[0].tzname;
   tzname[1] = _time_tzinfo[1].tzname;
   daylight = !!new_rules[1].tzname[0];
   timezone = new_rules[0].gmt_offset;
}



/*===========================================
End of Sourcesystime.c
=============================================*/
