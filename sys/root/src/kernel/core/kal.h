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
#ifndef _KAL_H
#define _KAL_H

/**
 * \addtogroup lepton_kernel
 * @{
 */

/**
 * \addtogroup kal couche d'abstraction pour les operations primitives sur le micro noyau temps rel
 * @{
 *
 */

/*============================================
| Includes
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/kernelconf.h"

/*============================================
| Declaration
==============================================*/
//for eCos
#if defined(USE_ECOS) && defined(CPU_GNU32)
//synthetic target for simulation under linux host
   #include <cyg/kernel/kapi.h>
   #include <cyg/hal/hal_io.h>
   #include <cyg/hal/var_arch.h>

   #define __va_list_copy(__dest_va_list__,__src_va_list__) va_copy(__dest_va_list__, \
                                                                    __src_va_list__)

typedef cyg_thread tcb_t;
typedef cyg_handle_t thr_id_t;
//contexte défini lors de l'entrée dans le handler de signaux (au sens Linux)
typedef cyg_hal_sys_ucontext_t k_handler_context_t;
//contexte nécessaire pour la sauvegarde de contexte dans le thread
typedef hal_gregset_t context_t;
typedef void (*_pthreadstart_routine_t)(void);
typedef _pthreadstart_routine_t pthreadstart_routine_t;

//macro de profilage (voir kal.h sur CVS)
/**
        début du zone de code atomique (non préemptible)
 *
 * \hideinitializer
 */
   #define __atomic_in() cyg_scheduler_lock();

/**
 * début du zone de code atomique (non préemptible)
 *
 * \hideinitializer
 */
   #define __atomic_out() cyg_scheduler_unlock();

   #define __begin_pthread(pthread_name) \
   void pthread_name(void){

   #define __end_pthread() \
   return; }
   #define _macro_stack_addr

   #define __is_thread_self(__pthread_ptr__) \
   (__pthread_ptr__->thr_id == cyg_thread_self())
//uninterruptible section in
   #define __disable_interrupt_section_in() cyg_interrupt_disable();
//uninterruptible section out
   #define __disable_interrupt_section_out() cyg_interrupt_enable();

   #define __stop_sched()

   #define __restart_sched()    //cyg_scheduler_start();

   #define __set_active_pthread(__pthread_ptr__)
//énumération de registres pour la sauvegarde de contexte (à mettre éventuellement dans le hal_io.h de synthetic)
enum enum_synth_regs {
   S_REG_GS = 0,
   S_REG_FS,
   S_REG_ES,
   S_REG_DS,
   S_REG_EDI,
   S_REG_ESI,
   S_REG_EBP,
   S_REG_ESP,
   S_REG_EBX,
   S_REG_EDX,
   S_REG_ECX,
   S_REG_EAX,
   S_REG_TRAPNO,
   S_REG_ERR,
   S_REG_EIP,
   S_REG_CS,
   S_REG_EFL,
   S_REG_UESP,
   S_REG_SS
};

//get the 5 registers defined on HAL_SavedRegisters (esp, ebp, ebx, esi, edi)
//and store it at the right place
   #define __get_thread_context(__context__){ \
      __asm__(    "mov %%edi,%0 \n\t" \
                  "mov %%esi,%1 \n\t" \
                  "mov %%ebp,%2 \n\t" \
                  "mov %%esp,%3 \n\t" \
                  "mov %%ebx,%4 \n\t" \
                  : "=m" (__context__[S_REG_EDI]),"=m" (__context__[S_REG_ESI]), \
                  "=m" (__context__[S_REG_EBP]),"=m" (__context__[S_REG_ESP]), \
                  "=m" (__context__[S_REG_EBX])); \
}
//get the main registers at the beginning of the thread
   #define __inline_bckup_thread_start_context(__context__,__pthread_ptr__){ \
      __get_thread_context(__context__); \
      __context__[S_REG_ESP] = __pthread_ptr__->tcb->stack_base+__pthread_ptr__->tcb->stack_size; \
}

//get the context of a thread
/*//	#define __inline_bckup_context(__context__,__pthread_ptr__){\
//		__get_thread_context(__context__);\
//		__context__[S_REG_ESP] = __pthread_ptr__->tcb->stack_ptr;\
//	}*/

//save the stack of current thread
   #define __inline_bckup_stack(__pthread_ptr__){ \
      int offsetEsp; \
      void* src_stack_ptr; \
      offsetEsp =  __pthread_ptr__->bckup_context[S_REG_ESP] - \
                  __pthread_ptr__->start_context[S_REG_ESP]; \
      src_stack_ptr = (char *)(__pthread_ptr__->start_context[S_REG_ESP]+offsetEsp); \
      __pthread_ptr__->bckup_stack = (char*)malloc( abs(offsetEsp) ); \
      if(!__pthread_ptr__->bckup_stack) \
         return -ENOMEM; \
      memcpy(__pthread_ptr__->bckup_stack,src_stack_ptr,abs(offsetEsp)); \
}
//restore the stack content
   #define __inline_rstr_stack(__pthread_ptr__){ \
      int offsetEsp; \
      void* src_stack_ptr; \
      offsetEsp = __pthread_ptr__->bckup_context[S_REG_ESP] - \
                  __pthread_ptr__->start_context[S_REG_ESP]; \
      src_stack_ptr = (char *)(__pthread_ptr__->start_context[S_REG_ESP]+offsetEsp); \
      memcpy(src_stack_ptr,__pthread_ptr__->bckup_stack,abs(offsetEsp)); \
      free(__pthread_ptr__->bckup_stack); \
}
//save the current tcb
   #define __inline_bckup_context(__context__,__pthread_ptr__){ \
      __pthread_ptr__->bckup_tcb = (tcb_t *) malloc(sizeof(tcb_t)); \
      if(!__pthread_ptr__->bckup_tcb) \
         return -ENOMEM; \
      memcpy((void *)__pthread_ptr__->bckup_tcb, (void *)__context__, sizeof(tcb_t)); \
}
//restore the previous tcb
   #define __inline_rstr_context(__context__,__pthread_ptr__){ \
      memcpy((void *)__context__, (void *) __pthread_ptr__->bckup_tcb, sizeof(tcb_t)); \
      free(__pthread_ptr__->bckup_tcb); \
      __pthread_ptr__->bckup_tcb = (tcb_t *)0; \
}

/**
 * permet de dérouter le flux d'exécution d'un process par la fonction _sys_kill().
 *
 * \param pid pid du process dont le flux d'exécution doit être dérouter.
 * \param sig_handler address de la fonction sighandler() (voir kernel/signal.c)
 *
 * \note le déroutage du flux d'exécution est obtenue en modifiant l'addresse de retour d'interruption du scheduler.
 * cette addresse de retour est généralement placée dans la structure qui permet de sauvegarder le contexte context_t de la tâche
 * lors de l'appel de l'ordonanceur par l'interuption du timer qui contrôle la préemption (le tick).
 * cette structure de contexte context_t depend du micro-noyau utilisé.
 *
 * \hideinitializer
 */
//Lors d'une sauvegarde de contexte une structure HAL_SavedRegisters est sauvegardée sur
//la pile du thread. Le registre ebp contient le frame pointer. D'après la norme abi386
//ebp+4 contient l'adresse de retour de la fonction à exécuter.
//Dans le cas d'un thread en mode noyau il sufit juste d'altérer les registres passés
//par l'exception
//Utilisation de données locales au thread eCos pour sauvegarder le statut du thread pour
//son type de restitution de contexte
   #define LOCAL_THREAD_SAVE_STATUS    0
   #define LOCAL_THREAD_IS_WAITING     1

//get state from $(ECOS_REPOSITORY)/kernel/current/include/thread.hxx
//gruik pour le moment on met explicitement la valeur du thread cible ds les données locales du thread
   #define __inline_swap_signal_handler(__pthread_ptr__, __sig_handler__){ \
      if((__pthread_ptr__->stat&PTHREAD_STATUS_KERNEL) == PTHREAD_STATUS_KERNEL) { \
         __pthread_ptr__->bckup_stack = \
            (char *)malloc(__pthread_ptr__->attr.stacksize+sizeof(context_t)); \
         memcpy((void *)__pthread_ptr__->bckup_stack, (void *)__pthread_ptr__->bckup_context, \
                sizeof(context_t)); \
         memcpy((void *)(__pthread_ptr__->bckup_stack+sizeof(context_t)), \
                (void *)__pthread_ptr__->tcb->stack_base, \
                __pthread_ptr__->attr.stacksize); \
         __pthread_ptr__->bckup_context[S_REG_EIP] = (int)__sig_handler__; \
         __pthread_ptr__->tcb->thread_data[LOCAL_THREAD_SAVE_STATUS] = PTHREAD_STATUS_KERNEL; \
      } \
      else { \
         char ** eip_addr=NULL; \
         HAL_SavedRegisters ctx; \
         int offset; \
         cyg_thread_info info; \
         cyg_thread_get_info(__pthread_ptr__->thr_id, __pthread_ptr__->tcb->unique_id, &info); \
         memcpy((void *)&ctx, (void *) __pthread_ptr__->tcb->stack_ptr, sizeof(HAL_SavedRegisters)); \
         if(!(__pthread_ptr__->bckup_stack = \
                 (char *)malloc(__pthread_ptr__->attr.stacksize+sizeof(HAL_SavedRegisters)))) { \
                 return; \
              } \
              memcpy((void *)__pthread_ptr__->bckup_stack, (void *)&ctx, sizeof(HAL_SavedRegisters)); \
              memcpy((void *)(__pthread_ptr__->bckup_stack+sizeof(HAL_SavedRegisters)), \
                     (void *)__pthread_ptr__->tcb->stack_base, \
                     __pthread_ptr__->attr.stacksize); \
              eip_addr = (ctx.ebp+4); \
              *eip_addr = __sig_handler__; \
              if(info.state & CYG_REASON_WAIT) \
                 __pthread_ptr__->tcb->thread_data[LOCAL_THREAD_IS_WAITING] = info.state; \
              } \
              }

