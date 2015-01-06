/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting  the provisions above and replace
them with the notice and other provisions required by the [eCos GPL] License.
If you do not delete the provisions above, a recipient may use your version of this file under
either the MPL or the [eCos GPL] License."
*/

/*===========================================
Compiler Directive
=============================================*/
#ifndef __KERNELCONF_H__
#define __KERNELCONF_H__

/*===========================================
Includes
=============================================*/
#include "kernel/core/ver.h"

#define __compiler_win32__       (0x01)
#define __compiler_gnuc__        (0x02)
#define __compiler_iar_m16c__    (0x03)
#define __compiler_iar_arm__     (0x04)
#define __compiler_keil_arm__    (0x05)


#define __tauon_cpu_core_win32_simulation__  (0x0001)
#define __tauon_cpu_core_gnu_syntetic__      (0x0002)
#define __tauon_cpu_core_m16c__              (0x0003)
#define __tauon_cpu_core_arm_arm7tdmi__      (0x0004)
#define __tauon_cpu_core_arm_arm926ejs__     (0x0005)
#define __tauon_cpu_core_arm_cortexM0__      (0x0007)
#define __tauon_cpu_core_arm_cortexM3__      (0x0008)
#define __tauon_cpu_core_arm_cortexM4__      (0x0009)

#define __tauon_cpu_device_win32_simulation__      (0x0001)
#define __tauon_cpu_device_gnu_synthetic__         (0x0002)
#define __tauon_cpu_device_arm7_at91m55800a__      (0x1702)
#define __tauon_cpu_device_arm7_at91sam7se__       (0x1703)
#define __tauon_cpu_device_arm7_at91sam7x__        (0x1704)
#define __tauon_cpu_device_arm9_at91sam9260__      (0x1905)
#define __tauon_cpu_device_arm9_at91sam9261__      (0x1906)
#define __tauon_cpu_device_cortexM0_at91samd20__   (0x1001)
#define __tauon_cpu_device_cortexM3_trifecta__     (0x1301)
#define __tauon_cpu_device_cortexM3_LM3S__         (0x1302)
#define __tauon_cpu_device_cortexM3_stm32f1__      (0x1303)
#define __tauon_cpu_device_cortexm_k60n512__       (0x1402)
#define __tauon_cpu_device_cortexM4_stm32f4__      (0x1401)


#if defined(WIN32)
   #define __tauon_compiler__ __compiler_win32__
#elif defined(__GNUC__)
   #define __tauon_compiler__ __compiler_gnuc__
#elif defined(__IAR_SYSTEMS_ICC)
   #define __tauon_compiler__  __compiler_iar_m16c__
#elif defined(__IAR_SYSTEMS_ICC__)
   #define __tauon_compiler__ __compiler_iar_arm__
#elif defined (__ARMCC_VERSION)
   #define __tauon_compiler__ __compiler_keil_arm__
#endif

#if (__tauon_compiler__==__compiler_win32__)
   //for win32 simulation
   #include "kernel/core/arch/win32/kernel_mkconf.h"
#elif (__tauon_compiler__ == __compiler_iar_arm__)
   #if ( (__CORE__==__ARM7M__) || (__CORE__==__ARM6M__) || (__CORE__==__ARM7EM__) )
      //ARMv7E-M Cortex-M4
      //ARMv7-M Cortex-M3
      //ARMv6-M Cortex-M0, Cortex-M0+ and Cortex-M1
      #include "kernel/core/arch/cortexm/kernel_mkconf.h"
   #else
      #include "kernel/core/arch/arm/kernel_mkconf.h"
   #endif
#elif (__tauon_compiler__ == __compiler_keil_arm__)
   #include "kernel/core/arch/arm/kernel_mkconf.h"
#elif (__tauon_compiler__==__compiler_gnuc__)
   #if defined(USE_KERNEL_STATIC)
      //for lepton as bootloader (no scheduler, static)
       #include "kernel/core/arch/synthetic/x86_static/kernel_mkconf.h"
   #else
      #include "kernel/core/arch/synthetic/x86/kernel_mkconf.h"
   #endif
