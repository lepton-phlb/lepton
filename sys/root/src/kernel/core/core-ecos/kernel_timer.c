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
#include <string.h>

#include "kernel/core/errno.h"
#include "kernel/core/syscall.h"
#include "kernel/core/timer.h"
#include "kernel/core/kernel_timer.h"
#include "kernel/core/interrupt.h"

/*============================================
| Global Declaration
==============================================*/
//0:timer period, 1: interval timer period
#define KERNEL_TIMER_NOT_CREATED       0
#define KERNEL_TIMER_CREATED           1
#define KERNEL_TIMER_VALUE_PERIOD      0
#define KERNEL_TIMER_INTERVAL_PERIOD   1

#define __timer_ns_to_ms(__ns__) ((__ns__)/1000000L)
#define __timer_ms_to_ns(__ms__) ((__ms__)*1000000L)

#define __timer_s_to_ms(__ns__) ((__ns__)*1000L)
#define __timer_ms_to_s(__ms__) ((__ms__)/1000L)

static const struct sigevent default_timer_sigevent={
   0, //SIGEV_NONE
   SIGALRM,
   0,
   (pfn_sigev_notify_function_t)0,
   (struct pthread_attr_st*)0
};

#if defined(__KERNEL_UCORE_ECOS)
void kernel_timer_generic_callback(alrm_hdl_t alarm_handle, cyg_addrword_t data);
kernel_timer_t * kernel_timer_find_timer(pid_t pid, cyg_handle_t hdl);
#endif

