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

#include "kernel/core/kernelconf.h"
#include "kernel/core/syscall.h"
#include "kernel/core/system.h"
#include "kernel/core/process.h"
#include "kernel/fs/vfs/vfs.h"
#include "kernel/fs/vfs/vfskernel.h"
#include "lib/libc/stdio/stdio.h"



/*===========================================
Global Declaration
=============================================*/

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:execl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t execl(const char* path, const char* arg,...){
   char* argv[ARG_MAX]={0};
   int argc=0;
   int i=1;
   execve_t execve_dt;
   va_list ptr;

   va_start(ptr, arg);

   //argv[0]= (char*)path;
   if(arg){
      argv[argc++] = (char*)arg;
      for(;argc<ARG_MAX;argc++){
         argv[argc] = va_arg( ptr,char*);
         if(!argv[argc]) break;
      }
   }
   va_end(ptr);

   //
   execve_dt.path=path;
   execve_dt.argv=(const char **)argv;
   execve_dt.envp=0;
   execve_dt.ppid=_sys_getppid();

   __mk_syscall(_SYSCALL_EXECVE,execve_dt)
   
   return execve_dt.pid;
}

/*-------------------------------------------
| Name:execv
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t execv(const char* path, const char* argv[]){
   execve_t execve_dt;

   execve_dt.path=path;
   execve_dt.argv=argv;
   execve_dt.envp=0;
   execve_dt.ppid=_sys_getppid();

   __mk_syscall(_SYSCALL_EXECVE,execve_dt)
   
   return execve_dt.pid;
}

/*-------------------------------------------
| Name:execve
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t execve(const char* path, const char* argv[],const  char* envp){
   execve_t execve_dt;

   execve_dt.path=path;
   execve_dt.argv=argv;
   execve_dt.envp=envp;
   execve_dt.ppid=_sys_getppid();

   __mk_syscall(_SYSCALL_EXECVE,execve_dt)
   
   return execve_dt.pid;
}

/*-------------------------------------------
| Name:pause
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int pause(void){
   __mk_syscall2(_SYSCALL_PAUSE);
   //to do: return -1 if errno=[EINTR]
   //A signal is caught by the calling process 
   //and control is returned from the signal-catching function. 

   return 0;
}

/*-------------------------------------------
| Name:alarm
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned int alarm(unsigned int seconds){
   alarm_t alarm_dt;
   alarm_dt.seconds = seconds;
#if defined(USE_SEGGER)
   __mk_syscall(_SYSCALL_ALARM,alarm_dt)
#endif
   return alarm_dt.seconds = seconds;
}

/*-------------------------------------------
| Name:vfork
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t vfork(void){

   int pid=_sys_getpid();

   fork_t fork_dt;

   fork_dt.pid=0;
   fork_dt.ppid=_sys_getpid();

   __mk_syscall(_SYSCALL_VFORK,fork_dt);

   if(fork_dt.pid == _sys_getpid())
      return 0;

   return fork_dt.pid;
}

/*-------------------------------------------
| Name:dup
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dup(int fildes){
   dup_t dup_dt;

   dup_dt.fd = fildes;
   __mk_syscall(_SYSCALL_DUP,dup_dt);
   
   return dup_dt.fd;
}

/*-------------------------------------------
| Name:dup2
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dup2(int fildes, int fildes2){
   dup2_t dup2_dt;

   dup2_dt.fd = fildes;
   dup2_dt.fd2 = fildes2;

   __mk_syscall(_SYSCALL_DUP2,dup2_dt);
   
   return dup2_dt.fd;
}

/*-------------------------------------------
| Name:pipe
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int pipe(int fildes[2]){
   pipe_t pipe_dt;
   
   __mk_syscall(_SYSCALL_PIPE,pipe_dt);

   fildes[0]=pipe_dt.fd_r;
   fildes[1]=pipe_dt.fd_w;

   return pipe_dt.ret;
}

/*-------------------------------------------
| Name:_system_exit
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _system_exit(int status){
   kernel_pthread_t* pthread_ptr= kernel_pthread_self();
   pid_t pid = pthread_ptr->pid;
   exit_t exit_dt;
      
   #if ATEXIT_MAX>0
   {
      atexit_func_t* p_atexit_func;
      while( *(p_atexit_func = process_lst[pid]->p_atexit_func++) ){
         (*p_atexit_func)();
      }
   }
   #endif
   
   //mutex stdio release
   if(pthread_ptr->stat&PTHREAD_STATUS_SIGHANDLER){
      //to do lonjmp() will must use this method to release semaphore;
      //release semaphore stdio_sem (instead of use atexit()???;)
      #if !defined(__KERNEL_LOAD_LIB)
         kernel_pthread_mutex_destroy(&stdin->mutex);
         kernel_pthread_mutex_destroy(&stdout->mutex);
         kernel_pthread_mutex_destroy(&stderr->mutex);
      #endif
   }


   exit_dt.pid=pid;
   exit_dt.status = status;

   __mk_syscall(_SYSCALL_EXIT,exit_dt);
   //no return: thread is destroyed
}

/*-------------------------------------------
| Name:_system_atexit
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _system_atexit(void (*func)(void)){

   int pid=_sys_getpid();
   //process_lst[pid].atexit_func = func;
   atexit_t atexit_dt;
   atexit_dt.func=func;
#if defined(USE_SEGGER)
   __mk_syscall(_SYSCALL_ATEXIT,atexit_dt);
#endif
   return 0;
}

/*-------------------------------------------
| Name:rmdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int rmdir(const char *path){
   rmdir_t rmdir_dt;
   rmdir_dt.ref=path;
   __mk_syscall(_SYSCALL_RMDIR,rmdir_dt);
   return rmdir_dt.ret;
}

/*-------------------------------------------
| Name:chdir
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int chdir(const char *path){
   chdir_t chdir_dt;
   chdir_dt.path=path;
   __mk_syscall(_SYSCALL_CHDIR,chdir_dt);
   return chdir_dt.ret;
}

/*--------------------------------------------
| Name:        sysctl
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int _system_sysctl(int *name, int namelen, void *oldp, int *oldlenp, void *newp,int newlen){
   sysctl_t sysctl_dt;

   sysctl_dt.name = name;
   sysctl_dt.namelen = namelen;
   sysctl_dt.oldp=oldp;
   sysctl_dt.oldlenp=oldlenp;
   sysctl_dt.newp=newp;
   sysctl_dt.newlen=newlen;

   __mk_syscall(_SYSCALL_SYSCTL,sysctl_dt);
   return sysctl_dt.ret;
}

/*-------------------------------------------
| Name:_system_system
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _system_system(const char * command){
   pid_t pid;

   //to do: block signal: see posix specification
   //to do: parse command for argv[]

   if(!(pid = vfork())){
      execve("/bin/lsh",0,0);
   }

   if(pid<0)
      return -1;

   //waitpid(pid)

   return 0;
}

/*-------------------------------------------
| Name:_system_malloc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void* _system_malloc(size_t size){
   malloc_t malloc_dt;
   malloc_dt.size = size;
   __mk_syscall(_SYSCALL_MALLOC,malloc_dt);
   return malloc_dt.p;
}

/*-------------------------------------------
| Name:_system_calloc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void *_system_calloc(size_t nelem, size_t elsize){
   calloc_t calloc_dt;
   calloc_dt.nelem = nelem;
   calloc_dt.elsize = elsize;
   __mk_syscall(_SYSCALL_CALLOC,calloc_dt);
   return calloc_dt.p;
}

/*-------------------------------------------
| Name:_system_realloc
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void *_system_realloc(void *p, size_t size){
   realloc_t realloc_dt;
   realloc_dt.p = p;
   realloc_dt.size = size;
   __mk_syscall(_SYSCALL_REALLOC,realloc_dt);
   return realloc_dt.p;
}


/*-------------------------------------------
| Name:_system_free
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _system_free(void* p){
   free_t free_dt;
   free_dt.p = p;
   __mk_syscall(_SYSCALL_FREE,free_dt);
}

/*-------------------------------------------
| Name:_system_getcwd
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
char* _system_getcwd(char *buf, size_t size){
   getcwd_t getcwd_dt;

   if(!buf || !size)
      return 0;

   getcwd_dt.buf = buf;
   getcwd_dt.size = size;
   __mk_syscall(_SYSCALL_GETCWD,getcwd_dt);

   if(getcwd_dt.ret<0)
      return 0;

   return getcwd_dt.buf;
}

/*-------------------------------------------
| Name:_system_setpgid
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t _system_setpgid(pid_t pid, pid_t id_grp){
   setpgid_t setpgid_dt;

   if(pid<0 || id_grp<0)
      return(pid_t)-1;

   if(!pid)
      setpgid_dt.pid = _sys_getpid();

   setpgid_dt.id_grp = id_grp;

   __mk_syscall(_SYSCALL_SETPGID,setpgid_dt);

   return setpgid_dt.id_grp;
}

/*-------------------------------------------
| Name:_system_getpgrp
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t _system_getpgrp(void){
   getpgrp_t getpgrp_dt;

   getpgrp_dt.pid = _sys_getpid();

   __mk_syscall(_SYSCALL_GETPGRP,getpgrp_dt);
   
   return getpgrp_dt.id_grp;
}

/*-------------------------------------------
| Name:_system_getpgrp2
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
pid_t _system_getpgrp2(pid_t pid){
   getpgrp_t getpgrp_dt;

   if(pid<0)
      return (pid_t)-1;

   if(!pid)
      getpgrp_dt.pid = _sys_getpid();
   else 
      getpgrp_dt.pid = pid;

   __mk_syscall(_SYSCALL_GETPGRP,getpgrp_dt);
      
   return getpgrp_dt.id_grp;
}

/*-------------------------------------------
| Name:_system_usleep
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _system_usleep(useconds_t useconds){
   __kernel_usleep(useconds);
   return 0;
}

/*--------------------------------------------
| Name:        _system_alloca
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
void* _system_alloca(size_t size){
   kernel_pthread_t* pthread_ptr= kernel_pthread_self();
   return kernel_pthread_alloca(pthread_ptr,size);
   
}

/*===========================================
End of Sourcesystem.c
=============================================*/
