//*****************************************************************************
//
// system_lm3s.c - System initialization file for CMSIS application on a
//                 Luminary Micro Stellaris microcontroller.
//
// Copyright (c) 2009 Luminary Micro, Inc.  All rights reserved.
// Software License Agreement
// 
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
// 
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 32 of the Stellaris CMSIS Package.
//
//*****************************************************************************

#include <stdint.h>
#include "lm3s_cmsis.h"
//#include "lm3s_config.h"


extern void forceLM3Sreset(void);
extern void sdram_init(void);
extern void lcd_init(void);
extern void SysCtlClockSet(unsigned long ulConfig);
extern unsigned long SysCtlClockGet(void);

/*----------------------------------------------------------------------------
  Local functions
 *---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define XTALI       (16000000UL)            /* Internal     oscillator freq */

//see sysctl.h/.c drom StellarisWare driverlib
//#include "driverlib/sysctl.h"
#define SYSCTL_SYSDIV_1         0x07800000  // Processor clock is osc/pll /1
#define SYSCTL_SYSDIV_2_5       0xC1000000uL  // Processor clock is pll / 2.5
#define SYSCTL_USE_PLL          0x00000000uL  // System clock is the PLL clock
#define SYSCTL_USE_OSC          0x00003800  // System clock is the osc clock
#define SYSCTL_XTAL_16MHZ       0x00000540uL  // External crystal is 16 MHz
#define SYSCTL_OSC_MAIN         0x00000000uL  // Oscillator source is main osc


/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock         = XTALI;   /*!< System Clock Frequency (Core Clock) */
static uint32_t boot_state_detection   = 0;


/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemCoreClock variable.
 */
#ifdef __ICCARM__
__interwork int __low_level_init (void)
#else
void SystemInit (void) //Must be called at MCU init, ideally the first call from the reset handler (see startup_ARMCM3.s)
#endif
{
//#if (CLOCK_SETUP)                       /* Clock Setup                        */
    //
    // Set to 16Mhz
    //
//    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
//                   SYSCTL_XTAL_16MHZ);
    //
    // Set cpu clock for 80 MHz
    //
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
//#endif
    SystemCoreClockUpdate();                                        /* Update the system clock variable */

#ifdef DEBUG
    //Force periferal reset on LM3S9D96 because of a soft reset issue (usefull for debug to avoid weird MCU behavior). See Stellaris errata note.
    if(boot_state_detection !=1)
    {
        boot_state_detection = 1;
        forceLM3Sreset();
    }
#endif
    sdram_init();
    
    lcd_init();
#ifdef __ICCARM__
    return 1; // Always initialize segments !
#endif
}


/**
 * Update SystemCoreClock variable
 *
 * @param  none
 * @return none
 *
 * @brief  Updates the SystemCoreClock with current core Clock 
 *         retrieved from cpu registers.
 */
void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
    SystemCoreClock = SysCtlClockGet();
}

