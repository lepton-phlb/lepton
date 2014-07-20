/*===========================================
Compiler Directive
=============================================*/
#ifndef _USER_KERNEL_MKCONF_H_
#define _USER_KERNEL_MKCONF_H_

//see kernel/core/kernelconf.h
//specific target include for at91samd20 xplaind pro board
extern unsigned int dev_at91samd20_xplained_pro_board_clock_source_get_hz(void);

//free rtos configuration
#include "kernel/core/ucore/freeRTOS_8-0-0/source/arch/cortex-m0+/at91samd20/FreeRTOSConfig.h"

//force definition of cpu device
#define __tauon_cpu_device__ __tauon_cpu_device_cortexM0_at91samd20__

//use pipe
#define __tauon_kernel_profile__ __tauon_kernel_profile_minimal__
//#define __KERNEL_PIPE_SIZE 256

#define __KERNEL_RTFS_NODETBL_SIZE 32
#define __KERNEL_RTFS_BLOCK_SIZE 16
#define __KERNEL_RTFS_NODE_BLOCK_NB_MAX 10


#define __KERNEL_STDIO_PRINTF_BUFSIZ 64

//force EFFS for stm32f407 on olimex-stm32-p407 board
#define __file_system_profile__  __file_system_profile_classic__
#define __KERNEL_VFS_SUPPORT_EFFS   0

//ip stack
//#define USE_UIP 
//#define USE_LWIP
//#define USE_IF_ETHERNET


/*===========================================
Includes
=============================================*/


/*===========================================
Declaration
=============================================*/

#endif
