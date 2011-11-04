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

/*===========================================
Includes
=============================================*/
#include "kernel/core/signal.h"
#include "kernel/core/process.h"
#include "kernel/core/syscall.h"
#include "kernel/core/libstd.h"

//#include "lib/libc/unistd/unistd.h"
/*===========================================
Global Declaration
=============================================*/
#if defined(USE_ECOS)
	#include <cyg/hal/hal_io.h>
#endif

#if defined(GNU_GCC)
extern void _system_exit(int status);
#endif

/*===========================================
Implementation
=============================================*/
struct sigaction const sigaction_dfl_lst[NSIGMAX+1]={
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // 0 no used 0
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGABRT   1  //ii  Process abort signal.
      {(sa_handler_t)SIG_DFL,{0,0},0,(sa_sigaction_t)0},   // SIGALRM   2  //i  Alarm clock.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGFPE    3  //ii  Erroneous arithmetic operation.  NOT IMPLEMENTED IN THIS VERSION
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGHUP    4  //i  Hangup.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGILL    5  //ii  Illegal instruction.             NOT IMPLEMENTED IN THIS VERSION
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGINT    6  //i  Terminal interrupt signal.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGQUIT   7  //ii  Terminal quit signal.
      {(sa_handler_t)SIG_DFL,{0,0},0,(sa_sigaction_t)0},   // SIGPIPE   8  //i  Write on a pipe with no one to read it.
      {(sa_handler_t)SIG_DFL,{0,0},0,(sa_sigaction_t)0},   // SIGKILL   9  //i  Kill (cannot be caught or ignored).
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGSEGV   10 //ii  Invalid memory reference.
      {(sa_handler_t)SIG_DFL,{0,0},0,(sa_sigaction_t)0},   // SIGTERM   11 //i  Termination signal.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGUSR1   12 //i  User-defined signal 1.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGUSR2   13 //i  User-defined signal 2.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0}    // SIGCHLD   14 //iii  Child process terminated or stopped.
   #ifdef __KERNEL_POSIX_REALTIME_SIGNALS
     ,{(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGCONT   15 //v  Continue executing, if stopped.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGSTOP   16 //iv  Stop executing (cannot be caught or ignored).
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGTSTP   17 //iv  Terminal stop signal.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGTTIN   18 //iv  Background process attempting read.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGTTOU   19 //iv  Background process attempting write.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGBUS    20   //ii  Access to an undefined portion of a memory object.  NOT IMPLEMENTED IN THIS VERSION
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGPOLL   21 //i  Pollable event.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGPROF   22   //i   Profiling timer expired.  NOT IMPLEMENTED IN THIS VERSION
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGSYS    23 //ii  Bad system call.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGTRAP   24 //ii  Trace/breakpoint trap.  NOT IMPLEMENTED IN THIS VERSION
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGURG    25 //iii  High bandwidth data is available at a socket.  NOT IMPLEMENTED IN THIS VERSION
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGVTALRM 26 //i  Virtual timer expired.  NOT IMPLEMENTED IN THIS VERSION
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGXCPU   27 //ii  CPU time limit exceeded.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   // SIGXFSZ   28 //ii  File size limit exceeded.
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           29 //not used
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           30 //not used
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           31 //not used

      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           32 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           33 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           34 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           35 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           36 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           37 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           38 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           39 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           40 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           41 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           42 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           43 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           44 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           45 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           46 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           47 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           48 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           49 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           50 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           51 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           52 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           53 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           54 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           55 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           56 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           57 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           58 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           59 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           60 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           61 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           62 //realtime signal
      {(sa_handler_t)SIG_IGN,{0,0},0,(sa_sigaction_t)0},   //           63 //realtime signal
   #endif
};

/*-------------------------------------------
| Name:sighandler
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void sighandler(void){
   kernel_pthread_t* pthread_ptr = kernel_pthread_self();
   int sig;

   sig=pthread_ptr->siginfo.si_signo;

   if((unsigned int)(pthread_ptr->sigaction_lst[sig].sa_handler)!=(unsigned int)SIG_DFL){
      pthread_ptr->sigaction_lst[sig].sa_handler(sig);
      __mk_syscall2(_SYSCALL_SIGEXIT);//return to main 
   }else{ //to do: call default handler
      switch(sig){
         case SIGTERM:
         case SIGALRM:
         case SIGKILL:
         case SIGPIPE:{
            _system_exit(0);//exit process
         }
         break;
         //to do: support for SIGSTOP(suspend process) and SIGCONT(resume process)
         //       see _sys_kill in process.c
      }
   }
}

/*-------------------------------------------
| Name:sigemptyset
| Description:
| Parameters:
| Return Type:
| Comments:unblock all signal
| See:
---------------------------------------------*/
int sigemptyset(sigset_t* set){
   set->std=0;
   set->rt=0;
  return 0;
}

/*-------------------------------------------
| Name:sigfillset
| Description:
| Parameters:
| Return Type:
| Comments:block all signal
| See:
---------------------------------------------*/
int sigfillset(sigset_t* set){
   set->std=0xffffffff;
   set->rt=0xffffffff;
   return 0;
}

/*-------------------------------------------
| Name:sigaddset
| Description:
| Parameters:
| Return Type:
| Comments: block sig
| See:
---------------------------------------------*/
int sigaddset(sigset_t* set,int sig){
   unsigned long msk = ( (sig<SIGRTMIN) ? (1 << (sig-1)) : (1 << (sig-SIGRTMIN)) );
   if(sig<SIGRTMIN)
      set->std|=msk;
   else
      set->rt|=msk;
   return 0;
}

/*-------------------------------------------
| Name:sigdelset
| Description:
| Parameters:
| Return Type:
| Comments: unblock sig
| See:
---------------------------------------------*/
int sigdelset(sigset_t* set,int sig){
   unsigned long msk = ( (sig<SIGRTMIN) ? (1 << (sig-1)) : (1 << (sig-SIGRTMIN)) );
   if(sig<SIGRTMIN)
      set->std&=~msk;
   else
      set->rt&=~msk;
   return 0;
}

/*-------------------------------------------
| Name:sigismember
| Description:
| Parameters:
| Return Type:
| Comments: is blocked?
| See:
---------------------------------------------*/
int sigismember(sigset_t* set,int sig){
   if(sig<SIGRTMIN)
      return (((set->std)>>(sig-1))&0x01);
   return (((set->rt)>>(sig-SIGRTMIN))&0x01);
}

/*--------------------------------------------
| Name:        signal
| Description:
| Parameters:  none
| Return Type: none
| Comments: only for compatibility (deprecated api use sigaction instead of).
| See:
----------------------------------------------*/
void (*signal(int sig, void (*func)(int)))(int){
   struct sigaction act;
   struct sigaction oact;
   act.sa_handler = func;
   if(sigaction(sig,&act,&oact)<0)
      return (void*)0;
   return oact.sa_handler;
}

/*-------------------------------------------
| Name:sigprocmask
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int sigprocmask(int how,sigset_t* set,sigset_t* oset){

   sigprocmask_t sigprocmask_dt;

   sigprocmask_dt.how   = how;
   sigprocmask_dt.set   = set;
   sigprocmask_dt.oset  = oset;

   __mk_syscall(_SYSCALL_SIGPROCMASK,sigprocmask_dt);

   return sigprocmask_dt.ret;
}

/*-------------------------------------------
| Name:sigpending
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int sigpending(sigset_t* set){

   sigpending_t sigpending_dt;

   sigpending_dt.set = set;

   __mk_syscall(_SYSCALL_SIGPENDING,sigpending_dt);

   return sigpending_dt.ret;
}

/*-------------------------------------------
| Name:sigaction
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int sigaction(int sig,struct sigaction* act,struct sigaction* oact){

   sigaction_t sigaction_dt;

   sigaction_dt.sig  = sig;
   sigaction_dt.act  = act;
   sigaction_dt.oact = oact;

   __mk_syscall(_SYSCALL_SIGACTION,sigaction_dt);

   return sigaction_dt.ret;
}

/*-------------------------------------------
| Name:kill
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int kill(pid_t pid, int sig){

   kill_t kill_dt;

   kill_dt.pid = pid;
   kill_dt.sig = sig;
   kill_dt.atomic = 1;

   if(pid==_sys_getpid()){
      __mk_syscall0(_SYSCALL_KILL,kill_dt);
   }else{
      __mk_syscall(_SYSCALL_KILL,kill_dt);
   }

   return kill_dt.ret;
}

/*-------------------------------------------
| Name:raise
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int raise(int sig){

   kill_t kill_dt;

   kill_dt.pid = _sys_getpid();
   kill_dt.sig = sig;
   kill_dt.atomic = 1;

   __mk_syscall0(_SYSCALL_KILL,kill_dt);

   return kill_dt.ret;
}


/*-------------------------------------------
| Name:kill
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int pthread_kill(pthread_t pthread, int sig){

   pthread_kill_t pthread_kill_dt;

   pthread_kill_dt.kernel_pthread = (kernel_pthread_t*)pthread;
   pthread_kill_dt.sig = sig;
   pthread_kill_dt.atomic = 1;

#if defined(USE_SEGGER)
   if(((kernel_pthread_t*)pthread)==kernel_pthread_self()){
      __mk_syscall0(_SYSCALL_PTHREAD_KILL,pthread_kill_dt);
   }else{
      __mk_syscall(_SYSCALL_PTHREAD_KILL,pthread_kill_dt);
   }
#endif

   return pthread_kill_dt.ret;
}


/*--------------------------------------------
| Name:        sigwaitinfo
| Description: 
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int sigwaitinfo(sigset_t* set,struct siginfo* info){

   kernel_sigevent_t kernel_sigevent;
   kernel_pthread_t* pthread_ptr = kernel_pthread_self();

   //1) no sigevent filter on si_code
   kernel_sigevent.si_code=SI_NONE;
   //2)set filter on sigset
   kernel_sigevent.set=set;
   //3) wait on thread sigqueue
   while(pthread_ptr->kernel_sigqueue.wait(&kernel_sigevent)<0);
   //4) fill singinfo structure with kernel sigevent
   info->si_code  =   kernel_sigevent.si_code;
   info->si_signo =   kernel_sigevent._sigevent.sigev_signo;
   info->si_value =   kernel_sigevent._sigevent.sigev_value;
   //to do must be implemented but more late
   info->si_errno =   0;

   return 0;
}

/*--------------------------------------------
| Name:        sigtimedwait
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int sigtimedwait(sigset_t *set, siginfo_t *info,const struct timespec *timeout){
   
   kernel_sigevent_t kernel_sigevent;
   kernel_pthread_t* pthread_ptr = kernel_pthread_self();

   //1) no sigevent filter on si_code
   kernel_sigevent.si_code=SI_NONE;
   //2)set filter on sigset
   kernel_sigevent.set=set;
   //3) wait on thread sigqueue until signal was raised or timeout reach.
   if(pthread_ptr->kernel_sigqueue.timedwait(&kernel_sigevent,0,timeout)<0)
      return -1;//timeout
   //4) fill singinfo structure with kernel sigevent
   info->si_code  =   kernel_sigevent.si_code;
   info->si_signo =   kernel_sigevent._sigevent.sigev_signo;
   info->si_value =   kernel_sigevent._sigevent.sigev_value;
   //to do must be implemented but more late
   info->si_errno =   0;

   return 0;
}

/*===========================================
| End of Source : signal.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.2  2009/04/22 11:07:56  jjp
| add dummy support for realtime sigqueue event for eCos/lepton
|
| Revision 1.1  2009/03/30 15:48:57  jjp
| first import of tauon
|
| Revision 1.1  2009/03/30 11:18:54  jjp
| First import of tauon
|
| Revision 1.1.1.1  2009/02/05 15:29:49  jjp
| First import in CVS
|
|---------------------------------------------
=============================================*/
