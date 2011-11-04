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
#ifndef _INTERRUPT_H
#define _INTERRUPT_H


/*===========================================
Includes
=============================================*/
//#include "kernel/core/kernel_pthread.h"
//#include "kernel/core/kernel_sem.h"
/*===========================================
Declaration
=============================================*/
/**
 * \addtogroup lepton_kernel
 * @{
 */

/**
 * \addtogroup syscall les appels systme
 * @{
 *
 */

//
typedef unsigned char kernel_intr_t;
#define KERNEL_INTERRUPT               0x01
#define KERNEL_RET_INTERRUPT           0x01//0x02

#if defined(__KERNEL_IO_EVENT)
   #define KERNEL_IO_INTERRUPT            0x02
   #define SYSTEM_IO_INTERRUPT            0x02//0x04
#endif
//see  pthread.h #define PTHREAD_EVENT                  0x04//0x10

#if defined(USE_SEGGER)
//

/**
 *
 * Ces macros sont dfinies et spcialises en fonction du micro-noyau utilis.
 * Les interruptions noyau, dans le cas de l'utilisation d'un micro-noyau segger,
 * sont gnres par les fonctions de gnration d'vnements.
 * les interruption noyau peuvent tre gnres par des appels systme en provenance de processus
 * ou par des interruptions au niveau des priphriques d'e/s.
 *
 */


/**
 *
 *  numero d'interruption noyau pour les appels systmes
 *
 */
#define KERNEL_INTERRUPT_NB         0x80

/**
 *
 *  numero d'interruption noyau pour le stack ip
 *
 */
#define KERNEL_NET_INTERRUPT_NB     0x81


