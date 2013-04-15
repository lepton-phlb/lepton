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
#ifndef _SYS_PTHREAD_H
#define _SYS_PTHREAD_H


/*============================================
| Includes
==============================================*/
#include "kernel/core/kernel_object.h"

/*============================================
| Declaration
==============================================*/

//pthread_mutex
typedef struct {
   struct kernel_object_st* kernel_object; //kernel mutex object
   pthread_mutexattr_t pthread_mutexattr;
}pthread_mutex_t;

#define PTHREAD_MUTEX_UNINITIALIZED  ((unsigned long)0x00000000)
#define PTHREAD_MUTEX_INITIALIZER {(kernel_object_t*)PTHREAD_MUTEX_UNINITIALIZED, \
                                   (pthread_mutexattr_t)0}


//pthread cond
typedef int pthread_condattr_t;

typedef struct pthread_condlist_st {
   struct kernel_pthread_st* pthread_ptr;
   struct pthread_condlist_st* next;
   struct pthread_condlist_st* prev;
}pthread_condlist_t;

typedef struct {
   struct kernel_object_st*   kernel_object; //kernel pthread mutex object
   pthread_condattr_t pthread_condattr;
   //
   struct pthread_condlist_st* list;
   //
   pthread_mutex_t* mutex;

   kernel_sigevent_t kernel_sigevent;
}pthread_cond_t;

#define PTHREAD_COND_UNINITIALIZED  ((unsigned long)0x00000000)
#define PTHREAD_COND_INITIALIZER {(kernel_object_t*)PTHREAD_COND_UNINITIALIZED, \
                                  (pthread_condattr_t)0,(pthread_condlist_t*)0,(pthread_mutex_t*)0}



#endif
