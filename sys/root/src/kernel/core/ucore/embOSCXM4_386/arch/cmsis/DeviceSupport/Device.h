/**************************************************************************//**
 * @file     DEVICE.h
 * @brief    CMSIS Cortex-M3 Device Access Layer Header File
 *           for the generic CM3 Device Series
 * @version  V2.00
 * @date     10. April 2011
 *
 * @note
 * Copyright (C) 2010-2011 Segger Microcontroller GmbH & CoKG
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

#ifndef __DEVICE_H__
#define __DEVICE_H__

#ifdef __cplusplus
  extern "C" {
#endif


typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                             */
  MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M3 Memory Management Interrupt              */
  BusFault_IRQn               = -11,    /*!< 5 Cortex-M3 Bus Fault Interrupt                      */
  UsageFault_IRQn             = -10,    /*!< 6 Cortex-M3 Usage Fault Interrupt                    */
  SVCall_IRQn                 = -5,     /*!< 11 Cortex-M3 SV Call Interrupt                       */
  DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M3 Debug Monitor Interrupt                 */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M3 Pend SV Interrupt                       */
  SysTick_IRQn                = -1      /*!< 15 Cortex-M3 System Tick Interrupt                   */

/******  Device specific exception numbers follow here... *****************************************/

} IRQn_Type;


/* __NVIC_PRIO_BITS and others have to be defined for CMSIS 2.x ***********************************/

#define __MPU_PRESENT             0         /*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          4         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */

#include "core_cm3.h"
#include "system_Device.h"

#ifdef __cplusplus
}
#endif

#endif // __DEVICE_H__

