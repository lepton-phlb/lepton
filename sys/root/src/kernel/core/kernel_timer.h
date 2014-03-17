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
#ifndef _KERNEL_TIMER_H
#define _KERNEL_TIMER_H


/*============================================
| Includes
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/timer.h"

#ifdef USE_ECOS
   #include <cyg/kernel/kapi.h>
   #include "kernel/core/core_rttimer.h"

typedef cyg_handle_t counter_hdl_t;
typedef struct tmr_obj_st {
   alrm_hdl_t alarm_hdl;
   alrm_t alarm_obj;
   counter_hdl_t cnt_obj;
}tmr_obj_t;
#else
typedef long tmr_obj_t;
#endif
/*============================================
| Declaration
==============================================*/

typedef struct kernel_timer_st {
#ifdef USE_SEGGER
   OS_TIMER timer;
#endif

#ifdef USE_ECOS
   tmr_obj_t timer;
#endif
   //
   unsigned char created;
   //
   clockid_t clockid;
   //
   struct sigevent sigevent;
   //
   struct itimerspec itimerspec;
   //
   struct kernel_pthread_st* kernel_pthread;
   //
   unsigned char interval; //0:timer period, 1: interval timer period
}kernel_timer_t;


int kernel_timer_create(clockid_t, struct sigevent *,kernel_timer_t *);
int kernel_timer_delete(kernel_timer_t*);
int kernel_timer_gettime(kernel_timer_t*, struct itimerspec *);
int kernel_timer_getoverrun(kernel_timer_t*);
int kernel_timer_settime(kernel_timer_t*, int, const struct itimerspec *,struct itimerspec *);


#endif