//restore previous stack and previous register
   #define __inline_exit_signal_handler(__pthread_ptr__){ \
   HAL_SavedRegisters ctx; \
   if(__pthread_ptr__->tcb->thread_data[LOCAL_THREAD_SAVE_STATUS] & PTHREAD_STATUS_KERNEL) { \
      memcpy((void *)__pthread_ptr__->bckup_context, (void *)__pthread_ptr__->bckup_stack, \
             sizeof(context_t)); \
      memcpy((void *)__pthread_ptr__->tcb->stack_base, (void *)__pthread_ptr__->bckup_stack+ \
             sizeof(context_t),__pthread_ptr__->attr.stacksize); \
      __pthread_ptr__->tcb->thread_data[LOCAL_THREAD_SAVE_STATUS] = 0; \
   } \
   else { \
      char ** eip_addr=NULL; \
      memcpy((void *)&ctx,(void *)__pthread_ptr__->bckup_stack, sizeof(HAL_SavedRegisters)); \
      memcpy((void *)__pthread_ptr__->tcb->stack_base, \
             (void *)(__pthread_ptr__->bckup_stack+sizeof(HAL_SavedRegisters)), \
             __pthread_ptr__->attr.stacksize); \
      eip_addr = (ctx.ebp+4); \
      __pthread_ptr__->bckup_context[S_REG_EIP] = *eip_addr; \
      __pthread_ptr__->bckup_context[S_REG_EBP] = ctx.ebp; \
      __pthread_ptr__->bckup_context[S_REG_ESP] = ctx.esp; \
      __pthread_ptr__->bckup_context[S_REG_EBX] = ctx.ebx; \
   } \
   if(__pthread_ptr__->tcb->thread_data[LOCAL_THREAD_IS_WAITING] & CYG_REASON_WAIT) { \
      __pthread_ptr__->tcb->wake_reason = CYG_REASON_BREAK; \
      __pthread_ptr__->tcb->thread_data[LOCAL_THREAD_IS_WAITING] = 0; \
   } \
   free(__pthread_ptr__->bckup_stack); \
   __pthread_ptr__->bckup_stack = 0; \
}

   #define __bckup_thread_start_context(__context__,__pthread_ptr__) \
   __inline_bckup_thread_start_context(__context__,__pthread_ptr__)

   #define __bckup_context(__context__,__pthread_ptr__) \
   __inline_bckup_context(__context__,__pthread_ptr__)

   #define __bckup_stack(__pthread_ptr__) \
   __inline_bckup_stack(__pthread_ptr__)

   #define __rstr_stack(__pthread_ptr__) \
   __inline_rstr_stack(__pthread_ptr__)

   #define __rstr_context(__context__,__pthread_ptr__) \
   __inline_rstr_context(__context__,__pthread_ptr__)

   #define __swap_signal_handler(__pthread_ptr__,sig_handler) \
   __inline_swap_signal_handler(__pthread_ptr__,sig_handler)

   #define __exit_signal_handler(__pthread_ptr__) \
   __inline_exit_signal_handler(__pthread_ptr__)

   #define PROFILER_PERIOD    1
   #define __kernel_profiler_start()
   #define __kernel_profiler_stop(__pid__)
   #define __kernel_profiler_get_counter(__pid__)  0
   #define __io_profiler_init()
   #define __io_profiler_start(__desc__)
   #define __io_profiler_stop(__desc__)
   #define __io_profiler_get_counter(__desc__)     0

#elif (defined(__GNUC__)) && (defined(CPU_ARM7) || defined(CPU_ARM9))
   #include <cyg/kernel/kapi.h>
   #include <cyg/hal/hal_arch.h>
   #include <string.h>

   #define __va_list_copy(__dest_va_list__,__src_va_list__) \
   memcpy((void *)&__dest_va_list__,(void *)&__src_va_list__,sizeof(__dest_va_list__))

typedef cyg_thread tcb_t;
typedef cyg_handle_t thr_id_t;
//contexte nécessaire pour la sauvegarde de contexte dans le thread
typedef HAL_SavedRegisters context_t;
typedef void (*_pthreadstart_routine_t)(void);
typedef _pthreadstart_routine_t pthreadstart_routine_t;

//macro de profilage (voir kal.h sur CVS)
/**
        début du zone de code atomique (non préemptible)
 *
 * \hideinitializer
 */
   #define __atomic_in() cyg_scheduler_lock();

/**
 * début du zone de code atomique (non préemptible)
 *
 * \hideinitializer
 */
   #define __atomic_out() cyg_scheduler_unlock();

   #define __begin_pthread(pthread_name) \
   void pthread_name(void){

   #define __end_pthread() \
   return; }

   #define _macro_stack_addr
   #define __is_thread_self(__pthread_ptr__) \
   (__pthread_ptr__->thr_id == cyg_thread_self())
//uninterruptible section in
   #define __disable_interrupt_section_in() cyg_interrupt_disable();
//uninterruptible section out
   #define __disable_interrupt_section_out() cyg_interrupt_enable();

   #define __stop_sched()

   #define __restart_sched()    //cyg_scheduler_start();

   #define __set_active_pthread(__pthread_ptr__)
//get the registers defined on HAL_SavedRegisters (except r0..4)
//and store it at the right place
   #define __get_thread_context(__context__) { \
}

//get the main registers at the beginning of the thread
   #define __inline_bckup_thread_start_context(__context__,__pthread_ptr__){ \
      __context__.sp = __pthread_ptr__->tcb->stack_base+__pthread_ptr__->tcb->stack_size; \
}

//save the stack of current thread
   #define __inline_bckup_stack(__pthread_ptr__){ \
      int offsetEsp; \
      void* src_stack_ptr; \
      offsetEsp =  __pthread_ptr__->bckup_context.sp - __pthread_ptr__->start_context.sp; \
      src_stack_ptr = (char *)(__pthread_ptr__->start_context.sp+offsetEsp); \
      __pthread_ptr__->bckup_stack = (char*)malloc( abs(offsetEsp) ); \
      if(!__pthread_ptr__->bckup_stack) \
         return -ENOMEM; \
      memcpy(__pthread_ptr__->bckup_stack,src_stack_ptr,abs(offsetEsp)); \
      __pthread_ptr__->tcb->thread_data[0] = abs(offsetEsp); \
}
//restore the stack content
   #define __inline_rstr_stack(__pthread_ptr__){ \
      int offsetEsp; \
      void* src_stack_ptr; \
      offsetEsp = __pthread_ptr__->bckup_context.sp - __pthread_ptr__->start_context.sp; \
      src_stack_ptr = (char *)(__pthread_ptr__->start_context.sp+offsetEsp); \
      memcpy(src_stack_ptr,__pthread_ptr__->bckup_stack,abs(offsetEsp)); \
      free(__pthread_ptr__->bckup_stack); \
}

//PEUT ETRE INTEGRER _BCKUP_STACK et 8BCKUP_CONTEXT
//commentaire voir dummy_swap_signal_handler
// ATTENTION REVENIR SUR LES CASTS DES QUE POSSIBLE
//is thread waiting for a sig see $ECOS_REPOSITORY/kernel/current/include/thread.hxx
   #define __inline_swap_signal_handler(__pthread_ptr__,__sig_handler__){ \
      char ** fp_ptr; \
      int offset; \
      if((__pthread_ptr__->stat&PTHREAD_STATUS_KERNEL) == PTHREAD_STATUS_KERNEL) { \
         offset = \
            (__pthread_ptr__->tcb->stack_base + \
             __pthread_ptr__->tcb->stack_size) - __pthread_ptr__->bckup_context.sp; \
         __pthread_ptr__->tcb->thread_data[0] = offset; \
         __pthread_ptr__->bckup_stack = malloc(abs(offset) + sizeof(context_t)); \
         if(!__pthread_ptr__->bckup_stack) return -ENOMEM; \
         memcpy((void *)__pthread_ptr__->bckup_stack, (void *)&(__pthread_ptr__->bckup_context), \
                sizeof(context_t)); \
         memcpy((void *)__pthread_ptr__->bckup_stack+sizeof(context_t), \
                (void *)__pthread_ptr__->bckup_context.sp, \
                offset); \
         fp_ptr = (char **)(__pthread_ptr__->bckup_context.fp - 4); \
         *fp_ptr = (char *)__sig_handler__; \
      } \
      else { \
         char **sp_ptr = (char **)__pthread_ptr__->tcb->stack_ptr; \
         context_t * ctx = (context_t *)sp_ptr; \
         offset = (__pthread_ptr__->tcb->stack_base + __pthread_ptr__->tcb->stack_size) - ctx->sp; \
         __pthread_ptr__->tcb->thread_data[0] = offset; \
         __pthread_ptr__->bckup_stack = malloc(offset + sizeof(context_t)); \
         if(!__pthread_ptr__->bckup_stack) return -ENOMEM; \
         memcpy((void *)__pthread_ptr__->bckup_stack, (void *)ctx, sizeof(context_t)); \
         memcpy((void *)(__pthread_ptr__->bckup_stack+sizeof(context_t)), (void *)(ctx->sp), offset); \
         fp_ptr = (char **)(ctx->fp - 4); \
         *fp_ptr = (char *)__sig_handler__; \
      } \
}