#endif

#if defined(WIN32)
   #define __tauon_cpu_device__ __tauon_cpu_device_win32_simulation__
#else
   #if !defined(__tauon_cpu_device__)
      //#define __tauon_cpu_device__ __tauon_cpu_device_arm9_at91sam9261__
      //#define __tauon_cpu_device__ __tauon_cpu_device_cortexM4_stm32f4__
      //#define __tauon_cpu_device__ __tauon_cpu_device_cortexM3_trifecta__ 
     //#define __tauon_cpu_device__ __tauon_cpu_device_cortexM3_stm32f4__
   #endif
#endif

#if __tauon_cpu_device__==__tauon_cpu_device_win32_simulation__
   #define __KERNEL_CPU_DEVICE_NAME "x86-win32-sim"
   #define __tauon_cpu_core__ __tauon_cpu_core_win32_simulation__

#elif __tauon_cpu_device__==__tauon_cpu_device_gnu_synthetic__
   #define __KERNEL_CPU_DEVICE_NAME "x86-gnu-synth"
   #define __tauon_cpu_core__ __tauon_cpu_core_gnu_syntetic__

#elif __tauon_cpu_device__==__tauon_cpu_device_arm7_at91m55800a__
   #define __KERNEL_CPU_DEVICE_NAME "arm7-at91m55800a"
   #define __tauon_cpu_core__ __tauon_cpu_core_arm_arm7tdmi__

#elif __tauon_cpu_device__==__tauon_cpu_device_arm7_at91sam7se__
   #define __KERNEL_CPU_DEVICE_NAME "arm7-at91sam7se"
   #define __tauon_cpu_core__ __tauon_cpu_core_arm_arm7tdmi__

#elif __tauon_cpu_device__==__tauon_cpu_device_arm7_at91sam7x__
   #define __KERNEL_CPU_DEVICE_NAME "arm7-at91sam7x"
   #define __tauon_cpu_core__ __tauon_cpu_core_arm_arm7tdmi__

#elif __tauon_cpu_device__==__tauon_cpu_device_arm9_at91sam9260__
   #define __KERNEL_CPU_DEVICE_NAME "arm9-at91sam9260"
   #define __tauon_cpu_core__ __tauon_cpu_core_arm_arm926ejs__

#elif __tauon_cpu_device__==__tauon_cpu_device_arm9_at91sam9261__
   #define __KERNEL_CPU_DEVICE_NAME "arm9-at91sam9261"
   #define __tauon_cpu_core__ __tauon_cpu_core_arm_arm926ejs__

#elif __tauon_cpu_device__==__tauon_cpu_device_cortexM0_at91samd20__
   #define __KERNEL_CPU_DEVICE_NAME "cortexM0+-at91samd20"
   #define __tauon_cpu_core__ __tauon_cpu_core_arm_cortexM0__

#elif __tauon_cpu_device__==__tauon_cpu_device_cortexM3_trifecta__
   #define __KERNEL_CPU_DEVICE_NAME "cortexM3-trifecta"
   #define __tauon_cpu_core__ __tauon_cpu_core_arm_cortexM3__

#elif __tauon_cpu_device__ == __tauon_cpu_device_cortexM3_LM3S__
   #define __KERNEL_CPU_DEVICE_NAME "cortexM3-TI-LM3S"
   #define __tauon_cpu_core__ __tauon_cpu_core_arm_cortexM3__

#elif __tauon_cpu_device__ == __tauon_cpu_device_cortexM3_stm32f1__
   #define __KERNEL_CPU_DEVICE_NAME "cortexM3-stm32f1"
   #define __tauon_cpu_core__ __tauon_cpu_core_arm_cortexM3__

#elif __tauon_cpu_device__ == __tauon_cpu_device_cortexm_k60n512__
   #define __KERNEL_CPU_DEVICE_NAME "cortexM4-kinetis-k60n512"
   #define __tauon_cpu_core__ __tauon_cpu_core_arm_cortexM4__

