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
#include "kernel/core/system.h"


#include "kernel/core/process.h"
#include "kernel/core/ioctl_board.h"
#include "kernel/fs/vfs/vfsdev.h"

#include "BSP.h"

OS_STACKPTR int StackHP[128], StackLP[128];          /* Task stacks */
OS_TASK TCBHP, TCBLP;                        /* Task-control-blocks */


static void HPTask(void) {
  while (1) {
    BSP_ToggleLED(0);
    OS_Delay (50);
  }
}

static void LPTask(void) {
  while (1) {
    BSP_ToggleLED(1);
    OS_Delay (200);
  }
}

extern void forceLM3Sreset(void);


/*********************************************************************
*
*       main
*
*********************************************************************/
volatile int  g_debug = 0;
int main(void) {
  OS_IncDI();                      /* Initially disable interrupts  */
  OS_InitKern();                   /* Initialize OS                 */
  OS_InitHW();                     /* Initialize Hardware for OS    */
  if(g_debug)
      forceLM3Sreset();
  //BSP_Init();                      /* Initialize LED ports          */
  /* You need to create at least one task before calling OS_Start() */
  //OS_CREATETASK(&TCBHP, "HP Task", HPTask, 100, StackHP);
  //OS_CREATETASK(&TCBLP, "LP Task", LPTask,  50, StackLP);
  
  _start_kernel(0);
  
  OS_Start();                      /* Start multitasking            */
  return 0;
}

