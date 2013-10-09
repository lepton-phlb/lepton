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
#ifndef _KERNEL_H
#define _KERNEL_H

/**
 * \addtogroup lepton_kernel
 * @{
 */

/**
 * \defgroup syscall les appels systme
 * @{
 * Le systmes d'exploitation fournit un ensemble de service au processus utilisateur.
 * L'ensemble des services fournis sont accessibles par l'intermdiaire d'un ensemble de fonctions,
 * dont le nom et le paramtrage dfinissent les actions ralises.
 * Dans le cas de lepton cette api respecte la norme posix 1003.1.
 * Ces fonctions appellent le noyau en lui communiquant le type de service demand ainsi que les paramtres affrents.
 * Cette tche est ralise par les macros dfinies dans la section 3.6.3.3 .
 * Cette demande de service est ensuite traite par le noyau.

 *
 * Une fonction standard encapsule l'appel systme de la manire suivante :\n
 *  1) Remplissage de la structure de donne associ  la fonction systmes qui sera excute.\n
 *  2) Utilisation de la macro __mk_syscall() avec syscall_nb qui est le numro de la fonction systme qui sera excute par le kernel et pdata le pointeur sur la structure de donne pralablement renseigne.\n
 *  3) la macro __mk_syscall rcupre le pid du processus courant.\n
 *  4) la macro __mk_syscall prend la main sur le smaphore du kernel ou attend sa libration. En effet pour des contraintes d'intgrit du systme un seul processus,  la fois, peut accder au kernel. Le smaphore sera libr par le kernel.\n
 *  5) la macro __mk_syscall renseigne la structure de donnes de l'appel systmes du processus courant.\n
 *  6) la macro __mk_syscall  gnre l'interruption systmes :  __make_interrupt().\n
 *  7) la macro __mk_syscall attend la fin du traitement de l'interruption systme : __wait_ret_int().\n
 *  8) le kernel est en attente d'une interruption systme, lorsque cette dernire est gnre par le processus (voir tape 6), le kernel recherche quel processus a lanc cette interruption systme.\n
 *  9) le kernel lit la structure de donnes de l'appel systme du processus et rcupre  le codede  la fonction systme qui doit tre excut.\n
 * 10) le kernel rcupre les donnes associes  la fonction systme par l'intermdiaire du pointeur de donne sur cette dernire.\n
 * 11) le kernel appel la fonction systmes demand.\n
 * 12) le kernel signale que le traitement de l'interruption systme est termin (voir tape 7).\n
 * 13) le kernel libre le smaphore des appels systme (voir tape 4).\n
 *
 */
/*===========================================
Includes
=============================================*/
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/kernel_pthread_mutex.h"
#include "kernel/core/interrupt.h"
#include "kernel/fs/vfs/vfstypes.h"
#include "kernel/core/kal.h"
/*===========================================
Declaration
=============================================*/

extern const char * _kernel_date;
extern const char * _kernel_time;

#define __KERNEL_DATE   _kernel_date
#define __KERNEL_TIME   _kernel_time

#define __kernel_date__   _kernel_date
#define __kernel_time__   _kernel_time


#define __KERNEL_ALARM_TIMER 100 //ms

