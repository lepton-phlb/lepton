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


/*============================================
| Compiler Directive   
==============================================*/
#ifndef _SYS_SIGINFO_H
#define _SYS_SIGINFO_H


/*============================================
| Includes 
==============================================*/


/*============================================
| Declaration  
==============================================*/

#define SIGQUEUE_MAX 16

/**
 *
 * nombre maximum de signaux supporter par lepton.
 *
 */
#define NSIG   15

#define SIGRTMIN 32
#define SIGRTMAX 63

#define SI_NONE   0
#define SI_USER   1
#define SI_QUEUE  2
#define SI_MESGQ  3
#define SI_TIMER  4

#define SI_SYSTEM 16

typedef struct sigset_st{ 
   unsigned long std;//standard signal 0 to SIGRTMIN-1
   unsigned long rt;//realtime signal SIGRTMIN to SIGRTMAX
}sigset_t; //must be (at less) 16 bits type (unsigned long 32 bits)

typedef union sigval{
   int   sival_int;
   void  *sival_ptr;
}sigval_t;

typedef struct siginfo{
   int si_signo; //signal number SIGRTMIN(32)<=si_signo<=SIGRTMAX(63)
   int si_errno; //return from rt handler (if it's called)
   int si_code;  //origin of signal SI_USER:kill() or raise(), SI_QUEUE: sigqueue(), SI_MESGQ:message queue posix api, SI_TIMER, realtime timer posix api
   union sigval si_value;
}siginfo_t;

typedef void (*pfn_sigev_notify_function_t)(unsigned sigval) ; //Notification function.
	
typedef struct sigevent{
   int  sigev_notify;   //Notification type. 
   int  sigev_signo;    //Signal number.
   union sigval sigev_value;   //Signal value.
   pfn_sigev_notify_function_t sigev_notify_function; //Notification function. calling thread function if sigev_notify=SIGEV_THREAD.
   struct pthread_attr_st* sigev_notify_attributes; //Notification attributes.
}sigevent_t;

//for si_code == SI_SYSTEM reserved system signo 
#define SIGNO_SYSTEM_PTHREAD_COND   ((int)0x01)

/*
sigev_notify:
SIGEV_NONE
    No asynchronous notification is delivered when the event of interest occurs.
SIGEV_SIGNAL
    A queued signal, with an application-defined value, is generated when the event of interest occurs.
SIGEV_THREAD
    A notification function is called to perform notification. 
*/

#endif
