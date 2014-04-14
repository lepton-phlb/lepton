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
#ifndef _KERNEL_PTHREAD_MUTEX_H
#define _KERNEL_PTHREAD_MUTEX_H


/*============================================
| Includes
==============================================*/

#include "kernel/core/kal.h"
//#include "kernel/core/kernel_pthread.h"

#ifdef __KERNEL_UCORE_ECOS
   #include <cyg/kernel/kapi.h>
#endif
/*============================================
| Declaration
==============================================*/
#define PTHREAD_MUTEX_DEFAULT 0
#define PTHREAD_MUTEX_NORMAL  1


//
typedef struct {
#ifdef __KERNEL_UCORE_EMBOS
   OS_RSEMA mutex;
#endif
#ifdef __KERNEL_UCORE_FREERTOS
   xSemaphoreHandle mutex;
#endif
#ifdef __KERNEL_UCORE_ECOS
   cyg_mutex_t mutex;
#endif
}kernel_pthread_mutex_t;

typedef int pthread_mutexattr_t;

int   kernel_pthread_mutex_init     (kernel_pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int   kernel_pthread_mutex_destroy  (kernel_pthread_mutex_t *mutex); //pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int   kernel_pthread_mutex_lock     (kernel_pthread_mutex_t *mutex);
int   kernel_pthread_mutex_trylock  (kernel_pthread_mutex_t *mutex);
int   kernel_pthread_mutex_unlock   (kernel_pthread_mutex_t *mutex);

//int kernel_pthread_mutex_owner_destroy(struct kernel_pthread_st *thread_ptr, kernel_pthread_mutex_t *mutex);

#endif
