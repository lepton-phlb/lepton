//generated from c:/tauon/sys/user/tauon-ijinus/etc/mkconf_tauon_basic_stm32f1_uip.xml by mklepton
/*===========================================
Compiler Directive
=============================================*/
#ifndef _KERNEL_MKCONF_H
#define _KERNEL_MKCONF_H

/*===========================================
Includes
=============================================*/

#include "c:/tauon/sys/root/src/kernel/core/arch/arm/dev_dskimg.h" 

/*===========================================
Declaration
=============================================*/

#define CPU_ARM7 

#define __KERNEL_CPU_FREQ 80000000L

#define __KERNEL_HEAP_SIZE 10000

#define __KERNEL_PTHREAD_MAX 10

#define __KERNEL_PROCESS_MAX 8

#define MAX_OPEN_FILE 16

#define OPEN_MAX 8

#define __KERNEL_ENV_PATH {"/usr","/usr/sbin","/usr/bin","usr/bin/net"}

#define __KERNEL_NET_IPSTACK


#endif