enum  _syscall_enum_t {
   _SYSCALL_WAITPID=0,
   _SYSCALL_EXECVE,
   _SYSCALL_EXIT,
   _SYSCALL_KILL,
   _SYSCALL_VFORK,
   _SYSCALL_SIGPROCMASK,
   _SYSCALL_SIGPENDING,
   _SYSCALL_SIGACTION,
#if defined(USE_SEGGER)
   _SYSCALL_ALARM,
#endif
   _SYSCALL_PAUSE,
   _SYSCALL_MOUNT,
   _SYSCALL_UMOUNT,
   _SYSCALL_OPENDIR,
   _SYSCALL_CLOSEDIR,
   _SYSCALL_READDIR,
   _SYSCALL_REWINDDIR,
   _SYSCALL_TELLDIR,
   _SYSCALL_SEEKDIR,
   _SYSCALL_CREAT,
   _SYSCALL_OPEN,
   _SYSCALL_CLOSE,
   _SYSCALL_READ,
   _SYSCALL_WRITE,
   _SYSCALL_SEEK,
   _SYSCALL_STAT,
   _SYSCALL_MKDIR,
   _SYSCALL_RMDIR,
   _SYSCALL_CHDIR,
   _SYSCALL_DUP,
   _SYSCALL_DUP2,
   _SYSCALL_PIPE,
   _SYSCALL_SIGEXIT,
   _SYSCALL_MKNOD,
   _SYSCALL_FTRUNCATE,
   _SYSCALL_TRUNCATE,
   _SYSCALL_RM,
   _SYSCALL_FSTATVFS,
   _SYSCALL_STATVFS,
   _SYSCALL_REMOVE,
   _SYSCALL_GETTIMEOFDAY,
#if defined(USE_SEGGER)
   _SYSCALL_ATEXIT,
#endif
   _SYSCALL_MALLOC,
   _SYSCALL_CALLOC,
   _SYSCALL_REALLOC,
   _SYSCALL_FREE,
   _SYSCALL_FCNTL,
   _SYSCALL_GETCWD,
   _SYSCALL_FSTAT,
   _SYSCALL_MKFS,
   _SYSCALL_SETPGID,
   _SYSCALL_GETPGRP,
   _SYSCALL_SETTIMEOFDAY,
   _SYSCALL_SYNC,
   _SYSCALL_FATTACH,
   _SYSCALL_FDETACH,
   _SYSCALL_IOCTL,
   _SYSCALL_SYSCTL,
   _SYSCALL_RENAME,
   _SYSCALL_PTHREAD_CREATE,
   _SYSCALL_PTHREAD_CANCEL,
   _SYSCALL_PTHREAD_EXIT,
#if defined(USE_SEGGER)
   _SYSCALL_PTHREAD_KILL,
#endif
   _SYSCALL_PTHREAD_MUTEX_INIT,
   _SYSCALL_PTHREAD_MUTEX_DESTROY,
   _SYSCALL_PTHREAD_COND_INIT,
   _SYSCALL_PTHREAD_COND_DESTROY,
   _SYSCALL_TIMER_CREATE,
   _SYSCALL_TIMER_DELETE,
   _SYSCALL_SEM_INIT,
   _SYSCALL_SEM_DESTROY,
   _SYSCALL_TOTAL_NB //do not remove this declaration
};


#define KERNEL_IN_STATIC_MODE   1
#define KERNEL_OUT_STATIC_MODE  0
/**
 * kernel en mode statique
 * \hideinitializer
 */
extern volatile int _kernel_in_static_mode;

/**
 * entr du kernel en mode statique
 * \hideinitializer
 */
#define __kernel_static_mode_in() (_kernel_in_static_mode=KERNEL_IN_STATIC_MODE)

/**
 * sortie du kernel du mode statique
 * \hideinitializer
 */
#define __kernel_static_mode_out() (_kernel_in_static_mode=KERNEL_OUT_STATIC_MODE)

/**
 * le kernel est-il en mode statique?
 * \hideinitializer
 */
#define __kernel_is_in_static_mode() (_kernel_in_static_mode==KERNEL_IN_STATIC_MODE)

/**
 * verrou sur le noyau
 * \hideinitializer
 */
extern kernel_pthread_mutex_t kernel_mutex;

/**
 * thread du noyau
 * \hideinitializer
 */
extern kernel_pthread_t kernel_thread;

void _start_kernel(char* arg);
void _stop_kernel(void);

/**
 * lancement du noyau
 * \hideinitializer
 */
#define __start_kernel() _start_kernel(0)

/**
 * pose un verrou exclusif sur les appels systme vers le noyau
 * \hideinitializer
 */
#define __syscall_lock()      kernel_pthread_mutex_lock(&kernel_mutex)
/**
 * tente de poser un verrou exclusif sur les appels systme vers le noyau
 * \hideinitializer
 */
#define __syscall_trylock()   kernel_pthread_mutex_trylock(&kernel_mutex)

/**
 * libre le verrou exclusif sur les appels systme vers le noyau
 * \hideinitializer
 */
#define __syscall_unlock()    kernel_pthread_mutex_unlock(&kernel_mutex)

typedef int (*SYSCALL)(kernel_pthread_t* pthread_ptr, pid_t pid, void* data);
typedef SYSCALL PSYSCALL;
typedef SYSCALL p_syscall_t;


#ifdef KERNEL_PROFILER
typedef struct {
   pid_t pid;
   unsigned short counter;
   const char* pname;
}kernel_profiler_result_t;

extern kernel_profiler_result_t kernel_profiler_result_lst[_SYSCALL_TOTAL_NB];

