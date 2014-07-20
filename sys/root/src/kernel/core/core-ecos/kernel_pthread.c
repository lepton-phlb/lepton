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

/*===========================================
Includes
=============================================*/
#include "kernel/core/errno.h"
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/kernel_pthread_mutex.h"
#include "kernel/core/syscall.h"
#include "kernel/core/interrupt.h"

#include "kernel/fs/vfs/vfs.h"

#include <string.h>

#if defined(GNU_GCC)
   #include <stdlib.h>
#endif
/*===========================================
Global Declaration
=============================================*/
#if defined(__KERNEL_UCORE_ECOS)
   #include <cyg/hal/hal_io.h>
#endif

#define __KERNEL_PTHREAD_ID_LIMIT 1024

kernel_pthread_t* g_pthread_lst=(kernel_pthread_t*)0;
int g_pthread_id=0;


/*===========================================
Implementation
=============================================*/
/*--------------------------------------------
| Name:        new_thread
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_init_pthread(kernel_pthread_t* p){
   kernel_pthread_t* pthread_ptr;
   int counter=1;
   //
   p->next  = (struct kernel_pthread_st*)0;
   p->gprev = (struct kernel_pthread_st*)0;
   p->gnext = (struct kernel_pthread_st*)0;
   //io desc
   p->io_desc =(desc_t)-1;

   //init counter
   g_pthread_id=((g_pthread_id<__KERNEL_PTHREAD_ID_LIMIT) ? g_pthread_id+1 : 0);
   pthread_ptr=g_pthread_lst;
   //
   while(pthread_ptr) {
      if(pthread_ptr->id==g_pthread_id) {
         g_pthread_id=((g_pthread_id<__KERNEL_PTHREAD_ID_LIMIT) ? g_pthread_id+1 : 0);
         pthread_ptr=g_pthread_lst;
         if((++counter)>__KERNEL_PTHREAD_ID_LIMIT)
            return -1;  //error no id available
         continue;
      }
      pthread_ptr=pthread_ptr->gnext;
   }

   p->id = g_pthread_id;

   return 0;
}

/*--------------------------------------------
| Name:        kernel_insert_gpthread
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_insert_gpthread(kernel_pthread_t* p){
   if(!p)
      return -1;
   //general pthread list
   p->gnext=g_pthread_lst;
   g_pthread_lst=p;
   if(!p->gnext)
      return 0;

   p->gnext->gprev = p;

   return 0;
}

/*--------------------------------------------
| Name:        kernel_remove_gpthread
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_remove_gpthread(kernel_pthread_t* p){
   if(!p)
      return -1;
   if(p->gprev)
      p->gprev->gnext = p->gnext;
   else
      g_pthread_lst = p->gnext;

   if(p->gnext)
      p->gnext->gprev = p->gprev;

   return 0;
}

/*-------------------------------------------
| Name:get_pthread_id
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int kernel_get_pthread_id(kernel_pthread_t *p){

   pthread_id_t _id=0;

   //to do: use mutex protection?
   kernel_init_pthread(p);
   kernel_insert_gpthread(p);

   return p->id;
}

/*-------------------------------------------
| Name:put_pthread_id
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int kernel_put_pthread_id(kernel_pthread_t *p){
   kernel_remove_gpthread(p);
   return 0;
}

/*--------------------------------------------
| Name:        kernel_pthread_alloca
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void* kernel_pthread_alloca(kernel_pthread_t *p, size_t size){

   //it's the bottom stack address
   void *p_heap=p->heap_top;
   char *p_heap_1byte=p->heap_top;

   //to do:must be check with current stack addr
   p_heap_1byte+=(unsigned long)size;
   {
      uint32_t _stack_addr = (uint32_t)p_heap_1byte;
      uchar8_t _align = (uchar8_t)(4-(_stack_addr%4));
      p_heap_1byte+=(_align*sizeof(uchar8_t));
      size+=(_align*sizeof(uchar8_t));
   }
   p->heap_top = p_heap_1byte;
   //clear heap mem allocated
   memset(p_heap,0,size);

   return p_heap;
}

/*-------------------------------------------
| Name:pthread_routine
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
__begin_pthread(pthread_routine){

   kernel_pthread_t* pthread;

   pthread=kernel_pthread_self();
   pthread->exit=pthread->start_routine(pthread->arg);

   //to do: call kernel. signal thread termination

   //if
   if(pthread->pid<1) {
      kernel_pthread_cancel(pthread); //native kernel pthread: no process container
   }else{
      //pthread in process container
      //use syscall
      pthread_exit_t pthread_exit_dt;
      pthread_exit_dt.kernel_pthread = pthread;
      pthread_exit_dt.value_ptr = (void*)0;
      //to do check if it's the main thread call exit
      //BEWARE : need syscall.h
      __mk_syscall(_SYSCALL_PTHREAD_EXIT,pthread_exit_dt);
   }

}
__end_pthread()

/*-------------------------------------------
| Name:pthread_create
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int   kernel_pthread_create(kernel_pthread_t *thread, const pthread_attr_t *attr,
                            start_routine_t start_routine,
                            void *arg){

   if(!thread)
      return -EINVAL;

   thread->attr.stacksize  = attr->stacksize;
   thread->attr.stackaddr  = attr->stackaddr;
   thread->attr.priority   = attr->priority;
   thread->attr.timeslice  = attr->timeslice;
   thread->attr.name       = attr->name;

   thread->arg             = arg;
   thread->start_routine   = start_routine;

   //heap see kernel_pthread_alloca().
   thread->heap_floor = thread->attr.stackaddr;
   thread->heap_top   = thread->heap_floor;

   thread->pid=(pid_t)-1;

#ifdef __KERNEL_UCORE_ECOS
   thread->thr_id = 0;
   if( !(thread->tcb=malloc(sizeof(tcb_t))) )
      return -EAGAIN;

   kernel_sem_init(&thread->sem_wait, 0, 0);
#endif

   if( kernel_get_pthread_id(thread)==-EAGAIN)
      return -EAGAIN;

   //
   thread->kernel_stack = malloc(KERNEL_STACK*sizeof(char));
   if(!thread->kernel_stack)
      return -EAGAIN;

   //création d'une tache eCos
#ifdef __KERNEL_UCORE_ECOS
   {
      char * thr_name = (char *)thread->attr.name;
      pid_t pid = thread->pid;
      if(thr_name)
         thr_name = thread->attr.name;
      else
         thr_name = "deamon_ecos_kernel_thread";

      //appel à la routine eCos pour la création de la tache
      cyg_thread_create(thread->attr.priority, (void *)pthread_routine,
                        (cyg_addrword_t) thread->arg, thr_name,
                        thread->attr.stackaddr, thread->attr.stacksize,
                        &thread->thr_id,
                        thread->tcb);
      //thread->id = (pthread_id_t) thread->thr_id;
      //init IO flag
   #if defined(__KERNEL_IO_EVENT)
      cyg_flag_init(&thread->io_flag);
   #elif defined(__KERNEL_IO_SEM)
      kernel_sem_init(&thread->io_sem,0,0);
   #endif

      cyg_thread_resume(thread->thr_id);
   }

#endif

   return 0;
}

/*-------------------------------------------
| Name:pthread_kill
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int   kernel_pthread_kill(kernel_pthread_t* thread, int sig){

   kernel_pthread_t* _thread=thread;
   int _sig=sig;

   //to do:

   return 0;
}

/*-------------------------------------------
| Name:pthread_cancel
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int   kernel_pthread_cancel(kernel_pthread_t* thread){
#ifdef __KERNEL_UCORE_ECOS
   thr_id_t current_id;
#endif
   desc_t desc;

   //   __atomic_in();
   //__disable_interrupt_section_in();

   desc=0;

#ifdef __KERNEL_UCORE_ECOS
   current_id = thread->thr_id;
   if(thread->thr_id==0)
      return 0;
#endif
   //begin of section: protection from io interrupt
   __disable_interrupt_section_in();
   __atomic_in();
   if(kernel_put_pthread_id(thread)==-ESRCH) {
      __disable_interrupt_section_out();
      __syscall_unlock();
      __atomic_out();
      return -ESRCH;
   }

   //unreference this thread in pthread owner from desc.
   for(desc=0; desc<MAX_OPEN_FILE; desc++) {
      if(!ofile_lst[desc].used
         ||
         ( ofile_lst[desc].owner_pthread_ptr_read!=thread
           && ofile_lst[desc].owner_pthread_ptr_write!=thread ) )
         continue;
      //
      if(ofile_lst[desc].owner_pthread_ptr_read==thread)
         ofile_lst[desc].owner_pthread_ptr_read    =(kernel_pthread_t*)0;
      if(ofile_lst[desc].owner_pthread_ptr_write==thread)
         ofile_lst[desc].owner_pthread_ptr_write   =(kernel_pthread_t*)0;
   }
   //

   __disable_interrupt_section_out();
   //
   if(thread->stat & PTHREAD_STATUS_KERNEL) {
      __syscall_unlock();
      __atomic_out();
   }

#ifdef __KERNEL_UCORE_ECOS
   kernel_sem_destroy(&thread->sem_wait);

   #if defined(__KERNEL_IO_EVENT)
   cyg_flag_destroy(&thread->io_flag);
   #elif defined(__KERNEL_IO_SEM)
   kernel_sem_destroy(&thread->io_sem);
   #endif

   //release all objects the thread is waiting on
   cyg_thread_release(thread->thr_id);
#endif

   //NOT ADVISE BY ECOS DEVELOPER
   //cyg_thread_exit();
   free(thread->kernel_stack);
   free(thread->attr.stackaddr);
   free(thread->tcb);
   free(thread);

#ifdef __KERNEL_UCORE_ECOS
   cyg_thread_delete(current_id);
#endif
   //__disable_interrupt_section_out();

   return 0;
}


/*-------------------------------------------
| Name:pthread_self
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
kernel_pthread_t* kernel_pthread_self(void){
   kernel_pthread_t* p;
   __atomic_in();
   p=g_pthread_lst;
   while(p) {
      if( __is_thread_self(p) ) {
         __atomic_out();
         return p;
      }
      p=p->gnext;
   }

   __atomic_out();
   return (kernel_pthread_t*)0;
}


/*===========================================
| End of Source : kernel_pthread.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.6  2010/02/12 14:37:48  jjp
| add new phlb manner for couting g_pthread_id
|
| Revision 1.5  2010/02/08 09:26:42  phlb
| support new kernel object manager interface and new read/write io semaphore.
|
| Revision 1.4  2009/11/13 12:06:29  jjp
| improve thread terminaison when in KERNEL mode
| all threads entry point is pthread_routine
|
| Revision 1.3  2009/06/12 13:04:28  jjp
| bug fix for synthetic target need disable_irq in kernel_pthread_cancel
|
| Revision 1.2  2009/06/09 10:16:52  jjp
| add io_flag for hardware event
|
| Revision 1.1  2009/03/30 15:48:57  jjp
| first import of tauon
|
| Revision 1.1  2009/03/30 11:18:54  jjp
| First import of tauon
|
|---------------------------------------------
=============================================*/