//
   //extern kernel_sem_t kernel_io_sem;

   /**
    * compteur de ticks du scheduler
    *
    * return le nombre de ticks
    * \hideinitializer
    */
   #define  __kernel_get_timer_ticks() OS_Time

   /**
    * interdiction des interruptions matrielles
    *
    * \hideinitializer
    */
   #define __clr_irq() __disable_interrupt_section_in()

   /**
    * autorisation des interruptions matrielles
    *
    * \hideinitializer
    */
   #define __set_irq() __disable_interrupt_section_out()

   /**
    * suspension de la tache en cours
    *
    * \param useconds temps de la suspention en micro secondes
    * \hideinitializer
    */
   #define __kernel_usleep(useconds){\
      ldiv_t lr =ldiv(useconds,1000);\
      if(lr.quot)OS_Delay(lr.quot);\
   }

   /**
    * met le noyau lepton (le thread kernel (voir kernel_routine() ) en attente d'une interruption noyau
    *
    * \hideinitializer
    */
   #define __kernel_wait_int() OS_WaitEvent(KERNEL_INTERRUPT)




   /**
    * debut d'un appel systme
    *
    * \param pid du processus qui appelant
    * \hideinitializer
    */
   #define __kernel_begin_syscall(__pthread_ptr__)\
      __pthread_ptr__->stat|=PTHREAD_STATUS_KERNEL;

   /**
    * fin d'un appel systme
    *
    * \param pid du processus qui appelant
    * \hideinitializer
    */
   #define __kernel_end_syscall(__pthread_ptr__)\
      __pthread_ptr__->stat&=~PTHREAD_STATUS_KERNEL;


   /**
    * signal retour d'une interruption noyau. (utilis au niveau noyau voir __wait_ret_int() )
    *
    * \param pid du processus qui a lanc cette interruption noyau
    * \hideinitializer
    */
   #define __kernel_ret_int(__pthread_ptr__){\
      __pthread_ptr__->irq_nb=0x00;\
      if(!(__pthread_ptr__->stat&PTHREAD_STATUS_KERNEL)){\
         /*OS_ClearEvents(&process_lst[pid]->pthread_ptr->tcb);*/\
         /*OS_SignalEvent(KERNEL_RET_INTERRUPT,&process_lst[__pid__]->pthread_ptr->tcb);*/\
         OS_SignalEvent(KERNEL_RET_INTERRUPT,__pthread_ptr__->tcb);\
      }\
   }


   /**
    * lancement d'une interruption noyau
    *
    * \param pid du processus qui lance cette interruption noyau
    * \param irq_nb numero d'interruption noyau
    * \hideinitializer
    */
   #define __make_interrupt(__pthread_ptr__,irq_nb){\
      if(irq_nb==KERNEL_INTERRUPT_NB){\
         /*OS_ClearEvents(&kernel_thread.tcb);*/\
         OS_ClearEvents(__pthread_ptr__->tcb);\
         OS_SignalEvent (KERNEL_INTERRUPT,kernel_thread.tcb);\
      }\
   }


   /**
    * attente du retour de l'interruption noyau (utilis au niveau processus voir __kernel_ret_int() )
    *
    * \hideinitializer
    */
   #define __wait_ret_int(){\
      OS_WaitEvent(KERNEL_RET_INTERRUPT);\
   }


   /**
    * signal une interruption materiel d'un pilote de priphrique d'e/s (utilis au niveau priphrique voir les priphriques d'e/s)
    *
    * \param pid du processus qui utilise ce priphrique
    * \hideinitializer
    */
   #if defined(__KERNEL_IO_SEM)
      #define __fire_io_int(__pthread_ptr__){\
         if(__pthread_ptr__!=((kernel_pthread_t*)0))\
            kernel_sem_post(&__pthread_ptr__->io_sem);\
      }
   #elif defined(__KERNEL_IO_EVENT)
      #define __fire_io_int(__pthread_ptr__){\
         if(__pthread_ptr__!=((kernel_pthread_t*)0))\
            OS_SignalEvent (SYSTEM_IO_INTERRUPT,__pthread_ptr__->tcb);\
      }
   #endif

   //if KERNEL_RET_INTERRUPT event then wait wans interrupted by posix signal.
   /**
    * attente d' une interruption materiel en provenance d'un pilote de priphrique d'e/s(utilis au niveau processus voir les priphriques d'e/s)
    *
    * \hideinitializer
    */
   #if defined(__KERNEL_IO_SEM)
      #define __wait_io_int(__pthread_ptr__) kernel_sem_wait(&__pthread_ptr__->io_sem)
   #elif defined(__KERNEL_IO_EVENT)
      #define __wait_io_int(__pthread_ptr__) OS_WaitEvent(SYSTEM_IO_INTERRUPT|KERNEL_RET_INTERRUPT)
   #endif


   //if KERNEL_RET_INTERRUPT event then wait wans interrupted by posix signal.
   /**
    * attente d' une interruption materiel en provenance d'un pilote de priphrique d'e/s (utilis au niveau processus voir les priphriques d'e/s)
    *
    * \param timeout dure maximale de l'attente
    * \return 0 si il y eu une interrurption et -1 si l'attente est arrive  chance.
    * \hideinitializer
    */
   #if defined(__KERNEL_IO_SEM)
      #define __wait_io_int2(__pthread_ptr__,__timeout__)  kernel_sem_timedwait(&__pthread_ptr__->io_sem,0,__timeout__)
      #define __wait_io_int3(__pthread_ptr__,__timeout__)  kernel_sem_timedwait(&__pthread_ptr__->io_sem,0,__timeout__)
   #elif defined(__KERNEL_IO_EVENT)
      #define __wait_io_int2(__pthread_ptr__,__timeout__) (OS_WaitEventTimed(SYSTEM_IO_INTERRUPT|KERNEL_RET_INTERRUPT,__time_s_to_ms((__timeout__)->tv_sec)+__time_ns_to_ms((__timeout__)->tv_nsec))? 0:-1)
      #define __wait_io_int3(__pthread_ptr__,__timeout__)  OS_WaitEventTimed(SYSTEM_IO_INTERRUPT|KERNEL_RET_INTERRUPT,__time_s_to_ms((__timeout__)->tv_sec)+__time_ns_to_ms((__timeout__)->tv_nsec)))
   #endif

   #if defined(__KERNEL_IO_SEM)
      #define __wait_io_int_abstime(__pthread_ptr__,__abs_timeout__) kernel_sem_timedwait(&__pthread_ptr__->io_sem,TIMER_ABSTIME,__abs_timeout__)
   #elif defined(__KERNEL_IO_EVENT)
      #define __wait_io_int_abstime(__pthread_ptr__,__abs_timeout__) OS_WaitEventTimed(SYSTEM_IO_INTERRUPT|KERNEL_RET_INTERRUPT,__time_s_to_ms((__abs_timeout__)->tv_sec)+__time_ns_to_ms((__abs_timeout__)->tv_nsec))? 0:-1)
   #endif


   //hardware interrupt declaration
   #ifdef WIN32
      #define __hw_interrupt(itv,fn)\
         void fn(void);\
         PFN_INTERRUPT interrupt##itv=fn;\
         void fn(void)
   #else
      #define __hw_interrupt(itv,fn)\
         interrupt [itv] void fn(void)
   #endif

   /**
    * debut d'une fonction d'interruption (voir les priphriques d'e/s)
    *
    * \hideinitializer
    */
   #define __hw_enter_interrupt()\
      OS_EnterInterrupt();

   /**
    * fin d'une fonction d'interruption (voir les priphriques d'e/s)
    *
    * \hideinitializer
    */
   #define __hw_leave_interrupt()\
      OS_LeaveInterrupt();

