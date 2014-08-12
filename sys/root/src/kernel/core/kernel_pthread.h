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

/*===========================================
Compiler Directive
=============================================*/
#ifndef _KERNEL_PTHREAD_H_
#define _KERNEL_PTHREAD_H_

/**
 * \addtogroup lepton_kernel
 * @{
 */

/**
 * \addtogroup kernel_pthread les kernel pthread
 * @{
 *
 */


/*===========================================
Includes
=============================================*/

#include "kernel/core/kal.h"
#include "kernel/core/kernel_sem.h"
#include "kernel/core/kernel_sigqueue.h"
#include "kernel/core/signal.h"

#if defined(__GNUC__)
   #include "kernel/core/kernel_sem.h"
#endif
/*===========================================
Declaration
=============================================*/

#ifdef CPU_WIN32
   #pragma pack(push, 8)
#endif


#define PTHREAD_THREADS_MAX   __KERNEL_PTHREAD_MAX //14//8


#define PTHREAD_EVENT    0x04 //0x10
//see interrupt.h (OS_WaitEvent critical kernel resource. Warning!!!)

#define PROFILER_START_COUNTER_VALUE 0xFFFF

//
typedef void*(*_start_routine_t)(void*);
typedef _start_routine_t start_routine_t;

#define __tcb(pid) process_lst[pid]->pthread_ptr->tcb

typedef unsigned char irq_nb_t;

//signal management
typedef unsigned short sigbits_t; //16 signal,one bit for each sgnal.
//
typedef unsigned char pthread_priority_t;

//TODO CHANGE
typedef uint16_t pthread_stacksize_t;
typedef int16_t pthread_timeslice_t;
typedef int32_t pthread_id_t;

/**
 * attribut d'un thread
 */
typedef struct pthread_attr_st {
   pthread_priority_t priority;   /**<priorit du thread>*/
   pthread_stacksize_t stacksize;  /**<taille de pile du thread>*/
   pthread_timeslice_t timeslice;  /**<timeslice pour l'ordonanceur premptif>*/
   void*                stackaddr; /**<adresse de la pile du thread>*/
   //
   char*                name;
}pthread_attr_t;

//
typedef unsigned char syscall_t;
typedef signed char irq_prior_t;

//
typedef struct {
   syscall_t syscall;
   struct kernel_pthread_st*  from_pthread_ptr;
   //
   void*       data;
}syscall_reg_t;

//
/**
 * le pthread en tat normal
 */
#define PTHREAD_STATUS_NULL               0x00
/**
 * le pthread a fait appel  fork()
 */
#define PTHREAD_STATUS_FORK               0x01
/**
 * le pthread a reu un signal et se trouve dans la routine associ  ce signal.
 */
#define PTHREAD_STATUS_SIGHANDLER         0x02
//use for explicit internal _syscall_xxxx.
/**
 * le pthread fait un appel systme.
 */
#define PTHREAD_STATUS_KERNEL             0x04 //see interrupt.h
/**
 * le pthread est stopper,suspendu et attend le retour d'un appel systme suspensif comme pause(),wait(),waitpid(),sleep(), ... .
 */
#define PTHREAD_STATUS_STOP               0x08
/**
 * le pthread est en tat zombi
 */
#define PTHREAD_STATUS_ZOMBI              0x10

typedef unsigned char pthread_status_t;
//
/**
 * structure de contrle des pthread
 */
