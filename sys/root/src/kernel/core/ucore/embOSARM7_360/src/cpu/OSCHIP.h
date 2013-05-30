/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co KG                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2008  SEGGER Microcontroller GmbH & Co KG         *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       embOS * Real time operating system for microcontrollers      *
*                                                                    *
*                                                                    *
*       Please note:                                                 *
*                                                                    *
*       Knowledge of this file may under no circumstances            *
*       be used to write a similar product or a real-time            *
*       operating system for in-house use.                           *
*                                                                    *
*       Thank you for your fairness !                                *
*                                                                    *
**********************************************************************
*                                                                    *
*       embOS version: 3.60d                                         *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : RTOS.h
Purpose : Include file for the OS,
          to be included in every C-module accessing OS-routines
--------  END-OF-HEADER  ---------------------------------------------
*/

#ifndef RTOS_H_INCLUDED        /* Avoid multiple inclusion          */
#define RTOS_H_INCLUDED

/*********************************************************************
*
*     Specifics for ARM cores and IAR compiler
*
**********************************************************************
*/

#include <string.h>         // required for memset() etc.

#define OS_PORT_REVISION          (0)                        // Port specific revision
#define OS_PTR_OP_IS_ATOMIC       (1)
#define OS_GetTime()              (OS_Time)
#define OS_GetTime32()            (OS_Time)
#define OS_I32                    int
#define OS_SIZEOF_INT             (4)                        // Size of integer in bytes
#define OS_BIG_ENDIAN             (1 - __LITTLE_ENDIAN__)
#define OS_SUPPORT_SYSSTACK_INFO  (1)                        // Option to supply more info  to viewer
#define OS_SUPPORT_OS_ALLOC       (1)                        // Define whether OS_Alloc is supported
#define OS_STACKFILL_CHAR         (0xcd)                     // To be compatible with IAR stack check plugin
#define OS_SP_ADJUST              (0)
#define OS_EnterIntStack()                                   // Int stack is automatically used by ARM
#define OS_LeaveIntStack()                                   // Int stack is automatically used by ARM

/*****  Core specific configuration *********************************/

#define OS_SUPPORT_INTSTACK_INFO            (1)              // Option to supply more info  to viewer
#define OS_SWITCH_FROM_INT_MODIFIES_STACK   (1)
#define OS_INTERRUPTS_ARE_NESTABLE_ON_ENTRY (0)
#define OS_SUPPORT_INT_PRIORITY             (0)              // No selectable interrupt priorities with ARM

/*****  End of configuration settings *******************************/

#if (__CPU_MODE__== 1)  // if THUMB mode
  #define OS_INTERWORK  __interwork
#else
  #define OS_INTERWORK
#endif

#define OS_DI() OS_DisableInt()
#define OS_EI() OS_EnableInt()

#define OS_DI_KERNEL() OS_DisableIntSVC()
#define OS_EI_KERNEL() OS_EnableIntSVC()

/*********************************************************************
*
*       Port specific (non OS generic) functions
*/
#ifdef __cplusplus
  extern "C" {
#endif

typedef void      OS_ISR_HANDLER(void);
OS_INTERWORK void OS_irq_handler(void);    // C-level interrupt handler in RTOSInit
void              OS_USER_irq_func(void);  // User interrupt handler from UserIrq.c
OS_ISR_HANDLER*   OS_ARM_InstallISRHandler (int ISRIndex, OS_ISR_HANDLER* pFunc);
void              OS_ARM_EnableISR         (int ISRIndex);
void              OS_ARM_DisableISR        (int ISRIndex);
int               OS_ARM_ISRSetPrio        (int ISRIndex, int Prio);
void              OS_ARM_AssignISRSource   (int ISRIndex, int Source);
void              OS_ARM_EnableISRSource   (int SourceIndex);
void              OS_ARM_DisableISRSource  (int SourceIndex);
void              OS_StartTask(void);

OS_INTERWORK void OS_DisableInt(void);     // ARM mode function
OS_INTERWORK void OS_EnableInt(void);      // ARM mode function
OS_INTERWORK void OS_DisableIntSVC(void);  // ARM mode function
OS_INTERWORK void OS_EnableIntSVC(void);   // ARM mode function

#if (__VER__ >= 441)
  void OS_InitSysLocks(void);
#endif

#define OS_INIT_SYS_LOCKS()      OS_InitSysLocks()

/*********************************************************************
*
*       Core specific implementation
*/

/****** Support for ARM VFP *****************************************/
  
extern const struct OS_EXTEND_TASK_CONTEXT OS_VFP_ExtendContext;

/****** Support for ARM MMU and caches ******************************/

void OS_ARM_ICACHE_Enable(void);
void OS_ARM_DCACHE_Enable(void);
void OS_ARM_DCACHE_CleanRange     (void* p, unsigned int NumBytes);
void OS_ARM_DCACHE_InvalidateRange(void* p, unsigned int NumBytes);
void OS_ARM_CACHE_Sync(void);

void OS_ARM_MMU_Enable(unsigned int* pTranslationTable);
void OS_ARM_MMU_InitTT(unsigned int* pTranslationTable);
void OS_ARM_MMU_AddTTEntries(unsigned int* pTranslationTable, unsigned int CacheMode, unsigned int VIndex, unsigned int PIndex, unsigned int NumEntries);

void OS_ARM720_CACHE_Enable(void);
void OS_ARM720_CACHE_CleanRange(void* p, unsigned int NumBytes);
void OS_ARM720_CACHE_InvalidateRange(void* p, unsigned int NumBytes);

void OS_ARM720_MMU_Enable(unsigned int* pTranslationTable);
void OS_ARM720_MMU_InitTT(unsigned int* pTranslationTable);
void OS_ARM720_MMU_AddTTEntries(unsigned int* pTranslationTable, unsigned int CacheMode, unsigned int VIndex, unsigned int PIndex, unsigned int NumEntries);

#define OS_ARM_CACHEMODE_NC_NB ((0 << 3) | (0 << 2))
#define OS_ARM_CACHEMODE_C_NB  ((1 << 3) | (0 << 2))
#define OS_ARM_CACHEMODE_NC_B  ((0 << 3) | (1 << 2))
#define OS_ARM_CACHEMODE_C_B   ((1 << 3) | (1 << 2))

#ifdef __cplusplus
  }
#endif

/*********************************************************************
*
*     Check configuration
*
**********************************************************************

Assemble library names and other constants according to configuration
*/

#if __CPU_MODE__==1
  #define OS_CPUMODE_PREFIX "T"
#else
  #define OS_CPUMODE_PREFIX "A"
#endif

#if    __CORE__ <= __ARM4TM__
  #define OS_CPU_PREFIX "4"
#elif  __CORE__ == __ARM5__
  #define OS_CPU_PREFIX "5"
#else
  #error "Selected core not supported"
#endif

#if __LITTLE_ENDIAN__==1
  #define OS_ENDIAN_PREFIX "L"
#else
  #define OS_ENDIAN_PREFIX "B"
#endif

#define OS_MODEL OS_CPUMODE_PREFIX OS_CPU_PREFIX OS_ENDIAN_PREFIX

#ifndef   __CPU_MODE__
  #error "__CPU_MODE__ not defined. OSCHIP has to be modified !"
#endif

/****** End of chip / compiler specific part ************************/
