/*===========================================
Compiler Directive
=============================================*/
#ifndef _USER_KERNEL_MKCONF_H_
#define _USER_KERNEL_MKCONF_H_

//see kernel/core/kernelconf.h

//force definition of cpu device
#define __tauon_cpu_device__ __tauon_cpu_device_cortexM4_stm32f4__

//force EFFS for stm32f407 on olimex-stm32-p407 board
#define __file_system_profile__  __file_system_profile_classic__
#define __KERNEL_VFS_SUPPORT_EFFS   0

//ip stack
//#define USE_UIP 
//#define USE_LWIP
#define USE_IF_ETHERNET
//specific target include for ijinus
#include "kernel/dev/arch/cortexm/stm32f4xx/target.h"

/*===========================================
Includes
=============================================*/


/*===========================================
Declaration
=============================================*/

#endif
