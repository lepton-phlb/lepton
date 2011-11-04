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
#include <string.h>
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
| Name:        pthread_mutex_init
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr){
   pthread_mutex_init_t pthread_mutex_init_dt;
   const pthread_mutex_t default_pthread_mutex = PTHREAD_MUTEX_INITIALIZER;

   pthread_mutex_init_dt.mutex = mutex;
   pthread_mutex_init_dt.ret=-1;

   if(!mutex)
      return -1;
   
   //paranoïac protection ;)
   __atomic_in();
   memcpy(mutex,&default_pthread_mutex,sizeof(pthread_mutex_t));
   if(attr)
      memcpy(&mutex->pthread_mutexattr,attr,sizeof(pthread_mutexattr_t));
   //paranoïac protection ;)
   __atomic_out();

   //
   __mk_syscall(_SYSCALL_PTHREAD_MUTEX_INIT,pthread_mutex_init_dt);

   return pthread_mutex_init_dt.ret;
}

/*--------------------------------------------
| Name:        pthread_mutex_destroy
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int pthread_mutex_destroy(pthread_mutex_t *mutex){
   pthread_mutex_destroy_t pthread_mutex_destroy_dt;

   if(!mutex) 
      return -1;
   if(!mutex->kernel_object)
      return -1;

   pthread_mutex_destroy_dt.mutex = mutex;
   pthread_mutex_destroy_dt.ret=-1;

   __mk_syscall(_SYSCALL_PTHREAD_MUTEX_DESTROY,pthread_mutex_destroy_dt);

   return pthread_mutex_destroy_dt.ret;
}

/*--------------------------------------------
| Name:        pthread_mutex_lock
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int pthread_mutex_lock(pthread_mutex_t *mutex){
   if(!mutex)
      return -1;
   //
   if(mutex->kernel_object==PTHREAD_MUTEX_UNINITIALIZED){
      //static mutex init
      if(pthread_mutex_init(mutex,&mutex->pthread_mutexattr)<0)
         return -1;
   }
   //
   if(mutex->kernel_object!=PTHREAD_MUTEX_UNINITIALIZED)
      return kernel_pthread_mutex_lock(&mutex->kernel_object->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   return -1;
}

/*--------------------------------------------
| Name:        pthread_mutex_trylock
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int pthread_mutex_trylock(pthread_mutex_t *mutex){
   if(!mutex)
      return -1;
   //
   if(mutex->kernel_object==PTHREAD_MUTEX_UNINITIALIZED){
      //static mutex init
      if(pthread_mutex_init(mutex,&mutex->pthread_mutexattr)<0)
         return -1;
   }
   //
   if(mutex->kernel_object!=PTHREAD_MUTEX_UNINITIALIZED)
      return kernel_pthread_mutex_trylock(&mutex->kernel_object->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   return -1;
}

/*--------------------------------------------
| Name:        pthread_mutex_unlock
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int pthread_mutex_unlock(pthread_mutex_t *mutex){
  if(mutex && mutex->kernel_object!=PTHREAD_MUTEX_UNINITIALIZED)
      return kernel_pthread_mutex_unlock(&mutex->kernel_object->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   return -1;
}


/*============================================
| End of Source  : pthread_mutex.c
==============================================*/
