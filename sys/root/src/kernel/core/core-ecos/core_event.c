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
| Includes
==============================================*/
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/core_event.h"
#include "kernel/core/kal.h"
#include "kernel/core/interrupt.h"
/*============================================
| Global Declaration
==============================================*/


/*============================================
| Implementation
==============================================*/
//
int core_event_set_flags(kernel_pthread_t * pthread_ptr, cyg_flag_value_t val) {
   //
   if(!pthread_ptr) {
      return -1;
   }
   //
   cyg_flag_setbits(&pthread_ptr->io_flag, val);
   //
   return 0;
}

//
int core_event_wait(cyg_flag_value_t val) {
   kernel_pthread_t * pthread_ptr = kernel_pthread_self();
   //
   if(!pthread_ptr) {
      return -1;
   }
   //
   cyg_flag_wait(&pthread_ptr->io_flag, val, CYG_FLAG_WAITMODE_CLR|CYG_FLAG_WAITMODE_AND);
   return 0;
}

//
int core_event_timed_wait(cyg_flag_value_t val, cyg_tick_count_t timeout) {
   kernel_pthread_t * pthread_ptr = kernel_pthread_self();
   //
   if(!pthread_ptr) {
      return -1;
   }
   //
   if(!cyg_flag_timed_wait(&pthread_ptr->io_flag, val, CYG_FLAG_WAITMODE_CLR|CYG_FLAG_WAITMODE_AND,
                           __get_timer_ticks()+ msec_to_tick(timeout))) {
      return -1;
   }

   return 0;
}

/*============================================
| End of Source  : core_event.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
==============================================*/
