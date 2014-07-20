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
#include <string.h>
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/syscall.h"
#include "lib/pthread/pthread.h"

/*============================================
| Global Declaration
==============================================*/


/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        kernel_object_insert
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int pthread_condlist_insert(pthread_cond_t* cond, pthread_condlist_t* p){
   if(!p)
      return -1;
   //
   p->next = (pthread_condlist_t*)0;
   p->prev = (pthread_condlist_t*)0;
   //chain list
   p->next=cond->list;
   cond->list=p;
   if(!p->next)
      return 0;

   p->next->prev = p;

   return 0;
}

/*--------------------------------------------
| Name:        kernel_object_remove
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int pthread_condlist_remove(pthread_cond_t* cond, pthread_condlist_t* p){
   if(!p)
      return -1;
   if(p->prev)
      p->prev->next = p->next;
   else
      cond->list = p->next;

   if(p->next)
      p->next->prev = p->prev;

   return 0;
}
/*--------------------------------------------
| Name:        pthread_cond_init
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int pthread_cond_init(pthread_cond_t* cond,  const pthread_condattr_t *attr){
#ifdef __KERNEL_POSIX_REALTIME_SIGNALS
   pthread_cond_init_t pthread_cond_init_dt;
   const pthread_cond_t default_pthread_cond = PTHREAD_COND_INITIALIZER;

   pthread_cond_init_dt.cond = cond;
   pthread_cond_init_dt.ret=-1;

   if(!cond)
      return -1;

   //parano�ac protection ;)
   __atomic_in();
   memcpy(cond,&default_pthread_cond,sizeof(pthread_cond_t));
   if(attr)
      memcpy(&cond->pthread_condattr,attr,sizeof(pthread_condattr_t));
   //parano�ac protection ;)
   __atomic_out();

   //
   __mk_syscall(_SYSCALL_PTHREAD_COND_INIT,pthread_cond_init_dt);
   //
   cond->kernel_sigevent.si_code = SI_SYSTEM;
   cond->kernel_sigevent._sigevent.sigev_signo=SIGNO_SYSTEM_PTHREAD_COND;
   //
   return pthread_cond_init_dt.ret;
#else
   return -1;
#endif
}

/*--------------------------------------------
| Name:        pthread_cond_destroy
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int pthread_cond_destroy(pthread_cond_t* cond){
#ifdef __KERNEL_POSIX_REALTIME_SIGNALS
   pthread_cond_destroy_t pthread_cond_destroy_dt;

   if(!cond)
      return -1;
   if(!cond->kernel_object)
      return -1;

   pthread_cond_destroy_dt.cond = cond;
   pthread_cond_destroy_dt.ret=-1;

   __mk_syscall(_SYSCALL_PTHREAD_COND_DESTROY,pthread_cond_destroy_dt);

   return pthread_cond_destroy_dt.ret;
#else
   return -1;
#endif
}

/*--------------------------------------------
| Name:        pthread_cond_wait
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int pthread_cond_wait(pthread_cond_t* cond,  pthread_mutex_t *mutex){
#ifdef __KERNEL_POSIX_REALTIME_SIGNALS
   pthread_condlist_t pthread_condlist;

   kernel_sigevent_t kernel_sigevent;

   if(!mutex)
      return -1;
   if(!cond)
      return -1;
   if(!cond->kernel_object)
      return -1;

   //1) kernel pthread mutex lock
   kernel_pthread_mutex_lock(&cond->kernel_object->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   //2) add this thread in cond
   pthread_condlist.pthread_ptr = kernel_pthread_self();
   pthread_condlist_insert(cond,&pthread_condlist);
   //3) unlock cond mutex if it's lock
   if(!cond->mutex)
      cond->mutex=mutex;
   pthread_mutex_unlock(cond->mutex);
   //4) kernel pthread mutex unlock
   kernel_pthread_mutex_unlock(&cond->kernel_object->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   //5) init sigevent filter
   kernel_sigevent.si_code=SI_SYSTEM;
   kernel_sigevent._sigevent.sigev_signo=SIGNO_SYSTEM_PTHREAD_COND;
   kernel_sigevent.set=(sigset_t*)0;
   //6) wait on thread sigqueue
   pthread_condlist.pthread_ptr->kernel_sigqueue.wait(&kernel_sigevent);
   //7) kernel pthread mutex lock
   kernel_pthread_mutex_lock(&cond->kernel_object->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   //8) lock cond mutex
   pthread_mutex_lock(cond->mutex);
   //9) kernel pthread mutex unlock
   kernel_pthread_mutex_unlock(&cond->kernel_object->object.kernel_object_pthread_mutex.kernel_pthread_mutex);

   return 0;
#else
   return -1;
#endif
}

/*--------------------------------------------
| Name:        pthread_cond_signal
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int pthread_cond_signal(pthread_cond_t* cond){
#ifdef __KERNEL_POSIX_REALTIME_SIGNALS
   pthread_condlist_t*  condlist;

   if(!cond)
      return -1;
   if(!cond->mutex)
      return -1;
   if(!cond->kernel_object)
      return -1;
   if(!(condlist = cond->list))
      return -1;

   //0) init sigevent
   cond->kernel_sigevent.from=(unsigned long)cond;
   cond->kernel_sigevent.si_code = SI_SYSTEM;
   cond->kernel_sigevent._sigevent.sigev_signo=SIGNO_SYSTEM_PTHREAD_COND;
   //1) kernel pthread mutex lock
   kernel_pthread_mutex_lock(&cond->kernel_object->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   //2) loop walk cond thread until first pthread in the fifo list
   while(condlist->next) condlist = condlist->next;
   //3) signal queue cond thread
   cond->list->pthread_ptr->kernel_sigqueue.send((kernel_pthread_t*)cond->list->pthread_ptr,&cond->kernel_sigevent);
   //4) remove cond thread
   pthread_condlist_remove(cond,cond->list);
   //5) kernel pthread mutex unlock
   kernel_pthread_mutex_unlock(&cond->kernel_object->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   //
   return 0;
#else
   return -1;
#endif
}

/*--------------------------------------------
| Name:        pthread_cond_broadcast
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int pthread_cond_broadcast(pthread_cond_t* cond){
#ifdef __KERNEL_POSIX_REALTIME_SIGNALS
   if(!cond)
      return -1;
   if(!cond->mutex)
      return -1;
   if(!cond->kernel_object)
      return -1;

   //0) init sigevent
   cond->kernel_sigevent.si_code = SI_SYSTEM;
   cond->kernel_sigevent._sigevent.sigev_signo=SIGNO_SYSTEM_PTHREAD_COND;
   //1) kernel pthread mutex lock
   kernel_pthread_mutex_lock(&cond->kernel_object->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   //2) loop walk cond thread{
   while(cond->list) {
      //3)     signal queue cond thread
      cond->list->pthread_ptr->kernel_sigqueue.send((kernel_pthread_t*)cond->list->pthread_ptr,&cond->kernel_sigevent);
      //4)     remove cond thread
      pthread_condlist_remove(cond,cond->list);
   }
   //5) kernel pthread mutex unlock
   kernel_pthread_mutex_unlock(&cond->kernel_object->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   //
   return 0;
#else
   return -1;
#endif
}



/*============================================
| End of Source  : pthread_cond.c
==============================================*/
