
#ifndef __SYS_MSVC_ARCH_H__
#define __SYS_MSVC_ARCH_H__


#include "kernel/kernel_pthread.h"

#define SYS_MBOX_NULL (sys_mbox_t)NULL
#define SYS_SEM_NULL  (sys_sem_t)NULL


typedef OS_MAILBOX*           sys_mbox_t;
typedef OS_CSEMA*             sys_sem_t;
typedef kernel_pthread_t *    sys_thread_t;

#endif