#elif __tauon_cpu_device__ == __tauon_cpu_device_cortexM4_stm32f4__
   #define __KERNEL_CPU_DEVICE_NAME "cortexM4-stm32f4"
   #define __tauon_cpu_core__ __tauon_cpu_core_arm_cortexM4__
#endif



/*===========================================
Declaration
=============================================*/
#define TAUON_POSIX
#define __TAUON_POSIX__
#define __tauon_posix__

//
#if (__tauon_compiler__!=__compiler_gnuc__)
   #define __attribute__(__attr__)
#endif

//what micro kernel
#if (__tauon_compiler__==__compiler_gnuc__)
//#define __KERNEL_UCORE_ECOS
#elif (__tauon_compiler__==__compiler_win32__)
   #define __KERNEL_UCORE_EMBOS
#elif (__tauon_compiler__==__compiler_iar_m16c__)
   #define __KERNEL_UCORE_EMBOS
#elif (__tauon_compiler__==__compiler_iar_arm__)
   //#define __KERNEL_UCORE_EMBOS
   #define __KERNEL_UCORE_FREERTOS
#elif (__tauon_compiler__==__compiler_keil_arm__)
   #define __KERNEL_UCORE_EMBOS
#endif

#define __KERNEL_COMPILER_SUPPORT_32_BITS_TYPE 32
#define __KERNEL_COMPILER_SUPPORT_64_BITS_TYPE 64

#if (__tauon_compiler__==__compiler_gnuc__)
#if (__tauon_cpu_core__ == __tauon_cpu_core_arm_cortexM0__)
   #define __KERNEL_COMPILER_SUPPORT_TYPE __KERNEL_COMPILER_SUPPORT_64_BITS_TYPE
#else
   #define __KERNEL_COMPILER_SUPPORT_TYPE __KERNEL_COMPILER_SUPPORT_32_BITS_TYPE
#endif
#elif (__tauon_compiler__==__compiler_win32__)
    #define __KERNEL_COMPILER_SUPPORT_TYPE __KERNEL_COMPILER_SUPPORT_32_BITS_TYPE 
#elif (__tauon_compiler__==__compiler_iar_m16c__)
   #define __KERNEL_COMPILER_SUPPORT_TYPE __KERNEL_COMPILER_SUPPORT_32_BITS_TYPE 
#elif (__tauon_compiler__==__compiler_iar_arm__)
   #define __KERNEL_COMPILER_SUPPORT_TYPE __KERNEL_COMPILER_SUPPORT_64_BITS_TYPE
#elif (__tauon_compiler__==__compiler_keil_arm__)
   #define __KERNEL_COMPILER_SUPPORT_TYPE __KERNEL_COMPILER_SUPPORT_64_BITS_TYPE   
#endif

#if (__tauon_compiler__==__compiler_win32__)
   #define USE_COMPILER_TRACE_DEBUG
#endif

#define CPU_ARCH_32  (32)
#define CPU_ARCH_16  (16)

#if (__tauon_cpu_core__==__tauon_cpu_core_win32_simulation__)
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "win32"
#elif (__tauon_cpu_core__ == __tauon_cpu_core_arm_arm7tdmi__)
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "arm7tdmi"
#elif (__tauon_cpu_core__ == __tauon_cpu_core_arm_arm926ejs__)
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "arm926ejs"
#elif (__tauon_cpu_core__==__tauon_cpu_core_gnu_syntetic__)
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "gnu32"
#elif (__tauon_cpu_core__==__tauon_cpu_core_m16c__)
   #define __KERNEL_CPU_ARCH CPU_ARCH_16
   #define __KERNEL_CPU_NAME "m16c62"
