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
| Includes
==============================================*/
#include <stdarg.h>
#include <string.h>

#include "kernel/core/errno.h"
#include "kernel/core/kernel_object.h"
#include "kernel/core/malloc.h"


#if defined(__GNUC__)
   #include <stdlib.h>
#endif
/*============================================
| Global Declaration
==============================================*/
#define va_strt      va_start

#define __KERNEL_OBJECT_ID_LIMIT 512 //2^N

static unsigned char kernel_object_id_vector[(__KERNEL_OBJECT_ID_LIMIT/8)+1]={0};

typedef int (*pfn_kernel_object_constructor_t)(kernel_object_t* p, va_list ap);
typedef int (*pfn_kernel_object_destructor_t)(kernel_object_t* p);

static kernel_object_t*  kernel_object_pool_head = (kernel_object_t*)0;

typedef struct kernel_object_op_st {
   pfn_kernel_object_constructor_t kernel_object_constructor;
   pfn_kernel_object_destructor_t kernel_object_destructor;
}kernel_object_op_t;

//
int kernel_object_constructor_null(kernel_object_t* p,va_list ap);
int kernel_object_destructor_null(kernel_object_t* p);

int kernel_object_constructor_pthread_mutex(kernel_object_t* p,va_list ap);
int kernel_object_destructor_pthread_mutex(kernel_object_t* p);

int kernel_object_constructor_sem(kernel_object_t* p,va_list ap);
int kernel_object_destructor_sem(kernel_object_t* p);

int kernel_object_constructor_timer(kernel_object_t* p,va_list ap);
int kernel_object_destructor_timer(kernel_object_t* p);



kernel_object_op_t kernel_object_op[]={
   {kernel_object_constructor_null,kernel_object_destructor_null},                     //KERNEL_OBJECT_FREE
   {kernel_object_constructor_pthread_mutex,kernel_object_destructor_pthread_mutex},   //KERNEL_OBJECT_PTRHEAD_MUTEX
   {kernel_object_constructor_sem,kernel_object_destructor_sem},                       //KERNEL_OBJECT_SEM
   {kernel_object_constructor_timer,kernel_object_destructor_timer},                   //KERNEL_OBJECT_TIMER
   {kernel_object_constructor_null,kernel_object_destructor_null}                      //KERNEL_OBJECT_PIPE
};

/*============================================
| Implementation
==============================================*/

