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
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"


#include "kernel/core/kernel_timer.h"

/*============================================
| Global Declaration 
==============================================*/


/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        timer_create
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int timer_create(clockid_t clockid, struct sigevent * sigevent,timer_t * timerid){
   timer_create_t timer_create_dt;
   timer_create_dt.clockid = clockid;
   timer_create_dt.psigevent=sigevent;
   timer_create_dt.ptimerid=timerid;
   __mk_syscall(_SYSCALL_TIMER_CREATE,timer_create_dt);
   return timer_create_dt.ret;
}

/*--------------------------------------------
| Name:        timer_delete
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int timer_delete(timer_t* timerid){
   timer_delete_t timer_delete_dt;
   timer_delete_dt.ptimerid=timerid;
   __mk_syscall(_SYSCALL_TIMER_DELETE,timer_delete_dt);
   return timer_delete_dt.ret;
}

/*--------------------------------------------
| Name:        timer_gettime
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int timer_gettime(timer_t* timerid, struct itimerspec * value){
   return -1;
}

/*--------------------------------------------
| Name:        timer_getoverrun
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int timer_getoverrun(timer_t* timerid){
   return -1;
}

/*--------------------------------------------
| Name:        timer_settime
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int timer_settime(timer_t* timerid, int flags, const struct itimerspec * value,struct itimerspec *ovalue){
   kernel_object_t* kernel_object=(kernel_object_t*)(*timerid);
   if(!kernel_object)
      return -1;
   if(kernel_object->type!=KERNEL_OBJECT_TIMER)
      return -1;
   //
   kernel_object->object.kernel_object_timer.kernel_timer.kernel_pthread=kernel_pthread_self();
   return kernel_timer_settime(&kernel_object->object.kernel_object_timer.kernel_timer,flags,value,ovalue);
}



/*============================================
| End of Source  : timer.c
==============================================*/
