/*********************************************************************
*               SEGGER MICROCONTROLLER SYSTEME GmbH                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (C) 2005   SEGGER Microcontroller Systeme GmbH               *
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
*       embOS version: 3.28n                                         *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : Main.c
Purpose : Skeleton program for embOS
--------  END-OF-HEADER  ---------------------------------------------
*/
#include "RTOS.H"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"


#include "kernel/core/process.h"
#include "kernel/core/ioctl_board.h"
#include "kernel/fs/vfs/vfsdev.h"
//#include "dev/arch/arm7/at91sam7se/dev_at91sam7se_cpu/sdram.h"




#define COPROC 0x0000000E

#define TWBUFF_SIZE 0x100
#define DCC_WRITEBUSY 0x02
#define DCC_READFULL  0x01


/*********************************************************************
*
*       main
*
*********************************************************************/

int main(void) {
   //remove to enable for detection of m16cs1. use interrupt.
   OS_IncDI();                      /* Initially disable interrupts  */

   //initialize OS
   OS_InitKern();
   //initialize Hardware for OS
   OS_InitHW();

   //Start lepton kernel
   _start_kernel(0);

   OS_Start();                      /* Start multitasking            */
   return 0;
}

