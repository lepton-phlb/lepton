
#ifndef __SYS_MSVC_ARCH_H__
#define __SYS_MSVC_ARCH_H__


#include "kernel/core/kernel_pthread.h"
#include "kernel/core/kernel_pthread_mutex.h"
#include "kernel/core/kernel_sem.h"

typedef cyg_handle_t mbox_hdl;
typedef cyg_mbox mbox_t;

typedef struct {
   //OS_MAILBOX  os_mailbox;
   mbox_hdl mb_handle;
   mbox_t mbox;
}sys_mbox_st;

typedef sys_mbox_st* sys_mbox_t;

//typedef OS_CSEMA*                sys_sem_t;
typedef cyg_sem_t *                                     sys_sem_t;
typedef kernel_pthread_t *       sys_thread_t;
typedef kernel_pthread_mutex_t*  sys_prot_t;

#define SYS_MBOX_NULL (sys_mbox_t)NULL
#define SYS_SEM_NULL  (sys_sem_t)NULL

#endif

