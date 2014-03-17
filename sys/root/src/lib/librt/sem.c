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
#include <stdlib.h>
#include <string.h>
#include "kernel/core/kernelconf.h"
#include "kernel/core/syscall.h"
#include "kernel/core/system.h"
#include "kernel/core/process.h"

#include "lib/librt/semaphore.h"

/*============================================
| Global Declaration
==============================================*/


/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        sem_init
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int sem_init(sem_t *sem, int pshared, unsigned int value){
   sem_init_t sem_init_dt;
   //
   if(!sem)
      return -1;
   //
   sem_init_dt.name=(char*)0;
   sem_init_dt.psem=sem;
   sem_init_dt.pshared=pshared;
   sem_init_dt.value=value;
   //
   __mk_syscall(_SYSCALL_SEM_INIT,sem_init_dt);
   //
   if(sem_init_dt.ret<0)
      return -1;

   return 0;
}

/*--------------------------------------------
| Name:        sem_destroy
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int sem_destroy(sem_t *sem){
   sem_destroy_t sem_destroy_dt;
   //
   if(!sem)
      return -1;
   //
   memset(sem,0,sizeof(sem_t));
   //
   sem_destroy_dt.psem=sem;
   //
   __mk_syscall(_SYSCALL_SEM_DESTROY,sem_destroy_dt);
   //
   if(sem_destroy_dt.ret<0)
      return -1;

   return 0;
}

/*--------------------------------------------
| Name:        sem_wait
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int sem_wait(sem_t *sem){
   if(sem->type!=KERNEL_OBJECT_SEM)
      return -1;
   return kernel_sem_wait(&sem->object.kernel_object_sem.kernel_sem);
}

/*--------------------------------------------
| Name:        sem_trywait
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int sem_trywait(sem_t *sem){
   if(sem->type!=KERNEL_OBJECT_SEM)
      return -1;
   return (kernel_sem_trywait(&sem->object.kernel_object_sem.kernel_sem)<0 ? -1 : 0);
}

/*--------------------------------------------
| Name:        sem_timedwait
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int sem_timedwait(sem_t *sem,const struct timespec * abs_timeout){
   if(sem->type!=KERNEL_OBJECT_SEM)
      return -1;
   //
   if(!abs_timeout)
      return (kernel_sem_trywait(&sem->object.kernel_object_sem.kernel_sem)<0 ? -1 : 0);
   //to convert absolute timeout in relative timeout
   return kernel_sem_timedwait(&sem->object.kernel_object_sem.kernel_sem,TIMER_ABSTIME,abs_timeout);
}

/*--------------------------------------------
| Name:        sem_post
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int sem_post(sem_t *sem){
   if(sem->type!=KERNEL_OBJECT_SEM)
      return -1;
   return (kernel_sem_post(&sem->object.kernel_object_sem.kernel_sem)<0 ? -1 : 0);
}

/*--------------------------------------------
| Name:        sem_getvalue
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int sem_getvalue(sem_t *sem, int *sval){
   if(sem->type!=KERNEL_OBJECT_SEM)
      return -1;
   if(!sval)
      return -1;
   return (kernel_sem_getvalue(&sem->object.kernel_object_sem.kernel_sem,sval)<0 ? -1 : 0);
}


/*============================================
| End of Source  : sem.c
==============================================*/
