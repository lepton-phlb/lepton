/*===========================================
Compiler Directive
=============================================*/
#ifndef _USER_KERNEL_MKCONF_H_
#define _USER_KERNEL_MKCONF_H_

//see kernel/core/kernelconf.h

//force definition of cpu device
#define __tauon_cpu_device__ __tauon_cpu_device_arm9_at91sam9261__

//force classic file system configuration
#define __file_system_profile__  __file_system_profile_classic__
#define __KERNEL_VFS_SUPPORT_EFFS   0

//ip stack
//#define USE_UIP 
#define USE_LWIP
#define USE_IF_ETHERNET

//initd. ugly config define. 
#define EVAL_BOARD

/*===========================================
Includes
=============================================*/


/*===========================================
Declaration
=============================================*/

#endif
