
#ifndef __SYS_ARM_ARCH_H__
#define __SYS_ARM_ARCH_H__


#include "kernel/core/kernel_pthread.h"
#include "kernel/core/kernel_pthread_mutex.h"


/* Use this pragma instead of the one below to disable all
warnings     */
#pragma warning(disable:4761;disable:4244)

#define SYS_MBOX_NULL (sys_mbox_t)NULL
#define SYS_SEM_NULL  (sys_sem_t)NULL

//
typedef struct {
#if defined(__KERNEL_UCORE_EMBOS)
   OS_MAILBOX os_mailbox;
#elif defined(__KERNEL_UCORE_FREERTOS)
   xQueueHandle os_mailbox;   
#else
   long  os_mailbox;
#endif

   char* p_buf;
}sys_mbox_st;

//
typedef kernel_pthread_t *     sys_thread_t;
typedef kernel_pthread_mutex_t* sys_prot_t;
typedef sys_mbox_st* sys_mbox_t;

//
#if defined(__KERNEL_UCORE_EMBOS)
   typedef OS_CSEMA* sys_sem_t;
#elif defined(__KERNEL_UCORE_FREERTOS)
   typedef xSemaphoreHandle sys_sem_t;
#else
   typedef void*  sys_sem_t;
#endif

   
//
#endif