#elif defined(USE_ECOS) && defined(CPU_GNU32)
//   extern kernel_sem_t kernel_io_sem;

   #include <cyg/hal/hal_io.h>
   #include <cyg/hal/hal_intr.h>
   #include "kernel/core/core_event.h"
   #include "kernel/core/kernel_sem.h"

   	/**numero d'interruption noyau pour les appels systèmes*/
   	#define KERNEL_INTERRUPT_NB         CYG_HAL_SYS_SIGUSR1
   	/**
      * debut d'un appel système
      *
      * \param pid du processus qui appelant
      * \hideinitializer
      */
     	#define __kernel_begin_syscall(__pthread_ptr__)\
   		__pthread_ptr__->stat|=PTHREAD_STATUS_KERNEL;

     	/**
      * fin d'un appel système
      *
      * \param pid du processus qui appelant
      * \hideinitializer
      */
     	#define __kernel_end_syscall(__pthread_ptr__)\
   		__pthread_ptr__->stat&=~PTHREAD_STATUS_KERNEL;

   	extern int cyg_hal_sys_kill(pid_t pid, int sig);
   	extern int cyg_hal_sys_getpid(void);
   	/**
       * lancement d'une interruption noyau
       * Attention au 0 à la place du pid (A REVOIR)
       * \param pid du processus qui lance cette interruption noyau
       * \param irq_nb numero d'interruption noyau
       * \hideinitializer
       */
   	#define __make_interrupt(__pthread_ptr__,irq_nb){\
   		if(irq_nb==KERNEL_INTERRUPT_NB){\
   			cyg_hal_sys_kill(cyg_hal_sys_getpid(), KERNEL_INTERRUPT_NB);\
   		}\
   	}

   	#define __wait_ret_int()
   	#define __kernel_wait_int()

   	#define __kernel_ret_syscall(__pthread_ptr__){\
         __pthread_ptr__->irq_nb=0x00;\
         if(__pthread_ptr__->stat&PTHREAD_STATUS_KERNEL){\
            __make_interrupt(__pthread_ptr__, KERNEL_INTERRUPT_NB);\
         }\
      }

		#define __kernel_ret_int(__pthread_ptr__)
   	/**
       * signal une interruption materiel d'un pilote de périphérique d'e/s (utilisé au niveau périphérique voir les périphériques d'e/s)
       *
       * \param pid du processus qui utilise ce périphérique
       * \hideinitializer
       */
   	#if defined(__KERNEL_IO_SEM)
      	#define __fire_io_int(__pthread_ptr__){\
            if(__pthread_ptr__!=((kernel_pthread_t*)0))\
               kernel_sem_post(&__pthread_ptr__->io_sem);\
         }
      #elif defined(__KERNEL_IO_EVENT)
         #define __fire_io_int(__pthread_ptr__) {\
            if(__pthread_ptr__!=((kernel_pthread_t*)0))\
               core_event_set_flags(__pthread_ptr__,KERNEL_IO_INTERRUPT);\
      }
   	#endif
   //if KERNEL_RET_INTERRUPT event then wait wans interrupted by posix signal.
      /**
       * attente d' une interruption materiel en provenance d'un pilote de périphérique d'e/s(utilisé au niveau processus voir les périphériques d'e/s)
       *
       * \hideinitializer
       */

      #if defined(__KERNEL_IO_SEM)
         #define __wait_io_int(__pthread_ptr__) kernel_sem_wait(&__pthread_ptr__->io_sem)
         #define __wait_io_int2(__pthread_ptr__,__timeout__)  kernel_sem_timedwait(&__pthread_ptr__->io_sem,0,__timeout__)
         #define __wait_io_int3(__pthread_ptr__,__timeout__)  kernel_sem_timedwait(&__pthread_ptr__->io_sem,0,__timeout__)
         #define __wait_io_int_abstime(__pthread_ptr__,__abs_timeout__) kernel_sem_timedwait(&__pthread_ptr__->io_sem,TIMER_ABSTIME,__abs_timeout__)
      #elif defined(__KERNEL_IO_EVENT)
         #define __wait_io_int(__pthread_ptr__)    core_event_wait(KERNEL_IO_INTERRUPT)
         #define __wait_io_int2(__pthread_ptr__,__timeout__)  ((core_event_timed_wait(KERNEL_IO_INTERRUPT, __timeout__)<0)? 0:-1)
         #define __wait_io_int3(__pthread_ptr__,__timeout__)  core_event_timed_wait(KERNEL_IO_INTERRUPT, __timeout__)
         #define __wait_io_int_abstime(__pthread_ptr__,__abs_timeout__) (core_event_timed_wait(KERNEL_IO_INTERRUPT, __time_s_to_ms((abs_timeout)->tv_sec)+__time_ns_to_ms((abs_timeout)->tv_nsec))<0? 0:-1)
   	#endif
   	/**
      * interdiction des interruptions matérielles
      *
      * \hideinitializer
      */
     	#define __clr_irq() __disable_interrupt_section_in()

     	/**
      * autorisation des interruptions matérielles
      *
      * \hideinitializer
      */
     	#define __set_irq() __disable_interrupt_section_out()

   	//get current ticks of timer counter
     	#define __get_timer_ticks()		cyg_current_time()
		#define __kernel_get_timer_ticks()	cyg_current_time()

      #define tick_to_msec(tick) ((unsigned int)((tick)*10+1))
      #define msec_to_tick(msec) ((cyg_tick_count_t)(msec+9)/10)

     	#define __kernel_usleep(useconds){\
   		ldiv_t lr=ldiv(useconds,1000);\
   		if(lr.quot)		cyg_thread_delay(msec_to_tick(lr.quot));\
      }

   	//for suspend ans resume thread
      #define __kernel_pthread_suspend(__pthread_ptr__)  cyg_thread_suspend(__pthread_ptr__->thr_id)
      #define __kernel_pthread_resume(__pthread_ptr__)   cyg_thread_resume(__pthread_ptr__->thr_id)
      #define __kernel_pthread_release(__pthread_ptr__)  cyg_thread_release(__pthread_ptr__->thr_id)

   #elif defined(USE_ECOS) && (defined(CPU_ARM7) || defined(CPU_ARM9))

