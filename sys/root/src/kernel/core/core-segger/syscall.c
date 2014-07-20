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
#include "kernel/core/process.h"
#include "kernel/core/fork.h"
#include "kernel/core/wait.h"
#include "kernel/core/syscall.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/sysctl.h"
#include "kernel/core/flock.h"
#include "kernel/core/malloc.h"
#include "kernel/fs/vfs/vfs.h"
#include "kernel/fs/vfs/vfskernel.h"

/*===========================================
Global Declaration
=============================================*/


/*===========================================
Implementation
=============================================*/


/*-------------------------------------------
| Name:_syscall_waitpid
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_waitpid(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   //to do: not yet implemented
   waitpid_t* waitpid_dt = (waitpid_t*)data;

   //no blocking call
   if(waitpid_dt->options&WNOHANG) {

      waitpid_dt->ret=_sys_waitpid(pid,waitpid_dt->pid,
                                   waitpid_dt->options,
                                   &waitpid_dt->status);
      __flush_syscall(pthread_ptr);
      __kernel_ret_int(pthread_ptr);
      return 0;
   }

   //blocking call
   if( (waitpid_dt->ret=_sys_waitpid(pid,waitpid_dt->pid,
                                     waitpid_dt->options,
                                     &waitpid_dt->status))!=0) {
      //errno=ECHILD //no child
      pthread_ptr->stat&=(~PTHREAD_STATUS_STOP);
      _dbg_printf(" signal calling p(%d) wait(%d) : p(%d) is terminated\n",pid,waitpid_dt->pid,waitpid_dt->ret);
      __flush_syscall(pthread_ptr);
      __kernel_ret_int(pthread_ptr);
      return 0;
   }

   if(pthread_ptr->reg.syscall!=_SYSCALL_WAITPID)
      return 0;

   //calling process is stopped until child process was stopped.
   pthread_ptr->stat|=PTHREAD_STATUS_STOP;
   __flush_syscall(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_execve
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_execve(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   execve_t* execve_dt=(execve_t*)data;


   //__atomic_in();
   //__stop_sched(); now is call in _sys_exec()
   //see kernel/process.c

   //close on exec
   _cloexec_process_fd(pid);
   //
   if(process_lst[pid]->pthread_ptr->parent_pthread_ptr && process_lst[pid]->pthread_ptr->parent_pthread_ptr->stat&PTHREAD_STATUS_FORK ){
      kernel_pthread_t* parent_pthread_ptr= process_lst[pid]->pthread_ptr;

      fork_t* fork_dt = (fork_t*)process_lst[pid]->pthread_ptr->parent_pthread_ptr->reg.data;
      //
      pid_t _pid= _sys_exec(execve_dt->path,
                            execve_dt->argv,
                            execve_dt->envp,
                            pthread_ptr);

      if(_pid<0) {
         //__restart_sched();
         __flush_syscall(pthread_ptr);
         __kernel_ret_int(pthread_ptr);
      }else{
         //restart son and father
         fork_dt->pid=pid;

         __flush_syscall(process_lst[pid]->pthread_ptr);

         __restart_sched();

         __flush_syscall(parent_pthread_ptr);
         __kernel_ret_int(parent_pthread_ptr); //father
         __atomic_out();
      }
   }else{
      pid_t _pid = _sys_exec(execve_dt->path,
                             execve_dt->argv,
                             execve_dt->envp,
                             pthread_ptr);

      __restart_sched();
      __flush_syscall(pthread_ptr);
      __kernel_ret_int(pthread_ptr);
      __atomic_out();
   }


   return 0;
}

/*-------------------------------------------
| Name:_syscall_exit
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_exit(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   pid_t ppid = process_lst[pid]->ppid;
   exit_t* exit_dt=(exit_t*)data;

   if(process_lst[pid]->pthread_ptr->parent_pthread_ptr && process_lst[pid]->pthread_ptr->parent_pthread_ptr->stat&PTHREAD_STATUS_FORK ){
      kernel_pthread_t* parent_pthread_ptr= process_lst[pid]->pthread_ptr;
      fork_t* fork_dt;

      //close all process(pid) file descriptor
      //must be execute before __atomic_in() and __stop_sched()
      //because in some _vfs_close() operation on driver, this operation need inter pthread communication to be completed
      //(ex: lwip_sock_close() deadlock risk);
      _close_process_fd(pid);

      //atomic code no task switching
      __atomic_in();

      //wakeup locked process
      _sys_unlockw();

      //warning all!!! micro-kernel operation is not advised.
      __stop_sched();

      fork_dt=(fork_t*)pthread_ptr->parent_pthread_ptr->reg.data;

      //restore father context
      _sys_vfork_exit(pthread_ptr,exit_dt->status);

      //restart father
      fork_dt->pid=pid;

      __flush_syscall(pthread_ptr);

      __restart_sched();

      __flush_syscall(parent_pthread_ptr);
      __kernel_ret_int(parent_pthread_ptr); //father

   }else {

      //close all process(pid) file descriptor
      //must be execute before __atomic_in() and __stop_sched()
      //because in some _vfs_close() operation on driver, this operation need inter pthread communication to be completed
      //(ex: lwip_sock_close() deadlock risk);
      _close_process_fd(exit_dt->pid);

      //atomic code no task switching
      __atomic_in();

      //wakeup locked process
      _sys_unlockw();

      //warning!!! all micro-kernel operation is not advised.
      __stop_sched();
      //
      _sys_exit(exit_dt->pid,exit_dt->status);
      //
      __flush_syscall(pthread_ptr);

      //ppid threads are on waitpid()?
      if( ppid) {
         //get main thread of father process
         kernel_pthread_t* _pthread_ptr=process_lst[ppid]->pthread_ptr;
         //walking on threads chained list in parent process
         while(_pthread_ptr) {
            //is thread blocked on waitpid()?
            if(_pthread_ptr->stat&PTHREAD_STATUS_STOP && _pthread_ptr->reg.syscall==_SYSCALL_WAITPID){
               waitpid_t* waitpid_dt = (waitpid_t*)_pthread_ptr->reg.data;
               //waitpid_dt->pid = pid;
               _syscall_waitpid(_pthread_ptr,ppid,waitpid_dt);
            }else{
               //not then send sig child to all pthread of father processus
               _sys_kill(_pthread_ptr,SIGCHLD,1);
               //send 1 times an only the signal to the first pthread which accept SIGCHLD
               //better conformance with posix specification
               break;
            }
            _pthread_ptr=_pthread_ptr->next;
         }
      }else{
         pid_t _pid;
         //it's a daemon process
         //free process
         _sys_free(process_lst[pid]);
         process_lst[pid]= 0;
         _dbg_printf("free(%d)\n",pid);
         //checkif it's the last process
         for(_pid=1; _pid<=PROCESS_MAX; _pid++)
            if(process_lst[_pid])
               goto end;
         return -1; //in this case stop all
      }
      //
   }
end:
   __restart_sched();
   __atomic_out();
   return 0;
}

/*-------------------------------------------
| Name:_syscall_atexit
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_atexit(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   atexit_t* atexit_dt = (atexit_t*)data;

   __atomic_in();
   __stop_sched();

#if ATEXIT_MAX>0
      if( process_lst[pid]->p_atexit_func!=(atexit_func_t*)((uchar8_t*)process_lst[pid]->pthread_ptr->heap_floor+(ATEXIT_MAX+1)*sizeof(atexit_func_t)) ){
      process_lst[pid]->p_atexit_func++;
      *process_lst[pid]->p_atexit_func = atexit_dt->func;
      atexit_dt->ret = 0;
   }else{
      atexit_dt->ret = -1;
   }
#endif

   __restart_sched();

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);

   __atomic_out();
   return 0;
}

/*-------------------------------------------
| Name:_syscall_sigexit
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_sigexit(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){

   __atomic_in();
   __stop_sched();

   if( (pthread_ptr->stat&PTHREAD_STATUS_SIGHANDLER) ) {
      //return to main
      _sys_kill_exit(pthread_ptr);

      //to do: WARNING
      //pid of proccess which call kill is the same pid of process pid catch
      // must call raise in this case
      __restart_sched();

      __flush_syscall(pthread_ptr);
      __kernel_ret_int(pthread_ptr);
   }

   __restart_sched();
   __atomic_out();
   return 0;
}

/*-------------------------------------------
| Name:_syscall_kill
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_kill(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   kill_t* kill_dt = (kill_t*)data;

   __atomic_in();

   __stop_sched();

   //
   if(kill_dt->pid>PROCESS_MAX || !process_lst[kill_dt->pid]) {
      kill_dt->ret = -1; //error
   }else if(kill_dt->pid>0) {
      //get main thread of father process
      kernel_pthread_t* _pthread_ptr=process_lst[kill_dt->pid]->pthread_ptr;
      //walking on threads chained list in curent process
      while(_pthread_ptr) {
         //send signal to all pthread of processus
         kill_dt->ret   = _sys_kill(_pthread_ptr,kill_dt->sig,kill_dt->atomic);
         //send 1 times an only the signal to the first pthread which accept standard signal
         //better conformance with posix specification
         if(kill_dt->sig<NSIG)
            break;
         //
         _pthread_ptr=_pthread_ptr->next;
      }
   }else if(kill_dt->pid==-1) {

      for(kill_dt->pid = PROCESS_MAX;
          kill_dt->pid > 1;
          kill_dt->pid--) {
         kernel_pthread_t* _pthread_ptr;
         if(kill_dt->pid == pid)   //calling process is not killed
            continue;
         //get main thread of father process
         _pthread_ptr= process_lst[kill_dt->pid]->pthread_ptr;
         //walking on threads chained list in current process
         while(_pthread_ptr) {
            //send signal to all pthread of processus
            kill_dt->ret   = _sys_kill(_pthread_ptr,kill_dt->sig,kill_dt->atomic);
            //send 1 times an only the signal to the first pthread which accept standard signal
            //better conformance with posix specification
            if(kill_dt->sig<NSIG)
               break;
            //
            _pthread_ptr=_pthread_ptr->next;
         }
      }
      kill_dt->ret = 0;
   }else if(kill_dt->pid<-1) {
      pid_t _pid = -kill_dt->pid;
      pid_t _pgid = -1;

      if(!process_lst[_pid])
         kill_dt->ret = -1; goto end;

      _pgid = process_lst[pid]->pgid;

      for(kill_dt->pid = PROCESS_MAX;
          kill_dt->pid > 1;
          kill_dt->pid--) {
         kernel_pthread_t* _pthread_ptr;
         if(kill_dt->pid == pid)    //calling process is not killed
            continue;
         if(!process_lst[kill_dt->pid])
            continue;
         if(process_lst[kill_dt->pid]->pgid != _pgid)
            continue;
         //get main thread of father process
         _pthread_ptr= process_lst[kill_dt->pid]->pthread_ptr;
         //walking on threads chained list in curent process
         while(_pthread_ptr) {
            //send signal to all pthread of processus
            kill_dt->ret   = _sys_kill(_pthread_ptr,kill_dt->sig,kill_dt->atomic);
            //send 1 times an only the signal to the first pthread which accept standard signal
            //better conformance with posix specification
            if(kill_dt->sig<NSIG)
               break;
            //
            _pthread_ptr=_pthread_ptr->next;
         }
      }
      kill_dt->ret = 0;
   }

   //
end:
   __restart_sched();

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);

   __atomic_out();
   return 0;
}

/*-------------------------------------------
| Name:_syscall_vfork
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_vfork(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   fork_t* fork_dt=(fork_t*)data;

   __atomic_in();
   __stop_sched();

   pthread_ptr->irq_nb=0x00; //disable father interrupt
   fork_dt->pid = _sys_vfork(pthread_ptr);
   if(fork_dt->pid>0) {
      //copy all father process file descriptor
      //nb_reader++ and/ou nbwriter++
      _copy_process_fd(fork_dt->pid,fork_dt->ppid,0);

      __restart_sched();

      __flush_syscall(pthread_ptr);
      __kernel_ret_int(pthread_ptr); //son
   }
   else{
      //error
      __restart_sched();

      __flush_syscall(pthread_ptr);
      __kernel_ret_int(pthread_ptr); //father
   }

   __atomic_out();
   return 0;
}

/*-------------------------------------------
| Name:_syscall_sigprocmask
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_sigprocmask(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   sigprocmask_t* sigprocmask_dt=(sigprocmask_t*)data;
   sigprocmask_dt->ret  = _sys_sigprocmask(pthread_ptr,
                                           sigprocmask_dt->how,
                                           sigprocmask_dt->set,
                                           sigprocmask_dt->oset);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_sigpending
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_sigpending(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   sigpending_t* sigpending_dt=(sigpending_t*)data;
   sigpending_dt->ret   = _sys_sigpending(pthread_ptr,sigpending_dt->set);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_sigaction
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_sigaction(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   sigaction_t* sigaction_dt=(sigaction_t*)data;
   sigaction_dt->ret = _sys_sigaction(pthread_ptr,
                                      sigaction_dt->sig,
                                      sigaction_dt->act,sigaction_dt->oact);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_alarm
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_alarm(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   alarm_t* alarm_dt = (alarm_t*)data;
   unsigned int time_remain = 0;

   if(pthread_ptr->time_out>((time_t)0))
      time_remain = pthread_ptr->time_out;

   pthread_ptr->time_out = (time_t)alarm_dt->seconds*(time_t)(1000/__KERNEL_ALARM_TIMER);

   alarm_dt->seconds = time_remain;

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_pause
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_pause(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   __flush_syscall(pthread_ptr);
   //no __kernel_ret_int: calling process is suspended
   return 0;
}

/*-------------------------------------------
| Name:_syscall_dup
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_dup(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   dup_t* dup_dt = (dup_t*)data;

   dup_dt->fd = _sys_dup(pid,dup_dt->fd,0);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_dup2
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_dup2(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   dup2_t* dup2_dt = (dup2_t*)data;

   dup2_dt->fd = _sys_dup2(pid,dup2_dt->fd,dup2_dt->fd2);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_pipe
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_pipe(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   const char fifo_path[]="/dev/fifo";
   char path[16];
   pipe_t* pipe_dt = (pipe_t*)data;
   open_t open_dt;

   pipe_dt->ret=0;

   __kernel_begin_syscall(pthread_ptr); //use for explicit internal syscall
   //read fd
   strcpy(path,fifo_path);
   open_dt.path = (char*)&path;
   open_dt.oflag = O_RDONLY;
   _syscall_open(pthread_ptr,pid,&open_dt);
   pipe_dt->fd_r = open_dt.fildes;

   //write fd
   strcpy(path,fifo_path);
   open_dt.path = (char*)&path;
   open_dt.oflag = O_WRONLY;
   _syscall_open(pthread_ptr,pid,&open_dt);
   pipe_dt->fd_w = open_dt.fildes;

   if(pipe_dt->fd_r<0 || pipe_dt->fd_w<0)
      pipe_dt->ret=-1;

   __kernel_end_syscall(pthread_ptr);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_gettimeofday
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_gettimeofday(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   gettimeofday_t* gettimeofday_dt = (gettimeofday_t*)data;

   gettimeofday_dt->ret=_sys_gettimeofday(gettimeofday_dt->tv, gettimeofday_dt->tz);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_settimeofday
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_settimeofday(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   settimeofday_t* settimeofday_dt = (settimeofday_t*)data;

   settimeofday_dt->ret=_sys_settimeofday(settimeofday_dt->tv, settimeofday_dt->tz);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}


/*-------------------------------------------
| Name:_syscall_malloc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_malloc(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   malloc_t* malloc_dt = (malloc_t*)data;
   malloc_dt->p = _sys_malloc(malloc_dt->size);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_calloc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_calloc(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   calloc_t* calloc_dt =  (calloc_t*)data;
   calloc_dt->p = _sys_calloc(calloc_dt->nelem,calloc_dt->elsize);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_calloc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_realloc(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   realloc_t* realloc_dt =  (realloc_t*)data;
   realloc_dt->p = _sys_realloc(realloc_dt->p,realloc_dt->size);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_free
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_free(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   free_t* free_dt = (free_t*)data;
   _sys_free(free_dt->p);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_fcntl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_fcntl(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   fcntl_t* fcntl_dt = (fcntl_t*)data;
   switch(fcntl_dt->cmd) {
   case F_DUPFD:
   case F_GETFD:
   case F_SETFD:
   case F_GETFL:
   case F_SETFL:
      fcntl_dt->ret = _sys_fcntl(pid,fcntl_dt->fd,fcntl_dt->cmd,
                                 fcntl_dt->argc,fcntl_dt->argv);
      __flush_syscall(pthread_ptr);
      __kernel_ret_int(pthread_ptr);
      break;


   case F_GETLK: {
      struct flock* p_flock;

      if(fcntl_dt->argc<0) {
         fcntl_dt->ret = -1;
         __flush_syscall(pthread_ptr);
         __kernel_ret_int(pthread_ptr);
         return 0;
      }

      if( !(p_flock = (struct flock*)fcntl_dt->argv[0]) ) {
         fcntl_dt->ret = -1;
         __flush_syscall(pthread_ptr);
         __kernel_ret_int(pthread_ptr);
         return 0;
      }

      p_flock->l_pid = pid;
      fcntl_dt->ret = _sys_lock(pthread_ptr,fcntl_dt->fd,fcntl_dt->cmd,p_flock);


      __flush_syscall(pthread_ptr);
      __kernel_ret_int(pthread_ptr);
   }
   break;

   case F_SETLKW:   //blocking call
   case F_SETLK: {
      struct flock* p_flock;

      fcntl_dt->ret;

      if(fcntl_dt->argc<0) {
         fcntl_dt->ret = -1;
         __flush_syscall(pthread_ptr);
         __kernel_ret_int(pthread_ptr);
         return 0;
      }

      if( !(p_flock = (struct flock*)fcntl_dt->argv[0]) ) {
         fcntl_dt->ret = -1;
         __flush_syscall(pthread_ptr);
         __kernel_ret_int(pthread_ptr);
         return 0;
      }

      p_flock->l_pid = pid;
      fcntl_dt->ret = _sys_lock(pthread_ptr,fcntl_dt->fd,fcntl_dt->cmd,p_flock);

      if(!fcntl_dt->ret && p_flock->l_type==F_UNLCK)
         _sys_unlockw();

      __flush_syscall(pthread_ptr);

      if( (fcntl_dt->cmd == F_SETLK)
          || (fcntl_dt->cmd == F_SETLKW && !fcntl_dt->ret)) {
         //to do: list all pthread of this process lock on this node
         __kernel_ret_int(pthread_ptr);    // locked ok
      }else if(fcntl_dt->cmd == F_SETLKW && fcntl_dt->ret<0) {
         pthread_ptr->stat=PTHREAD_STATUS_STOP;      //cannot be locked:proccess is waiting
      }

   }
   break;
   }
   return 0;
}

/*--------------------------------------------
| Name:        _syscall_ioctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_ioctl(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   va_list ap;
   ioctl_t* ioctl_dt = (ioctl_t*)data;
   desc_t desc = process_lst[pid]->desc_tbl[ioctl_dt->fd];

   __va_list_copy(ap, ioctl_dt->ap);

   switch(ioctl_dt->request) {
   //I_LINK
   case I_LINK: {
      int fd_link;
      desc_t desc_link;

      fd_link = va_arg(ap, int);
      if(fd_link<0) {
         __flush_syscall(pthread_ptr);
         __kernel_ret_int(pthread_ptr);
         return -1;
      }

      //get kernel open file descriptor from process file descriptor;
      if((desc_link= process_lst[pid]->desc_tbl[fd_link])<0) {
         __flush_syscall(pthread_ptr);
         __kernel_ret_int(pthread_ptr);
         return -1;
      }
      //retrieve original ap position in stack
      __va_list_copy(ap, ioctl_dt->ap);
      //vfs ioctl use only kernel open file descriptor
      ioctl_dt->ret = _vfs_ioctl(desc,ioctl_dt->request,desc_link,ap);
   }
   break;

   //I_UNLINK,....
   default:
      ioctl_dt->ret = _vfs_ioctl2(desc,ioctl_dt->request,ioctl_dt->ap);
      break;
   }

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*--------------------------------------------
| Name:        _syscall_sysctl
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_sysctl(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   sysctl_t* sysctl_dt = (sysctl_t*)data;

   sysctl_dt->ret = _sys_sysctl(sysctl_dt->name,sysctl_dt->namelen,
                                sysctl_dt->oldp,sysctl_dt->oldlenp,
                                sysctl_dt->newp,sysctl_dt->newlen);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_setpgid
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_setpgid(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   setpgid_t* setpgid_dt = (setpgid_t*)data;
   setpgid_dt->id_grp = _sys_setpgid(setpgid_dt->pid,setpgid_dt->id_grp);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*-------------------------------------------
| Name:_syscall_getpgrp
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _syscall_getpgrp(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   getpgrp_t* getpgrp_dt = (getpgrp_t*)data;
   getpgrp_dt->id_grp = _sys_getpgrp(getpgrp_dt->pid);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*--------------------------------------------
| Name:        _syscall_pthread_create
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_pthread_create(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   pthread_create_t* pthread_create_dt = (pthread_create_t*)data;

   pthread_create_dt->ret=_sys_pthread_create(&pthread_create_dt->kernel_pthread,pthread_ptr,pthread_create_dt->attr,
                       pthread_create_dt->start_routine,pthread_create_dt->arg,pid);

   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*--------------------------------------------
| Name:        _syscall_pthread_cancel
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_pthread_cancel(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   pthread_cancel_t* pthread_cancel_dt = (pthread_cancel_t*)data;

   if(process_lst[pid]->pthread_ptr!=pthread_cancel_dt->kernel_pthread) {
      //it's a thread annexe
      //pthread sigqueue
#ifdef __KERNEL_POSIX_REALTIME_SIGNALS
      //remove sigqueue objects explicitly only for annexe thread in other case "put all object" is used in process exit scenario.
      pthread_cancel_dt->kernel_pthread->kernel_sigqueue.destructor(&pthread_cancel_dt->kernel_pthread->kernel_sigqueue);
#endif
      //
      pthread_cancel_dt->ret = _sys_pthread_cancel(pthread_cancel_dt->kernel_pthread,pid);

      //calling thread kill himself???
      if(pthread_cancel_dt->kernel_pthread==_syscall_owner_pthread_ptr)
         return 0;  ///yes it's same than pthread_exit()
      //
      __flush_syscall(pthread_ptr);
      // in the other case retur to callin thread
      __kernel_ret_int(pthread_ptr);
      return 0;
   }else{
      //it's the main thread
      //all thread must be terminated
      exit_t exit_dt;
      exit_dt.pid = pid;
      exit_dt.status = 0;
      return _syscall_exit(pthread_ptr,pid,&exit_dt);
   }

   return 0;
}

/*--------------------------------------------
| Name:        _syscall_pthread_kill
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_pthread_kill(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){

   pthread_kill_t* pthread_kill_dt = (pthread_kill_t*)data;

   pthread_kill_dt->ret=-1;

   __atomic_in();

   //send signal to pthread
   if(pthread_kill_dt->kernel_pthread) {
      __stop_sched();
      pthread_kill_dt->ret = _sys_kill(pthread_kill_dt->kernel_pthread,
                                       pthread_kill_dt->sig,
                                       pthread_kill_dt->atomic);
      __restart_sched();
   }

   //
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);

   __atomic_out();
   return 0;
}

/*--------------------------------------------
| Name:        _syscall_pthread_exit
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_pthread_exit(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   pthread_exit_t* pthread_exit_dt = (pthread_exit_t*)data;

   if(pid>0 && process_lst[pid]->pthread_ptr!=pthread_exit_dt->kernel_pthread) {
      //pthread sigqueue
#ifdef __KERNEL_POSIX_REALTIME_SIGNALS
      //remove sigqueue objects explicitly only for annexe thread in other case "put all object" is used in process exit scenario.
      pthread_exit_dt->kernel_pthread->kernel_sigqueue.destructor(&pthread_exit_dt->kernel_pthread->kernel_sigqueue);
#endif
      //it's a thread annexe
      _sys_pthread_cancel(pthread_exit_dt->kernel_pthread,pid);
      //__flush_syscall(pthread_ptr);
      return 0;
   }else if(pid>0){
      //it's the main thread
      //all thread must be terminated
      exit_t exit_dt;
      exit_dt.pid = pid;
      exit_dt.status = 0;
      return _syscall_exit(pthread_ptr,pid,&exit_dt);
   }else{
      //native kernel pthread. no process container
      _sys_pthread_cancel(pthread_exit_dt->kernel_pthread,pid);
   }

   return 0;
}

/*--------------------------------------------
| Name:        _syscall_pthread_mutex_init
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_pthread_mutex_init(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   pthread_mutex_init_t* pthread_mutex_init_dt = (pthread_mutex_init_t*)data;
   //
   pthread_mutex_init_dt->ret=0;
   //parano�ac protection ;)
   __atomic_in();
   //
   if(pthread_mutex_init_dt->mutex && pthread_mutex_init_dt->mutex->kernel_object==PTHREAD_MUTEX_UNINITIALIZED){
      pthread_mutex_init_dt->mutex->kernel_object = kernel_object_manager_get(&process_lst[pid]->kernel_object_head, KERNEL_OBJECT_PTRHEAD_MUTEX, KERNEL_OBJECT_SRC_POOL);
      if(!pthread_mutex_init_dt->mutex->kernel_object)
         pthread_mutex_init_dt->ret = -1;
   }
   //parano�ac protection ;)
   __atomic_out();
   //
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}
/*--------------------------------------------
| Name:        _syscall_pthread_mutex_destroy
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_pthread_mutex_destroy(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   pthread_mutex_destroy_t* pthread_mutex_destroy_dt = (pthread_mutex_destroy_t*)data;
   //
   pthread_mutex_destroy_dt->ret=0;
   //parano�ac protection ;)
   __atomic_in();
   //
   if(pthread_mutex_destroy_dt->mutex && pthread_mutex_destroy_dt->mutex->kernel_object!=PTHREAD_MUTEX_UNINITIALIZED){
      if(!kernel_object_manager_put(&process_lst[pid]->kernel_object_head, pthread_mutex_destroy_dt->mutex->kernel_object))
         pthread_mutex_destroy_dt->ret=-1;
      if(!pthread_mutex_destroy_dt->ret)
         pthread_mutex_destroy_dt->mutex->kernel_object=PTHREAD_MUTEX_UNINITIALIZED;
   }
   //parano�ac protection ;)
   __atomic_out();
   //
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*--------------------------------------------
| Name:        _syscall_pthread_cond_init
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_pthread_cond_init(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   pthread_cond_init_t* pthread_cond_init_dt = (pthread_cond_init_t*)data;
   //
   pthread_cond_init_dt->ret=0;
   //parano�ac protection ;)
   __atomic_in();
   //
   if(pthread_cond_init_dt->cond && pthread_cond_init_dt->cond->kernel_object==PTHREAD_COND_UNINITIALIZED){
      pthread_cond_init_dt->cond->kernel_object = kernel_object_manager_get(&process_lst[pid]->kernel_object_head, KERNEL_OBJECT_PTRHEAD_MUTEX, KERNEL_OBJECT_SRC_POOL);
      if(!pthread_cond_init_dt->cond->kernel_object)
         pthread_cond_init_dt->ret = -1;
   }
   //parano�ac protection ;)
   __atomic_out();
   //
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*--------------------------------------------
| Name:        _syscall_pthread_cond_destroy
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_pthread_cond_destroy(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   pthread_cond_destroy_t* pthread_cond_destroy_dt = (pthread_cond_destroy_t*)data;
   //
   pthread_cond_destroy_dt->ret=0;
   //parano�ac protection ;)
   __atomic_in();
   //
   if(pthread_cond_destroy_dt->cond && pthread_cond_destroy_dt->cond->kernel_object!=PTHREAD_COND_UNINITIALIZED){
      if(!kernel_object_manager_put(&process_lst[pid]->kernel_object_head, pthread_cond_destroy_dt->cond->kernel_object))
         pthread_cond_destroy_dt->ret=-1;
      if(!pthread_cond_destroy_dt->ret)
         pthread_cond_destroy_dt->cond->kernel_object=PTHREAD_COND_UNINITIALIZED;
   }
   //parano�ac protection ;)
   __atomic_out();
   //
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*--------------------------------------------
| Name:        _syscall_timer_create
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_timer_create(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   timer_create_t* timer_create_dt = (timer_create_t*)data;
   kernel_object_t* kernel_object;

   timer_create_dt->ret=0;
   //
   if( !(kernel_object = kernel_object_manager_get(&process_lst[pid]->kernel_object_head, KERNEL_OBJECT_TIMER, KERNEL_OBJECT_SRC_POOL,
                                      timer_create_dt->clockid,
                                      timer_create_dt->psigevent,
                                      timer_create_dt->ptimerid)) )
      timer_create_dt->ret=-1;
   //
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*--------------------------------------------
| Name:        _syscall_timer_delete
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_timer_delete(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   timer_delete_t* timer_delete_dt = (timer_delete_t*)data;

   kernel_object_t* kernel_object=(kernel_object_t*)(*timer_delete_dt->ptimerid);

   kernel_object_manager_put(&process_lst[pid]->kernel_object_head, kernel_object);
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*--------------------------------------------
| Name:        _syscall_sem_init
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_sem_init(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   sem_init_t* sem_init_dt = (sem_init_t*)data;
   kernel_object_t* kernel_object;
   //
   sem_init_dt->ret = 0;
   //
   if(!sem_init_dt->name) { //anonymous semaphore
      if(!(kernel_object=kernel_object_manager_get(&process_lst[pid]->kernel_object_head, KERNEL_OBJECT_SEM, KERNEL_OBJECT_SRC_EXTERN,
                                        sem_init_dt->psem,
                                        sem_init_dt->value)))
         sem_init_dt->ret = -1;
   }
   //
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*--------------------------------------------
| Name:        _syscall_sem_destroy
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _syscall_sem_destroy(kernel_pthread_t* pthread_ptr, pid_t pid, void* data){
   sem_destroy_t* sem_destroy_dt = (sem_destroy_t*)data;

   kernel_object_t* kernel_object=sem_destroy_dt->psem;

   kernel_object_manager_put(&process_lst[pid]->kernel_object_head, kernel_object);

   sem_destroy_dt->ret = 0;
   //
   __flush_syscall(pthread_ptr);
   __kernel_ret_int(pthread_ptr);
   return 0;
}

/*===========================================
End of Source syscall.c
=============================================*/


