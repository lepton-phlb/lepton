/**************************************************************************//**
 * @file     system_Device.c
 * @brief    CMSIS Cortex-M3 Device Peripheral Access Layer Source File
 *           generic sample
 * @version  V1.00
 * @date     30. April 2010
 *
 * @note
 * Copyright (C) 2010 Segger Microcontroller GmbH & CoKG
 *
 * @par
 * SEGGER Microcontroller (SEGGER) is supplying this software for use with Cortex-M
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that support SEGGER middleware
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * SEGGER SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/

#include "Device.h"                // The device specific header

#define __CORE_CLK  72000000uL


/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/

uint32_t SystemCoreClock = __CORE_CLK;/*!< System Clock Frequency (Core Clock)*/


/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/

void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
  /* Determine clock frequency according to clock register values             */
  SystemCoreClock = __CORE_CLK;
}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemCoreClock variable.
 */
void SystemInit (void) {
 /* Initialize ext. memory access, clock, PLL and so on... */
}

/****** End Of File *************************************************/