//   extern kernel_sem_t kernel_io_sem;
   #include <cyg/hal/hal_io.h>
   #include <cyg/hal/hal_intr.h>
   #include "kernel/core/core_event.h"

   	/**numero d'interruption noyau pour les appels systèmes*/
   	#define KERNEL_INTERRUPT_NB         0x80
   	/**
      * debut d'un appel système
      *
      * \param pid du processus qui appelant
      * \hideinitializer
      */
     	#define __kernel_begin_syscall(__pthread_ptr__)\
   		__pthread_ptr__->stat|=PTHREAD_STATUS_KERNEL;

     	/**
      * fin d'un appel système
      *
      * \param pid du processus qui appelant
      * \hideinitializer
      */
     	#define __kernel_end_syscall(__pthread_ptr__)\
   		__pthread_ptr__->stat&=~PTHREAD_STATUS_KERNEL;

   	/**
       * lancement d'une interruption noyau
       * Attention au 0 à la place du pid (A REVOIR)
       * \param pid du processus qui lance cette interruption noyau
       * \param irq_nb numero d'interruption noyau
       * \hideinitializer
       */
      #define __make_interrupt(__pthread_ptr__,irq_nb){\
         if(irq_nb==KERNEL_INTERRUPT_NB){\
   		__asm__ volatile("swi 0x00");\
         }\
      }
   	#define __wait_ret_int()
   	#define __kernel_wait_int()

		#define __kernel_ret_syscall(__pthread_ptr__){\
         __pthread_ptr__->irq_nb=0x00;\
         if(__pthread_ptr__->stat&PTHREAD_STATUS_KERNEL){\
            __make_interrupt(__pthread_ptr__, KERNEL_INTERRUPT_NB);\
         }\
      }

		#define __kernel_ret_int(__pthread_ptr__)
   /**
       * signal une interruption materiel d'un pilote de périphérique d'e/s (utilisé au niveau périphérique voir les périphériques d'e/s)
       *
       * \param pid du processus qui utilise ce périphérique
       * \hideinitializer
       */
      #if defined(__KERNEL_IO_SEM)
         #define __fire_io_int(__pthread_ptr__){\
            if(__pthread_ptr__!=((kernel_pthread_t*)0))\
               kernel_sem_post(&__pthread_ptr__->io_sem);\
         }
      #elif defined(__KERNEL_IO_EVENT)
         #define __fire_io_int(__pthread_ptr__) {\
            if(__pthread_ptr__!=((kernel_pthread_t*)0))\
               core_event_set_flags(__pthread_ptr__,KERNEL_IO_INTERRUPT);\
      }
      #endif

   	//get current ticks of timer counter
     	#define __get_timer_ticks()		cyg_current_time()
		#define __kernel_get_timer_ticks()	cyg_current_time()
   	//if KERNEL_RET_INTERRUPT event then wait wans interrupted by posix signal.
      /**
       * attente d' une interruption materiel en provenance d'un pilote de périphérique d'e/s(utilisé au niveau processus voir les périphériques d'e/s)
       *
       * \hideinitializer
       */

      #if defined(__KERNEL_IO_SEM)
         #define __wait_io_int(__pthread_ptr__) kernel_sem_wait(&__pthread_ptr__->io_sem)
         #define __wait_io_int2(__pthread_ptr__,__timeout__)  kernel_sem_timedwait(&__pthread_ptr__->io_sem,0,__timeout__)
         #define __wait_io_int3(__pthread_ptr__,__timeout__)  kernel_sem_timedwait(&__pthread_ptr__->io_sem,0,__timeout__)
         #define __wait_io_int_abstime(__pthread_ptr__,__abs_timeout__) kernel_sem_timedwait(&__pthread_ptr__->io_sem,TIMER_ABSTIME,__abs_timeout__)
      #elif defined(__KERNEL_IO_EVENT)
         #define __wait_io_int(__pthread_ptr__)    core_event_wait(KERNEL_IO_INTERRUPT)
         #define __wait_io_int2(__pthread_ptr__,__timeout__)  ((core_event_timed_wait(KERNEL_IO_INTERRUPT, __timeout__)<0)? 0:-1)
         #define __wait_io_int3(__pthread_ptr__,__timeout__)  core_event_timed_wait(KERNEL_IO_INTERRUPT, __timeout__)
         #define __wait_io_int_abstime(__pthread_ptr__,__abs_timeout__) (core_event_timed_wait(KERNEL_IO_INTERRUPT, __time_s_to_ms((abs_timeout)->tv_sec)+__time_ns_to_ms((abs_timeout)->tv_nsec))<0? 0:-1)
      #endif

   	/**
      * interdiction des interruptions matérielles
      *
      * \hideinitializer
      */
     	#define __clr_irq() __disable_interrupt_section_in()

     	/**
      * autorisation des interruptions matérielles
      *
      * \hideinitializer
      */
     	#define __set_irq() __disable_interrupt_section_out()

     	#define __kernel_usleep(useconds){\
   		ldiv_t lr=ldiv(useconds,1000);\
   		if(lr.quot)		cyg_thread_delay(lr.quot);\
      }

      //
      #define tick_to_msec(tick) ((unsigned int)(tick))
      #define msec_to_tick(msec) ((cyg_tick_count_t)(msec))

      //for suspend ans resume thread
      #define __kernel_pthread_suspend(__pthread_ptr__)  cyg_thread_suspend(__pthread_ptr__->thr_id)
      #define __kernel_pthread_resume(__pthread_ptr__)   cyg_thread_resume(__pthread_ptr__->thr_id)
      #define __kernel_pthread_release(__pthread_ptr__)  cyg_thread_release(__pthread_ptr__->thr_id)