/*--------------------------------------------
| Name:        kernel_timer_find_timer
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
#if defined(__KERNEL_UCORE_ECOS)
kernel_timer_t * kernel_timer_find_timer(pid_t pid, cyg_handle_t hdl)
{
   kernel_object_t *k_object_ptr;
   __atomic_in();
   k_object_ptr = process_lst[pid]->kernel_object_head;
   while(k_object_ptr) {
      if(k_object_ptr->type == KERNEL_OBJECT_TIMER) {
         kernel_object_timer_t *k_obj_timer_ptr = (kernel_object_timer_t *)&(k_object_ptr->object);
         if(k_obj_timer_ptr->kernel_timer.timer.alarm_hdl == hdl) {
            __atomic_out();
            return (kernel_timer_t *)&(k_obj_timer_ptr->kernel_timer);
         }
      }
      k_object_ptr = k_object_ptr->next;
   }
   __atomic_out();
   return NULL;
}

/*--------------------------------------------
| Name:        kernel_timer_generic_callback
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void kernel_timer_generic_callback(alrm_hdl_t alarm_handle, cyg_addrword_t data)
{
   kernel_timer_t* p_kernel_timer   = (kernel_timer_t*)0;
   kernel_pthread_t* pthread_ptr    = (kernel_pthread_t*)0;
   pid_t pid;

   //pthread_ptr = kernel_pthread_self();
   pthread_ptr = (kernel_pthread_t *) data;
   if(!pthread_ptr) return;
   p_kernel_timer = kernel_timer_find_timer(pthread_ptr->pid, alarm_handle);
   if(!p_kernel_timer) return;

   //cyg_alarm_disable(p_kernel_timer->timer.alarm_hdl);
   p_kernel_timer->interval=!p_kernel_timer->interval;
   if(p_kernel_timer->interval && p_kernel_timer->itimerspec.it_interval.tv_nsec) {
   }
   else if(!p_kernel_timer->interval && p_kernel_timer->itimerspec.it_interval.tv_nsec) {
      return;
   }

   if(p_kernel_timer->kernel_pthread==(kernel_pthread_t*)0) {
      if(p_kernel_timer->sigevent.sigev_notify_function)
         p_kernel_timer->sigevent.sigev_notify_function(p_kernel_timer->sigevent.sigev_signo);
      cyg_alarm_enable(p_kernel_timer->timer.alarm_hdl);
      return;
   }

   //all operation critical operation protected by atomic region,
   //software timer not call in the atomic (see segger documentation)
   //call kill on pid of process container of pthread
   pid = p_kernel_timer->kernel_pthread->pid;
   if(pid<=0)
      return;
   pthread_ptr=process_lst[pid]->pthread_ptr;
   //walking on threads chained list in current process
   while(pthread_ptr) {
      //send signal to all pthread of processus
      _sys_kill(pthread_ptr,p_kernel_timer->sigevent.sigev_signo,0);
      pthread_ptr=pthread_ptr->next;
   }
   //cyg_alarm_enable(p_kernel_timer->timer.alarm_hdl);
}
#endif

/*--------------------------------------------
| Name:        kernel_timer_create
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_timer_create(clockid_t clockid, struct sigevent * p_sigevent,
                        kernel_timer_t* p_kernel_timer)
{
   if(!p_kernel_timer)
      return -1;
   switch(clockid) {
   case CLOCK_REALTIME:
      break;
   default:
      return -1;
   }
   //set sigevent
   if(!p_sigevent) {
      memcpy((void *)&p_kernel_timer->sigevent,(void *)&default_timer_sigevent,
             sizeof(struct sigevent));
   }else{
      memcpy((void *)&p_kernel_timer->sigevent,(void *)p_sigevent,sizeof(struct sigevent));
   }
   //
   p_kernel_timer->created=KERNEL_TIMER_NOT_CREATED;
   p_kernel_timer->clockid=clockid;
   p_kernel_timer->interval=KERNEL_TIMER_VALUE_PERIOD;
   p_kernel_timer->itimerspec.it_value.tv_nsec=0;
   p_kernel_timer->itimerspec.it_value.tv_sec=0;

   p_kernel_timer->itimerspec.it_interval.tv_nsec=0;
   p_kernel_timer->itimerspec.it_interval.tv_sec=0;

   p_kernel_timer->kernel_pthread=(kernel_pthread_t*)0;

   return 0;
}


/*--------------------------------------------
| Name:        kernel_timer_delete
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_timer_delete(kernel_timer_t* p_kernel_timer)
{
   if(!p_kernel_timer)
      return -1;
   if(!p_kernel_timer->created)
      return -1;

   //disable and delete
#if defined(__KERNEL_UCORE_ECOS)
   cyg_alarm_disable(p_kernel_timer->timer.alarm_hdl);
   cyg_alarm_delete(p_kernel_timer->timer.alarm_hdl);
#endif
   //
   p_kernel_timer->created=KERNEL_TIMER_NOT_CREATED;
   return 0;
}

/*--------------------------------------------
| Name:        kernel_timer_gettime
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_timer_gettime(kernel_timer_t* p_kernel_timer, struct itimerspec* value)
{
   int elsapse_time_ms;

   value->it_value.tv_sec=0;
   value->it_value.tv_nsec=0;
   if(!p_kernel_timer)
      return -1;
   if(!p_kernel_timer->created)
      return -1;

#if defined (__KERNEL_UCORE_ECOS)
   elsapse_time_ms = cyg_counter_current_value(p_kernel_timer->timer.cnt_obj);
#endif

   value->it_value.tv_sec= elsapse_time_ms/1000;
   value->it_value.tv_nsec=__timer_ms_to_ns( (elsapse_time_ms%1000)  );
   return 0;
}

/*--------------------------------------------
| Name:        kernel_timer_getoverrun
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_timer_getoverrun(kernel_timer_t* p_kernel_timer)
{
   return 0;
}

/*--------------------------------------------
| Name:        kernel_timer_settime
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_timer_settime(kernel_timer_t* p_kernel_timer, int flags, const struct itimerspec* value,
                         struct itimerspec* ovalue){

   switch(flags) {
   case 0:   //relative time
      break;
   //
   case TIMER_ABSTIME:
   default:
      return -1;
   }

   //
   if(ovalue) {
      int elsapse_time_ms = 1000;           //dummy value
      //
#if defined (__KERNEL_UCORE_ECOS)
      elsapse_time_ms = cyg_counter_current_value(p_kernel_timer->timer.cnt_obj);
#endif
      //
      ovalue->it_value.tv_sec= elsapse_time_ms/1000;
      ovalue->it_value.tv_nsec=__timer_ms_to_ns( (elsapse_time_ms%1000)  );
   }

   //
   p_kernel_timer->interval=KERNEL_TIMER_VALUE_PERIOD;
   //
   p_kernel_timer->itimerspec.it_value.tv_sec=value->it_value.tv_sec;
   p_kernel_timer->itimerspec.it_value.tv_nsec=value->it_value.tv_nsec;
   p_kernel_timer->itimerspec.it_interval.tv_sec=value->it_interval.tv_sec;
   p_kernel_timer->itimerspec.it_interval.tv_nsec=value->it_interval.tv_nsec;

   //
   if(!p_kernel_timer->created) {
#if defined (__KERNEL_UCORE_ECOS)
      cyg_clock_to_counter(cyg_real_time_clock(), &p_kernel_timer->timer.cnt_obj);

      p_kernel_timer->created=KERNEL_TIMER_CREATED;
      cyg_alarm_create( p_kernel_timer->timer.cnt_obj,
                        &kernel_timer_generic_callback,
                        (cyg_addrword_t) kernel_pthread_self(),
                        &p_kernel_timer->timer.alarm_hdl,
                        &p_kernel_timer->timer.alarm_obj);
#endif
   }else{
#if defined (__KERNEL_UCORE_ECOS)
      cyg_alarm_disable(p_kernel_timer->timer.alarm_hdl);
#endif
   }
   //
   if(p_kernel_timer->itimerspec.it_value.tv_nsec!=0 ||
      p_kernel_timer->itimerspec.it_value.tv_sec!=0) {
#if defined (__KERNEL_UCORE_ECOS)
      cyg_alarm_initialize(p_kernel_timer->timer.alarm_hdl,
                           __get_timer_ticks() +
                           msec_to_tick((__timer_s_to_ms(value->it_value.tv_sec)+
                                         __timer_ns_to_ms(value->it_value.tv_nsec))),
                           msec_to_tick((__timer_s_to_ms(value->it_interval.tv_sec)+
                                         __timer_ns_to_ms(value->it_interval.tv_nsec))));
#endif
   }

   return 0;
}



/*============================================
| End of Source  : kernel_timer.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.5  2010/02/12 14:32:13  jjp
| add __KERNEL_UCORE_ECOS macro to compile for libkernel.so
|
| Revision 1.4  2010/01/07 14:12:42  jjp
| bug fix on kernel_timer_generic_callback and clean source
|
| Revision 1.1  2009/03/30 15:48:57  jjp
| first import of tauon
|
| Revision 1.1  2009/03/30 11:18:54  jjp
| First import of tauon
|
| Revision 1.1.1.1  2009/02/05 15:29:49  jjp
| First import in CVS
|
|---------------------------------------------
==============================================*/
