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
#include "kernel/core/syscall.h"
#include "lib/pthread/pthread.h"

/*============================================
| Global Declaration 
==============================================*/


/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        pthread_create
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine)(void*), void *arg){
   pthread_create_t pthread_create_dt;

   pthread_create_dt.arg             = arg;
   pthread_create_dt.attr            = (pthread_attr_t *)attr;
   pthread_create_dt.start_routine   = start_routine;

   __mk_syscall(_SYSCALL_PTHREAD_CREATE,pthread_create_dt);

   if(pthread_create_dt.ret<0)
      return -1;
   //
   if( !(*thread= (pthread_t)pthread_create_dt.kernel_pthread) )
      return -1;

   return 0;
}


/*--------------------------------------------
| Name:        pthread_cancel
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int pthread_cancel(pthread_t thread){
   pthread_cancel_t pthread_cancel_dt;
   pthread_cancel_dt.kernel_pthread = (kernel_pthread_t*)thread;
   //to do check if it's the main thread call exit
   __mk_syscall(_SYSCALL_PTHREAD_CANCEL,pthread_cancel_dt);
   return pthread_cancel_dt.ret;
}

/*--------------------------------------------
| Name:        pthread_exit
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void pthread_exit(void *value_ptr){
   pthread_exit_t pthread_exit_dt;
   pthread_exit_dt.kernel_pthread = kernel_pthread_self();
   pthread_exit_dt.value_ptr = value_ptr;
   //to do check if it's the main thread call exit
   __mk_syscall(_SYSCALL_PTHREAD_EXIT,pthread_exit_dt);
   return;
}

/*--------------------------------------------
| Name:        pthread_self
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
pthread_t pthread_self(void){
   return(pthread_t)kernel_pthread_self();
}

/*============================================
| End of Source  : pthread.c
==============================================*/