/*--------------------------------------------
| Name:        kernel_object_constructor_null
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_object_constructor_null(kernel_object_t* p,va_list ap){
   return 0;
}

/*--------------------------------------------
| Name:        kernel_object_destructor_null
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_object_destructor_null(kernel_object_t* p){
   return 0;
}

/*--------------------------------------------
| Name:        kernel_object_constructor_pthread_mutex
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_object_constructor_pthread_mutex(kernel_object_t* p,va_list ap){

   pthread_mutexattr_t mutex_attr=0;

   if(kernel_pthread_mutex_init(&p->object.kernel_object_pthread_mutex.kernel_pthread_mutex,
                                &mutex_attr)<0)
      return -1;

   p->type = KERNEL_OBJECT_PTRHEAD_MUTEX;

   return 0;

}

/*--------------------------------------------
| Name:        kernel_object_destructor_pthread_mutex
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_object_destructor_pthread_mutex(kernel_object_t* p){

   if(kernel_pthread_mutex_destroy(&p->object.kernel_object_pthread_mutex.kernel_pthread_mutex)<0)
      return -1;

#ifndef CPU_M16C62
   p->type = KERNEL_OBJECT_FREE; //could be reused for any kernel object type
   memset(&p->object.kernel_object_pthread_mutex.kernel_pthread_mutex,0,
          sizeof(kernel_pthread_mutex_t));
#endif

   return 0;
}

/*--------------------------------------------
| Name:        kernel_object_constructor_sem
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_object_constructor_sem(kernel_object_t* p,va_list ap){
   int init_count=-1;
   //
   init_count = va_arg( ap, int);
   //
   p->object.kernel_object_sem.init_count = init_count;
   //
   if(kernel_sem_init(&p->object.kernel_object_sem.kernel_sem,0,
                      p->object.kernel_object_sem.init_count)<0)
      return -1;
   //
   p->type = KERNEL_OBJECT_SEM;
   return 0;
}

/*--------------------------------------------
| Name:        kernel_object_destructor_sem
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_object_destructor_sem(kernel_object_t* p){
   if(kernel_sem_destroy(&p->object.kernel_object_sem.kernel_sem)<0)
      return -1;
   //
   memset(&p->object.kernel_object_sem.kernel_sem,0,sizeof(kernel_sem_t));
   //
   p->type = KERNEL_OBJECT_FREE; //could be reused for any kernel object type
   return 0;
}

/*--------------------------------------------
| Name:        kernel_object_constructor_timer
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_object_constructor_timer(kernel_object_t* p,va_list ap){

   clockid_t clockid;
   struct sigevent* psigevent;
   timer_t*    ptimerid;

#if defined(__GNUC__)
   clockid= va_arg(ap, unsigned int);
#else
   clockid= va_arg(ap, clockid_t);
#endif
   psigevent= va_arg(ap, struct sigevent*);
   ptimerid= va_arg(ap, timer_t*);

   if(kernel_timer_create(clockid,
                          psigevent,
                          &p->object.kernel_object_timer.kernel_timer)<0)
      return -1;

   *ptimerid=(timer_t)&p->object.kernel_object_timer.kernel_timer;

   p->type = KERNEL_OBJECT_TIMER;
   return 0;
}

/*--------------------------------------------
| Name:        kernel_object_destructor_timer
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_object_destructor_timer(kernel_object_t* p){
   if(kernel_timer_delete(&p->object.kernel_object_timer.kernel_timer)<0)
      return -1;
   p->type = KERNEL_OBJECT_FREE; //could be reused for any kernel object type
   return 0;
}

/*--------------------------------------------
| Name:        kernel_object_get_id
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
kernel_object_id_t kernel_object_get_id(void){
   int byte;
   unsigned char bits;

   for(byte=0; byte<((__KERNEL_OBJECT_ID_LIMIT)/8); byte++) {
      if(kernel_object_id_vector[byte]==0x00) continue;

      for(bits=0; bits<8; bits++) {
         unsigned char msk=(0x01<<bits);
         if(!(kernel_object_id_vector[byte]&msk) )
            continue;
         kernel_object_id_vector[byte]&=(~msk);
         return (kernel_object_id_t)((byte<<3)+bits);
      }
   }

   return -1;
}

/*--------------------------------------------
| Name:        kernel_object_put_id
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void kernel_object_put_id(kernel_object_id_t id){
   int byte = id>>3;
   int bits = id-(byte<<8);
   unsigned char msk=(0x01<<bits);

   if(id<0)
      return;

   kernel_object_id_vector[byte]|=msk;
}

/*--------------------------------------------
| Name:        kernel_object_alloc
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
kernel_object_t* kernel_object_alloc(kernel_object_type_t type){
   kernel_object_t* p;

   switch(type) {
   case KERNEL_OBJECT_FREE:
   case KERNEL_OBJECT_PTRHEAD_MUTEX:
   case KERNEL_OBJECT_SEM:
   case KERNEL_OBJECT_TIMER:
   case KERNEL_OBJECT_PIPE:
      if( !(p = (kernel_object_t*)malloc(sizeof(kernel_object_t))) )
         return (kernel_object_t*)0;
      break;

   default:
      return (kernel_object_t*)0;
   }

   memset(p,0,sizeof(kernel_object_t));

   p->type = type;
   p->src  = KERNEL_OBJECT_SRC_POOL;
   p->next = (kernel_object_t*)0;
   p->prev = (kernel_object_t*)0;

   return p;
}

/*--------------------------------------------
| Name:        kernel_object_free
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_object_free(kernel_object_t* p){
   if(!p)
      return -1;
   free(p);
   return 0;
}

/*--------------------------------------------
| Name:        kernel_object_insert
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
kernel_object_t* kernel_object_insert(kernel_object_t* kernel_object_head, kernel_object_t* p){
   if(!p)
      return (kernel_object_t*)0;
   //
   p->next = (kernel_object_t*)0;
   p->prev = (kernel_object_t*)0;
   //chain list
   p->next=kernel_object_head;
   kernel_object_head=p;
   if(!p->next)
      return kernel_object_head;

   p->next->prev = p;

   return kernel_object_head;
}

/*--------------------------------------------
| Name:        kernel_object_remove
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
kernel_object_t* kernel_object_remove(kernel_object_t* kernel_object_head, kernel_object_t* p){
   if(!p)
      return (kernel_object_t*)0;
   if(p->prev)
      p->prev->next = p->next;
   else
      kernel_object_head = p->next;

   if(p->next)
      p->next->prev = p->prev;

   return kernel_object_head;
}

/*--------------------------------------------
| Name:        kernel_object_pool_put
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
kernel_object_t* kernel_object_pool_put(kernel_object_t* p){
   //insert kernel object pool list
   return (kernel_object_pool_head=kernel_object_insert(kernel_object_pool_head,p));
}

/*--------------------------------------------
| Name:        kernel_object_pool_get
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
kernel_object_t* kernel_object_pool_get(kernel_object_type_t type){
   kernel_object_t* p = kernel_object_pool_head;

   while(p) {
      if(p->type==KERNEL_OBJECT_FREE || p->type==type) {
         //remove from kernel object pool list
         kernel_object_pool_head=kernel_object_remove(kernel_object_pool_head,p);
         return p;
      }
      p=p->next;
   }

   return (kernel_object_t*)0;
}

/*--------------------------------------------
| Name:        kernel_object_manager
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int kernel_object_manager_pool(int kernel_object_no){
   kernel_object_t* p;

   memset(kernel_object_id_vector,0xff,(__KERNEL_OBJECT_ID_LIMIT/8));

   for(; kernel_object_no>0; kernel_object_no--) {
      if( !(p = kernel_object_alloc(KERNEL_OBJECT_FREE)) )
         return -1;

      if(!kernel_object_pool_put(p))
         return -1;
   }

   return 0;
}

/*--------------------------------------------
| Name:        kernel_object_manager_get
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
kernel_object_t* kernel_object_manager_get(kernel_object_t** pp_kernel_object_head,
                                           kernel_object_type_t type,
                                           kernel_object_src_t src, ...){
   va_list ptr;
   kernel_object_t* p=(kernel_object_t*)0;

   //
   va_strt(ptr, src);
   //
   if(src!=KERNEL_OBJECT_SRC_POOL) {
      p=va_arg(ptr,kernel_object_t*);
      memset(p,0,sizeof(kernel_object_t));
      p->src=src;
   }else if( !(p=kernel_object_pool_get(type)) ) {
      if( !(p=kernel_object_alloc(type)) ) {
         return (kernel_object_t*)0; //kernel panic not enough space in heap
      }
   }
   //
   if(kernel_object_op[type].kernel_object_constructor) {
      if(kernel_object_op[type].kernel_object_constructor(p,ptr)<0) {
         va_end(ptr);
         return (kernel_object_t*)0;
      }
   }
   //
   va_end(ptr);

   //
   p->id = kernel_object_get_id();

   //
   *pp_kernel_object_head= kernel_object_insert(*pp_kernel_object_head,p);


   return (*pp_kernel_object_head);
}

/*--------------------------------------------
| Name:        kernel_object_manager_put
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
kernel_object_t* kernel_object_manager_put(kernel_object_t** pp_kernel_object_head,
                                           kernel_object_t* p){
   *pp_kernel_object_head = kernel_object_remove(*pp_kernel_object_head,p);

   if(kernel_object_op[p->type].kernel_object_destructor)
      kernel_object_op[p->type].kernel_object_destructor(p);

   //
   kernel_object_put_id(p->id);

   //
   if(p->src!=KERNEL_OBJECT_SRC_POOL)
      return *pp_kernel_object_head;  //kernel objet not come from pool.

   if(!kernel_object_pool_put(p))
      return (kernel_object_t*)0;

   return *pp_kernel_object_head;
}

/*--------------------------------------------
| Name:        kernel_object_manager_put_all
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
kernel_object_t* kernel_object_manager_put_all(kernel_object_t** pp_kernel_object_head){

   while(*pp_kernel_object_head) {
      kernel_object_t* p = *pp_kernel_object_head;
      *pp_kernel_object_head=kernel_object_manager_put(pp_kernel_object_head,p);
   }

   return (*pp_kernel_object_head);
}


/*============================================
| End of Source  : kernel_object.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.3  2009/07/06 10:05:16  jjp
| bug fix from phlb 3.0.0.8
|
| Revision 1.2  2009/05/04 13:20:08  jjp
| bug fix from phlb
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
==============================================*/
