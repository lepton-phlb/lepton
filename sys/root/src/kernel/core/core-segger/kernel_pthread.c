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

/**
 * \addtogroup lepton_kernel
 * @{
 *
 */

/**
 * \defgroup kernel_pthread les kernel pthread
 * @{
 *
 * Implementation des kernel pthread qui réalise l'abstration logiciel vis a vis du micro noyau
 * temps réel.\n
 * Les kernel pthread permettent d'encapsuler les mécanismes de gestion de tâches et de sémaphores du micro noyau utilisé. 
 * Les fonctions implémentées dans cette bibliothèque respectent les définitions de la norme POSIX Realtime Extension.. 
 * En revanche certaine fonctionnalité décrite dans le pthread ne sont pas disponibles dans cette version, 
 * notamment pour un soucis de portabilité.
 * La gestion des kernel thread est en prise directe avec le noyau temps réel utilisé. 
 * Un certain nombre de macros sont donc à spécialiser afin d'assurer la portabilité.
 * 
 */

/**
 * \file
 * implementation des kernel pthread
 * \author philippe le boulanger
 *
 *
 */

/*===========================================
Includes
=============================================*/
#include "kernel/core/errno.h"
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/kernel_pthread_mutex.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/syscall.h"


/*===========================================
Global Declaration
=============================================*/
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

   p->next  = (struct kernel_pthread_st*)0;
   p->gprev = (struct kernel_pthread_st*)0;
   p->gnext = (struct kernel_pthread_st*)0;
   //io desc
   p->io_desc =(desc_t)-1;

   //init counter
   g_pthread_id=((g_pthread_id<__KERNEL_PTHREAD_ID_LIMIT)?g_pthread_id+1:0);
   pthread_ptr=g_pthread_lst;
   //
   while(pthread_ptr){
      if(pthread_ptr->id==g_pthread_id){
         g_pthread_id=((g_pthread_id<__KERNEL_PTHREAD_ID_LIMIT)?g_pthread_id+1:0);
         pthread_ptr=g_pthread_lst;
         if((++counter)>__KERNEL_PTHREAD_ID_LIMIT)
            return -1;//error no id available
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

   /*if( ((char*)p->attr.stackaddr+size)>=( (char*)p->attr.stackaddr + p->attr.stacksize) )
      return (void*)0;*/

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
   if(pthread->pid<1){
      kernel_pthread_cancel(pthread);//native kernel pthread: no process container
   }else{
      //pthread in process container
      //use syscall
      pthread_exit_t pthread_exit_dt;
      pthread_exit_dt.kernel_pthread = pthread;
      pthread_exit_dt.value_ptr = (void*)0;
       //to do check if it's the main thread call exit
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
int   kernel_pthread_create(kernel_pthread_t *thread, const pthread_attr_t *attr,start_routine_t start_routine, void *arg){
   
   if(!thread)
      return -EINVAL;

   //reset kernel_pthread_t structure
   memset(thread,0,sizeof(kernel_pthread_t));

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

   //set field
   //not attached to process
   thread->pid=(pid_t)-1;
   //timeout disabled
   thread->time_out  = (time_t)-1;//for alarm()
   //status 
   thread->stat=PTHREAD_STATUS_NULL;
   //
   memcpy(thread->sigaction_lst,sigaction_dfl_lst,sizeof(sigaction_dfl_lst));

   //alloc tcb
   if( !(thread->tcb=_sys_malloc(sizeof(tcb_t))) )
      return -EAGAIN;
   //add to kernel pthread list
   if( kernel_get_pthread_id(thread)==-EAGAIN)
      return -EAGAIN;
   
   #ifdef USE_SEGGER
   {
      char* name = (char*)thread->attr.name;
      pid_t pid = thread->pid; 
      //only for debug with embos view
      if(name)
         name = thread->attr.name;
      else
         name = "daemon_kernel_thread";
      //
      OS_CreateTask(
         thread->tcb,
         name,
         thread->attr.priority,
         pthread_routine,
         (void OS_STACKPTR*)thread->attr.stackaddr,
         thread->attr.stacksize,
         thread->attr.timeslice
        );
      #if OS_CHECKSTACK
         //check bottom stack with OS_STACKFILL_CHAR. 
         //don't use the last byte at the bottom of thread stack.
      {
         uint32_t _stack_addr = (uint32_t)thread->attr.stackaddr;
         uchar8_t _align = (4-(_stack_addr%4))+4;///to remove: just debug test
         
         thread->heap_floor = (uchar8_t*)(thread->attr.stackaddr)+_align*sizeof(uchar8_t);//data alignement 4 bytes
         thread->heap_top   = thread->heap_floor;

      }
      #endif
   }
   #endif

   #if defined(__KERNEL_IO_SEM)
      kernel_sem_init(&thread->io_sem,0,0);
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

   desc_t desc=0;

   if(thread->tcb==(tcb_t*)0)//empty pthread. just a container fo bacup context in vfork() operation.
      return 0;

   //
   if(kernel_put_pthread_id(thread)==-ESRCH)
      return -ESRCH;

   //unreference this thread in pthread owner from desc.
   __atomic_in();
   //begin of section: protection from io interrupt
   __disable_interrupt_section_in();
   //
   for(desc=0;desc<MAX_OPEN_FILE;desc++){
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
  
   #ifdef USE_SEGGER
   {
         OS_TASK* whois_lock_kernel_mutex = OS_GetResourceOwner(&kernel_mutex.mutex);
         OS_TASK* this_task = thread->tcb;

         
         //warning!!!: preprocessor OS_SUPPORT_CLEANUP_ON_TERMINATE only supported for embos ver<=3.32 for all target arm7 and m16c.
         #ifndef OS_SUPPORT_CLEANUP_ON_TERMINATE 
         {
            //patch for m16c version or when OS_SUPPORT_CLEANUP_ON_TERMINATE not supported.
            //free kernel mutex. it was taken by this pthread.
            if(this_task == whois_lock_kernel_mutex){
               #ifndef CPU_WIN32 //ugly patch :(
                __syscall_unlock();
               #endif
            }
         }
         #endif

      //terminate thread in scheduler
      OS_Terminate(thread->tcb);//if define OS_SUPPORT_CLEANUP_ON_TERMINATE implicit cleanup ressource

      //free tcb
      if(thread->tcb){
         _sys_free(thread->tcb);
         thread->tcb = (tcb_t*)0;
      }


      #ifndef CPU_WIN32 //ugly patch :(
      if(this_task == whois_lock_kernel_mutex){
         //patch free ressource semaphore without proprietary. this pthread owner was terminated.
         __syscall_lock();//kernel is proprietary now. the next _syscall_unlock() it's safe now.
      }
      #endif
      //
   }
   #endif
   
   //end of section: protection from io interrupt
   __disable_interrupt_section_out();
   //
   __atomic_out();
   //

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
   while(p){
      if( __is_thread_self(p->tcb) ){
         __atomic_out();
         return p;
      }
      p=p->gnext;
   }

   __atomic_out();
   return (kernel_pthread_t*)0;
}

/** @} */
/** @} */
/*===========================================
End of Sourcethread.c
=============================================*/
