/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co KG                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2013  SEGGER Microcontroller GmbH & Co KG         *
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
*       OS version: 3.88a                                            *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : BSP.c
Purpose : BSP for AT91SAM9261-EK
--------  END-OF-HEADER  ---------------------------------------------
*/

#define BSP_C
#include "BSP.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

/****** Addresses of sfrs and peripheral identifiers ****************/

#define _PIOA_BASE_ADDR  (0xFFFFF400)
#define _PIOB_BASE_ADDR  (0xFFFFF600)
#define _PIOC_BASE_ADDR  (0xFFFFF800)

#define _PMC_BASE_ADDR   (0xFFFFFC00)

#define _PIOA_ID          (2)
#define _PIOB_ID          (3)
#define _PIOC_ID          (4)
#define _PIO_PER_OFFS     (0x00)
#define _PIO_ODR_OFFS     (0x14)
#define _PIO_OER_OFFS     (0x10)
#define _PIO_SODR_OFFS    (0x30)    /* Set output data              */
#define _PIO_CODR_OFFS    (0x34)    /* Clear output data register   */
#define _PIO_ODSR_OFFS    (0x38)    /* output data status register  */
#define _PIO_PDSR_OFFS    (0x3c)    /* pin data status register     */

/****** SFRs and identifiers used for LED demo **********************/

#define _PMC_PCER (*(volatile unsigned int*) (_PMC_BASE_ADDR + 0x10))

/****** Assign LEDs to Ports ****************************************/

/****** Target specific configuration *******************************/

#define _LEDPORT_BASE_ADDR  (_PIOA_BASE_ADDR)
#define _LEDPORT_ID    (_PIOA_ID)
#define _LED0_BIT      (13) /* LED1 on AT91SAM9261-EK board, active low */
#define _LED1_BIT      (14) /* LED2 on AT91SAM9261-EK board, active low */

#define _LEDPORT_PER   (*(volatile unsigned int*) (_LEDPORT_BASE_ADDR + _PIO_PER_OFFS))
#define _LEDPORT_OER   (*(volatile unsigned int*) (_LEDPORT_BASE_ADDR + _PIO_OER_OFFS))
#define _LEDPORT_SODR  (*(volatile unsigned int*) (_LEDPORT_BASE_ADDR + _PIO_SODR_OFFS))
#define _LEDPORT_CODR  (*(volatile unsigned int*) (_LEDPORT_BASE_ADDR + _PIO_CODR_OFFS))
#define _LEDPORT_STATE (*(volatile unsigned int*) (_LEDPORT_BASE_ADDR + _PIO_ODSR_OFFS))

#define _LEDMASK_ALL   ((1 << _LED0_BIT) | (1 << _LED1_BIT))

//
// Ports for external Ethernet controller
#define _PIOC_PER   (*(volatile unsigned int*) (_PIOC_BASE_ADDR + _PIO_PER_OFFS))
#define _PIOC_ODR   (*(volatile unsigned int*) (_PIOC_BASE_ADDR + _PIO_ODR_OFFS))
#define _PIOC_OER   (*(volatile unsigned int*) (_PIOC_BASE_ADDR + _PIO_OER_OFFS))
#define _PIOC_SODR  (*(volatile unsigned int*) (_PIOC_BASE_ADDR + _PIO_SODR_OFFS))
#define _PIOC_CODR  (*(volatile unsigned int*) (_PIOC_BASE_ADDR + _PIO_CODR_OFFS))


/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       LED_Init()
*/
void BSP_Init(void) {
  _PMC_PCER     |= (1 << _LEDPORT_ID); /* Enable peripheral clock for LED-Port    */
  _LEDPORT_PER   = _LEDMASK_ALL;       /* enable Port           */
  _LEDPORT_OER   = _LEDMASK_ALL;       /* switch to output mode */
  _LEDPORT_SODR  = _LEDMASK_ALL;       /* Initially clear LEDs  */
}

/*********************************************************************
*
*       BSP_ETH_Init()
*/
void BSP_ETH_Init(unsigned Unit) {
  int i;

  (void) Unit;    // Multiple units not supported by this version, suppress warning

  _PMC_PCER   |= (1 << _PIOC_ID);    // Enable peripheral clock
  _PIOC_PER   = (1 << 10) | (1 << 11); // Enable Ports for RESET and Interrupt
  _PIOC_OER   = (1 << 10);             // Switch RESET to output mode
  _PIOC_ODR   = (1 << 11);             // Switch Interrupt to output mode
  //
  // Activate & deactivate RESET of Ethernet controller.
  // We do this in a loop to allow sufficient time for Controller to get out of RESET
  //
  for (i = 0; i < 1000; i++) {
    _PIOC_SODR  = (1 << 10);             // Activate RESET
  }
  for (i = 0; i < 1000; i++) {
    _PIOC_CODR  = (1 << 10);             // Deactivate RESET
  }
}

/*********************************************************************
*
*       LED switching routines
*       LEDs are switched on with low level on port lines
*/
void BSP_SetLED(int Index) {
  _LEDPORT_CODR = (1 << (Index + 13));         /* Switch on LED 0  */
}

void BSP_ClrLED(int Index) {
  _LEDPORT_SODR = (1 << (Index + 13));         /* Switch off LED 0 */
}

void BSP_ToggleLED(int Index) {
  if (_LEDPORT_STATE & (1 << (Index + 13))) {  /* LED is switched off */
    BSP_SetLED(Index);
  } else {
    BSP_ClrLED(Index);
  }
}

/****** EOF *********************************************************/

