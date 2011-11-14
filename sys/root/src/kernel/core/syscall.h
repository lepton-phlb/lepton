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
#ifndef _SYSCALL_H
#define _SYSCALL_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"
#include "kernel/core/signal.h"
#include "kernel/core/kernel.h"
#include "kernel/core/kernel_object.h"
#include "kernel/core/sys/pthread.h"
#include "kernel/core/sys/semaphore.h"
#include "kernel/core/process.h"
#include "kernel/core/systime.h"
#include "kernel/core/fcntl.h"

#include "kernel/core/malloc.h"

/*===========================================
Declaration
=============================================*/

int _syscall_waitpid       (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_execve        (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_exit          (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_atexit        (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_kill          (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_vfork         (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_sigprocmask   (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_sigpending    (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_sigaction     (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_alarm         (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_pause         (kernel_pthread_t* pthread_ptr, pid_t pid, void* data); 
int _syscall_dup           (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_dup2          (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_pipe          (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_sigexit       (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_gettimeofday  (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_settimeofday  (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_malloc        (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_calloc        (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);//not yet implemented
int _syscall_realloc       (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);//not yet implemented
int _syscall_free          (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_fcntl         (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_ioctl         (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_sysctl        (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_setpgid       (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_getpgrp       (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);

int _syscall_pthread_create (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_pthread_cancel (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_pthread_exit   (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);

int _syscall_pthread_kill   (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);

int _syscall_pthread_mutex_init     (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_pthread_mutex_destroy  (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);

int _syscall_pthread_cond_init      (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_pthread_cond_destroy   (kernel_pthread_t* pthread_ptr, pid_t pid, void* data);

int _syscall_timer_create(kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_timer_delete(kernel_pthread_t* pthread_ptr, pid_t pid, void* data);

int _syscall_sem_init(kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
int _syscall_sem_destroy(kernel_pthread_t* pthread_ptr, pid_t pid, void* data);

//exeve syscall parameter
typedef struct{
   const char* path;
   const char** argv;
   const char* envp;
   pid_t ppid;
   pid_t pid;//return value
}execve_t;

//waitpid syscall parameter
typedef struct{
   pid_t pid; //input parameter and return value
   int   options;
   int   status;//return value
   int   ret;//return value
}waitpid_t;

//fork syscall parameter
typedef struct{
   pid_t ppid; //input parameter
   pid_t pid; //return value
}fork_t;

//exit process  syscall parameter
typedef struct{
   pid_t pid; //input parameter
   int status;
}exit_t;

//exit process function syscall parameter
typedef struct{
   void (*func)(void);
   int ret;
}atexit_t;

//
typedef struct{
   int how;//input parameter
   sigset_t* set;//input parameter
   sigset_t* oset;//input and return parameter
   int ret; //return value
}sigprocmask_t;

//
typedef struct{
   sigset_t* set;//input parameter
   int ret; //return value
}sigpending_t;

//
typedef struct{
   int sig;//input parameter
   struct sigaction* act;//input parameter
   struct sigaction* oact;//input and return parameter
   int ret; //return value
}sigaction_t;

//
typedef struct{
   unsigned int seconds;//input output parameter
}alarm_t;

//
typedef struct{
   pid_t pid;//input parameter
   int sig;//input parameter
   int atomic;//input parameter;
   int ret; //return value
}kill_t;

//
typedef struct{
   int fd;//input and return parameter
}dup_t;

//
typedef struct{
   int fd;//input and return parameter
   int fd2;//input parameter
}dup2_t;

//
typedef struct{
   int fd_r; //return value
   int fd_w; //return value
   int ret; //return value
}pipe_t;

typedef struct{
    struct __timeval *tv;//input output parameter
    struct timezone *tz;//input output parameter
    int ret; //return value
}gettimeofday_t,settimeofday_t;

typedef struct{
   size_t size;//input parameter
   void *p;//return value
}malloc_t;

typedef struct{
   size_t nelem;//input parameter
   size_t elsize;//input parameter
   void *p;//return value
}calloc_t;

typedef struct{
   size_t size;//input parameter
   void *p;//return value
}realloc_t;

typedef struct{
   void *p;//input parameter
}free_t;

typedef struct{
   int fd;//input parameter
   int cmd;//input parameter
   int argc;//input parameter
   void *argv[FCNTL_ARG_MAX];//input parameter
   int ret;
}fcntl_t;

typedef struct{
   int fd;//input parameter
   int request;//input parameter
   va_list ap;//input parameter
   int ret;
}ioctl_t;

typedef struct{
   int *name;//input parameter
   int namelen;//input parameter
   void *oldp;//input/output parameter
   int *oldlenp;//input/output parameter
   void *newp;//input parameter
   int newlen;//input parameter
   int ret;
}sysctl_t;

typedef struct{
   pid_t pid;//input parameter
   pid_t id_grp;////input output parameter
}setpgid_t;

typedef struct{
   pid_t pid;//input parameter
   pid_t id_grp;//input parameter
}getpgrp_t;

typedef struct{
   kernel_pthread_t* kernel_pthread;//output parameter
   pthread_attr_t *attr;//input parameter
   void *(*start_routine)(void*);//input parameter
   void *arg;//input parameter
   int ret; //return value
}pthread_create_t;

typedef struct{
   kernel_pthread_t* kernel_pthread;//input parameter
   int ret; //return value
}pthread_cancel_t;

typedef struct{
   kernel_pthread_t* kernel_pthread;//input parameter
   void* value_ptr;//input parameter
}pthread_exit_t;

typedef struct{
   kernel_pthread_t* kernel_pthread;//input parameter
   int sig;//input parameter
   int atomic;//input parameter;
   int ret; //return value
}pthread_kill_t;

typedef struct{
   pthread_mutex_t* mutex;//input output parameter
   int ret;//output parameter
}pthread_mutex_init_t,pthread_mutex_destroy_t;

typedef struct{
   pthread_cond_t*  cond;//input output parameter
   int ret;//output parameter
}pthread_cond_init_t,pthread_cond_destroy_t,pthread_cond_signal_t;

typedef struct{
   clockid_t   clockid;//input parameter   
   struct sigevent* psigevent;//input parameter
   timer_t* ptimerid;//output parameter
   int ret;//output parameter
}timer_create_t;

typedef struct{
   timer_t* ptimerid;//input parameter
   int ret;//output parameter
}timer_delete_t;

typedef struct{
   char* name;//input parameter
   sem_t* psem;//input output parameter
   int pshared;//input parameter
   unsigned int value;//input parameter
   int ret;//output parameter
}sem_init_t;

typedef struct{
   sem_t* psem;//input parameter
   int ret;//output parameter
}sem_destroy_t;

//specific for network interface and ip stack (see also kernel/kernel.h and kernel/interrupt.h)
typedef struct{
   desc_t desc;//input parameter
}netsend_t;

#endif