#elif (__tauon_cpu_core__ == __tauon_cpu_core_arm_cortexM0__)
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "cortexm0"
#elif (__tauon_cpu_core__ == __tauon_cpu_core_arm_cortexM3__)
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "cortexm3"
#elif (__tauon_cpu_core__ == __tauon_cpu_core_arm_cortexM4__)
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "cortexm4"
#else
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "unknow"
#endif

//specific compiler directive definition
#if (__tauon_compiler__==__compiler_gnuc__)
   #define __compiler_directive__packed
#elif (__tauon_compiler__==__compiler_win32__)
   #define __compiler_directive__packed
#elif (__tauon_compiler__==__compiler_iar_m16c__)
  #define __compiler_directive__packed
#elif (__tauon_compiler__==__compiler_iar_arm__)
   #define __compiler_directive__packed __packed
#elif (__tauon_compiler__==__compiler_keil_arm__)
   #define __compiler_directive__packed __packed
#endif

//
//link option: SRAM optimization
#if (__tauon_cpu_device__ == __tauon_cpu_device_cortexM4_stm32f4__)
   #if (__tauon_compiler__==__compiler_iar_arm__)
      //use internal CCM Core Coupled Memory. warning! cannot be used with DMA.
      #define CORTEXM4_CCM_RAM _Pragma("section=\"CCM_RAM_SECTION\"")\
      _Pragma("location=\"CCM_RAM_SECTION\"")

      #define __KERNEL_SRAM_LOCATION CORTEXM4_CCM_RAM
   #endif
#endif

#ifndef __KERNEL_SRAM_LOCATION
   #define __KERNEL_SRAM_LOCATION
#endif

//features profile
#define __tauon_kernel_profile_minimal__         0x0001
#define __tauon_kernel_profile_classic__         0x0002
#define __tauon_kernel_profile_full__            0x000F
#define __tauon_kernel_profile_user_defined__    0x8000

#if (__tauon_cpu_core__==__tauon_cpu_core_gnu_syntetic__) || (__tauon_cpu_core__==__tauon_cpu_core_win32_simulation__)
   #define __tauon_kernel_profile__ __tauon_kernel_profile_full__
#elif (__tauon_cpu_core__ == __tauon_cpu_core_arm_arm7tdmi__)
   #define __tauon_kernel_profile__ __tauon_kernel_profile_classic__
#elif (__tauon_cpu_core__ == __tauon_cpu_core_arm_arm926ejs__)
   #define __tauon_kernel_profile__ __tauon_kernel_profile_classic__
#elif (__tauon_cpu_core__ == __tauon_cpu_core_arm_cortexM3__)
   #define __tauon_kernel_profile__ __tauon_kernel_profile_minimal__
#endif

#if !defined(__tauon_kernel_profile__)
   #define __tauon_kernel_profile__ __tauon_kernel_profile_minimal__
#endif 

#if (__tauon_kernel_profile__==__tauon_kernel_profile_full__)
   //
   #define __KERNEL_OBJECT_POOL_MAX 10

   #ifndef __KERNEL_MAX_PIPE
      #define __KERNEL_MAX_PIPE 10
   #endif

   #ifndef __KERNEL_PIPE_SIZE
      #define __KERNEL_PIPE_SIZE  1024 //256
   #endif

   #define __KERNEL_RTFS_BLOCK_SIZE 32 //default size 16
// to do: put this definition in kernel_mkconf.h with mklepton.
   #define __KERNEL_UFS_BLOCK_SIZE_MAX 256

#elif (__tauon_kernel_profile__==__tauon_kernel_profile_classic__)
   #define __KERNEL_OBJECT_POOL_MAX 10

   #ifndef __KERNEL_MAX_PIPE
      #define __KERNEL_MAX_PIPE 10
   #endif

   #ifndef __KERNEL_PIPE_SIZE
      #define __KERNEL_PIPE_SIZE  1024 //256
   #endif

   #ifndef __KERNEL_RTFS_BLOCK_SIZE
      #define __KERNEL_RTFS_BLOCK_SIZE 32 //default size 16
   #endif

   #ifndef __KERNEL_UFS_BLOCK_SIZE_MAX
      #define __KERNEL_UFS_BLOCK_SIZE_MAX 256
   #endif