#elif defined(USE_ECOS) && defined(CPU_CORTEXM)

//   extern kernel_sem_t kernel_io_sem;
   #include <cyg/hal/hal_io.h>
   #include <cyg/hal/hal_intr.h>
   #include "kernel/core/core_event.h"

   	/**numero d'interruption noyau pour les appels systèmes*/
   	#define KERNEL_INTERRUPT_NB         0x80
   	/**
      * debut d'un appel système
      *
      * \param pid du processus qui appelant
      * \hideinitializer
      */
     	#define __kernel_begin_syscall(__pthread_ptr__)\
   		__pthread_ptr__->stat|=PTHREAD_STATUS_KERNEL;

     	/**
      * fin d'un appel système
      *
      * \param pid du processus qui appelant
      * \hideinitializer
      */
     	#define __kernel_end_syscall(__pthread_ptr__)\
   		__pthread_ptr__->stat&=~PTHREAD_STATUS_KERNEL;

   	/**
       * lancement d'une interruption noyau
       * Attention au 0 à la place du pid (A REVOIR)
       * \param pid du processus qui lance cette interruption noyau
       * \param irq_nb numero d'interruption noyau
       * \hideinitializer
       */
      #define __make_interrupt(__pthread_ptr__,irq_nb){\
         if(irq_nb==KERNEL_INTERRUPT_NB){\
        	 do_swi();\
         }\
      }
   	#define __wait_ret_int()
   	#define __kernel_wait_int()

		#define __kernel_ret_syscall(__pthread_ptr__){\
         __pthread_ptr__->irq_nb=0x00;\
         if(__pthread_ptr__->stat&PTHREAD_STATUS_KERNEL){\
            __make_interrupt(__pthread_ptr__, KERNEL_INTERRUPT_NB);\
         }\
      }

		#define __kernel_ret_int(__pthread_ptr__)
   /**
       * signal une interruption materiel d'un pilote de périphérique d'e/s (utilisé au niveau périphérique voir les périphériques d'e/s)
       *
       * \param pid du processus qui utilise ce périphérique
       * \hideinitializer
       */
      #if defined(__KERNEL_IO_SEM)
         #define __fire_io_int(__pthread_ptr__){\
            if(__pthread_ptr__!=((kernel_pthread_t*)0))\
               kernel_sem_post(&__pthread_ptr__->io_sem);\
         }
      #elif defined(__KERNEL_IO_EVENT)
         #define __fire_io_int(__pthread_ptr__) {\
            if(__pthread_ptr__!=((kernel_pthread_t*)0))\
               core_event_set_flags(__pthread_ptr__,KERNEL_IO_INTERRUPT);\
      }
      #endif

   	//get current ticks of timer counter
     	#define __get_timer_ticks()		cyg_current_time()
		#define __kernel_get_timer_ticks()	cyg_current_time()
   	//if KERNEL_RET_INTERRUPT event then wait wans interrupted by posix signal.
      /**
       * attente d' une interruption materiel en provenance d'un pilote de périphérique d'e/s(utilisé au niveau processus voir les périphériques d'e/s)
       *
       * \hideinitializer
       */

      #if defined(__KERNEL_IO_SEM)
         #define __wait_io_int(__pthread_ptr__) kernel_sem_wait(&__pthread_ptr__->io_sem)
         #define __wait_io_int2(__pthread_ptr__,__timeout__)  kernel_sem_timedwait(&__pthread_ptr__->io_sem,0,__timeout__)
         #define __wait_io_int3(__pthread_ptr__,__timeout__)  kernel_sem_timedwait(&__pthread_ptr__->io_sem,0,__timeout__)
         #define __wait_io_int_abstime(__pthread_ptr__,__abs_timeout__) kernel_sem_timedwait(&__pthread_ptr__->io_sem,TIMER_ABSTIME,__abs_timeout__)
      #elif defined(__KERNEL_IO_EVENT)
         #define __wait_io_int(__pthread_ptr__)    core_event_wait(KERNEL_IO_INTERRUPT)
         #define __wait_io_int2(__pthread_ptr__,__timeout__)  ((core_event_timed_wait(KERNEL_IO_INTERRUPT, __timeout__)<0)? 0:-1)
         #define __wait_io_int3(__pthread_ptr__,__timeout__)  core_event_timed_wait(KERNEL_IO_INTERRUPT, __timeout__)
         #define __wait_io_int_abstime(__pthread_ptr__,__abs_timeout__) (core_event_timed_wait(KERNEL_IO_INTERRUPT, __time_s_to_ms((abs_timeout)->tv_sec)+__time_ns_to_ms((abs_timeout)->tv_nsec))<0? 0:-1)
      #endif

   	/**
      * interdiction des interruptions matérielles
      *
      * \hideinitializer
      */
     	#define __clr_irq() __disable_interrupt_section_in()

     	/**
      * autorisation des interruptions matérielles
      *
      * \hideinitializer
      */
     	#define __set_irq() __disable_interrupt_section_out()

     	#define __kernel_usleep(useconds){\
   		ldiv_t lr=ldiv(useconds,1000);\
   		if(lr.quot)		cyg_thread_delay(lr.quot);\
      }

      //
      #define tick_to_msec(tick) ((unsigned int)(tick))
      #define msec_to_tick(msec) ((cyg_tick_count_t)(msec))

      //for suspend ans resume thread
      #define __kernel_pthread_suspend(__pthread_ptr__)  cyg_thread_suspend(__pthread_ptr__->thr_id)
      #define __kernel_pthread_resume(__pthread_ptr__)   cyg_thread_resume(__pthread_ptr__->thr_id)
      #define __kernel_pthread_release(__pthread_ptr__)  cyg_thread_release(__pthread_ptr__->thr_id)
   	
   	  //for svc exception
	  #define KERNEL_STACK_ALIGN_MASK		0xfffffff8

   #elif defined(USE_KERNEL_STATIC)
   	/**numero d'interruption noyau pour les appels systèmes*/
   	#define KERNEL_INTERRUPT_NB         0
   	/**
      * debut d'un appel système
      *
      * \param pid du processus qui appelant
      * \hideinitializer
      */
     	#define __kernel_begin_syscall(__pthread_ptr__)\
   		__pthread_ptr__->stat|=PTHREAD_STATUS_KERNEL;

     	/**
      * fin d'un appel système
      *
      * \param pid du processus qui appelant
      * \hideinitializer
      */
     	#define __kernel_end_syscall(__pthread_ptr__)\
   		__pthread_ptr__->stat&=~PTHREAD_STATUS_KERNEL;

   	/**
       * lancement d'une interruption noyau
       * Attention au 0 à la place du pid (A REVOIR)
       * \param pid du processus qui lance cette interruption noyau
       * \param irq_nb numero d'interruption noyau
       * \hideinitializer
       */
   	#define __make_interrupt(__pthread_ptr__,irq_nb)

   	#define __wait_ret_int()
   	#define __kernel_wait_int()

   	#define __kernel_ret_syscall(__pthread_ptr__){\
      	__pthread_ptr__->irq_nb=0x00;\
         if(__pthread_ptr__->stat&PTHREAD_STATUS_KERNEL){\
         	__make_interrupt(__pthread_ptr__, KERNEL_INTERRUPT_NB);\
         }\
      }

   		#define __kernel_ret_int(__pthread_ptr__)

   	/**
       * signal une interruption materiel d'un pilote de périphérique d'e/s (utilisé au niveau périphérique voir les périphériques d'e/s)
       *
       * \param pid du processus qui utilise ce périphérique
       * \hideinitializer
       */
   	#define __fire_io_int(__pthread_ptr__)
   //if KERNEL_RET_INTERRUPT event then wait wans interrupted by posix signal.
      /**
       * attente d' une interruption materiel en provenance d'un pilote de périphérique d'e/s(utilisé au niveau processus voir les périphériques d'e/s)
       *
       * \hideinitializer
       */

   	#define __wait_io_int(__pthread_ptr__)
   	#define __wait_io_int2(__pthread_ptr__,timeout)
      #define __wait_io_int3(__pthread_ptr__,timeout)
      #define __wait_io_int_abstime(__pthread_ptr__,__abs_timeout__)
   	/**
      * interdiction des interruptions matérielles
      *
      * \hideinitializer
      */
     	#define __clr_irq()

     	/**
      * autorisation des interruptions matérielles
      *
      * \hideinitializer
      */
     	#define __set_irq()

   	//get current ticks of timer counter
   	#define __get_timer_ticks()		0
		#define __kernel_get_timer_ticks()	0
   	#define __kernel_usleep(useconds)

	//
	#define tick_to_msec(tick) ((tick)*10+1))
   #define msec_to_tick(msec) ((msec+9)/10)
   
   //for suspend ans resume thread
   #define __kernel_pthread_suspend(__pthread_ptr__)
   #define __kernel_pthread_resume(__pthread_ptr__)
   #define __kernel_pthread_release(__pthread_ptr__)
#endif


/** @} */
/** @} */

#endif
