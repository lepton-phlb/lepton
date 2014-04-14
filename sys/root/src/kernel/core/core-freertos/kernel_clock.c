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


/*============================================
| Includes
==============================================*/
#include "kernel/core/kernelconf.h"
#include "kernel/core/kal.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/time.h"
#include "kernel/core/systime.h"

/*============================================
| Global Declaration
==============================================*/


/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        kernel_clock_gettime
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_clock_gettime(clockid_t clk_id, struct timespec *tp){

   switch(clk_id) {
   case CLOCK_MONOTONIC: {
      unsigned long s=0;
      unsigned long ms=0;
      //
      s=  (__kernel_get_timer_ticks()/_SC_CLK_TCK);    //s
      ms= (__kernel_get_timer_ticks()%_SC_CLK_TCK);    //ms
      //
      tp->tv_sec  = s;
      tp->tv_nsec = ms*1000000L;
   }
   break;

   case CLOCK_REALTIME: {
      struct timeval tv={0};
      //get time in second since EPOCH
      _sys_gettimeofday(&tv,0L);
      //
      tp->tv_sec  = tv.tv_sec;
      tp->tv_nsec = tv.tv_usec*1000L;
   }
   break;


   default:
      //only CLOCK_REALTIME and CLOCK_MONOTONIC supported at this time.
      return -1;
   }

   return 0;
}

/*--------------------------------------------
| Name:        kernel_clock_timeout
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
unsigned long kernel_clock_timeout(clockid_t clk_id, const struct timespec *abs_timeout){
   struct timespec cur_time;
   struct timespec to_time;

   unsigned long timeout=0;

   if(kernel_clock_gettime(clk_id,&cur_time)<0)
      return 0;
   //
   to_time.tv_sec  =  abs_timeout->tv_sec -  cur_time.tv_sec;
   if(to_time.tv_sec<0)
      return 0;
   //
   if(abs_timeout->tv_nsec>=cur_time.tv_nsec) {
      to_time.tv_nsec =  abs_timeout->tv_nsec - cur_time.tv_nsec;
   }else{ //overflow
      to_time.tv_nsec = (1000000000L-cur_time.tv_nsec)+abs_timeout->tv_nsec;
      if(to_time.tv_sec)
         to_time.tv_sec--;
      if(to_time.tv_sec<0)
         return 0;
   }
   //
   timeout = __time_s_to_ms(to_time.tv_sec)+__time_ns_to_ms(to_time.tv_nsec);
   //
   return timeout;
}

/*============================================
| End of Source  : kernel_clock.c
==============================================*/
