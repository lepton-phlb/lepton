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
File    : OS_VFP.c
Purpose : Support for ARM Floating point coprocessor VFP
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"

void OS_SaveVFP(void * pStack);
void OS_RestoreVFP(const void * pStack);

/*********************************************************************
*
*       Local types and defines
*
**********************************************************************
*/
typedef struct {
  unsigned int acVfpRegs[32+1];  // 16 double registers plus one extra word
  unsigned int FPSCR;
  unsigned int FPSXC;
} CONTEXT_EXTENSION;

/*********************************************************************
*
*       Local, non API functions
*
**********************************************************************
*/

/* These functions are not declared static to ensure, they are not inlined */

OS_INTERWORK __arm void OS_VFP_SaveRegs(void* pData);
OS_INTERWORK __arm void OS_VFP_RestoreRegs(void* pData);
OS_INTERWORK __arm int  OS_VFP_IsEnabled(void);
void OS_VFP_Save(void * pStack);
void OS_VFP_Restore(const void * pStack);

/*********************************************************************
*
*       OS_VFP_SaveRegs()
*/
OS_INTERWORK __arm void OS_VFP_SaveRegs(void* pData) {
  __asm("FSTMIAX R0!, {d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15}");
  __asm("FMRX    R1, FPSCR");
  __asm("FMRX    R2, FPEXC");
  __asm("STMIA   R0, {R1, R2}");
}

/*********************************************************************
*
*       OS_VFP_RestoreRegs()
*/
OS_INTERWORK __arm void OS_VFP_RestoreRegs(void* pData) {
  __asm("FLDMIAX R0!, {d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15}");
  __asm("LDMIA   R0, {R1, R2}");
  __asm("FMXR    FPSCR, R1");
  __asm("FMXR    FPEXC, R2");
}

/*********************************************************************
*
*       OS_VFP_IsEnabled()
*/
#pragma diag_suppress=Pe940 // IAR specific: No warning for missing return
OS_INTERWORK __arm int OS_VFP_IsEnabled(void) {
  __asm("FMRX    R0, FPEXC");
  __asm("ANDS    R0,R0,#0x40000000");
}

/*********************************************************************
*
*       OS_VFP_Save
*
*  Function description
*    This function saves the VFP registers into the extended
*    task context.
*/
void OS_VFP_Save(void * pStack) {
  CONTEXT_EXTENSION * p;
  //
  // VFP must not be accessed if it is not enabled !
  //
  if (OS_VFP_IsEnabled()) {
    p = ((CONTEXT_EXTENSION*)pStack) - (1 - OS_STACK_AT_BOTTOM);   // Create pointer to our structure
    //
    // Save all members of the structure
    //
    OS_VFP_SaveRegs((void*) p);
  }
}

/*********************************************************************
*
*       OS_VFP_Restore
*
*  Function description
*    This function restores the VFP registers from the extended
*    task context.
*/
void OS_VFP_Restore(const void * pStack) {
  CONTEXT_EXTENSION * p;
  //
  // VFP must not be accessed if it is not enabled !
  //
  if (OS_VFP_IsEnabled()) {
    p = ((CONTEXT_EXTENSION*)pStack) - (1 - OS_STACK_AT_BOTTOM);     // Create pointer to our structure
    //
    // Restore all members of the structure
    //
    OS_VFP_RestoreRegs((void*) p);
  }
}

/*********************************************************************
*
*       Global variables
*
**********************************************************************
*/

const OS_EXTEND_TASK_CONTEXT OS_VFP_ExtendContext = {
  OS_VFP_Save,
  OS_VFP_Restore
};

/****** End of file *************************************************/