#elif (__tauon_kernel_profile__==__tauon_kernel_profile_minimal__)
   #ifndef __KERNEL_OBJECT_POOL_MAX
      #define __KERNEL_OBJECT_POOL_MAX 8
   #endif

   #ifndef __KERNEL_MAX_PIPE
      #define __KERNEL_MAX_PIPE 1
   #endif

   #ifndef __KERNEL_PIPE_SIZE
      #define __KERNEL_PIPE_SIZE  32
   #endif

// to do: put this definition in kernel_mkconf.h with mklepton.
   #define __KERNEL_RTFS_BLOCK_SIZE 16 //32 //default size 16
// to do: put this definition in kernel_mkconf.h with mklepton.
   #define __KERNEL_UFS_BLOCK_SIZE_MAX 256
#endif

//
#if (__tauon_cpu_core__==__tauon_cpu_core_win32_simulation__)
   #ifdef _DEBUG
      #define __KERNEL_DEBUG
   #endif
#endif

//for 20KB RAM default setting
//distrib: must be set
#ifndef __KERNEL_CPU_FREQ
   #pragma message("warning!!! __KERNEL_CPU_FREQ not defined")
   #define __KERNEL_CPU_FREQ  10000000 //10MHz
#endif
#ifndef __KERNEL_HEAP_SIZE
   #pragma message("warning!!! __KERNEL_HEAP_SIZE not defined")
   #if (__tauon_compiler__==__compiler_keil_arm__) 
      #define __KERNEL_HEAP_SIZE  0x8000  //32KBytes
   #else
      #define __KERNEL_HEAP_SIZE  8000 //12000//10000//8000//5000//2000
   #endif
#endif

#ifndef __KERNEL_PTHREAD_MAX
   #pragma message("warning!!! __KERNEL_PTHREAD_MAX not defined")
   #define __KERNEL_PTHREAD_MAX 8
#endif

#ifndef __KERNEL_PROCESS_MAX
   #pragma message("warning!!! __KERNEL_PROCESS_MAX not defined")
   #ifdef __KERNEL_NET_IPSTACK //specific for ipstack
      #define __KERNEL_PROCESS_MAX __KERNEL_PTHREAD_MAX-2 //for ip stack. see thread in kernel.c
   #else //normal
      #define __KERNEL_PROCESS_MAX __KERNEL_PTHREAD_MAX-1 //-2 for ip stack //-5 //for low ram
   #endif

#endif

#ifndef __KERNEL_ENV_PATH
   #pragma message("warning!!! __KERNEL_ENV_PATH not defined")
   #define __KERNEL_ENV_PATH {"/bin","/usr/sbin","/usr/bin"}
#endif

//profiler
#if (__tauon_cpu_device__ == __tauon_cpu_device_arm7_at91m55800a__)\
    ||(__tauon_cpu_device__ == __tauon_cpu_device_arm9_at91sam9260__)\
    ||(__tauon_cpu_device__== __tauon_cpu_device_arm9_at91sam9261__)
   #define KERNEL_PROFILER
#endif

#define KERNEL_PROCESS_VFORK_CLRSET_IRQ

//
#if (__tauon_cpu_device__==__tauon_cpu_device_arm7_at91m55800a__)
   #ifdef __KERNEL_DEBUG
      #define __KERNEL_ARCH_DELAY_1US 1 //around 30ns per cycle 38:ARM7 at 32 MHz
   #else
      #define __KERNEL_ARCH_DELAY_1US 38
   #endif
#elif (__tauon_cpu_device__==__tauon_cpu_device_arm9_at91sam9261__)
   #ifdef __KERNEL_DEBUG
      #define __KERNEL_ARCH_DELAY_1US 1 //around 30ns per cycle 38:ARM7 at 32 MHz
   #else
      #define __KERNEL_ARCH_DELAY_1US 38
   #endif