typedef struct {
   unsigned short counter[4];
   unsigned long nbacces[4];
   unsigned short size[4];
   float avg[4];
   float rate_max[4];
   float rate_min[4];
   const char* pname;
}io_profiler_result_t;

extern io_profiler_result_t*       io_profiler_result_lst;

typedef struct {
   p_syscall_t p_syscall;
   const char* p_syscall_name;
}kernel_syscall_t;
   #define __add_syscall(__syscall_name__) {__syscall_name__,# __syscall_name__ }

   #define __profiler_add_result(__pthread_ptr__,__syscall_nb__,__counter__) \
   kernel_profiler_result_lst[__syscall_nb__].pid = __pthread_ptr__->pid; \
   kernel_profiler_result_lst[__syscall_nb__].counter = __counter__; \
   kernel_profiler_result_lst[__syscall_nb__].pname = \
      kernel_syscall_lst[__syscall_nb__].p_syscall_name;

   #define __io_profiler_add_result(__desc__,__mode__,__size__,__counter__){ \
   int __dev_nb__ = ofile_lst[__desc__].ext.dev; \
   float __time__ = (float)((__counter__) * PROFILER_PERIOD); \
   float __rate__=(float)0.0; \
   if(__dev_nb__<__KERNEL_DEV_MAX) {  /*protect for ext.dev from BSD socket type device*/ \
      if(__time__) { \
         __rate__ = (float)( (__size__)/(__time__) ); \
         if(__rate__>io_profiler_result_lst[__dev_nb__].rate_max[__mode__]) { \
            io_profiler_result_lst[__dev_nb__].rate_max[__mode__]=__rate__; \
         }else if(__rate__<io_profiler_result_lst[__dev_nb__].rate_min[__mode__]) { \
            io_profiler_result_lst[__dev_nb__].rate_min[__mode__]=__rate__; \
         }else{ \
            io_profiler_result_lst[__dev_nb__].rate_max[__mode__]=__rate__; \
            io_profiler_result_lst[__dev_nb__].rate_min[__mode__]=__rate__; \
         } \
      } \
      io_profiler_result_lst[__dev_nb__].nbacces[__mode__]++; \
      io_profiler_result_lst[__dev_nb__].size[__mode__]=__size__; \
      io_profiler_result_lst[__dev_nb__].counter[__mode__]= __counter__; \
      io_profiler_result_lst[__dev_nb__].avg[__mode__]= \
         (float)((io_profiler_result_lst[__dev_nb__].avg[__mode__]+__rate__)/(float)2.0); \
      io_profiler_result_lst[__dev_nb__].pname=ofile_lst[__desc__].pfsop->fdev.dev_name; \
   } \
}


#else
typedef struct {
   p_syscall_t p_syscall;
}kernel_syscall_t;
   #define  __add_syscall(__syscall_name__) {__syscall_name__}
   #define __profiler_add_result(__pthread_ptr__,__syscall_nb__,__counter__)
   #define __io_profiler_add_result(__desc__,__mode__,__size__,__counter__)
#endif

extern kernel_syscall_t const kernel_syscall_lst[];

/**
 * pid du processus qui a gnr l'appel systme
 * \hideinitializer
 */
extern volatile pid_t _syscall_owner_pid;

/**
 * fixe le pid du processus qui a gnr l'appel systme
 * \param pid du processus
 * \hideinitializer
 */
#define __set_syscall_owner_pid(__pid__) _syscall_owner_pid = __pid__

/**
 * obtention du pid du processus qui a gnr l'appel systme
 * \return pid du processus
 * \hideinitializer
 */
#define __get_syscall_owner_pid() _syscall_owner_pid



extern kernel_pthread_t* _syscall_owner_pthread_ptr;

/**
 * fixe le pointeur sur pthread qui a gnr l'appel systme
 * \param pid du processus
 * \hideinitializer
 */
#define __set_syscall_owner_pthread_ptr(__pthread_ptr__) _syscall_owner_pthread_ptr = \
   __pthread_ptr__

/**
 * obtention du pointeur sur pthread qui a gnr l'appel systme
 * \return pid du processus
 * \hideinitializer
 */
#define __get_syscall_owner_pthread_ptr() _syscall_owner_pthread_ptr

//errno macro routine
extern int __g_kernel_static_errno;