//restaure le contexte du programme interropu par un signal
//recopie de la pile sauvegardée dans pile thread courant
//ecrase contexte sauvegardée lors retour appel système
   #define __inline_exit_signal_handler(__pthread_ptr__){ \
      context_t *ctx = (context_t *)__pthread_ptr__->bckup_stack; \
      int offset = __pthread_ptr__->tcb->thread_data[0]; \
      char * p_sp = __pthread_ptr__->tcb->stack_base+ \
                    (char *)(__pthread_ptr__->tcb->stack_size - offset); \
      memcpy((void *)&__pthread_ptr__->bckup_context, (void *)ctx, sizeof(context_t)); \
      memcpy((void *)p_sp, (void *)(__pthread_ptr__->bckup_stack+sizeof(context_t)), offset); \
      free(__pthread_ptr__->bckup_stack); \
      __pthread_ptr__->bckup_stack=0; \
}

//PEUT ETRE REECRIRE BCKUP_CONTEXT ET RSTR_CONTEXT
//save the current tcb
   #define __inline_bckup_context(__context__,__pthread_ptr__){ \
      __pthread_ptr__->bckup_tcb = (tcb_t *) malloc(sizeof(tcb_t)); \
      if(!__pthread_ptr__->bckup_tcb) \
         return -ENOMEM; \
      memcpy((void *)__pthread_ptr__->bckup_tcb, (void *)__context__, sizeof(tcb_t)); \
}
//restore the previous tcb
   #define __inline_rstr_context(__context__,__pthread_ptr__){ \
      memcpy((void *)__context__, (void *) __pthread_ptr__->bckup_tcb, sizeof(tcb_t)); \
      free(__pthread_ptr__->bckup_tcb); \
      __pthread_ptr__->bckup_tcb = (tcb_t *)0; \
}

   #define __bckup_thread_start_context(__context__,__pthread_ptr__) \
   __inline_bckup_thread_start_context(__context__,__pthread_ptr__)

   #define __bckup_context(__context__,__pthread_ptr__) \
   __inline_bckup_context(__context__,__pthread_ptr__)

   #define __bckup_stack(__pthread_ptr__) \
   __inline_bckup_stack(__pthread_ptr__)

   #define __rstr_stack(__pthread_ptr__) \
   __inline_rstr_stack(__pthread_ptr__)

   #define __rstr_context(__context__,__pthread_ptr__) \
   __inline_rstr_context(__context__,__pthread_ptr__)

   #define __swap_signal_handler(__pthread_ptr__,sig_handler) \
   __inline_swap_signal_handler(__pthread_ptr__,sig_handler)

   #define __exit_signal_handler(__pthread_ptr__) \
   __inline_exit_signal_handler(__pthread_ptr__)

//include for profiling
   #if (__tauon_cpu_device__ == __tauon_cpu_device_arm9_at91sam9260__)
      #include "cyg/hal/at91sam9260.h"
   #endif

   #if (__tauon_cpu_device__ == __tauon_cpu_device_arm9_at91sam9261__)
      #include "cyg/hal/at91sam9261.h"
   #endif

//profiling for ARM9
   #if ((__tauon_cpu_device__ == __tauon_cpu_device_arm9_at91sam9260__) \
   || (__tauon_cpu_device__ == __tauon_cpu_device_arm9_at91sam9261__)) \
   && defined(KERNEL_PROFILER)

//
      #define PROFILER_PERIOD    (1.0/32000.0)
//
      #define PROFILER_START_COUNTER_VALUE 0xFFFF
//
//#define __kernel_profiler_start()
//#define __kernel_profiler_stop(__pid__)
//#define __kernel_profiler_get_counter(__pid__) (0)

      #define __kernel_profiler_start(){ \
      *AT91C_PMC_PCER |= 0x80000; \
      *AT91C_TCB0_BMR = 0x02; \
      *AT91C_TC2_CCR  = 2; \
      *AT91C_TC2_CMR  = 0x00004004; \
      *AT91C_TC2_RC   = PROFILER_START_COUNTER_VALUE; \
      *AT91C_TC2_CCR  = 1; \
      *AT91C_TC2_CCR  = 5; \
}

//
      #define __kernel_profiler_stop(__pthread_ptr__){ \
      *AT91C_TC2_CCR = 2; \
      if(__pthread_ptr__) \
         __pthread_ptr__->_profile_counter=*AT91C_TC2_CV; \
}
//
      #define __kernel_profiler_get_counter(__pthread_ptr__) (__pthread_ptr__ ? __pthread_ptr__-> \
                                                              _profile_counter : 0)

//
      #define __io_profiler_init(){ \
      *AT91C_PMC_PCER |= 0x40000; \
      *AT91C_TCB0_BMR = 0x01; \
      *AT91C_TC1_CCR  = 2; \
      *AT91C_TC1_CMR  = 0x00004004; \
      *AT91C_TC1_RC   = PROFILER_START_COUNTER_VALUE; \
      *AT91C_TC1_CCR  = 1; \
      *AT91C_TC1_CCR  = 5; \
}
//
      #define __io_profiler_start(__desc__){ \
      ofile_lst[__desc__]._profile_counter=(*AT91C_TC1_CV); \
}
//
      #define __io_profiler_stop(__desc__){ \
      unsigned short __counter__ = (*AT91C_TC1_CV); \
      if(__counter__ > ofile_lst[__desc__]._profile_counter ) \
         ofile_lst[__desc__]._profile_counter=(__counter__)-ofile_lst[__desc__]._profile_counter; \
      else \
         ofile_lst[__desc__]._profile_counter= \
            (PROFILER_START_COUNTER_VALUE-ofile_lst[__desc__]._profile_counter)+__counter__; \
}
//
      #define __io_profiler_get_counter(__desc__) ofile_lst[__desc__]._profile_counter

   #else
      #define PROFILER_PERIOD         1/32000
      #define __kernel_profiler_start()
      #define __kernel_profiler_stop(__pid__)
      #define __kernel_profiler_get_counter(__pid__)  0
      #define __io_profiler_init()
      #define __io_profiler_start(__desc__)
      #define __io_profiler_stop(__desc__)
      #define __io_profiler_get_counter(__desc__)  0
   #endif

#elif defined(CPU_GNU32) && defined(USE_KERNEL_STATIC)

   #define __va_list_copy(__dest_va_list__,__src_va_list__) __dest_va_list__ = __src_va_list__

//	typedef cyg_thread tcb_t;
//   typedef cyg_handle_t thr_id_t;
//   //contexte défini lors de l'entrée dans le handler de signaux (au sens Linux)
//   typedef cyg_hal_sys_ucontext_t k_handler_context_t;
//   //contexte nécessaire pour la sauvegarde de contexte dans le thread
//   typedef hal_gregset_t context_t;
//   typedef void (*_pthreadstart_routine_t)(void);
//   typedef _pthreadstart_routine_t pthreadstart_routine_t;
typedef struct
{
   unsigned int esp;
   unsigned int next_context;
   unsigned int ebp;
   unsigned int ebx;
   unsigned int esi;
   unsigned int edi;
   short interrupts;
} context_t;

typedef int tcb_t;
typedef int thr_id_t;
//macro de profilage (voir kal.h sur CVS)
/**
        début du zone de code atomique (non préemptible)
 *
 * \hideinitializer
 */
   #define __atomic_in()

/**
 * début du zone de code atomique (non préemptible)
 *
 * \hideinitializer
 */
   #define __atomic_out()

   #define __begin_pthread(pthread_name) \
   void pthread_name(void){

   #define __end_pthread() \
   return; }

   #define _macro_stack_addr
   #define __is_thread_self(__pthread_ptr__)    0

//uninterruptible section in
   #define __disable_interrupt_section_in()
//uninterruptible section out
   #define __disable_interrupt_section_out()

   #define __stop_sched()

   #define __restart_sched()    //cyg_scheduler_start();

   #define __set_active_pthread(__pthread_ptr__)
//énumération de registres pour la sauvegarde de contexte (à mettre éventuellement dans le hal_io.h de synthetic)
enum enum_synth_regs {
   S_REG_GS = 0,
   S_REG_FS,
   S_REG_ES,
   S_REG_DS,
   S_REG_EDI,
   S_REG_ESI,
   S_REG_EBP,
   S_REG_ESP,
   S_REG_EBX,
   S_REG_EDX,
   S_REG_ECX,
   S_REG_EAX,
   S_REG_TRAPNO,
   S_REG_ERR,
   S_REG_EIP,
   S_REG_CS,
   S_REG_EFL,
   S_REG_UESP,
   S_REG_SS
};

//get the 5 registers defined on HAL_SavedRegisters (esp, ebp, ebx, esi, edi)
//and store it at the right place
   #define __get_thread_context(__context__)
//get the main registers at the beginning of the thread
   #define __inline_bckup_thread_start_context(__context__,__pthread_ptr__)

//save the stack of current thread
   #define __inline_bckup_stack(__pthread_ptr__)

//restore the stack content
   #define __inline_rstr_stack(__pthread_ptr__)

//save the current tcb
   #define __inline_bckup_context(__context__,__pthread_ptr__)

//restore the previous tcb
   #define __inline_rstr_context(__context__,__pthread_ptr__)

   #define __inline_swap_signal_handler(__pthread_ptr__,__sig_handler__)

