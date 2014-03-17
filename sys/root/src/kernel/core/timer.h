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


/*============================================
| Compiler Directive
==============================================*/
#ifndef _TIMER_H
#define _TIMER_H


/*============================================
| Includes
==============================================*/


/*============================================
| Declaration
==============================================*/

#if defined(CPU_GNU32) || defined(CPU_CORTEXM)
   #define _SC_CLK_TCK     100 //100 ticks/s 1ticks/ 10ms
#else
   #define _SC_CLK_TCK     1000 //1000 ticks/s 1ticks/ms
#endif

#define CLOCK_REALTIME     0x01
#define CLOCK_VITRUAL      0x02
#define CLOCK_PROF         0x03
#define CLOCK_MONOTONIC    0x04

#define TIMER_ABSTIME      0xFF

typedef unsigned long timer_t; //timer id
typedef unsigned char clockid_t; //only CLOCK_REALTIME supported at this time.

typedef struct timespec {
   time_t tv_sec;     //Seconds.
   long tv_nsec;      //Nanoseconds.
}timespec_t;

//The <time.h> header shall also declare the itimerspec structure, which has at least the following members:
typedef struct itimerspec {
   struct timespec it_interval; //  Timer period.
   struct timespec it_value; //     Timer expiration
}itimerspec_t;


#define __time_ns_to_ms(__ns__) ((__ns__)/1000000L)
#define __time_ms_to_ns(__ms__) ((__ms__)*1000000L)

#define __time_s_to_ms(__ns__) ((__ns__)*1000L)
#define __time_ms_to_s(__ms__) ((__ms__)/1000L)


#endif