#define __kernel_set_errno(__errno__) \
   if(!__kernel_is_in_static_mode()) { \
      if(_syscall_owner_pthread_ptr && \
         !_syscall_owner_pthread_ptr->_errno) _syscall_owner_pthread_ptr->_errno=__errno__; \
   }else{ \
      __g_kernel_static_errno=__g_kernel_static_errno; \
   }

//RTC specific function
#define __kernel_dev_settime(__desc,__buf,__size){ \
   if(ofile_lst[desc].pfsop->fdev.pfdev_ext) \
      ((fdev_rtc_t*)(ofile_lst[desc].pfsop->fdev.pfdev_ext))->fdev_rtc_settime(__desc,__buf,__size); \
}

#define __kernel_dev_gettime(__desc,__buf,__size){ \
   if(ofile_lst[desc].pfsop->fdev.pfdev_ext) \
      ((fdev_rtc_t*)(ofile_lst[desc].pfsop->fdev.pfdev_ext))->fdev_rtc_gettime(__desc,__buf,__size); \
}


//
//WARNING __syscall_lock(); unlock by kernel (see kernel.c)

#if defined(USE_SEGGER)
//
/**
 * gnre un appel systme
 *
 * \param syscall_nb numro de l'appel systme
 * \param pdata pointeur sur les donnes lies  cet appel systme
 *
 * \note le numro est spcifi par syscall_nb et les donnes associes par cet appel sont accessibles par le kernel au moyen d'un pointeur sur la structure de donnes.
 * Cette macro est utiliss par toutes les fonctions standards (kill, exec, fork, etc).
 */
   #define __mk_syscall(__syscall_nb__,__pdata__){ \
   kernel_pthread_t* __pthread_ptr__; \
   __pthread_ptr__ = kernel_pthread_self(); \
   __syscall_lock(); \
   __kernel_profiler_start(); \
   __pthread_ptr__->reg.from_pthread_ptr = __pthread_ptr__; \
   __pthread_ptr__->reg.syscall=__syscall_nb__; \
   __pthread_ptr__->reg.data=(void*)&__pdata__; \
   __pthread_ptr__->irq_nb=KERNEL_INTERRUPT_NB; \
   __pthread_ptr__->irq_prior=-1; \
   __atomic_in(); \
   __make_interrupt(__pthread_ptr__,__pthread_ptr__->irq_nb); \
   __atomic_out(); \
   __wait_ret_int(); \
   __kernel_profiler_stop(__pthread_ptr__); \
   __profiler_add_result(__pthread_ptr__,__syscall_nb__, \
                         __kernel_profiler_get_counter(__pthread_ptr__)); \
}

//no blocking call

   #define __mk_syscall0(__syscall_nb__,__pdata__){ \
   kernel_pthread_t* __pthread_ptr__; \
   __pthread_ptr__ = kernel_pthread_self(); \
   __syscall_lock(); \
   __kernel_profiler_start(); \
   __pthread_ptr__->reg.from_pthread_ptr = __pthread_ptr__; \
   __pthread_ptr__->reg.syscall=__syscall_nb__; \
   __pthread_ptr__->reg.data=(void*)&__pdata__; \
   __pthread_ptr__->irq_nb=KERNEL_INTERRUPT_NB; \
   __pthread_ptr__->irq_prior=-1; \
   __atomic_in(); \
   __make_interrupt(__pthread_ptr__,__pthread_ptr__->irq_nb); \
   __atomic_out(); \
   __wait_ret_int(); /*to remove????*/ \
   __kernel_profiler_stop(__pthread_ptr__); \
   __profiler_add_result(__pthread_ptr__,__syscall_nb__, \
                         __kernel_profiler_get_counter(__pthread_ptr__)); \
}

/**
 * gnre un appel systme sans passage de donnes
 *
 * \param syscall_nb numro de l'appel systme
 *
 * \ note c'est le mme fonctionnement que pour __mk_syscall().
 * \hideinitializer
 */
   #define __mk_syscall2(__syscall_nb__){ \
   kernel_pthread_t* __pthread_ptr__; \
   __pthread_ptr__ = kernel_pthread_self(); \
   __syscall_lock(); \
   __pthread_ptr__->reg.from_pthread_ptr = __pthread_ptr__; \
   __pthread_ptr__->reg.syscall=__syscall_nb__; \
   __pthread_ptr__->reg.data=(void*)0; \
   __pthread_ptr__->irq_nb=KERNEL_INTERRUPT_NB; \
   __pthread_ptr__->irq_prior=-1; \
   __atomic_in(); \
   __make_interrupt(__pthread_ptr__,__pthread_ptr__->irq_nb); \
   __atomic_out(); \
   __wait_ret_int(); \
}