//restaure le contexte du programme interropu par un signal
   #define __inline_exit_signal_handler(__pthread_ptr__)

   #define __bckup_thread_start_context(__context__,__pthread_ptr__) \
   __inline_bckup_thread_start_context(__context__,__pthread_ptr__)

   #define __bckup_context(__context__,__pthread_ptr__) \
   __inline_bckup_context(__context__,__pthread_ptr__)

   #define __bckup_stack(__pthread_ptr__) \
   __inline_bckup_stack(__pthread_ptr__)

   #define __rstr_stack(__pthread_ptr__) \
   __inline_rstr_stack(__pthread_ptr__)

   #define __rstr_context(__context__,__pthread_ptr__) \
   __inline_rstr_context(__context__,__pthread_ptr__)

   #define __swap_signal_handler(__pthread_ptr__,sig_handler) \
   __inline_swap_signal_handler(__pthread_ptr__,sig_handler)

   #define __exit_signal_handler(__pthread_ptr__) \
   __inline_exit_signal_handler(__pthread_ptr__)

   #define __kernel_profiler_start()
   #define __kernel_profiler_stop(__pid__)
   #define __kernel_profiler_get_counter(__pid__)
   #define __io_profiler_init()
   #define __io_profiler_start(__desc__)
   #define __io_profiler_stop(__desc__)
   #define __io_profiler_get_counter(__desc__)
//
#elif defined(CPU_WIN32)
//
   #pragma pack(push, 8)
   #include "kernel/core/windows.h"
   #include <malloc.h>

   #ifdef USE_SEGGER
      #include "kernel/core/ucore/embOSW32_100/seggerwin32.h"
   #endif

   #define __va_list_copy(__dest_va_list__,__src_va_list__) __dest_va_list__ = __src_va_list__


//
typedef LPTHREAD_START_ROUTINE pthreadstart_routine_t;
typedef int thr_id_t;
typedef OS_TASK tcb_t;

/**
 * structure de contexte utilis par le micro-noyau\n
 * cette structure contient gnralement toutes les informations ncessaire  la commutation de tache:\n
 *  1) sauvegarde de certains registres du processeur comme le pointeur de pile par exemple.\n
 *  2) sauvegarde de du compteur programme.\n
 * \n
 * ce sont les deux informations les plus importantes pour lepton. Elle permettent de raliser le vfork()
 * et la gestion des signaux avec kill().
 * \hideinitializer
 */
typedef CONTEXT context_t;

/**
 * definition du prototype de fonction de la tache gre par le micro-noyau
 *
 * \param pthread_name nom de la fonction
 *
 * \hideinitializer
 */
   #define __begin_pthread(pthread_name) \
   DWORD WINAPI pthread_name(LPVOID lpvParam){

/**
 * definition de la sortie de fonction de la tache gre par le micro-noyau
 *
 * \hideinitializer
 */
   #define __end_pthread() \
   return 0; }

/**
 * permet de savoir si le task control block tcb est bien celui de la tache courante
 *
 * \param tcb task control block
 *
 * \hideinitializer
 */
   #define __is_thread_self(tcb) \
   (tcb->Id == GetCurrentThreadId())

   #define _macro_stack_addr OS_STACKPTR


   #define PTHREAD_CONTEXT_DUMP CONTEXT_FULL|CONTEXT_DEBUG_REGISTERS|CONTEXT_FLOATING_POINT| \
   CONTEXT_EXTENDED_REGISTERS

/**
* permet de sauvegarder le contexte de dpart du thread __pthread_ptr dans context.
*
* \param __pthread_ptr pointeur sur la structure pthread_t du pthread.
*
* \note voir les fonctions _sys_krnl_exec(), _sys_exec().
* \hideinitializer
*/
   #define __inline_bckup_thread_start_context(__context__,__pthread_ptr__){ \
      CONTEXT __local_context__; \
      __local_context__.ContextFlags=PTHREAD_CONTEXT_DUMP; \
      GetThreadContext(__pthread_ptr__->tcb->hTask,&__local_context__); \
      memcpy(&__context__,&__local_context__,sizeof(CONTEXT)); \
      _dbg_printf("start_context.Esp=0x%08x\r\n",__pthread_ptr__->start_context.Esp); \
}

/**
 * permet de sauvegarder le contexte du thread __pthread_ptr dans context.
 *
 * \param context variable de type context_t dans laquelle sera sauvegarde le contexte.
 * \param __pthread_ptr pointeur sur la structure pthread_t du pthread.
 *
 * \note voir les fonctions _sys_vfork(), _sys_krnl_exec(), _sys_exec().
 * \hideinitializer
 */
   #define __inline_bckup_context(__context__,__pthread_ptr__){ \
      __context__.ContextFlags=PTHREAD_CONTEXT_DUMP; \
      GetThreadContext(__pthread_ptr__->tcb->hTask,&__context__); \
}

/**
 * permet de restaurer le contexte context dans celui du thread __pthread_ptr.
 *
 * \param context variable de type context_t dans laquelle est plac le contexte  restaurer.
 * \param __pthread_ptr pointeur sur la structure pthread_t du pthread.
 *
 * \note voir les fonctions _sys_vfork(), _sys_vfork_exit(), _sys_krnl_exec(), _sys_exec().
 * \hideinitializer
 */
   #define __inline_rstr_context(__context__,__pthread_ptr__){ \
      __context__.ContextFlags=PTHREAD_CONTEXT_DUMP; \
      SetThreadContext(__pthread_ptr__->tcb->hTask, &__context__); \
}

//Use Dynamic Allocation!!!
/**
 * permet de sauvegarder la pile d'un process
 *
 * \param pid pid du process dont il faut sauvegarder la pile.
 *
 * \note voir les fonctions _sys_vfork(), _sys_krnl_exec(), _sys_exec().
 * \hideinitializer
 */
   #define __inline_bckup_stack(__pthread_ptr__){ \
      int offsetEsp; \
      void* src_stack_ptr; \
      _dbg_printf("backup.Esp=0x%08x  start_context.Esp=0x%08x\r\n", \
                  __pthread_ptr__->bckup_context.Esp, \
                  __pthread_ptr__->start_context.Esp); \
      offsetEsp = __pthread_ptr__->bckup_context.Esp - __pthread_ptr__->start_context.Esp; \
      src_stack_ptr = (PDWORD)(__pthread_ptr__->start_context.Esp+offsetEsp); \
      __pthread_ptr__->bckup_stack = (char*)malloc( abs(offsetEsp) ); \
      if(!__pthread_ptr__->bckup_stack) \
         return -ENOMEM; \
      _dbg_printf("bckup_stack:0x%08x src_stack_ptr:0x%08x , offsetEsp=%d \r\n", \
                  __pthread_ptr__->bckup_stack,src_stack_ptr, \
                  offsetEsp); \
      memcpy(__pthread_ptr__->bckup_stack,src_stack_ptr,abs(offsetEsp)); \
}

/**
 * permet de restaurer la pile d'un process
 *
 * \param pid pid du process dont il faut restaurer la pile.
 *
 * \note voir les fonctions _sys_vfork(), _sys_krnl_exec(), _sys_exec().
 * \hideinitializer
 */
   #define __inline_rstr_stack(__pthread_ptr__){ \
      int offsetEsp; \
      void* src_stack_ptr; \
      offsetEsp = __pthread_ptr__->bckup_context.Esp - __pthread_ptr__->start_context.Esp; \
      src_stack_ptr = (PDWORD)(__pthread_ptr__->start_context.Esp+offsetEsp); \
      memcpy(src_stack_ptr,__pthread_ptr__->bckup_stack,abs(offsetEsp)); \
      free(__pthread_ptr__->bckup_stack); \
}

/**
 * permet de drouter le flux d'excution d'un process par la fonction _sys_kill().
 *
 * \param pid pid du process dont le flux d'excution doit tre drouter.
 * \param sig_handler address de la fonction sighandler() (voir kernel/signal.c)
 *
 * \note le droutage du flux d'excution est obtenue en modifiant l'addresse de retour d'interruption du scheduler.
 * cette addresse de retour est gnralement place dans la structure qui permet de sauvegarder le contexte context_t de la tche
 * lors de l'appel de l'ordonanceur par l'interuption du timer qui contrle la premption (le tick).
 * cette structure de contexte context_t depend du micro-noyau utilis.
 *
 * \hideinitializer
 */
   #define __inline_swap_signal_handler(__pthread_ptr__,sig_handler){ \
      context_t context; \
      __bckup_context(context,__pthread_ptr__); \
      context.Eip=(unsigned long)(sig_handler); \
      __rstr_context(context,__pthread_ptr__); \
      if(__pthread_ptr__->tcb->hCurrentWaitObject) \
         SetEvent(__pthread_ptr__->tcb->hCurrentWaitObject); \
}

/**
 * permet de restaurer, aprs le droutement par __swap_signal_handler(), le flux d'excution d'un process.
 *
 * \param pid pid du process dont le flux d'excution doit tre restaurer.
 */
   #define __inline_exit_signal_handler(__pthread_ptr__){ \
      __inline_rstr_context(__pthread_ptr__->bckup_context,__pthread_ptr__); \
}

/**
 * permet de rveiller un process.
 *
 * \param pid pid du process que l'on doit rveiller.
 * \note voir _kernel_timer() dans kernel/kernel.c
 */
   #define __set_active_pthread(pthread)

/**
 * dbut du zone de code atomique (non premptible)
 *
 * \hideinitializer
 */
   #define __atomic_in() OS_EnterRegion();

/**
 * dbut du zone de code atomique (non premptible)
 *
 * \hideinitializer
 */
   #define __atomic_out() OS_LeaveRegion();


