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
File    : OS_Priv.h
Purpose : CPU and compiler specifics for IAR compiler ARM, OS internal
--------  END-OF-HEADER  ---------------------------------------------
*/

#ifndef OS_PRIVATE_H                    /* Avoid multiple inclusion */
#define OS_PRIVATE_H

#pragma rtmodel="__dlib_file_descriptor","*"
#pragma diag_suppress=Pe940 // IAR specific: No warning for missing return
#pragma segment="IRQ_STACK"
#pragma segment="CSTACK"

#ifndef   __CORE__        /* Missing in some version of the compiler */
  #error "__CORE__ not defined. OSCHIP has to be modified !"
#endif

#if    __CORE__ <= __ARM4TM__
  #define OS_CPU "IAR ARM V4"
#elif  __CORE__ == __ARM5__
  #define OS_CPU "IAR ARM V5"
#else
  #error "Selected core not supported"
#endif

#ifdef __cplusplus
  #define OS_CFUNC extern "C"
#else
  #define OS_CFUNC
#endif

#define OS_PORT_VERSION 36040      // Version number of tested version

/*********************************************************************
*
*       Register structure, Chip specifics ARM
*
**********************************************************************
*/

#ifdef __cplusplus
  extern "C" {
#endif
  OS_INTERWORK void OS_InitMode(void);
#ifdef __cplusplus
  }
#endif

#define OS_INIT_STACK_INFO()  OS_InitMode()

struct OS_REGS {
  OS_U32 Counters;
  OS_U32 R4;
  OS_U32 R5;
  OS_U32 R6;
  OS_U32 R7;
  OS_U32 R8;
  OS_U32 R9;
  OS_U32 R10;
  OS_U32 R11;
  OS_U32 PC;
};

typedef struct {
  OS_REGS Regs;
  OS_U32  PC_Task;
} OS_REGS_INIT;


#if OS_DEBUG
  #define OS_INITREGS_DEBUG()                                               \
     ((OS_REGS_INIT*)pStack)->Regs.R4       = 0xCCCC0004;                   \
     ((OS_REGS_INIT*)pStack)->Regs.R5       = 0xCCCC0005;                   \
     ((OS_REGS_INIT*)pStack)->Regs.R6       = 0xCCCC0006;                   \
     ((OS_REGS_INIT*)pStack)->Regs.R7       = 0xCCCC0007;                   \
     ((OS_REGS_INIT*)pStack)->Regs.R8       = 0xCCCC0008;                   \
     ((OS_REGS_INIT*)pStack)->Regs.R9       = 0xCCCC0009;                   \
     ((OS_REGS_INIT*)pStack)->Regs.R10      = 0xCCCC000A;                   \
     ((OS_REGS_INIT*)pStack)->Regs.R11      = 0xCCCC000B;
#else
  #define OS_INITREGS_DEBUG()
#endif


#define OS_INITREGS()                                                       \
   pStack                            = (char*) ((int) pStack & 0xfffffffc); \
   ((OS_REGS_INIT*)pStack)->Regs.Counters = 0;                              \
   ((OS_REGS_INIT*)pStack)->Regs.PC       = (OS_U32)OS_StartTask;           \
   ((OS_REGS_INIT*)pStack)->PC_Task  = (int)pRoutine;                       \
   OS_INITREGS_DEBUG()

#define OS_INITREGS_EX(pContext) OS_INITREGS()                              \
   ((OS_REGS_INIT*)pStack)->Regs.R4       = (int)pContext;


#define OS_CPU_C                                                            \
  /* __ramfunc */ OS_INTERWORK __arm void OS_EnableInt(void) {              \
        __asm("mrs     r12,CPSR");                                          \
        __asm("bic     r12,r12,#128");                                      \
        __asm("msr     CPSR_c,r12");                                        \
       }                                                                    \
  /* __ramfunc */  OS_INTERWORK __arm void OS_DisableInt(void) {            \
        __asm("mrs     r12,CPSR");                                          \
        __asm("orr     r12,r12,#128");                                      \
        __asm("msr     CPSR_c,r12");                                        \
       }                                                                    \
  /* __ramfunc */  OS_INTERWORK __arm void OS_EnableIntSVC(void) {          \
    __asm("msr     CPSR_c,#0x13");                                          \
       }                                                                    \
  /* __ramfunc */  OS_INTERWORK __arm void OS_DisableIntSVC(void) {         \
    __asm("msr     CPSR_c,#0x93");                                          \
       }

#define OS_CPU_C_STACK                                                                                 \
  OS_U32 OS_GetIntStackBase(void) { return (OS_U32) __sfb("IRQ_STACK"); }                              \
  OS_U32 OS_GetIntStackSize(void) { return (OS_U32) __sfe("IRQ_STACK") - (OS_U32)__sfb("IRQ_STACK"); } \
  OS_U32 OS_GetSysStackBase(void) { return (OS_U32) __sfb("CSTACK"); }                                 \
  OS_U32 OS_GetSysStackSize(void) { return (OS_U32) __sfe("CSTACK")    - (OS_U32)__sfb("CSTACK");    } \
  OS_U32 OS_GetSP(void)           { __asm("mov     R0,SP"); }


/*********************************************************************
*
*       Variables and functions needed for system locks
*       which are implemented since IAR compiler version 4.41A
*/
#if (__VER__ >= 441)
  #include "MtxWrapper.h"

  OS_CFUNC void OS_InitSysLocks(void);

  #define OS_CPU1_C                                    \
    static unsigned char _SysLockIsInited;             \
    static OS_RSEMA _OS_LockSema;                      \
                                                       \
    static void _OS_LockSys(void) {                    \
      if (_SysLockIsInited == 0) {                     \
        OS_CREATERSEMA(&_OS_LockSema);                 \
        _SysLockIsInited = 1;                          \
      }                                                \
      OS_Use(&_OS_LockSema);                           \
    }                                                  \
                                                       \
    static void _OS_UnLockSys(void) {                  \
      OS_Unuse(&_OS_LockSema);                         \
    }                                                  \
                                                       \
    void OS_InitSysLocks(void) {                       \
      _MtxlockRegister((_MtxFuncPtr) _OS_LockSys);     \
      _MtxunlockRegister((_MtxFuncPtr) _OS_UnLockSys); \
    }                                                  \
    OS_CPU_C_STACK
#else
  #define OS_CPU1_C    OS_CPU_C_STACK
#endif  /* __VER__ >= 441 */

/*********************************************************************
*
*       Check required configuration defines
*/
#ifndef __VER__
  #error "__VER__ not defined. OS_Priv has to be modified !"
#endif

/********************************************************************/

#endif                                // Avoid multiple inclusion

/*****  EOF  ********************************************************/