#elif defined(__GNUC__) && (defined(CPU_ARM7) || defined(CPU_ARM9) || defined(CPU_GNU32))
//
   #define __mk_syscall(__syscall_nb__,__pdata__){ \
   kernel_pthread_t* __pthread_ptr__; \
   __pthread_ptr__ = kernel_pthread_self(); \
   __pthread_ptr__->reg.from_pthread_ptr = __pthread_ptr__; \
   __pthread_ptr__->reg.syscall=__syscall_nb__; \
   __pthread_ptr__->reg.data=(void*)&__pdata__; \
   __pthread_ptr__->irq_nb=KERNEL_INTERRUPT_NB; \
   __pthread_ptr__->irq_prior=-1; \
   __clr_irq(); \
   __make_interrupt(__pthread_ptr__,__pthread_ptr__->irq_nb); \
   __set_irq(); \
}

   #define __mk_syscall0(__syscall_nb__,__pdata__){ \
   kernel_pthread_t* __pthread_ptr__; \
   __pthread_ptr__ = kernel_pthread_self(); \
   __pthread_ptr__->reg.from_pthread_ptr = __pthread_ptr__; \
   __pthread_ptr__->reg.syscall=__syscall_nb__; \
   __pthread_ptr__->reg.data=(void*)&__pdata__; \
   __pthread_ptr__->irq_nb=KERNEL_INTERRUPT_NB; \
   __pthread_ptr__->irq_prior=-1; \
   __clr_irq(); \
   __make_interrupt(__pthread_ptr__,__pthread_ptr__->irq_nb); \
   __set_irq(); \
}

   #define __mk_syscall2(__syscall_nb__){ \
   kernel_pthread_t* __pthread_ptr__; \
   __pthread_ptr__ = kernel_pthread_self(); \
   __pthread_ptr__->reg.from_pthread_ptr = __pthread_ptr__; \
   __pthread_ptr__->reg.syscall=__syscall_nb__; \
   __pthread_ptr__->reg.data=(void*)0; \
   __pthread_ptr__->irq_nb=KERNEL_INTERRUPT_NB; \
   __pthread_ptr__->irq_prior=-1; \
   __clr_irq(); \
   __make_interrupt(__pthread_ptr__,__pthread_ptr__->irq_nb); \
   __set_irq(); \
}

#elif defined(__GNUC__) && defined(CPU_CORTEXM)
//
   #define __mk_syscall(__syscall_nb__,__pdata__){ \
   kernel_pthread_t* __pthread_ptr__; \
   __pthread_ptr__ = kernel_pthread_self(); \
   __pthread_ptr__->reg.from_pthread_ptr = __pthread_ptr__; \
   __pthread_ptr__->reg.syscall=__syscall_nb__; \
   __pthread_ptr__->reg.data=(void*)&__pdata__; \
   __pthread_ptr__->irq_nb=KERNEL_INTERRUPT_NB; \
   __pthread_ptr__->irq_prior=-1; \
   __make_interrupt(__pthread_ptr__,__pthread_ptr__->irq_nb); \
}

   #define __mk_syscall0(__syscall_nb__,__pdata__){ \
   kernel_pthread_t* __pthread_ptr__; \
   __pthread_ptr__ = kernel_pthread_self(); \
   __pthread_ptr__->reg.from_pthread_ptr = __pthread_ptr__; \
   __pthread_ptr__->reg.syscall=__syscall_nb__; \
   __pthread_ptr__->reg.data=(void*)&__pdata__; \
   __pthread_ptr__->irq_nb=KERNEL_INTERRUPT_NB; \
   __pthread_ptr__->irq_prior=-1; \
   __make_interrupt(__pthread_ptr__,__pthread_ptr__->irq_nb); \
}

   #define __mk_syscall2(__syscall_nb__){ \
   kernel_pthread_t* __pthread_ptr__; \
   __pthread_ptr__ = kernel_pthread_self(); \
   __pthread_ptr__->reg.from_pthread_ptr = __pthread_ptr__; \
   __pthread_ptr__->reg.syscall=__syscall_nb__; \
   __pthread_ptr__->reg.data=(void*)0; \
   __pthread_ptr__->irq_nb=KERNEL_INTERRUPT_NB; \
   __pthread_ptr__->irq_prior=-1; \
   __make_interrupt(__pthread_ptr__,__pthread_ptr__->irq_nb); \
}
#endif
/**
 * rinitialise la structure de donnes du processus qui contient les informations sur l'appel systme en cours.
 *
 * \param pid du processus qui a lanc l'appel systeme.
 *
 * \note cette macro n'est utils que dans le noyau.
 */