/**
 * arrte le timer qui appel rgulirement l'ordonanceur.
 *
 * \note voir _syscall_execve(), _syscall_vfork(), _syscall_kill(), _syscall_exit(), _syscall_atexit(), _syscall_sigexit().
 */
   #define __stop_sched()

/**
 *  redmarre le timer qui appel rgulirement l'ordonanceur.
 *
 * \note voir _syscall_execve(), _syscall_vfork(), _syscall_kill(), _syscall_exit(), _syscall_atexit(), _syscall_sigexit().
 */
   #define __restart_sched()


//uninterruptible section in
   #define __disable_interrupt_section_in()
//uninterruptible section out
   #define __disable_interrupt_section_out()


   #ifdef KERNEL_PROFILER
      #define PROFILER_PERIOD           1
      #define __kernel_profiler_start()
      #define __kernel_profiler_stop(__pid__)
      #define __kernel_profiler_get_counter(__pid__) 0
      #define __io_profiler_init()
      #define __io_profiler_start(__desc__)
      #define __io_profiler_stop(__desc__)
      #define __io_profiler_get_counter(__desc__) 0
   #else
      #define __kernel_profiler_start()
      #define __kernel_profiler_stop(__pid__)
      #define __kernel_profiler_get_counter(__pid__)
      #define __io_profiler_init()
      #define __io_profiler_start(__desc__)
      #define __io_profiler_stop(__desc__)
      #define __io_profiler_get_counter(__desc__)
   #endif

   #pragma pack (pop)


#elif ( defined(__IAR_SYSTEMS_ICC) && defined (USE_SEGGER) && defined(CPU_M16C62))


   #include "RTOS.H"
/*modif for 3.06h version*/ \
//#include "OSKern.H"
   #include "OS_Private.h"


   #include "OSint.h"
   #include "stdlib.h"
//#include "stdio.h"
   #include "icclbutl.h"
   #include <stdarg.h>

//#define __va_list_copy(__dest_va_list__,__src_va_list__) memcpy(&__dest_va_list__,&__src_va_list__,sizeof(__dest_va_list__))
   #define __va_list_copy(__dest_va_list__,__src_va_list__) memcpy((__dest_va_list__), \
                                                                   (__src_va_list__), \
                                                                   sizeof((__dest_va_list__)))


typedef OS_TASK tcb_t;
typedef void (*_pthreadstart_routine_t)(void);
typedef _pthreadstart_routine_t pthreadstart_routine_t;
typedef int thr_id_t;

   #define __begin_pthread(pthread_name) \
   void pthread_name(void){

   #define __end_pthread() \
   return; }

   #define __is_thread_self(__tcb__) \
   (__tcb__ == OS_pCurrentTask)

   #define _macro_stack_addr OS_STACKPTR

typedef struct {
   OS_TASK os_task;
   OS_REGS os_regs;
}context_t;

   #define __bckup_thread_start_context(__context__,__pthread_ptr__){ \
      memcpy(&__context__.os_task,__pthread_ptr__->tcb,sizeof(OS_TASK)); \
      memcpy(&__context__.os_regs,((OS_REGS OS_STACKPTR *)__pthread_ptr__->tcb->pStack), \
             sizeof(OS_REGS)); \
}

   #define __bckup_context(__context__,__pthread_ptr__){ \
      memcpy(&__context__.os_task,__pthread_ptr__->tcb,sizeof(OS_TASK)); \
      memcpy(&__context__.os_regs,((OS_REGS OS_STACKPTR *)__pthread_ptr__->tcb->pStack), \
             sizeof(OS_REGS)); \
}

   #define __rstr_context(__context__,__pthread_ptr__){ \
      void OS_DEFPTR   *pPrev; \
      void OS_DEFPTR   *pNext; \
      pPrev= __pthread_ptr__->tcb->pPrev; \
      pNext= __pthread_ptr__->tcb->pNext; \
      memcpy(__pthread_ptr__->tcb,&__context__.os_task,sizeof(OS_TASK)); \
      memcpy(((OS_REGS OS_STACKPTR *)__pthread_ptr__->tcb->pStack),&__context__.os_regs, \
             sizeof(OS_REGS)); \
      __pthread_ptr__->tcb->pNext = pNext; \
      __pthread_ptr__->tcb->pPrev = pPrev; \
}

//Use Dynamic Allocation!!!
   #define __bckup_stack(__pthread_ptr__){ \
      int stack_size; \
      pid_t _pid_=__pthread_ptr__->pid; \
      void* src_stack_ptr; \
      stack_size = process_lst[_pid_]->bckup_context.os_task.pStack - \
                   __pthread_ptr__->start_context.os_task.pStack; \
      src_stack_ptr = (void*)(((uchar8_t*)__pthread_ptr__->start_context.os_task.pStack)+stack_size); \
      process_lst[_pid_]->bckup_stack = (char*)malloc( abs(stack_size) ); \
      if(!process_lst[_pid_]->bckup_stack) \
         return -ENOMEM; \
      memcpy(process_lst[_pid_]->bckup_stack,src_stack_ptr,abs(stack_size)); \
}

   #define __rstr_stack(__pthread_ptr){ \
      int stack_size; \
      pid_t _pid_=__pthread_ptr->pid; \
      void* src_stack_ptr; \
      stack_size = process_lst[_pid_]->bckup_context.os_task.pStack- \
                   __pthread_ptr->start_context.os_task.pStack; \
      src_stack_ptr = (void*)(((uchar8_t*)__pthread_ptr->start_context.os_task.pStack)+stack_size); \
      memcpy(src_stack_ptr,process_lst[_pid_]->bckup_stack,abs(stack_size)); \
      free(process_lst[_pid_]->bckup_stack); \
}

   #define __swap_signal_handler(__pid__,sig_handler){ \
      /*modif for 3.06h version*/ \
      /*((OS_REGS OS_STACKPTR *)process_lst[pid]->pthread_ptr->tcb->pStack)->RetAdr4    = OS_MakeIntAdr(sig_handler);*/ \
      /* First return adr (see OS_Private.h)*/ \
      ((OS_REGS OS_STACKPTR *)process_lst[__pid__]->pthread_ptr->tcb->pStack)->PC0= \
         (OS_U32)sig_handler; \
      ((OS_REGS OS_STACKPTR *)process_lst[__pid__]->pthread_ptr->tcb->pStack)->Counters= 0; \
      process_lst[pid]->pthread_ptr->tcb->TASK_Timeout=0; \
      process_lst[pid]->pthread_ptr->tcb->Stat=0; \
}

/*TS_WAIT_TIME*/
   #define __exit_signal_handler(__pid__){ \
      __rstr_context(process_lst[__pid__]->bckup_context,process_lst[__pid__]->pthread_ptr); \
      /*process_lst[pid]->pthread_ptr->tcb->TASK_Timeout=0;*/ \
      /*process_lst[pid]->pthread_ptr->tcb->Stat=0;*/ \
}

   #define __set_active_pid(__pid__) \
   if(__pid__ && process_lst[__pid__]->pthread_ptr) \
      OS_MakeTaskReady(&process_lst[__pid__]->pthread_ptr->tcb);

//start switching task
   #define __atomic_in() OS_EnterRegion(); //stop task switching not the scheduler, kernel timeslice must be set to 0 cooprative mode).
//restart task switching
   #define __atomic_out() OS_LeaveRegion(); //restart task switching

//stop timer (TIMER A0) tick for scheduler.
   #define __stop_sched() TABSR .0=0;
//restart timer (TIMER A0) tick for scheduler.
   #define __restart_sched() TABSR .0=1;

//uninterruptible section in
   #define __disable_interrupt_section_in() OS_IncDI()
//uninterruptible section out
   #define __disable_interrupt_section_out() OS_DecRI()

   #ifdef KERNEL_PROFILER
      #define __kernel_profiler_start(){ \
      TABSR .1=0; \
      TA1MR=0x80; \
      UDF|=0x02; \
      TA1 =PROFILER_START_COUNTER_VALUE; \
      TABSR .1=1; \
}

      #define __kernel_profiler_stop(__pid__){ \
      TABSR .1=0; \
      process_lst[__pid__]->pthread_ptr->_profile_counter=(PROFILER_START_COUNTER_VALUE-TA1); \
}

      #define __kernel_profiler_get_counter(__pid__) process_lst[__pid__]->pthread_ptr-> \
   _profile_counter;

//io
      #define __io_profiler_init(){ \
      TABSR .3=0; \
      TA3MR=0x80; \
      UDF|=0x08; \
      TA3 =PROFILER_START_COUNTER_VALUE; \
      TABSR .3=1; \
}

      #define __io_profiler_start(__desc__){ \
      ofile_lst[__desc__]._profile_counter=(TA3); \
}

      #define __io_profiler_stop(__desc__){ \
      unsigned short __counter__ = TA3; \
      if(__counter__ > ofile_lst[__desc__]._profile_counter ) \
         ofile_lst[__desc__]._profile_counter= \
            (PROFILER_START_COUNTER_VALUE-__counter__)+ofile_lst[__desc__]._profile_counter; \
      else \
         ofile_lst[__desc__]._profile_counter=ofile_lst[__desc__]._profile_counter-__counter__; \
}

      #define __io_profiler_get_counter(__desc__) ofile_lst[__desc__]._profile_counter

   #else
      #define __kernel_profiler_start()
      #define __kernel_profiler_stop(__pid__)
      #define __kernel_profiler_get_counter(__pid__)
      #define __io_profiler_init()
      #define __io_profiler_start(__desc__)
      #define __io_profiler_stop(__desc__)
      #define __io_profiler_get_counter(__desc__)
   #endif