typedef struct kernel_pthread_st {
   pid_t pid;                 /**<identificateur du processus conteneur>*/
   pthread_id_t id;           /**<numro d'identification du thread>*/
   pthread_attr_t attr;       /**<attribut du thread>*/
   void*                arg;  /**<pointeur sur >*/
   void*                exit;
   void*                heap_top; /**<adresse allocation en pile de thread (voir alloca())>*/
   void*                heap_floor;
   //
   desc_t io_desc;
#if defined(__KERNEL_IO_SEM)
   kernel_sem_t io_sem;
#endif
   //
   start_routine_t start_routine;

   //
   pthread_status_t stat;

#ifdef KERNEL_PROFILER
   unsigned short _profile_counter;
#endif

#ifdef __KERNEL_UCORE_EMBOS
   OS_TASK*  tcb;
#endif
   
#ifdef __KERNEL_UCORE_FREERTOS
   freertos_tcb_t* tcb;
   EventGroupHandle_t event_group_handle; //used by kernel (KERNEL_INTERRUPT and KERNEL_RET_INTERRUPT).
#endif

#ifdef __KERNEL_UCORE_ECOS
   tcb_t * tcb;
   tcb_t * bckup_tcb; //for signal handler
   thr_id_t thr_id;
   char * kernel_stack; //pile noyau du thread

   kernel_sem_t sem_wait; //sem for waitpid

   cyg_flag_t io_flag; //flag pour les I/O
#endif

   struct kernel_pthread_st* parent_pthread_ptr;

   //alarm
   time_t time_out; /**<compteur pour le reveil du pthread lors d'utilisation d'appel systme comme alarm()*/ //for alarm

   //signal
   sigset_t sig_pending; /**<vecteur de bits sur les signaux en suspensions*/
   sigset_t sig_mask; /**<vecteur de bits sur les signaux masquer*/
#ifdef __KERNEL_POSIX_REALTIME_SIGNALS
   struct sigaction sigaction_lst[SIGRTMAX+1];    /**< liste des actions associes au signaux*/
#else
   struct sigaction sigaction_lst[NSIG+1];    /**< liste des actions associes au signaux*/
#endif

   siginfo_t siginfo; /**<*/

   //sig queue
#ifdef __KERNEL_POSIX_REALTIME_SIGNALS
   struct kernel_sigqueue_st kernel_sigqueue;
#endif

   //kernel interrupt for syscall
   irq_nb_t irq_nb;          /**<numero d'interuption noyau voir les appels systme*/
   irq_prior_t irq_prior;       /**<*/
   syscall_reg_t reg;     /**<structure contenant les donnes relatives  l'appel systme*/

   //context
   /**
    *
    * Ces paramtres et structures de donnes sont utiliss par les macros __bckup_context() et __rstr_context() (voir kernel/kernel_pthread.h)
    * et plus particulirement dans le cadre du vfork() et kill().
    */
   context_t start_context; /**<sauvegarde du contexte de dpart du thread*/
   context_t bckup_context; /**<sauvegarde du contexte courant du process*/

   char*   bckup_stack; /**<pointeur sur la zone de donnes qui sauvegarde la pile du process*/

   int _errno;


   struct kernel_pthread_st* next;

   struct kernel_pthread_st* gnext;
   struct kernel_pthread_st* gprev;
}kernel_pthread_t;

extern kernel_pthread_t* g_pthread_lst;
extern int g_pthread_id;
//
int kernel_get_pthread_id(kernel_pthread_t *p);
int kernel_put_pthread_id(kernel_pthread_t *p);
//
int   kernel_pthread_create(kernel_pthread_t *thread, const pthread_attr_t *attr,start_routine_t start_routine, void *arg);
int   kernel_pthread_cancel(kernel_pthread_t* thread);
//
#ifndef __cplusplus
kernel_pthread_t* kernel_pthread_self(void);
#else
extern "C" kernel_pthread_t* kernel_pthread_self(void);
#endif

void  kernel_pthread_exit(void *value_ptr); //not yet implemented
int   kernel_pthread_kill(kernel_pthread_t* thread, int sig); //not yet implemented
#define  __pthread_exit(value_ptr){}

void* kernel_pthread_alloca(kernel_pthread_t *p, size_t size);

#define __kernel_pthread_errno (kernel_pthread_self()->_errno)


//debug
#if defined (WIN32) && defined(_DEBUG)
   #define _dbg_printf printf
#else
   #define _dbg_printf //printf
#endif

//size of kernel stack
#if defined(__GNUC__)
   #if defined(CPU_ARM7) || defined(CPU_ARM9)
      #define KERNEL_STACK 4096   //2048
   #elif defined(CPU_CORTEXM)
      #define KERNEL_STACK  2048
   #else
      #define KERNEL_STACK 8192 //4096
   #endif
#endif

#ifdef CPU_WIN32
   #pragma pack(pop)
#endif


/** @} */
/** @} */

#endif
