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


/*===========================================
Includes
=============================================*/
#include <stdlib.h>

#include "kernel/core/errno.h"
#include "kernel/core/kernel.h"
#include "kernel/core/process.h"
#include "kernel/core/system.h"
#include "kernel/core/fork.h"
#include "kernel/core/systime.h"
#include "kernel/core/time.h"
#include "kernel/core/malloc.h"


/*===========================================
Global Declaration
=============================================*/


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:_chkesp
| Description:
| Parameters:
| Return Type:
| Comments:Win32 Hacking code, disable esp check
| See:/GZ VC++ compiler option
---------------------------------------------*/
#if defined(WIN32) && defined(LEPTON_CHKESP)
   #pragma message ("check esp patch")
void __declspec ( naked ) _chkesp(void)
{
   _asm {
      ret
   }
}
#endif

/*-------------------------------------------
| Name:_sys_vfork
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t _sys_vfork(kernel_pthread_t* pthread_ptr){

   //to do: copy all father process descriptor
   pid_t pid  = 0;
   pid_t ppid = pthread_ptr->pid;
   char* p;
   struct __timeval tv;
   int argc;

   kernel_pthread_t* backup_parent_pthread_ptr;

   //
#ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
   __clr_irq();
#endif
   //profiler
   __kernel_profiler_start();
   //

   //1)process operation
   //pseudo process creation
   if(_nextpid(&pid)==-EAGAIN) {
      //kernel panic!!!
#ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
      __set_irq();
#endif
      return (pid_t)-ENOMEM;
   }
   //dynamic allocation:alloc
   p = _sys_malloc(sizeof(process_t));
   if(!p) {
      //kernel panic!!!
#ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
      __set_irq();
#endif
      return (pid_t)(-ENOMEM);
   }
   process_lst[pid] = (process_t*)p;
   memset(process_lst[pid],0,sizeof(process_t));
   //
   //_dbg_printf("fork _pid:%d\n",_pid);
   //copy all ppid father process properties to the son (pthread_ptr, ...)
   memcpy(process_lst[pid],process_lst[ppid],sizeof(process_t));

   //modify specific process properties of new process
   _sys_gettimeofday(&tv,NULL);
   process_lst[pid]->pid         = pid;
   process_lst[pid]->ppid        = ppid;
   process_lst[pid]->pgid        = process_lst[ppid]->pgid;
   process_lst[pid]->start_time  = tv.tv_sec;
   //prepare kernel object chained list (see _sys_krnl_exec() )
   process_lst[pid]->kernel_object_head = (kernel_object_t*)0;

   process_lst[pid]->inode_curdir = process_lst[ppid]->inode_curdir;

   //copy argument
   argc=0;
   process_lst[pid]->argc=0;

   process_lst[pid]->argv[argc]=strtok(process_lst[pid]->arg," ");
   while(process_lst[pid]->argv[argc++]) {
      process_lst[pid]->argv[argc]=strtok(0," ");
   }
   process_lst[pid]->argc=argc-1;

   //2)pthread operation
   backup_parent_pthread_ptr=(kernel_pthread_t*)_sys_malloc(sizeof(kernel_pthread_t));
   if(!backup_parent_pthread_ptr) {
      //
      _sys_free(process_lst[pid]);
      //kernel panic!!!
#ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
      __set_irq();
#endif
      return -ENOMEM; //kernel panic!!!
   }

   //brutal copy parent pthread in new pthread
   memcpy(backup_parent_pthread_ptr,pthread_ptr,sizeof(kernel_pthread_t));
   //switch pthread_ptr
   process_lst[pid]->pthread_ptr=pthread_ptr;
   process_lst[pid]->pthread_ptr->parent_pthread_ptr = backup_parent_pthread_ptr;

   //back_up calling thread context in parent pthread
   __bckup_context(backup_parent_pthread_ptr->bckup_context,backup_parent_pthread_ptr);
   //back_up ppid  stack in parent pthread
   //must be placed after process_t allocation. heap external fragmentation.
   // see process.c _sys_exec and restore stack
   __bckup_stack(backup_parent_pthread_ptr);

   //to remove:
   _dbg_printf("vfork() pid:%d\n",pid);
   //attach thread with process
   process_lst[pid]->pthread_ptr->pid = pid;
   //reinit status
   process_lst[pid]->pthread_ptr->stat = PTHREAD_STATUS_NULL;
   //
   process_lst[pid]->pthread_ptr->time_out  = (time_t)-1; //for alarm()
   //retinit parent thread status
   process_lst[pid]->pthread_ptr->parent_pthread_ptr->stat |= PTHREAD_STATUS_FORK;

   //remove pthread from the pthread list of father process
   _sys_process_remove_pthread(process_lst[ppid],pthread_ptr);
   //reinit thread chain list in new process container
   process_lst[pid]->pthread_ptr->next = (kernel_pthread_t*)0; //(not need kernel_put_pthread_id() and kernel_get_pthread_id() operation)

   //to do: reinit sigqueue struct
   //thread sigqueue
#ifdef __KERNEL_POSIX_REALTIME_SIGNALS
   memcpy(&process_lst[pid]->pthread_ptr->kernel_sigqueue,&_kernel_sigqueue_initializer,
          sizeof(kernel_sigqueue_t));
   process_lst[pid]->pthread_ptr->kernel_sigqueue.constructor(
      &process_lst[pid]->kernel_object_head, &process_lst[pid]->pthread_ptr->kernel_sigqueue);
#endif

   //profiler
   __kernel_profiler_stop(backup_parent_pthread_ptr);
   __profiler_add_result(backup_parent_pthread_ptr,_SYSCALL_VFORK,
                         __kernel_profiler_get_counter(backup_parent_pthread_ptr));
   //
#ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
   __set_irq();
#endif

   return pid;
}

/*-------------------------------------------
| Name:_sys_vfork_exit
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _sys_vfork_exit(kernel_pthread_t* pthread_ptr,int status){

   pid_t pid;
   pid_t ppid;

   kernel_pthread_t* backup_parent_pthread_ptr;

   //
#ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
   __clr_irq();
#endif

   //get pid
   pid  = pthread_ptr->pid;
   //get ppid
   ppid = process_lst[pid]->ppid;

   //get main thread;
   pthread_ptr=process_lst[pid]->pthread_ptr;

   //put all kernel object from chained list (see _sys_exit() )
   //must be use before all pthreads stack destroyed and local variable too.
   kernel_object_manager_put_all(&process_lst[pid]->kernel_object_head);

   //to do: cancel all annexe thread in pid process not the main thread
   _sys_pthread_cancel_all_except(pid,pthread_ptr);

   //prepare unswicth pthread_ptr
   backup_parent_pthread_ptr = pthread_ptr->parent_pthread_ptr;
   //preserve glist
   backup_parent_pthread_ptr->gnext = pthread_ptr->gnext;
   backup_parent_pthread_ptr->gprev = pthread_ptr->gprev;
   //backup_parent_pthread_ptr->next  = pthread_ptr->next;

   //unswicth pthread_ptr
   memcpy(pthread_ptr,backup_parent_pthread_ptr,sizeof(kernel_pthread_t));
   process_lst[pid]->pthread_ptr = backup_parent_pthread_ptr;

   //
   //re-insert pthread from the pthread list of father process
   _sys_process_insert_pthread(process_lst[ppid],pthread_ptr);

   //restore ppid stack
   __rstr_stack(pthread_ptr);
   //__rstr_stack(pthread_ptr,&backup_parent_pthread_ptr->bckup_context,backup_parent_pthread_ptr->bckup_stack);

   //restore pthread context
   __rstr_context(pthread_ptr->bckup_context,pthread_ptr);
   //__rstr_context(pthread_ptr,&backup_parent_pthread_ptr->bckup_context);

   _sys_adopt(pid);

   //cancel pseudo process
   //retire status
   pthread_ptr->stat &= (~PTHREAD_STATUS_FORK);

   //see _sys_waitpid() and _sys_exit()
   //cancel all annexe thread in pid process main thread will be cancelled in _sys_waitpid()
   memset(process_lst[pid]->pthread_ptr,0,sizeof(kernel_pthread_t));
   //reattach pthread to proces (see _sys_pthread_cancel() )
   process_lst[pid]->pthread_ptr->pid=pid;
   process_lst[pid]->status = status;
   process_lst[pid]->pthread_ptr->stat|=(PTHREAD_STATUS_ZOMBI);
   process_lst[pid]->pthread_ptr->tcb = (tcb_t*)0; //dettach tcb of this zombi process and reattach to the original process.
   process_lst[pid]->pthread_ptr->io_desc = -1;


   //dynamic allocation:free: see _sys_waitpid()
   //free(process_lst[pid]);
   //process_lst[pid]= 0;
   //
   _dbg_printf("vfork exit _pid:%d\n",pid);

#ifdef KERNEL_PROCESS_VFORK_CLRSET_IRQ
   __set_irq();
#endif

   return 0;
}

/*===========================================
End of Sourcefork.c
=============================================*/