#elif ( defined(__IAR_SYSTEMS_ICC__) && defined (USE_SEGGER) && ( defined(CPU_ARM7) || \
   defined(CPU_ARM9) ) )


   #include "RTOS.H"
/*modif for segger version 3.28h */
//#include "OSKern.H"
   #include "OS_Priv.h"
/*modif for segger version 3.52e */
//#include "OSint.h"

//must be set to 1 for segger version 3.28n and set to 0 for 3.32e
   #if 0
//#include "OSint.h"
   #endif

   #include "stdlib.h"

   #if (__tauon_cpu_device__ == __tauon_cpu_device_arm7_at91m55800a__)
      #include <ioat91m55800.h>
   #endif

   #if (__tauon_cpu_device__ == __tauon_cpu_device_arm7_at91sam7x__)
      #include <ioat91sam7x256.h>
   #endif

   #if (__tauon_cpu_device__ == __tauon_cpu_device_arm9_at91sam9261__)
      #include <ioat91sam9261.h>
   #endif

   #define __va_list_copy(__dest_va_list__,__src_va_list__) memcpy(&__dest_va_list__, \
                                                                   &__src_va_list__, \
                                                                   sizeof(__dest_va_list__))
//for compatibility with m16c 3.06h
//

typedef OS_TASK tcb_t;
typedef void (*_pthreadstart_routine_t)(void);
typedef _pthreadstart_routine_t pthreadstart_routine_t;
typedef int thr_id_t;

   #define __begin_pthread(pthread_name) \
   void pthread_name(void){

   #define __end_pthread() \
   return; }

   #define __is_thread_self(__tcb__) \
   (__tcb__ == OS_pCurrentTask)

   #define _macro_stack_addr OS_STACKPTR

typedef struct {
   OS_TASK os_task;
   OS_REGS os_regs;
}context_t;

   #define __inline_bckup_thread_start_context(__context__,__pthread_ptr__){ \
      memcpy(&__context__.os_task,__pthread_ptr__->tcb,sizeof(OS_TASK)); \
      memcpy(&__context__.os_regs,((OS_REGS OS_STACKPTR *)__pthread_ptr__->tcb->pStack), \
             sizeof(OS_REGS)); \
}

   #define __inline_bckup_context(__context__,__pthread_ptr__){ \
      memcpy(&__context__.os_task,__pthread_ptr__->tcb,sizeof(OS_TASK)); \
      memcpy(&__context__.os_regs,((OS_REGS OS_STACKPTR *)__pthread_ptr__->tcb->pStack), \
             sizeof(OS_REGS)); \
}

   #define __inline_rstr_context(__context__,__pthread_ptr__){ \
      OS_TASK   *pPrev; \
      OS_TASK   *pNext; \
      pPrev= __pthread_ptr__->tcb->pPrev; \
      pNext= __pthread_ptr__->tcb->pNext; \
      memcpy(__pthread_ptr__->tcb,&__context__.os_task,sizeof(OS_TASK)); \
      memcpy(((OS_REGS OS_STACKPTR *)__pthread_ptr__->tcb->pStack),&__context__.os_regs, \
             sizeof(OS_REGS)); \
      __pthread_ptr__->tcb->pNext = pNext; \
      __pthread_ptr__->tcb->pPrev = pPrev; \
}

//Use Dynamic Allocation!!!
   #define __inline_bckup_stack(__pthread_ptr__){ \
      int __stack_size__; \
      void* __src_stack_ptr__; \
      __stack_size__ = \
         ((int)(__pthread_ptr__->bckup_context.os_task.pStack) - \
          (int)(__pthread_ptr__->start_context.os_task.pStack)); \
      __src_stack_ptr__ = \
         (void*)(((uchar8_t*)__pthread_ptr__->start_context.os_task.pStack)+__stack_size__); \
      __pthread_ptr__->bckup_stack = (char*)_sys_malloc( abs(__stack_size__) ); \
      if(!__pthread_ptr__->bckup_stack) \
         return -ENOMEM; \
      memcpy(__pthread_ptr__->bckup_stack,__src_stack_ptr__,abs(__stack_size__)); \
}

   #define __inline_rstr_stack(__pthread_ptr__){ \
      int __stack_size__; \
      void* __src_stack_ptr__; \
      __stack_size__ = \
         ((int)(__pthread_ptr__->bckup_context.os_task.pStack)- \
          (int)(__pthread_ptr__->start_context.os_task.pStack)); \
      __src_stack_ptr__ = \
         (void*)(((uchar8_t*)__pthread_ptr__->start_context.os_task.pStack)+__stack_size__); \
      memcpy(__src_stack_ptr__,__pthread_ptr__->bckup_stack,abs(__stack_size__)); \
      _sys_free(__pthread_ptr__->bckup_stack); \
}

   #define __inline_swap_signal_handler(__pthread_ptr__,__sig_handler__){ \
      /*modif for 3.06h version*/ \
      /*((OS_REGS OS_STACKPTR *)process_lst[pid]->pthread_ptr->tcb->pStack)->RetAdr4    = OS_MakeIntAdr(sig_handler);*/ \
      /* First return adr (see OS_Private.h)*/ \
      /*modif for 3.32 replace RetAdr4 by PC0 */ \
      /*((OS_REGS OS_STACKPTR *)__pthread_ptr__->tcb->pStack)->PC0= (OS_U32)(__sig_handler__);*/ \
      /*modif for 3.52e and 3.60 replace PC0 by PC */ \
      ((OS_REGS OS_STACKPTR *)__pthread_ptr__->tcb->pStack)->PC= (OS_U32)(__sig_handler__); \
      ((OS_REGS OS_STACKPTR *)__pthread_ptr__->tcb->pStack)->Counters= 0; \
      __pthread_ptr__->tcb->Timeout=0; \
      __pthread_ptr__->tcb->Stat=0; \
      OS_MakeTaskReady(__pthread_ptr__->tcb); \
}

/*TS_WAIT_TIME*/
   #define __inline_exit_signal_handler(__pthread_ptr__){ \
      __rstr_context(__pthread_ptr__->bckup_context,__pthread_ptr__); \
      /*process_lst[pid]->pthread_ptr->tcb->TASK_Timeout=0;*/ \
      /*process_lst[pid]->pthread_ptr->tcb->Stat=0;*/ \
}

   #define __set_active_pthread(__pthread_ptr__) \
   if(__pthread_ptr__) OS_MakeTaskReady(__pthread_ptr__->tcb)

//stop task switching and software timer.
   #define __atomic_in() OS_EnterRegion(); //stop task switching not the scheduler, kernel timeslice must be set to 0 cooprative mode).
//restart task switching
   #define __atomic_out() OS_LeaveRegion(); //restart task switching

   #if (__tauon_cpu_device__ == __tauon_cpu_device_arm7_at91m55800a__)
//stop timer (TIMER A0) tick for scheduler.TABSR.0=0;
      #define __stop_sched() __TC_CCRC0 &= ~(1);
//restart timer (TIMER A0) tick for scheduler.TABSR.0=1;
      #define __restart_sched() __TC_CCRC0 |= 1;
   #endif

   #if (__tauon_cpu_device__ == __tauon_cpu_device_arm7_at91sam7x__)
      #define __LEPTON_KAL_PIT_BASE     (0xFFFFFD30)
      #define __LEPTON_KAL_PIT_MR       (*(volatile OS_U32*) (__LEPTON_KAL_PIT_BASE + 0x00))
//stop timer (PIT periodic interval timer) tick for scheduler.PITIEN=0;
      #define __stop_sched() __LEPTON_KAL_PIT_MR &= ~(1<<25);
//restart timer (PIT periodic interval timer) tick for scheduler.PITIEN=1;
      #define __restart_sched() __LEPTON_KAL_PIT_MR |= (1<<25);
   #endif

   #if (__tauon_cpu_device__ == __tauon_cpu_device_arm9_at91sam9261__)
      #define __LEPTON_KAL_PIT_BASE     (0xFFFFFD30)
      #define __LEPTON_KAL_PIT_MR       (*(volatile OS_U32*) (__LEPTON_KAL_PIT_BASE + 0x00))
//stop timer (PIT periodic interval timer) tick for scheduler.PITIEN=0;
      #define __stop_sched() __LEPTON_KAL_PIT_MR &= ~(1<<25);
//restart timer (PIT periodic interval timer) tick for scheduler.PITIEN=1;
      #define __restart_sched() __LEPTON_KAL_PIT_MR |= (1<<25);
   #endif


//uninterruptible section in
   #define __disable_interrupt_section_in() OS_IncDI()
//uninterruptible section out
   #define __disable_interrupt_section_out() OS_DecRI()

//profiler macros for arm7 (at91m55800a)
   #if (__tauon_cpu_device__ == __tauon_cpu_device_arm7_at91m55800a__) && defined(KERNEL_PROFILER)
//
      #define PROFILER_PERIOD (1024.0/32000000.0)
//
      #define PROFILER_START_COUNTER_VALUE 0xFFFF
//
      #define __kernel_profiler_start(){ \
      __APMC_PCER |= 0x200; \
      __TC_BMR1   = 0x02; \
      __TC_CCR1C0 = 2; \
      __TC_CMR1C0 = 0x00004004; \
      __TC_RC1C1 = PROFILER_START_COUNTER_VALUE;   /*0x0bdb;*//*32000000/2/1000;*/ \
      __TC_CCR1C0 = 1; \
      __TC_CCR1C0 = 5; \
}
//
      #define __kernel_profiler_stop(__pthread_ptr__){ \
      __TC_CCR1C0 = 2; \
      if(__pthread_ptr__) \
         __pthread_ptr__->_profile_counter=__TC_CV1C0; \
}
//
      #define __kernel_profiler_get_counter(__pthread_ptr__) (__pthread_ptr__ ? __pthread_ptr__-> \
                                                              _profile_counter : 0)

