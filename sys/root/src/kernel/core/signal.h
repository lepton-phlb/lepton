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
#ifndef _SIGNAL_H
#define _SIGNAL_H



/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/timer.h"
#include "kernel/core/sys/siginfo.h"


/*===========================================
Declaration
=============================================*/


#define SIG_DFL -1L

#define SIG_IGN 0L

/*
i
Abnormal termination of the process. 
The process is terminated with all the consequences of _exit() except that the status made available 
to wait() and waitpid() indicates abnormal termination by the specified signal. 

ii
Abnormal termination of the process. Additionally, implementation-dependent abnormal termination actions, 
such as creation of a core file, may occur. 

iii
Ignore the signal. 

iv
Stop the process. 

v
Continue the process, if it is stopped; otherwise ignore the signal. 

*/

/**
 * liste des signaux supports par lepton
 */

/**
 * SIGABRT 
 */
#define SIGABRT   1  //ii  Process abort signal.  

/**
 * SIGALRM 
 */
#define SIGALRM   2  //i  Alarm clock.  

/**
 * SIGFPE
 */
#define SIGFPE    3  //ii  Erroneous arithmetic operation.  NOT IMPLEMENTED IN THIS VERSION

/**
 * SIGHUP 
 */
#define SIGHUP    4  //i  Hangup.  

/**
 * SIGILL 
 */
#define SIGILL    5  //ii  Illegal instruction.             NOT IMPLEMENTED IN THIS VERSION 

/**
 * SIGINT 
 */
#define SIGINT    6  //i  Terminal interrupt signal.  

/**
 * SIGQUIT 
 */
#define SIGQUIT   7  //ii  Terminal quit signal.  

/**
 * SIGPIPE 
 */
#define SIGPIPE   8  //i  Write on a pipe with no one to read it.

/**
 * SIGKILL 
 */
#define SIGKILL   9  //i  Kill (cannot be caught or ignored).  

/**
 * SIGSEGV 
 */
#define SIGSEGV   10 //ii  Invalid memory reference.  

/**
 * SIGTERM 
 */
#define SIGTERM   11 //i  Termination signal.  

/**
 * SIGUSR1 
 */
#define SIGUSR1   12 //i  User-defined signal 1.  

/**
 * SIGUSR2 
 */
#define SIGUSR2   13 //i  User-defined signal 2.  

/**
 * SIGCHLD 
 */
#define SIGCHLD   14 //iii  Child process terminated or stopped. 

//not use 
#define SIGCONT   15 //v  Continue executing, if stopped. // to do: must be implemented 
#define SIGSTOP   16 //iv  Stop executing (cannot be caught or ignored).  // to do: must be implemented 

#define SIGTSTP   17 //iv  Terminal stop signal.  
#define SIGTTIN   18 //iv  Background process attempting read.  
#define SIGTTOU   19 //iv  Background process attempting write.  
#define SIGBUS    20 //ii  Access to an undefined portion of a memory object.  NOT IMPLEMENTED IN THIS VERSION
#define SIGPOLL   21 //i  Pollable event.  
#define SIGPROF   22 //i   Profiling timer expired.  NOT IMPLEMENTED IN THIS VERSION
#define SIGSYS    23 //ii  Bad system call.  
#define SIGTRAP   24 //ii  Trace/breakpoint trap.  NOT IMPLEMENTED IN THIS VERSION
#define SIGURG    25 //iii  High bandwidth data is available at a socket.  NOT IMPLEMENTED IN THIS VERSION
#define SIGVTALRM 26 //i  Virtual timer expired.  NOT IMPLEMENTED IN THIS VERSION
#define SIGXCPU   27 //ii  CPU time limit exceeded.  
#define SIGXFSZ   28 //ii  File size limit exceeded.  

typedef void (*__sa_handler)(int);
typedef __sa_handler sa_handler_t;

typedef void (*__sa_sigaction)(int,siginfo_t *, void *);
typedef __sa_sigaction sa_sigaction_t;


#define  SA_ONSTACK	   0x0001	/* take signal on signal stack: not yet supported on lepton */
#define  SA_RESTART	   0x0002	/* restart system call on signal return : not yet supported on lepton */
#define	SA_RESETHAND	0x0004	/* reset to SIG_DFL when taking signal : not yet supported on lepton */
#define	SA_NODEFER	   0x0010	/* don't mask the signal we're delivering : not yet supported on lepton */
#define	SA_NOCLDWAIT	0x0020	/* don't keep zombies around : not yet supported on lepton */
#define	SA_SIGINFO	   0x0040	/* signal handler with SA_SIGINFO args */


/**
 *
 * sighandler must be use the following declaration  void (*__sa_handler)(int);
 *
 */
struct sigaction{
   sa_handler_t sa_handler;
   sigset_t sa_mask; /* not yet used, preserve POSIX compatibility*/
   int sa_flags;     /* not yet used, preserve POSIX compatibility*/
   sa_sigaction_t sa_sigaction; //Pointer to a signal-catching function
};


#define SIG_SETMASK     0
#define SIG_BLOCK       1
#define SIG_UNBLOCK     2

#ifdef __cplusplus
extern "C" {
#endif

//sigset manipulation routine
int sigemptyset(sigset_t* set);  //unblock all signal
int sigfillset(sigset_t* set);   //block all signal
int sigaddset(sigset_t* set,int sig); //block sig
int sigdelset(sigset_t* set,int sig); //unblock sig
int sigismember(sigset_t* set,int sig);//is blocked?

//signal syscall
int sigprocmask(int how,sigset_t* set,sigset_t* oset);
int sigpending(sigset_t* set);
void (*signal(int sig, void (*func)(int)))(int);
int sigaction(int sig,struct sigaction* act,struct sigaction* oact);
int kill(pid_t pid, int sig);
int raise(int sig);
int pthread_kill(pthread_t pthread, int sig);
//realtime signal. posix 1003.1 realtime signal extension
int sigwaitinfo(sigset_t* set,struct siginfo* info);
int sigtimedwait(sigset_t *set, siginfo_t *info,const struct timespec *timeout);

//generic signal handler
void sighandler(void);

#ifdef __cplusplus
}
#endif

#ifdef __KERNEL_POSIX_REALTIME_SIGNALS
   #define NSIGMAX  SIGRTMAX
#else
   #define NSIGMAX  NSIG
#endif

extern struct sigaction const sigaction_dfl_lst[NSIGMAX+1];

#endif
