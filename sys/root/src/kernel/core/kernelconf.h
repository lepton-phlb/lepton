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

/*===========================================
Compiler Directive
=============================================*/
#ifndef _KERNELCONF_H
#define _KERNELCONF_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/ver.h"

#define __compiler_win32__       (0x01)
#define __compiler_gnuc__        (0x02)
#define __compiler_iar_m16c__    (0x03)
#define __compiler_iar_arm__     (0x04)

#define __compiler_cpu_target_win32__  (0x0100)
#define __compiler_cpu_target_gnuc__   (0x0200)
#define __compiler_cpu_target_m16c__   (0x0300)
#define __compiler_cpu_target_arm__    (0x0400)

#define __tauon_cpu_device_win32_simulation__   (0x0001)
#define __tauon_cpu_device_gnu_synthetic__      (0x0002)
#define __tauon_cpu_device_arm7_at91m55800a__   (0x0102)
#define __tauon_cpu_device_arm7_at91sam7se__    (0x0103)
#define __tauon_cpu_device_arm7_at91sam7x__     (0x0104)
#define __tauon_cpu_device_arm9_at91sam9260__   (0x0205)
#define __tauon_cpu_device_arm9_at91sam9261__   (0x0206)
#define __tauon_cpu_device_cortexm_k60n512__    (0x0306)


#if defined(WIN32)
   #define __tauon_compiler__ __compiler_win32__
   #define __tauon_compiler_cpu_target__  __compiler_cpu_target_win32__
#elif defined(__GNUC__)
   #define __tauon_compiler__    __compiler_gnuc__
#elif defined(__IAR_SYSTEMS_ICC)
   #define __tauon_compiler__  __compiler_iar_m16c__
   #define __tauon_compiler_cpu_target__  __compiler_cpu_target_m16c__
#elif defined(__IAR_SYSTEMS_ICC__)
   #define __tauon_compiler__ __compiler_iar_arm__
   #define __tauon_compiler_cpu_target__ __compiler_cpu_target_arm__
#endif


#if (__tauon_compiler_cpu_target__==__compiler_cpu_target_win32__)
//for win32
   #include "kernel/core/arch/win32/kernel_mkconf.h"
#elif (__tauon_compiler_cpu_target__==__compiler_cpu_target_arm__) || defined(CPU_CORTEXM)
//for cortexm
   #include "kernel/core/arch/cortexm/kernel_mkconf.h"
#elif (__tauon_compiler_cpu_target__==__compiler_cpu_target_arm__) || (defined(CPU_ARM7) || \
   defined(CPU_ARM9))
//for arm7 and arm9
   #include "kernel/core/arch/arm/kernel_mkconf.h"
#elif (__tauon_compiler_cpu_target__==__compiler_cpu_target_gnuc__) && !defined(USE_KERNEL_STATIC)
//for unix
   #include "kernel/core/arch/synthetic/x86/kernel_mkconf.h"
#else
   #include "kernel/core/arch/synthetic/x86_static/kernel_mkconf.h"
#endif

/*===========================================
Declaration
=============================================*/
#define TAUON_POSIX
#define __TAUON_POSIX__
#define __tauon_posix__

//what micro kernel
#if (__tauon_compiler__==__compiler_gnuc__)
//#define USE_ECOS
#elif (__tauon_compiler__==__compiler_win32__)
   #define USE_SEGGER
#elif (__tauon_compiler__==__compiler_iar_m16c__)
   #define USE_SEGGER
#elif (__tauon_compiler__==__compiler_iar_arm__)
   #define USE_SEGGER
#endif

#define CPU_ARCH_32  (32)
#define CPU_ARCH_16  (16)

#if defined(CPU_WIN32)
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "win32"
#elif defined(CPU_ARM7)
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "arm7"
#elif defined(CPU_ARM9)
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "arm9"
#elif defined(CPU_GNU32)
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "gnu32"
#elif defined(CPU_M16C62)
   #define __KERNEL_CPU_ARCH CPU_ARCH_16
   #define __KERNEL_CPU_NAME "m16c62"
#elif defined(CPU_CORTEXM)
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "cortexm"
#else
   #define __KERNEL_CPU_ARCH CPU_ARCH_32
   #define __KERNEL_CPU_NAME "unknow"
#endif

#define __KERNEL_CPU_ARCH_SUPPORT_FORMAT 32 //(see libc/stdint.h)

