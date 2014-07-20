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
#ifndef _SYSTIME_H
#define _SYSTIME_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/time.h"


/*===========================================
Declaration
=============================================*/
#define HZ      _SC_CLK_TCK //1 tiks/ms
//_POSIX_TZNAME_MAX	6
#define TZNAME_MAX 6

#ifndef TZNAME_MAX
   #define TZNAME_MAX _POSIX_TZNAME_MAX
#endif

typedef struct {
   long gmt_offset;
   long dst_offset;
   short day;                                           /* for J or normal */
   short week;
   short month;
   short rule_type;                             /* J, M, \0 */
   char tzname[TZNAME_MAX+1];
} rule_struct;

extern struct __timeval xtime;
extern rule_struct _time_tzinfo[2];

extern jiff_t jiffies;

int      _sys_settimeofday(struct __timeval *tv, register struct timezone *tz);
int      _sys_gettimeofday(register struct __timeval *tv, struct timezone *tz);
time_t   _sys_mktime(struct tm *timeptr);
void do_timer(void);


#endif