//io
      #define __io_profiler_init(){ \
      __APMC_PCER |= 0x400; \
      __TC_CCR1C1 = 2; \
      __TC_BMR1   = 0x02; \
      __TC_CMR1C1 = 0x00000004; \
      __TC_RC1C1 = PROFILER_START_COUNTER_VALUE; \
      __TC_CCR1C1 = 1; \
      __TC_CCR1C1 = 5; \
}
//
      #define __io_profiler_start(__desc__){ \
      ofile_lst[__desc__]._profile_counter=(__TC_CV1C1); \
}
//
      #define __io_profiler_stop(__desc__){ \
      unsigned short __counter__ = __TC_CV1C1; \
      if(__counter__ > ofile_lst[__desc__]._profile_counter ) \
         ofile_lst[__desc__]._profile_counter=(__counter__)-ofile_lst[__desc__]._profile_counter; \
      else \
         ofile_lst[__desc__]._profile_counter= \
            (PROFILER_START_COUNTER_VALUE-ofile_lst[__desc__]._profile_counter)+__counter__; \
}
//
      #define __io_profiler_get_counter(__desc__) ofile_lst[__desc__]._profile_counter
//
   #endif //END KERNEL_PROFILER CPU_ARM7

//profiler macros for arm9 (at91sam9260 and at91sam9261)
   #if (__tauon_cpu_device__ == __tauon_cpu_device_arm9_at91sam9261__) && defined(KERNEL_PROFILER)

//
      #define PROFILER_PERIOD           (1.0/32000.0)
//
      #define PROFILER_START_COUNTER_VALUE 0xFFFF
//
//#define __kernel_profiler_start()
//#define __kernel_profiler_stop(__pid__)
//#define __kernel_profiler_get_counter(__pid__) (0)

      #define __kernel_profiler_start(){ \
      *AT91C_PMC_PCER |= 0x80000; \
      *AT91C_TCB0_BMR = 0x02; \
      *AT91C_TC2_CCR  = 2; \
      *AT91C_TC2_CMR  = 0x00004004; \
      *AT91C_TC2_RC   = PROFILER_START_COUNTER_VALUE; \
      *AT91C_TC2_CCR  = 1; \
      *AT91C_TC2_CCR  = 5; \
}

//
      #define __kernel_profiler_stop(__pthread_ptr__){ \
      *AT91C_TC2_CCR = 2; \
      if(__pthread_ptr__) \
         __pthread_ptr__->_profile_counter=*AT91C_TC2_CV; \
}
//
      #define __kernel_profiler_get_counter(__pthread_ptr__) (__pthread_ptr__ ? __pthread_ptr__-> \
                                                              _profile_counter : 0)

//
      #define __io_profiler_init(){ \
      *AT91C_PMC_PCER |= 0x40000; \
      *AT91C_TCB0_BMR = 0x01; \
      *AT91C_TC1_CCR  = 2; \
      *AT91C_TC1_CMR  = 0x00004004; \
      *AT91C_TC1_RC   = PROFILER_START_COUNTER_VALUE; \
      *AT91C_TC1_CCR  = 1; \
      *AT91C_TC1_CCR  = 5; \
}
//
      #define __io_profiler_start(__desc__){ \
      ofile_lst[__desc__]._profile_counter=(*AT91C_TC1_CV); \
}
//
      #define __io_profiler_stop(__desc__){ \
      unsigned short __counter__ = (*AT91C_TC1_CV); \
      if(__counter__ > ofile_lst[__desc__]._profile_counter ) \
         ofile_lst[__desc__]._profile_counter=(__counter__)-ofile_lst[__desc__]._profile_counter; \
      else \
         ofile_lst[__desc__]._profile_counter= \
            (PROFILER_START_COUNTER_VALUE-ofile_lst[__desc__]._profile_counter)+__counter__; \
}
//
      #define __io_profiler_get_counter(__desc__) ofile_lst[__desc__]._profile_counter

//
   #endif //END KERNEL_PROFILER CPU_ARM9

//profiling option not enabled (see in sys/root/src/kernel/core/kernelconf.h)
   #if !defined(KERNEL_PROFILER)
      #define __kernel_profiler_start()
      #define __kernel_profiler_stop(__pid__)
      #define __kernel_profiler_get_counter(__pid__)
      #define __io_profiler_init()
      #define __io_profiler_start(__desc__)
      #define __io_profiler_stop(__desc__)
      #define __io_profiler_get_counter(__desc__)
   #endif
#elif (defined(__GNUC__)) && defined(CPU_CORTEXM)
   #include <cyg/kernel/kapi.h>
   #include <cyg/hal/hal_arch.h>
   #include <string.h>

   #define __va_list_copy(__dest_va_list__,__src_va_list__) \
   memcpy((void *)&__dest_va_list__,(void *)&__src_va_list__,sizeof(__dest_va_list__))

typedef cyg_thread tcb_t;
typedef cyg_handle_t thr_id_t;

//contexte nécessaire pour la sauvegarde de contexte dans le thread
//!TODO : structures spéciales pour cette plateforem
//exception save registers
typedef struct svc_reg_st {
   unsigned int r0;
   unsigned int r1;
   unsigned int r2;
   unsigned int r3;
   unsigned int r12;
   unsigned int r14;
   unsigned int ret_addr;
   unsigned int xpsr;
} svc_reg_t;

typedef struct context_st {
   unsigned int user_stack_addr;
   unsigned int kernel_stack_addr;

   unsigned int save_user_ret_addr;
   unsigned int save_user_r14;

   unsigned int svc_r12;
   unsigned int * ret_addr;

   svc_reg_t * svc_regs;
} context_t;

//
typedef void (*_pthreadstart_routine_t)(void);
typedef _pthreadstart_routine_t pthreadstart_routine_t;

//macro de profilage (voir kal.h sur CVS)
/**
        début du zone de code atomique (non préemptible)
 *
 * \hideinitializer
 */
   #define __atomic_in() cyg_scheduler_lock();

/**
 * début du zone de code atomique (non préemptible)
 *
 * \hideinitializer
 */
   #define __atomic_out() cyg_scheduler_unlock();

   #define __begin_pthread(pthread_name) \
   void pthread_name(void){

   #define __end_pthread() \
   return; }

   #define _macro_stack_addr
   #define __is_thread_self(__pthread_ptr__) \
   (__pthread_ptr__->thr_id == cyg_thread_self())
//uninterruptible section in
   #define __disable_interrupt_section_in() cyg_interrupt_disable();
//uninterruptible section out
   #define __disable_interrupt_section_out() cyg_interrupt_enable();

   #define __stop_sched()

   #define __restart_sched()    //cyg_scheduler_start();

   #define __set_active_pthread(__pthread_ptr__)
//get the registers defined on HAL_SavedRegisters (except r0..4)
//and store it at the right place
   #define __get_thread_context(__context__) { \
}

//get the main registers at the beginning of the thread
   #define __inline_bckup_thread_start_context(__context__,__pthread_ptr__){ \
      __context__.user_stack_addr = __pthread_ptr__->tcb->stack_base+ \
                                    __pthread_ptr__->tcb->stack_size; \
}

//save the stack of current thread
   #define __inline_bckup_stack(__pthread_ptr__){ \
      int offsetEsp; \
      void* src_stack_ptr; \
      offsetEsp =  __pthread_ptr__->bckup_context.user_stack_addr - \
                  __pthread_ptr__->start_context.user_stack_addr; \
      src_stack_ptr = (char *)(__pthread_ptr__->start_context.user_stack_addr+offsetEsp); \
      __pthread_ptr__->bckup_stack = (char*)malloc( abs(offsetEsp) ); \
      if(!__pthread_ptr__->bckup_stack) \
         return -ENOMEM; \
      memcpy(__pthread_ptr__->bckup_stack,src_stack_ptr,abs(offsetEsp)); \
      __pthread_ptr__->tcb->thread_data[0] = abs(offsetEsp); \
}
//restore the stack content
   #define __inline_rstr_stack(__pthread_ptr__){ \
      int offsetEsp; \
      void* src_stack_ptr; \
      offsetEsp = __pthread_ptr__->bckup_context.user_stack_addr - \
                  __pthread_ptr__->start_context.user_stack_addr; \
      src_stack_ptr = (char *)(__pthread_ptr__->start_context.user_stack_addr+offsetEsp); \
      memcpy(src_stack_ptr,__pthread_ptr__->bckup_stack,abs(offsetEsp)); \
      free(__pthread_ptr__->bckup_stack); \
}

//PEUT ETRE INTEGRER _BCKUP_STACK et 8BCKUP_CONTEXT
//commentaire voir dummy_swap_signal_handler
// ATTENTION REVENIR SUR LES CASTS DES QUE POSSIBLE
//is thread waiting for a sig see $ECOS_REPOSITORY/kernel/current/include/thread.hxx
   #define __inline_swap_signal_handler(__pthread_ptr__,__sig_handler__)


//restaure le contexte du programme interropu par un signal
//recopie de la pile sauvegardée dans pile thread courant
//ecrase contexte sauvegardée lors retour appel système
   #define __inline_exit_signal_handler(__pthread_ptr__)

