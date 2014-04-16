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
#include "kernel/core/signal.h"
#include "kernel/core/kernel_object.h"
#include "kernel/core/kernel_sigqueue.h"

#include <string.h>
/*============================================
| Global Declaration
==============================================*/
int kernel_sigqueue_constructor(struct kernel_object_st** kernel_object_head,
                                struct kernel_sigqueue_st* p);
int kernel_sigqueue_destructor(struct kernel_sigqueue_st* p);

int kernel_sigqueue_send(struct kernel_pthread_st* kernel_pthread,
                         struct kernel_sigevent_st* kernel_sigevent);
int kernel_sigqueue_wait(struct kernel_sigevent_st* kernel_sigevent);
int kernel_sigqueue_timedwait(struct kernel_sigevent_st* kernel_sigevent,int flag,
                              const struct timespec * timeout);



/*============================================
| Implementation
==============================================*/

const kernel_sigqueue_t _kernel_sigqueue_initializer=KERNEL_SIGQUEUE_INITIALIZER;
/*--------------------------------------------
| Name:        kernel_sigqueue_constructor
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_sigqueue_constructor(struct kernel_object_st** pp_kernel_object_head,
                                struct kernel_sigqueue_st* p){
   int i=0;
   if(!p)
      return -1;
   memcpy(p,&_kernel_sigqueue_initializer,sizeof(kernel_sigqueue_t));
   p->self=p;
   p->kernel_object_head = pp_kernel_object_head;
   if((p->kernel_sem =
          kernel_object_manager_get(pp_kernel_object_head, KERNEL_OBJECT_SEM,
                                    KERNEL_OBJECT_SRC_POOL,
                                    0))==(kernel_object_t*)0)
      return -1;
   if((p->kernel_mutex =
          kernel_object_manager_get(pp_kernel_object_head, KERNEL_OBJECT_PTRHEAD_MUTEX,
                                    KERNEL_OBJECT_SRC_POOL))==(kernel_object_t*)0)
      return -1;

   for(i=0; i<KERNEL_SIGQUEUE_MAX; i++) {
      p->sigqueue[i].si_code=SI_NONE;
      p->sigqueue[i].from=(unsigned long)0x00000000;
      p->sigqueue[i].counter=0;
   }

   return 0;
}

/*--------------------------------------------
| Name:        kernel_sigqueue_destructor
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_sigqueue_destructor(struct kernel_sigqueue_st* p){
   if(!p)
      return -1;
   kernel_object_manager_put(p->kernel_object_head,p->kernel_sem);
   kernel_object_manager_put(p->kernel_object_head,p->kernel_mutex);
   return 0;
}


/*--------------------------------------------
| Name:        kernel_sigqueue_insert
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_sigqueue_insert(struct kernel_sigqueue_st* p,kernel_sigevent_t* kernel_sigevent){
   int i=0;
   //
   if(!p)
      return -1;
   if(!kernel_sigevent)
      return -1;
   //
   kernel_pthread_mutex_lock(
      &p->kernel_mutex->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   //
   for(i=0; i<KERNEL_SIGQUEUE_MAX; i++) {
      if(p->sigqueue[i].si_code==SI_NONE) { //new sigevent
         memcpy(&p->sigqueue[i],kernel_sigevent,sizeof(kernel_sigevent_t));
         //to do: check overrun
         p->sigqueue[i].counter++;
         //
         kernel_pthread_mutex_unlock(
            &p->kernel_mutex->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
         return 0;
      }else if(p->sigqueue[i].si_code==kernel_sigevent->si_code
               && p->sigqueue[i].from == kernel_sigevent->from
               && p->sigqueue[i]._sigevent.sigev_signo == kernel_sigevent->_sigevent.sigev_signo) { //it's the same sigevent
         //to do: check overrun
         p->sigqueue[i].counter++;
         //
         kernel_pthread_mutex_unlock(
            &p->kernel_mutex->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
         return 0;
      }
   }
   //
   kernel_pthread_mutex_unlock(
      &p->kernel_mutex->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   //
   return -1; //errno = [EAGAIN]
}

/*--------------------------------------------
| Name:        kernel_sigqueue_extract
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_sigqueue_extract(struct kernel_sigqueue_st* p, kernel_sigevent_t* kernel_sigevent){
   int i;
   int sigev_signo=SIGRTMAX;
   int sigev_pos=-1;
   //
   if(!p)
      return -1;
   if(!kernel_sigevent)
      return -1;
   //
   kernel_pthread_mutex_lock(
      &p->kernel_mutex->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   //
   for(i=0; i<KERNEL_SIGQUEUE_MAX; i++) {
      if((p->sigqueue[i].si_code!=SI_NONE) && (p->sigqueue[i].si_code!=SI_SYSTEM) &&
         (p->sigqueue[i]._sigevent.sigev_signo<sigev_signo)) {
         int signo= p->sigqueue[i]._sigevent.sigev_signo;
         //filter
         if( kernel_sigevent->set
             && !((kernel_sigevent->set->std)&(0x01<<(signo-1)))
             && !((kernel_sigevent->set->rt)&(0x01<<(signo-SIGRTMIN))))
            continue;
         sigev_signo = signo;
         sigev_pos = i;
      }
   }
   //
   if(sigev_pos<0) {
      kernel_pthread_mutex_unlock(
         &p->kernel_mutex->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
      return -1;
   }
   //
   memcpy(kernel_sigevent,&p->sigqueue[sigev_pos],sizeof(kernel_sigevent_t));
   //free this entry
   if( !(--(p->sigqueue[sigev_pos].counter)) )
      p->sigqueue[sigev_pos].si_code = SI_NONE;
   //
   kernel_pthread_mutex_unlock(
      &p->kernel_mutex->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   return 0;
}

/*--------------------------------------------
| Name:        kernel_sigqueue_sysextract
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_sigqueue_sysextract(struct kernel_sigqueue_st* p,kernel_sigevent_t* kernel_sigevent){
   int i;
   int sigev_signo=0;
   int sigev_pos=-1;
   //
   if(!p)
      return -1;
   if(!kernel_sigevent)
      return -1;
   //filter on sigev_signo
   sigev_signo = kernel_sigevent->_sigevent.sigev_signo;
   //
   kernel_pthread_mutex_lock(
      &p->kernel_mutex->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   //
   for(i=0; i<KERNEL_SIGQUEUE_MAX; i++) {
      if(p->sigqueue[i].si_code==SI_SYSTEM && p->sigqueue[i]._sigevent.sigev_signo==sigev_signo) {
         sigev_signo = p->sigqueue[i]._sigevent.sigev_signo;
         sigev_pos = i;
      }
   }
   //
   if(sigev_pos<0) {
      kernel_pthread_mutex_unlock(
         &p->kernel_mutex->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
      return -1;
   }
   //
   memcpy(kernel_sigevent,&p->sigqueue[sigev_pos],sizeof(kernel_sigevent_t));
   //free this entry
   if( !(--(p->sigqueue[sigev_pos].counter)) )
      p->sigqueue[sigev_pos].si_code = SI_NONE;
   //
   kernel_pthread_mutex_unlock(
      &p->kernel_mutex->object.kernel_object_pthread_mutex.kernel_pthread_mutex);
   return 0;
}

/*--------------------------------------------
| Name:        kernel_sigqueue_send
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_sigqueue_send(struct kernel_pthread_st* kernel_pthread,
                         struct kernel_sigevent_st* kernel_sigevent){
   kernel_sigqueue_t* p;
   if(!kernel_pthread)
      return -1;
   if(!(p=&kernel_pthread->kernel_sigqueue))
      return -1;
   if(kernel_sigqueue_insert(p,kernel_sigevent)<0)
      return -1;
   if(kernel_sem_post(&p->kernel_sem->object.kernel_object_sem.kernel_sem)<0)
      return -1;
   return 0;
}

/*--------------------------------------------
| Name:        kernel_sigqueue_wait
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_sigqueue_wait(struct kernel_sigevent_st* kernel_sigevent){
   struct kernel_sigqueue_st* p= &kernel_pthread_self()->kernel_sigqueue;
   if(!p)
      return -1;
   //wait event
   if(kernel_sem_wait(&p->kernel_sem->object.kernel_object_sem.kernel_sem)<0)
      return -1;

   //extract sigevent in queue
   if(kernel_sigevent->si_code==SI_SYSTEM) { //filter event on SI_SYSTEM
      if(kernel_sigqueue_sysextract(p,kernel_sigevent)<0)
         return -1;
   }else{
      if(kernel_sigqueue_extract(p,kernel_sigevent)<0)
         return -1;
   }

   return 0;
}

/*--------------------------------------------
| Name:        kernel_sigqueue_timedwait
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_sigqueue_timedwait(struct kernel_sigevent_st* kernel_sigevent,int flag,
                              const struct timespec * abs_timeout){
   struct kernel_sigqueue_st* p= &kernel_pthread_self()->kernel_sigqueue;
   if(!p)
      return -1;

   //wait event until timeout or event signaled
   if(kernel_sem_timedwait(&p->kernel_sem->object.kernel_object_sem.kernel_sem,flag,abs_timeout)<0)
      return -1;

   //extract sigevent in queue
   if(kernel_sigevent->si_code==SI_SYSTEM) { //filter event on SI_SYSTEM
      if(kernel_sigqueue_sysextract(p,kernel_sigevent)<0)
         return -1;
   }else{
      if(kernel_sigqueue_extract(p,kernel_sigevent)<0)
         return -1;
   }

   return 0;
}

/*============================================
| End of Source  : kernel_sigqueue.c
==============================================*/
