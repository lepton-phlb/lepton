
#ifndef __SYS_MSVC_ARCH_H__
#define __SYS_MSVC_ARCH_H__


#include "kernel/core/kernel_pthread.h"
#include "kernel/core/kernel_pthread_mutex.h"


/* Use this pragma instead of the one below to disable all
warnings     */
#pragma warning(disable:4761;disable:4244)



#define SYS_MBOX_NULL (sys_mbox_t)NULL
#define SYS_SEM_NULL  (sys_sem_t)NULL


typedef struct {
   OS_MAILBOX os_mailbox;
   char* p_buf;
}sys_mbox_st;

typedef sys_mbox_st* sys_mbox_t;

typedef OS_CSEMA*             sys_sem_t;
typedef kernel_pthread_t *    sys_thread_t;
typedef OS_RSEMA*             sys_prot_t;

#endif

