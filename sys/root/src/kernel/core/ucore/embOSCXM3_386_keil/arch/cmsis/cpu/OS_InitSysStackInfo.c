/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co KG                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2012  SEGGER Microcontroller GmbH & Co KG         *
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
*       OS version: 3.86g                                            *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : OS_InitSysStackInfo.c
Purpose : Initialize stack info variables needed by embOS
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"

/*********************************************************************
*
*       Reference to other modules
*
**********************************************************************
*/

extern unsigned long Stack_Limit;       // Located in the startup code
extern unsigned long Stack_Mem;         // Located in the startup code

/*********************************************************************
*
*       OS_InitSysStackInfo
*/
void OS_InitSysStackInfo(void) {
  OS_SysStackBaseAddr = (unsigned long) &Stack_Mem;
  OS_SysStackSize     = (unsigned int) ((unsigned long)&Stack_Limit - (unsigned long)&Stack_Mem);
  OS_SysStackLimit    = (unsigned long) &Stack_Limit;
}
  								   
/*****  End of File  ************************************************/
