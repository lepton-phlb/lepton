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
File    : BSP.c
Purpose : BSP for Luminary LM3S9B96
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
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // Port F Clock Gating Control.

#define USER_LED  (1uL << 3)

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       BSP_Init()
*/
void BSP_Init(void) {
  int i = 0xFFFF;
  //
  // Enable the GPIO port that is used for the on-board LED.
  //
  SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;
  while (i--); // Wait a short delay until GPIO is enabled
  //
  // Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
  // enable the GPIO pin for digital function.
  //
  GPIO_PORTF_DIR_R = USER_LED;
  GPIO_PORTF_DEN_R = USER_LED;
}

/*********************************************************************
*
*       LED switching routines
*       LEDs are switched on with low level on port lines
*/
void BSP_SetLED(int Index) {
  if (Index == 0) {
    GPIO_PORTF_DATA_R |= USER_LED;
  }
}

void BSP_ClrLED(int Index) {
  if (Index == 0) {
    GPIO_PORTF_DATA_R &= ~USER_LED;
  }
}

void BSP_ToggleLED(int Index) {
  if (Index == 0) {
    GPIO_PORTF_DATA_R ^= USER_LED;
  }
}

/****** EOF *********************************************************/