#else //default
   #ifdef __KERNEL_DEBUG
      #define __KERNEL_ARCH_DELAY_1US 1 //around 30ns per cycle 38:ARM7 at 32 MHz
   #else
      #define __KERNEL_ARCH_DELAY_1US 38
   #endif
#endif

//boot device
#ifndef __BOOT_DEVICE
   #define __BOOT_DEVICE "/dev/hd/hda" //must be hda cpufs
#endif

#define __tauon_stdio_profile_minimal__         0x0001
#define __tauon_stdio_profile_classic__         0x0002
#define __tauon_stdio_profile_full__            0x000F

#ifndef __tauon_stdio_profile__
   #define __tauon_stdio_profile__  __tauon_stdio_profile_classic__
#endif

#if __tauon_stdio_profile__ > __tauon_stdio_profile_minimal__
   //support full sdtio printf options (float %f%e%g).
   #define USE_FULL_STDIO_PRINTF 1
#endif

//realtime posix extension
#if defined(__KERNEL_UCORE_EMBOS)
   #define ATEXIT_MAX    4
   #define __KERNEL_POSIX_REALTIME_SIGNALS
   #define __KERNEL_LOAD_LIB
   #define __KERNEL_USE_FILE_LOCK
   #define __KERNEL_IO_SEM
#endif

#if defined(__KERNEL_UCORE_FREERTOS)
   #define ATEXIT_MAX    4
   #if (__tauon_cpu_core__ != __tauon_cpu_core_arm_cortexM0__)
      #define __KERNEL_POSIX_REALTIME_SIGNALS
   #endif
   #define __KERNEL_LOAD_LIB
   #define __KERNEL_USE_FILE_LOCK
   #define __KERNEL_IO_SEM
#endif

#if defined(__GNUC__)
   #define __KERNEL_LOAD_LIB
#if (__tauon_cpu_core__ != __tauon_cpu_core_arm_cortexM0__)
   #define __KERNEL_POSIX_REALTIME_SIGNALS
#endif

   #if (__tauon_cpu_core__ != __tauon_cpu_core_arm_cortexM3__) && (__tauon_cpu_core__ != __tauon_cpu_core_arm_cortexM4__)
      #define __KERNEL_USE_FILE_LOCK
   #endif
   //#define __KERNEL_IO_EVENT
   #define __KERNEL_IO_SEM
//test for assert
   #define __KERNEL_USE_ASSERT   1
#endif

//enable load lib in secondary pthread (used for stdio)
#if defined(__KERNEL_LOAD_LIB)
   #define __KERNEL_LOAD_LIB_PTHREAD  1
#endif

#if !defined(__KERNEL_IO_EVENT) && !defined(__KERNEL_IO_SEM)
   #define __KERNEL_IO_EVENT
#endif

//ip stack definition
#if !defined(USE_UIP_VER)
   #define USE_UIP_VER 2500 
#endif

#if defined (__KERNEL_NET_IPSTACK)

   #if !defined(USE_LWIP) && !defined(USE_UIP)
      //error no stack defined
   #endif

   #if defined(USE_LWIP) && defined(USE_UIP)
      //error multiple stack defined
   #endif

   #if defined(USE_UIP)
      #define USE_UIP_CORE
      //#define USE_IF_ETHERNET
      #define USE_IF_PPP //GD
   #endif
   
#endif

#if defined (__KERNEL_NET_IPSTACK)
   #if !defined(USE_IF_ETHERNET) && !defined(USE_IF_SLIP) && !defined(USE_IF_PPP)
      #define USE_IF_SLIP
   #endif
#endif


// file system profile
#define __file_system_profile_minimal__         0x0001
#define __file_system_profile_classic__         0x0002
#define __file_system_profile_classic_yaffs__   0x0003
#define __file_system_profile_full__            0x000F
#define __file_system_profile_user_defined__    0x8000

#if __tauon_cpu_core__ == __tauon_cpu_core_win32_simulation__
   #if !defined(__file_system_profile__)
      #define __file_system_profile__  __file_system_profile_classic_yaffs__
   #endif
