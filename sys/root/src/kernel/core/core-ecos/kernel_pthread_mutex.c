/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

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
//#include "kernel/core/errno.h"
//#include "kernel/core/kernel_pthread.h"
//#include "kernel/core/kernel_pthread_mutex.h"

#include "kernel/core/errno.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"
#include "kernel/core/kernel_pthread.h"

/*============================================
| Global Declaration 
==============================================*/


/*============================================
| Implementation 
==============================================*/
/*-------------------------------------------
| Name:pthread_mutex_init
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int   kernel_pthread_mutex_init(kernel_pthread_mutex_t *mutex, const pthread_mutexattr_t *attr){
   #if defined(USE_ECOS)
    	cyg_mutex_init(&mutex->mutex);
	#endif

   return 0;
}

/*-------------------------------------------
| Name:pthread_mutex_destroy
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int   kernel_pthread_mutex_destroy(kernel_pthread_mutex_t *mutex){
   int count;
   //
   __atomic_in();

   #if defined(USE_ECOS)
      //release all threads waiting on the mutex
      cyg_mutex_release(&mutex->mutex);
      //destroy mutex
      cyg_mutex_destroy(&mutex->mutex);
	#endif

   __atomic_out();
   //
   return 0;
}

/*--------------------------------------------
| Name:        kernel_pthread_mutex_owner_destroy
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int   kernel_pthread_mutex_owner_destroy(kernel_pthread_t* thread_ptr,kernel_pthread_mutex_t *mutex){
   int count;
	//
   __atomic_in();

	#if defined(USE_ECOS)
      //no equivalent for eCos for the moment
   	//just destroy it for the moment
	   kernel_pthread_mutex_destroy(mutex);
   #endif

	__atomic_out();
   //

   return 0;
}

/*-------------------------------------------
| Name:pthread_mutex_lock
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int   kernel_pthread_mutex_lock(kernel_pthread_mutex_t *mutex){

   ////
   #if defined(USE_ECOS)
      if(!cyg_mutex_lock(&mutex->mutex)) {
         __kernel_set_errno(-EINTR);
         return -1;
      }
   #endif
   return 0;
}

/*-------------------------------------------
| Name:pthread_mutex_trylock
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int   kernel_pthread_mutex_trylock(kernel_pthread_mutex_t *mutex){

	#if defined(USE_ECOS)
      if(!cyg_mutex_trylock(&mutex->mutex)) {
         __kernel_set_errno(-EBUSY);
      	return -1;
      }
	#endif      

   return 0;
}

/*-------------------------------------------
| Name:pthread_mutex_unlock
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int   kernel_pthread_mutex_unlock(kernel_pthread_mutex_t *mutex){

	#if defined(USE_ECOS)
      	cyg_mutex_unlock(&mutex->mutex);
	#endif

   return 0;
}

/*============================================
| End of Source  : kernel_pthread_mutex.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
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
