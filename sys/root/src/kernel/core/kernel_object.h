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


/*============================================
| Compiler Directive
==============================================*/
#ifndef _KERNEL_OBJECT_H
#define _KERNEL_OBJECT_H


/*============================================
| Includes
==============================================*/
#include "kernel/core/kernel_pthread_mutex.h"
#include "kernel/core/kernel_sem.h"
#include "kernel/core/kernel_timer.h"

/*============================================
| Declaration
==============================================*/

typedef struct kernel_object_pthread_mutex_st {
   kernel_pthread_mutex_t kernel_pthread_mutex;
}kernel_object_pthread_mutex_t;

typedef struct kernel_object_sem_st {
   kernel_sem_t kernel_sem;
   int init_count;
}kernel_object_sem_t;

typedef struct kernel_object_timer_st {
   kernel_timer_t kernel_timer;
}kernel_object_timer_t;

typedef union {
   kernel_object_pthread_mutex_t kernel_object_pthread_mutex;
   kernel_object_sem_t kernel_object_sem;
   kernel_object_timer_t kernel_object_timer;
   //kernel_pipe_t  kernel_pipe;
}kernel_object_u;

typedef enum {
   KERNEL_OBJECT_FREE = 0,
   KERNEL_OBJECT_PTRHEAD_MUTEX,
   KERNEL_OBJECT_SEM,
   KERNEL_OBJECT_TIMER,
   KERNEL_OBJECT_PIPE
}kernel_object_type_t;

//
#define KERNEL_OBJECT_SRC_POOL   1
#define KERNEL_OBJECT_SRC_EXTERN 2
typedef int kernel_object_src_t;

//
typedef int kernel_object_id_t;

//
typedef struct kernel_object_st {
   kernel_object_u object;

   kernel_object_id_t id;
   kernel_object_type_t type;
   kernel_object_src_t src;

   struct kernel_object_st* prev;
   struct kernel_object_st* next;
}kernel_object_t;

//
int kernel_object_manager_pool(int kernel_object_no);
kernel_object_t* kernel_object_manager_get(kernel_object_t** pp_kernel_object_head, kernel_object_type_t type, kernel_object_src_t src,...);
kernel_object_t* kernel_object_manager_put(kernel_object_t** pp_kernel_object_head,kernel_object_t* p);
kernel_object_t* kernel_object_manager_put_all(kernel_object_t** pp_kernel_object_head);

#endif