#if defined(CPU_GNU32)
   #define __KERNEL_OBJECT_POOL_MAX 10

   #ifndef __KERNEL_MAX_PIPE
      #define __KERNEL_MAX_PIPE 10
   #endif

//   #ifndef __KERNEL_PIPE_SIZE
//      #define __KERNEL_PIPE_SIZE  16
//   #endif

   #ifndef __KERNEL_PIPE_SIZE
      #define __KERNEL_PIPE_SIZE  1024 //256
   #endif

// to do: put this definition in kernel_mkconf.h with mklepton.
   #define __KERNEL_UFS_BLOCK_SIZE_MAX 256

#elif defined(CPU_WIN32)
//
   #ifdef _DEBUG
      #define __KERNEL_DEBUG
   #endif

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

#elif defined(CPU_ARM7) || defined(CPU_ARM9)
   #define __KERNEL_OBJECT_POOL_MAX 10

   #ifndef __KERNEL_MAX_PIPE
      #define __KERNEL_MAX_PIPE 10
   #endif

   #ifndef __KERNEL_PIPE_SIZE
      #define __KERNEL_PIPE_SIZE  1024 //256
   #endif

// to do: put this definition in kernel_mkconf.h with mklepton.
   #define __KERNEL_RTFS_BLOCK_SIZE 32 //default size 16
// to do: put this definition in kernel_mkconf.h with mklepton.
   #define __KERNEL_UFS_BLOCK_SIZE_MAX 256

#elif defined(CPU_CORTEXM)
   #define __KERNEL_OBJECT_POOL_MAX 8

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


//for 20KB RAM default setting
//distrib: must be set
#ifndef __KERNEL_CPU_FREQ
   #pragma message("warning!!! __KERNEL_CPU_FREQ not defined")
   #define __KERNEL_CPU_FREQ  10000000 //10MHz
#endif
#ifndef __KERNEL_HEAP_SIZE
   #pragma message("warning!!! __KERNEL_HEAP_SIZE not defined")
   #define __KERNEL_HEAP_SIZE  8000 //12000//10000//8000//5000//2000
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


#ifndef __KERNEL_ENV_PATH
   #pragma message("warning!!! __KERNEL_ENV_PATH not defined")
   #define __KERNEL_ENV_PATH {"/bin","/usr/sbin","/usr/bin"}
#endif


#if !defined(WIN32)  && !defined(USE_KERNEL_STATIC) && !defined(CPU_CORTEXM)
   #define KERNEL_PROFILER
#endif

#define KERNEL_PROCESS_VFORK_CLRSET_IRQ

//
#if defined(CPU_ARM7)
   #ifdef __KERNEL_DEBUG
      #define __KERNEL_ARCH_DELAY_1US 1 //around 30ns per cycle 38:ARM7 at 32 MHz
   #else
      #define __KERNEL_ARCH_DELAY_1US 38
   #endif
#endif
//
#if defined(CPU_ARM9)
   #ifdef __KERNEL_DEBUG
      #define __KERNEL_ARCH_DELAY_1US 1 //around 30ns per cycle 38:ARM7 at 32 MHz
   #else
      #define __KERNEL_ARCH_DELAY_1US 38
   #endif
#endif
//
#if defined(CPU_CORTEXM)
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

//realtime posix extension
#if defined(USE_SEGGER)
   #define ATEXIT_MAX    4
   #define __KERNEL_POSIX_REALTIME_SIGNALS
   #define __KERNEL_LOAD_LIB
   #define __KERNEL_USE_FILE_LOCK
   #define __KERNEL_IO_SEM
#endif

#if defined(__GNUC__)
   #define __KERNEL_LOAD_LIB
   #define __KERNEL_POSIX_REALTIME_SIGNALS
   #if !defined(CPU_CORTEXM)
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
#if defined (__KERNEL_NET_IPSTACK)
   #define USE_IF_ETHERNET
   #define USE_LWIP
   #define USE_UIP
#endif

#if defined (__KERNEL_NET_IPSTACK)
   #if !defined(USE_IF_ETHERNET) && !defined(USE_IF_SLIP)
      #define USE_IF_SLIP
   #endif
#endif


#if (__tauon_compiler__!=__compiler_gnuc__)
   #define __attribute__(__attr__)
#endif


#endif
