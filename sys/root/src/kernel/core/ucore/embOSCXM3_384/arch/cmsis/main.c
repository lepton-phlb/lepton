/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co KG                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2011  SEGGER Microcontroller GmbH & Co KG         *
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
*       OS version: 3.82u                                            *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : Start_LEDBlink.c
Purpose : Sample program for OS running on EVAL-boards with LEDs
--------- END-OF-HEADER --------------------------------------------*/


#include "RTOS.h"
#include "kernel/core/kernel.h"
//#include "kernel/core/system.h"
//
//
//#include "kernel/core/process.h"
//#include "kernel/core/ioctl_board.h"
//#include "kernel/fs/vfs/vfsdev.h"
//
//
///*********************************************************************
//*
//*       main
//*
//*********************************************************************/
//int main(void) {
//  OS_IncDI();                      /* Initially disable interrupts  */
//  OS_InitKern();                   /* Initialize OS                 */
//  OS_InitHW();                     /* Initialize Hardware for OS    */
//  
//  _start_kernel(0);
//  
//  OS_Start();                      /* Start multitasking            */
//  return 0;
//}

//#include "RTOS.h"

OS_STACKPTR int StackHP[512];//, StackLP[128];          /* Task stacks */
OS_TASK TCBHP, TCBLP;                        /* Task-control-blocks */

static void HPTask(void) {
    _start_kernel(0);
  while (1) {
    OS_Delay (1000);
  }
}

//static void LPTask(void) {
//  while (1) {
//    OS_Delay (50);
//  }
//}

/*********************************************************************
*
*       main
*
*********************************************************************/

int main(void) {
//    char table[256];
  OS_IncDI();                      /* Initially disable interrupts  */
  OS_InitKern();                   /* Initialize OS                 */
  OS_InitHW();                     /* Initialize Hardware for OS    */
//  _start_kernel(0,table);
  /* You need to create at least one task here !                    */
  OS_CREATETASK(&TCBHP, "HP Task", HPTask, 100, StackHP);
//  OS_CREATETASK(&TCBLP, "LP Task", LPTask,  50, StackLP);
  OS_Start();                      /* Start multitasking            */
  return 0;
}