//PEUT ETRE REECRIRE BCKUP_CONTEXT ET RSTR_CONTEXT
//save the current tcb
   #define __inline_bckup_context(__context__,__pthread_ptr__)

//restore the previous tcb
   #define __inline_rstr_context(__context__,__pthread_ptr__)

   #define __bckup_thread_start_context(__context__,__pthread_ptr__) \
   __inline_bckup_thread_start_context(__context__,__pthread_ptr__)

   #define __bckup_context(__context__,__pthread_ptr__) \
   __inline_bckup_context(__context__,__pthread_ptr__)

   #define __bckup_stack(__pthread_ptr__) \
   __inline_bckup_stack(__pthread_ptr__)

   #define __rstr_stack(__pthread_ptr__) \
   __inline_rstr_stack(__pthread_ptr__)

   #define __rstr_context(__context__,__pthread_ptr__) \
   __inline_rstr_context(__context__,__pthread_ptr__)

   #define __swap_signal_handler(__pthread_ptr__,sig_handler) \
   __inline_swap_signal_handler(__pthread_ptr__,sig_handler)

   #define __exit_signal_handler(__pthread_ptr__) \
   __inline_exit_signal_handler(__pthread_ptr__)

//profiling for ARM9
   #if (__tauon_cpu_device__ == __tauon_cpu_device_cortexm_k60n512__) && defined(KERNEL_PROFILER)

      #define PROFILER_PERIOD         1/32000
      #define __kernel_profiler_start()
      #define __kernel_profiler_stop(__pid__)
      #define __kernel_profiler_get_counter(__pid__)  0
      #define __io_profiler_init()
      #define __io_profiler_start(__desc__)
      #define __io_profiler_stop(__desc__)
      #define __io_profiler_get_counter(__desc__)     0

   #else
      #define PROFILER_PERIOD         1/32000
      #define __kernel_profiler_start()
      #define __kernel_profiler_stop(__pid__)
      #define __kernel_profiler_get_counter(__pid__)  0
      #define __io_profiler_init()
      #define __io_profiler_start(__desc__)
      #define __io_profiler_stop(__desc__)
      #define __io_profiler_get_counter(__desc__)     0
   #endif
#else
/**
* structure de contexte utilis par le micro-noyau\n
* cette structure contient gnralement toutes les informations ncessaire  la commutation de tache:\n
*  1) sauvegarde de certains registres du processeur comme le pointeur de pile par exemple.\n
*  2) sauvegarde de du compteur programme.\n
* \n
* ce sont les deux informations les plus importantes pour lepton. Elle permettent de raliser le vfork()
* et la gestion des signaux avec kill().
* \hideinitializer
*/
typedef CONTEXT context_t;

/**
 * definition du prototype de fonction de la tache gre par le micro-noyau
 *
 * \param pthread_name nom de la fonction
 *
 * \hideinitializer
 */
   #define __begin_pthread(pthread_name)

/**
 * definition de la sortie de fonction de la tache gre par le micro-noyau
 *
 * \hideinitializer
 */
   #define __end_pthread()

/**
 * permet de savoir si le task control block tcb est bien celui de la tache courante
 *
 * \param tcb task control block
 *
 * \hideinitializer
 */
   #define __is_thread_self(tcb)

/**
 * permet de sauvegarder le contexte du thread __pthread_ptr dans context.
 *
 * \param context variable de type context_t dans laquelle sera sauvegarde le contexte.
 * \param __pthread_ptr pointeur sur la structure pthread_t du pthread.
 *
 * \note voir les fonctions _sys_vfork(), _sys_krnl_exec(), _sys_exec().
 * \hideinitializer
 */
   #define __bckup_context(context,__pthread_ptr)

/**
 * permet de restaurer le contexte context dans celui du thread __pthread_ptr.
 *
 * \param context variable de type context_t dans laquelle est plac le contexte  restaurer.
 * \param __pthread_ptr pointeur sur la structure pthread_t du pthread.
 *
 * \note voir les fonctions _sys_vfork(), _sys_vfork_exit(), _sys_krnl_exec(), _sys_exec().
 * \hideinitializer
 */
   #define __rstr_context(context,__pthread_ptr)

/**
 * permet de sauvegarder la pile d'un process
 *
 * \param pid pid du process dont il faut sauvegarder la pile.
 *
 * \note voir les fonctions _sys_vfork(), _sys_krnl_exec(), _sys_exec().
 * \hideinitializer
 */
   #define __bckup_stack(pid)

/**
 * permet de restaurer la pile d'un process
 *
 * \param pid pid du process dont il faut restaurer la pile.
 *
 * \note voir les fonctions _sys_vfork(), _sys_krnl_exec(), _sys_exec().
 * \hideinitializer
 */
   #define __rstr_stack(pid)

/**
 * permet de drouter le flux d'excution d'un process par la fonction _sys_kill().
 *
 * \param pid pid du process dont le flux d'excution doit tre drouter.
 * \param sig_handler address de la fonction sighandler() (voir kernel/signal.c)
 *
 * \note le droutage du flux d'excution est obtenue en modifiant l'addresse de retour d'interruption du scheduler.
 * cette addresse de retour est gnralement place dans la structure qui permet de sauvegarder le contexte context_t de la tche
 * lors de l'appel de l'ordonanceur par l'interuption du timer qui contrle la premption (le tick).
 * cette structure de contexte context_t depend du micro-noyau utilis.
 *
 * \hideinitializer
 */
   #define __swap_signal_handler(pid,sig_handler)

/**
 * permet de restaurer, aprs le droutement par __swap_signal_handler(), le flux d'excution d'un process.
 *
 * \param pid pid du process dont le flux d'excution doit tre restaurer.
 */
   #define __exit_signal_handler(pid)

/**
 * permet de rveiller un process.
 *
 * \param pid pid du process que l'on doit rveiller.
 * \note voir _kernel_timer() dans kernel/kernel.c
 */
   #define __set_active_pid(pid)

/**
 * dbut du zone de code atomique (non premptible)
 *
 * \hideinitializer
 */
   #define __atomic_in() OS_EnterRegion(); //stop task switching and the scheduler, kernel timeslice must be set to 0 cooprative mode).

/**
 * dbut du zone de code atomique (non premptible)
 *
 * \hideinitializer
 */
   #define __atomic_out() OS_LeaveRegion(); //restart task switching and scheduler.

/**
 * arrte le timer qui appel rgulirement l'ordonanceur.
 *
 * \note voir _syscall_execve(), _syscall_vfork(), _syscall_kill(), _syscall_exit(), _syscall_atexit(), _syscall_sigexit().
 */
   #define __stop_sched()

/**
 *  redmarre le timer qui appel rgulirement l'ordonanceur.
 *
 * \note voir _syscall_execve(), _syscall_vfork(), _syscall_kill(), _syscall_exit(), _syscall_atexit(), _syscall_sigexit().
 */
   #define __restart_sched()

#endif

#if 0
#ifdef USE_DEBUG_KAL

void _debug_bckup_thread_start_context(CONTEXT* __context__,
                                       struct kernel_pthread_st *__pthread_ptr__);
void _debug_bckup_context(CONTEXT* __context__,struct kernel_pthread_st *__pthread_ptr__);
void _debug_rstr_context(CONTEXT* __context__,struct kernel_pthread_st *__pthread_ptr__);
int _debug_bckup_stack(struct kernel_pthread_st *__pthread_ptr__);
void _debug_rstr_stack(struct kernel_pthread_st *__pthread_ptr__);
void _debug_swap_signal_handler(struct kernel_pthread_st *__pthread_ptr__,void* sig_handler);
void _debug_inline_exit_signal_handler(struct kernel_pthread_st *__pthread_ptr__);


   #define __bckup_thread_start_context(__context__, \
                                        __pthread_ptr__) _debug_bckup_thread_start_context( \
      &__context__,__pthread_ptr__)
   #define __bckup_context(__context__,__pthread_ptr__)              _debug_bckup_context( \
      &__context__,__pthread_ptr__)
   #define __rstr_context(__context__,__pthread_ptr__)               _debug_rstr_context( \
      &__context__,__pthread_ptr__)
   #define __bckup_stack(__pthread_ptr__)                            _debug_bckup_stack( \
      __pthread_ptr__)
   #define __rstr_stack(__pthread_ptr__)                             _debug_rstr_stack( \
      __pthread_ptr__)
   #define __swap_signal_handler(__pthread_ptr__,sig_handler)        _debug_swap_signal_handler( \
      __pthread_ptr__,sig_handler)
   #define __exit_signal_handler(__pthread_ptr__)                    _debug_exit_signal_handler( \
      __pthread_ptr__)

#else

   #define __bckup_thread_start_context(__context__, \
                                        __pthread_ptr__) __inline_bckup_thread_start_context( \
      __context__,__pthread_ptr__)
   #define __bckup_context(__context__,__pthread_ptr__)              __inline_bckup_context( \
      __context__,__pthread_ptr__)
   #define __rstr_context(__context__,__pthread_ptr__)               __inline_rstr_context( \
      __context__,__pthread_ptr__)
   #define __bckup_stack(__pthread_ptr__)                            __inline_bckup_stack( \
      __pthread_ptr__)
   #define __rstr_stack(__pthread_ptr__)                             __inline_rstr_stack( \
      __pthread_ptr__)
   #define __swap_signal_handler(__pthread_ptr__,sig_handler)        __inline_swap_signal_handler( \
      __pthread_ptr__,sig_handler)
   #define __exit_signal_handler(__pthread_ptr__)                    __inline_exit_signal_handler( \
      __pthread_ptr__)

#endif
#endif

/** @} */
/** @} */

#endif