#define __flush_syscall(__pthread_ptr__){ \
   __pthread_ptr__->irq_nb=0x00; \
}

#define __K_IS_SYSCALL(__INTR) ((__INTR) &KERNEL_INTERRUPT)
#define __K_IS_IOINTR(__INTR) ((__INTR) &SYSTEM_IO_INTERRUPT)

#define _SYSCALL_NET_SND         100

//cpu device
extern fdev_map_t*   __g_kernel_cpu;
extern desc_t __g_kernel_desc_cpu;

#define __set_cpu(__p_kernel_cpu__) __g_kernel_cpu = __p_kernel_cpu__
#define __get_cpu() __g_kernel_cpu

#define __set_cpu_desc(__desc_kernel_cpu__) __g_kernel_desc_cpu = __desc_kernel_cpu__
#define __get_cpu_desc() __g_kernel_desc_cpu

//i2c interface
extern fdev_map_t*   __g_kernel_if_i2c_master;
extern desc_t __g_kernel_desc_if_i2c_master;

#define __set_if_i2c_master(__p_if_i2c_master__) __g_kernel_if_i2c_master = __p_if_i2c_master__
#define __get_if_i2c_master() __g_kernel_if_i2c_master

#define __set_if_i2c_master_desc(__desc_if_i2c_master__) __g_kernel_desc_if_i2c_master = \
   __desc_if_i2c_master__
#define __get_if_i2c_master_desc() __g_kernel_desc_if_i2c_master

extern kernel_pthread_mutex_t _i2c_core_mutex;

#define _i2c_lock() kernel_pthread_mutex_lock(&_i2c_core_mutex);
#define _i2c_unlock() kernel_pthread_mutex_unlock(&_i2c_core_mutex);

//spi interface
extern fdev_map_t*   __g_kernel_if_spi_master;
extern desc_t __g_kernel_desc_if_spi_master;

#define __set_if_spi_master(__p_if_spi_master__) __g_kernel_if_spi_master = __p_if_spi_master__
#define __get_if_spi_master() __g_kernel_if_spi_master

#define __set_if_spi_master_desc(__desc_if_spi_master__) __g_kernel_desc_if_spi_master = \
   __desc_if_spi_master__
#define __get_if_spi_master_desc() __g_kernel_desc_if_spi_master

extern kernel_pthread_mutex_t _spi_core_mutex;

#define _spi_lock()   kernel_pthread_mutex_lock  (&_spi_core_mutex);
#define _spi_unlock() kernel_pthread_mutex_unlock(&_spi_core_mutex);

#if defined(__GNUC__)
//gestion des syscall sous synthetic
void _init_syscall(void);
void _kernel_routine(void* arg);

//wrapper for external call
   #ifndef USE_ECOS
void __wrpr_kernel_dev_gettime(desc_t __desc, char * __buf, int __size);
   #endif

   #if defined(CPU_GNU32) && !defined(USE_KERNEL_STATIC)
void _kernel_syscall_handler_synth(int sig, cyg_hal_sys_siginfo_t * info, void *ptr);
   #elif defined(CPU_ARM7) || defined(CPU_ARM9)
void _kernel_syscall_handler(cyg_addrword_t data, cyg_code_t number, cyg_addrword_t info);
   #elif defined(CPU_CORTEXM)
void do_swi(void);
void _kernel_syscall_handler(void);
   #endif

#endif

//trace in kernel for system call
#define KERNEL_SYSCALL_STATUS_START       0x01
#define KERNEL_SYSCALL_STATUS_END         0x02
#define KERNEL_SYSCALL_STATUS_ENDERROR    0x03
//
typedef unsigned char kernel_syscall_status_t;
//
typedef struct {
   pid_t _syscall_owner_pid;
   kernel_pthread_t*  _syscall_owner_pthread_ptr;
   kernel_syscall_t _kernel_syscall;
   kernel_syscall_status_t kernel_syscall_status;
}_kernel_syscall_trace_t;
extern _kernel_syscall_trace_t _g_kernel_syscall_trace;

/** @} */
/** @} */

#endif

