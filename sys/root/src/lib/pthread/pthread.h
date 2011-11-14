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
#ifndef _PTHREAD_H
#define _PTHREAD_H


/*============================================
| Includes 
==============================================*/

#include "kernel/core/sys/pthread.h"

/*============================================
| Declaration  
==============================================*/
#ifdef __cplusplus
extern "C" {
#endif

int         pthread_create (pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine)(void*), void *arg);
int         pthread_cancel (pthread_t thread);
void        pthread_exit   (void *value_ptr);
pthread_t   pthread_self   (void);

int pthread_mutex_init     (pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy  (pthread_mutex_t *mutex);
int pthread_mutex_lock     (pthread_mutex_t *mutex);
int pthread_mutex_trylock  (pthread_mutex_t *mutex);
int pthread_mutex_unlock   (pthread_mutex_t *mutex);

int pthread_cond_init(pthread_cond_t* cond,  const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t* cond);
int pthread_cond_wait(pthread_cond_t* cond,  pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t* cond);
int pthread_cond_broadcast(pthread_cond_t* cond);

#ifdef __cplusplus
}
#endif

#endif
