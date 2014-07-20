/*********************************************************************
*               SEGGER MICROCONTROLLER SYSTEME GmbH                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (C) 2006   SEGGER Microcontroller Systeme GmbH               *
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
*       embOS version: 3.32e                                         *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : __low_level_init.c                (for ATMEL AT91SAM7S CPUs)
Purpose : Initializes pll and interrupt controller
          Feel free to modify this file acc. to your
          target system.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.H"
#include "OS_Config.h"           /* contains default clock settings */

/*********************************************************************
*
*       Configuration
*
*********************************************************************/

/*********************************************************************
*
*       Clock frequency settings
*
*       Should be defined in OS_Config.h
*/
#ifndef OS_FSYS
  #error "OS_FSYS has to be defined in OS_Config.h"
#endif

/*********************************************************************
*
*       Local defines (sfrs used in __low_level_init.c)
*
**********************************************************************
*/

/*      Reset controller */
#define __RSTC_BASE  (0xFFFFFD00)
#define __RSTC_CR    (*(volatile OS_U32*) (__RSTC_BASE + 0x00))
#define __RSTC_SR    (*(volatile OS_U32*) (__RSTC_BASE + 0x04))
#define __RSTC_MR    (*(volatile OS_U32*) (__RSTC_BASE + 0x08))
#define RSTC_BODIEN  (1 << 16)  /* Brownout interrupt enable   */
#define RSTC_URSTIEN (1 <<  4)  /* User reset interrupt enable */
#define RSTC_BODSTS  (1 <<  1)  /* Brownout status             */
#define RSTC_URSTS   (1 <<  0)  /* User reset status           */

/*      Watchdog */
#define __WDT_BASE   (0xFFFFFD40)
#define __WDT_CR     (*(volatile OS_U32*) (__WDT_BASE + 0x00))
#define __WDT_MR     (*(volatile OS_U32*) (__WDT_BASE + 0x04))
#define __WDT_SR     (*(volatile OS_U32*) (__WDT_BASE + 0x08))
#define WDT_WDFIEN   (1 << 12)   /* Watchdog interrupt enable flag in mode register */
#define WDT_WDERR    (1 <<  1)   /* Watchdog error status flag                      */
#define WDT_WDUNF    (1 <<  0)   /* Watchdog underflow status flag                  */

/*      Memory controller */
#define __MC_BASE    (0xFFFFFF00)
#define __MC_FMR     (*(volatile OS_U32*) (__MC_BASE + 0x60))

#define __MC_FWS_0FWS ((unsigned int) 0x0 <<  8) /* (MC) 1 cycle for Read,  2 for Write operations */
#define __MC_FWS_1FWS ((unsigned int) 0x1 <<  8) /* (MC) 2 cycles for Read, 3 for Write operations */
#define __MC_FWS_2FWS ((unsigned int) 0x2 <<  8) /* (MC) 3 cycles for Read, 4 for Write operations */
#define __MC_FWS_3FWS ((unsigned int) 0x3 <<  8) /* (MC) 4 cycles for Read, 4 for Write operations */
#if   (OS_FSYS <= 30000000)
  #define __MC_FWS (__MC_FWS_0FWS)
#elif (OS_FSYS <= 50000000)
  #define __MC_FWS (__MC_FWS_1FWS)
#else
  #error "Please examine settings for memory controller, Flash wait states"
#endif

/*      Power management controller */
#define __PMC_BASE   (0xFFFFFC00)
#define __PMC_PCER   (*(volatile OS_U32*) (__PMC_BASE + 0x10))  /* Peripheral clock enable register */
#define __PMC_MOR    (*(volatile OS_U32*) (__PMC_BASE + 0x20))  /* main oscillator register */
#define __PMC_PLLR   (*(volatile OS_U32*) (__PMC_BASE + 0x2c))  /* PLL register */
#define __PMC_MCKR   (*(volatile OS_U32*) (__PMC_BASE + 0x30))  /* Master clock register */
#define __PMC_SR     (*(volatile OS_U32*) (__PMC_BASE + 0x68))  /* status register */
#define __PMC_IMR    (*(volatile OS_U32*) (__PMC_BASE + 0x6C))  /* interrupt mask register */
#define PMC_PCKRDY2  (1 << 10)
#define PMC_PCKRDY1  (1 <<  9)
#define PMC_PCKRDY0  (1 <<  8)
#define PMC_MCKRDY   (1 <<  3)
#define PMC_LOCK     (1 <<  2)
#define PMC_MOSCS    (1 <<  0)
#define PMC_MASK_ALL (PMC_PCKRDY2 | PMC_PCKRDY1 | PMC_PCKRDY0 | \
                      PMC_MCKRDY  | PMC_LOCK    | PMC_MOSCS)

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       __low_level_init()
*
*       Initialize memory controller, clock generation and pll
*
*       Has to be modified, if another CPU clock frequency should be
*       used. This function is called during startup and
*       has to return 1 to perform segment initialization
*
* NOTE:
*  ICODE is the same segment as cstartup. By placing __low_level_init
*  in the same segment, we make sure it can be reached with BL.
*/
OS_INTERWORK int __low_level_init(void);  /* Avoid "no ptototype" warning */

#ifdef __IAR_SYSTEMS_ICC__
#pragma location="ICODE"
#endif

OS_INTERWORK int __low_level_init(void) {
  __WDT_MR = (1 << 15);                                           /* Initially disable watchdog */
  __MC_FMR = (0xFF << 16) & ((OS_FSYS/1000000) << 16) | __MC_FWS; /* Setup wait states          */
  /*
  * Init PLL to run at 96 MHz
  */
  __PMC_MOR  = (1 << 0)                   /* Enable main oscillator         */
             | (6 << 8);                  /* Set startup delay time         */
  while ((__PMC_SR & (1 << 0)) == 0);     /* Wait until end of startup time */
  __PMC_PLLR = 0x05                       /* Setup Pll multiplier           */
             | (28 << 8)
             | (25 << 16)
             | ( 1 << 28);                /* USBDIV is 2                    */
  while((__PMC_SR & (1 << 2)) == 0);      /* Wait until pll locked          */
  while((__PMC_SR & (1 << 3)) == 0);      /* Wait until main clock is ready */
  /* Selection of Master Clock and Processor Clock
  * select the PLL clock divided by 2
  * Switch main clock derived from pll output divided by 2
  */
  __PMC_MCKR = 3                          /* (PMC) Clock from PLL is selected  */
             | (1 <<  2);                 /* (PMC) Selected clock divided by 2 */
  while((__PMC_SR & (1 << 3)) == 0);      /* Wait until main clock is ready    */

  __RSTC_MR = (((OS_U32)0xA5 << 24) | (1 << 0));  /* write KEY and URSTEN to allow USER RESET  */
  return 1;
}

/*****  EOF  ********************************************************/

