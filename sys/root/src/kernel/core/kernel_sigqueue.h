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


/*============================================
| Compiler Directive
==============================================*/
#ifndef _KERNEL_SIGQUEUE_H
#define _KERNEL_SIGQUEUE_H


/*============================================
| Includes
==============================================*/
#include "kernel/core/sys/siginfo.h"
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/kernel_object.h"
/*============================================
| Declaration
==============================================*/

#ifdef __KERNEL_POSIX_REALTIME_SIGNALS

   #define KERNEL_SIGQUEUE_MAX   SIGQUEUE_MAX


typedef struct kernel_sigevent_st {
   sigevent_t _sigevent;
   int si_code;  //origin of signal SI_USER:kill() or raise(), SI_QUEUE: sigqueue(), SI_MESGQ:message queue posix api, SI_TIMER, realtime timer posix api
   int pos; //position in fifo_queue;
   unsigned long from; //sigevent source (address kernel object)
   unsigned long counter;
   sigset_t* set; //filter for sigwaitinfo
}kernel_sigevent_t;

//try
   #if defined(__GNUC__)
typedef struct kernel_sigqueue_st kernel_sigqueue_st;
typedef struct kernel_pthread_st kernel_pthread_st;
typedef struct kernel_object_st kernel_object_st;
   #endif

typedef int (*pfn_kernel_sigqueue_constructor_t)(struct kernel_object_st** pp_kernel_object_head,
                                                 struct kernel_sigqueue_st* p);
typedef int (*pfn_kernel_sigqueue_destructor_t)(struct kernel_sigqueue_st* p);
typedef int (*pfn_kernel_sigqueue_send_t)(struct kernel_pthread_st* kernel_pthread,
                                          struct kernel_sigevent_st* kernel_sigevent);
typedef int (*pfn_kernel_sigqueue_wait_t)(struct kernel_sigevent_st* kernel_sigevent);
typedef int (*pfn_kernel_sigqueue_timedwait_t)(struct kernel_sigevent_st* kernel_sigevent, int flag,
                                               const struct timespec * timeout);


typedef struct kernel_sigqueue_st {
   struct kernel_sigqueue_st* self;
   //
   pfn_kernel_sigqueue_constructor_t constructor;
   pfn_kernel_sigqueue_destructor_t destructor;
   pfn_kernel_sigqueue_send_t send;
   pfn_kernel_sigqueue_wait_t wait;
   pfn_kernel_sigqueue_timedwait_t timedwait;

   //
   struct kernel_object_st** kernel_object_head;
   struct kernel_object_st* kernel_sem;
   struct kernel_object_st* kernel_mutex;
   //
   kernel_sigevent_t sigqueue[KERNEL_SIGQUEUE_MAX];
}kernel_sigqueue_t;

extern const kernel_sigqueue_t _kernel_sigqueue_initializer;

   #define KERNEL_SIGQUEUE_INITIALIZER {(kernel_sigqueue_t*)&_kernel_sigqueue_initializer, \
                                        (pfn_kernel_sigqueue_constructor_t) \
                                        kernel_sigqueue_constructor, \
                                        (pfn_kernel_sigqueue_destructor_t) \
                                        kernel_sigqueue_destructor, \
                                        (pfn_kernel_sigqueue_send_t)kernel_sigqueue_send, \
                                        (pfn_kernel_sigqueue_wait_t)kernel_sigqueue_wait, \
                                        (pfn_kernel_sigqueue_timedwait_t)kernel_sigqueue_timedwait, \
                                        (struct kernel_object_st**)0, \
                                        (struct kernel_object_st*)0, \
                                        (struct kernel_object_st*)0, \
                                        {0} \
}


#endif
#endif