#endif

#if !defined(__file_system_profile__)
   #define __file_system_profile__  __file_system_profile_classic__
#endif

#if (__file_system_profile__ == __file_system_profile_minimal__)
   #define __KERNEL_VFS_SUPPORT_ROOTFS 1
   #define __KERNEL_VFS_SUPPORT_UFS    1
   #define __KERNEL_VFS_SUPPORT_UFSX   0
   #define __KERNEL_VFS_SUPPORT_KOFS   1
   #define __KERNEL_VFS_SUPPORT_MSDOS  0
   #define __KERNEL_VFS_SUPPORT_VFAT   0
   #define __KERNEL_VFS_SUPPORT_YAFFS  0
#elif (__file_system_profile__ == __file_system_profile_classic__) 
   #define __KERNEL_VFS_SUPPORT_ROOTFS 1
   #define __KERNEL_VFS_SUPPORT_UFS    1
   #define __KERNEL_VFS_SUPPORT_UFSX   1
   #define __KERNEL_VFS_SUPPORT_KOFS   1
   #define __KERNEL_VFS_SUPPORT_MSDOS  0
   #define __KERNEL_VFS_SUPPORT_VFAT   0
   #define __KERNEL_VFS_SUPPORT_YAFFS  0
#elif (__file_system_profile__ == __file_system_profile_classic_yaffs__)
   #define __KERNEL_VFS_SUPPORT_ROOTFS 1
   #define __KERNEL_VFS_SUPPORT_UFS    1
   #define __KERNEL_VFS_SUPPORT_UFSX   1
   #define __KERNEL_VFS_SUPPORT_KOFS   1
   #define __KERNEL_VFS_SUPPORT_MSDOS  0
   #define __KERNEL_VFS_SUPPORT_VFAT   0
   #define __KERNEL_VFS_SUPPORT_YAFFS  1
#elif (__file_system_profile__ == __file_system_profile_full__)
   #define __KERNEL_VFS_SUPPORT_ROOTFS 1
   #define __KERNEL_VFS_SUPPORT_UFS    1
   #define __KERNEL_VFS_SUPPORT_UFSX   1
   #define __KERNEL_VFS_SUPPORT_KOFS   1
   #define __KERNEL_VFS_SUPPORT_MSDOS  1
   #define __KERNEL_VFS_SUPPORT_VFAT   1
   #define __KERNEL_VFS_SUPPORT_YAFFS  1
#endif 

//file system ufs
#if !defined(__GNUC__)
   #define __KERNEL_SUPPORT_UFS_DRIVER_1_3 13 //only 16 bits file size support
#endif
#define __KERNEL_SUPPORT_UFS_DRIVER_1_4 14  //16/32 bits file size support
#define __KERNEL_SUPPORT_UFS_DRIVER_1_5 15  //full 32 bits file size support

#define __KERNEL_SUPPORT_UFS_DRIVER __KERNEL_SUPPORT_UFS_DRIVER_1_5

//ufs file system max block size
#ifndef __KERNEL_UFS_BLOCK_SIZE_MAX
   #define __KERNEL_UFS_BLOCK_SIZE_MAX   64 //default size
#endif

//file system yaffs
//yaffs definition
//vc++ 6 definition:CONFIG_YAFFS_DIRECT,CONFIG_YAFFS_SHORT_NAMES_IN_RAM,CONFIG_YAFFS_YAFFS2,CONFIG_YAFFS_PROVIDE_DEFS,CONFIG_YAFFSFS_PROVIDE_VALUES,NO_Y_INLINE
#define CONFIG_YAFFS_DIRECT
#define CONFIG_YAFFS_SHORT_NAMES_IN_RAM
#define CONFIG_YAFFS_YAFFS2
#define CONFIG_YAFFS_PROVIDE_DEFS
#define CONFIG_YAFFSFS_PROVIDE_VALUES
#define NO_Y_INLINE

#endif
