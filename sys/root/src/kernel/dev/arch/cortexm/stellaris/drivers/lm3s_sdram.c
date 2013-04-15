//*****************************************************************************
//
// sdram.c - Example demonstrating how to configure the EPI bus in SDRAM
// mode.
//
// Copyright (c) 2010-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 8049 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_epi.h"
#include "inc/hw_gpio.h"
#include "driverlib/epi.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"

//*****************************************************************************
//
//! \addtogroup epi_examples_list
//! <h1>EPI SDRAM Mode (sdram)</h1>
//!
//! This example shows how to configure the EPI bus in SDRAM mode.  This
//! example has been written to be compatible with the Texas Instruments 8MB
//! SDRAM expansion card for the DK-LM3S9B96.
//!
//! For the EPI SDRAM mode, the pinout is as follows:
//!     Address11:0 - EPI0S11:0
//!     Bank1:0     - EPI0S14:13
//!     Data15:0    - EPI0S15:0
//!     DQML        - EPI0S16
//!     DQMH        - EPI0S17
//!     /CAS        - EPI0S18
//!     /RAS        - EPI0S19
//!     /WE         - EPI0S28
//!     /CS         - EPI0S29
//!     SDCKE       - EPI0S30
//!     SDCLK       - EPI0S31
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - EPI0 peripheral
//! - GPIO Port C peripheral (for EPI0 pins)
//! - GPIO Port E peripheral (for EPI0 pins)
//! - GPIO Port F peripheral (for EPI0 pins)
//! - GPIO Port G peripheral (for EPI0 pins)
//! - GPIO Port H peripheral (for EPI0 pins)
//! - GPIO Port J peripheral (for EPI0 pins)
//! - EPI0S0 - PH3
//! - EPI0S1 - PH2
//! - EPI0S2 - PC4
//! - EPI0S3 - PC5
//! - EPI0S4 - PC6
//! - EPI0S5 - PC7
//! - EPI0S6 - PH0
//! - EPI0S7 - PH1
//! - EPI0S8 - PE0
//! - EPI0S9 - PE1
//! - EPI0S10 - PH4
//! - EPI0S11 - PH5
//! - EPI0S12 - PF4
//! - EPI0S13 - PG0
//! - EPI0S14 - PG1
//! - EPI0S15 - PF5
//! - EPI0S16 - PJ0
//! - EPI0S17 - PJ1
//! - EPI0S18 - PJ2
//! - EPI0S19 - PJ3
//! - EPI0S28 - PJ4
//! - EPI0S29 - PJ5
//! - EPI0S30 - PJ6
//! - EPI0S31 - PG7
//!
//! The following UART signals are configured only for displaying console
//! messages for this example.  These are not required for operation of EPI0.
//! - UART0 peripheral
//! - GPIO Port A peripheral (for UART0 pins)
//! - UART0RX - PA0
//! - UART0TX - PA1
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//!
//
//*****************************************************************************

//*****************************************************************************
//
// Use the following to specify the GPIO pins used by the SDRAM EPI bus.
//
//*****************************************************************************
#define EPI_PORTC_PINS (GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4)
#define EPI_PORTE_PINS (GPIO_PIN_1 | GPIO_PIN_0)
#define EPI_PORTF_PINS (GPIO_PIN_5 | GPIO_PIN_4)
#define EPI_PORTG_PINS (GPIO_PIN_7 | GPIO_PIN_1 | GPIO_PIN_0)
#define EPI_PORTH_PINS (GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2 | \
                        GPIO_PIN_1 | GPIO_PIN_0)
#define EPI_PORTJ_PINS (GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | \
                        GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0)

//*****************************************************************************
//
// The starting and ending address for the 8MB SDRAM chip (4Meg x 16bits) on
// the SDRAM daughter board.
//
//*****************************************************************************
#define SDRAM_START_ADDRESS     0x000000
#define SDRAM_END_ADDRESS       0x3FFFFF

#define SDRAM_RATE_0NS          20
#define SDRAM_RATE_1NS          20
#define SDRAM_REFRESH_INTERVAL  64
#define SDRAM_NUM_ROWS          4096

//*****************************************************************************
//
// A pointer to the EPI memory aperture.  Note that g_pusEPISdram is declared
// as volatile so the compiler should not optimize reads out of the image.
//
//*****************************************************************************
//static volatile unsigned short *g_pusEPISdram;

//*****************************************************************************
//
// Given the system clock rate and a desired EPI rate, calculate the divider
// necessary to set the EPI clock at or lower than but as close as possible to
// the desired rate.  The divider is returned and the desired rate is updated
// to give the actual EPI clock rate (in nanoseconds) that will result from
// the use of the calculated divider.
//
//*****************************************************************************
static unsigned short
EPIDividerFromRate(unsigned short *pusDesiredRate, unsigned long ulClknS)
{
    unsigned long ulDivider, ulDesired;

    //
    // If asked for an EPI clock that is at or above the system clock rate,
    // set the divider to 0 and update the EPI rate to match the system clock
    // rate.
    //
    if((unsigned long)*pusDesiredRate <= ulClknS)
    {
        *pusDesiredRate = (unsigned short)ulClknS;
        return(0);
    }

    //
    // The desired EPI rate is slower than the system clock so determine
    // the divider value to use to achieve this as best we can.  The divider
    // generates the EPI clock using the following formula:
    //
    //                     System Clock
    // EPI Clock =   -----------------------
    //                ((Divider/2) + 1) * 2
    //
    // The formula for ulDivider below is determined by reforming this
    // equation and including a (ulClknS - 1) term to ensure that we round
    // the correct way, generating an EPI clock that is never faster than
    // the requested rate.
    //
    ulDesired = (unsigned long)*pusDesiredRate;
    ulDivider = 2 * ((((ulDesired + (ulClknS - 1)) / ulClknS) / 2) - 1) + 1;

    //
    // Now calculate the actual EPI clock period based on the divider we
    // just chose.
    //
    *pusDesiredRate = (unsigned short)(ulClknS * (2 * ((ulDivider / 2) + 1)));

    //
    // Return the divider we calculated.
    //
    return((unsigned short)ulDivider);
}

//*****************************************************************************
//
// Calculate the divider parameter required by EPIDividerSet() based on the
// current system clock rate and the desired EPI rates supplied in the
// usRate0nS and usRate1nS fields of the daughter board information structure.
//
// The dividers are calculated to ensure that the EPI rate is no faster than
// the requested rate and the rate fields in psInfo are updated to reflect the
// actual rate that will be used based on the calculated divider.
//
//*****************************************************************************
static unsigned long
CalcEPIDivider(unsigned long ulClknS)
{
    unsigned short usDivider0, usDivider1;
    unsigned short pusRate[2];

    //
    // Calculate the dividers required for the two rates specified.
    //
    pusRate[0] = SDRAM_RATE_0NS;
    pusRate[1] = SDRAM_RATE_1NS;
    usDivider0 = EPIDividerFromRate(&(pusRate[0]), ulClknS);
    usDivider1 = EPIDividerFromRate(&(pusRate[1]), ulClknS);
//    psInfo->usRate0nS = pusRate[0];
//    psInfo->usRate1nS = pusRate[1];

    //
    // Munge the two dividers together into a format suitable to pass to
    // EPIDividerSet().
    //
    return((unsigned long)usDivider0 | (((unsigned long)usDivider1) << 16));
}

//*****************************************************************************
//
// Returns the configuration parameters for EPIConfigSDRAMSet() based on the
// config flags, device size and refresh interval provided in psInfo and the
// system clock rate provided in ulClkHz.
//
//*****************************************************************************
static unsigned long
SDRAMConfigGet(unsigned long ulClkHz,
               unsigned long *pulRefresh)
{
    unsigned long ulConfig;

    //
    // Start with the config flags.
    //
    ulConfig = 0;

    //
    // Set the SDRAM core frequency depending upon the system clock rate.
    //
    if(ulClkHz < 15000000)
    {
        ulConfig |= EPI_SDRAM_CORE_FREQ_0_15;
    }
    else if(ulClkHz < 30000000)
    {
        ulConfig |= EPI_SDRAM_CORE_FREQ_15_30;
    }
    else if(ulClkHz < 50000000)
    {
        ulConfig |= EPI_SDRAM_CORE_FREQ_30_50;
    }
    else
    {
        ulConfig |= EPI_SDRAM_CORE_FREQ_50_100;
    }

    //
    // Now determine the correct refresh count required to refresh the entire
    // device in the time specified.
    //
    *pulRefresh = ((ulClkHz / (unsigned short)SDRAM_NUM_ROWS) *
                  (unsigned long)SDRAM_REFRESH_INTERVAL) / 1000;

    //
    // Return the calculated configuration parameter to the caller.
    //
    return(ulConfig);
}

//*****************************************************************************
//
// Configure EPI0 in SDRAM mode.  The EPI memory space is setup using an a
// simple C array.  This example shows how to read and write to an SDRAM card
// using the EPI bus in SDRAM mode.
//
//*****************************************************************************
void
sdram_init(void)
{
    unsigned long ulClk, ulNsPerTick, ulConfig, ulRefresh;
    
    //
    // Display the setup on the console.
    //
//    UARTprintf("EPI SDRAM Mode ->\n");
//    UARTprintf("  Type: SDRAM\n");
//    UARTprintf("  Starting Address: 0x6000.0000\n");
//    UARTprintf("  End Address: 0x603F.FFFF\n");
//    UARTprintf("  Data: 16-bit\n");
//    UARTprintf("  Size: 8MB (4Meg x 16bits)\n\n");

    //
    // The EPI0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EPI0);

    //
    // For this example EPI0 is used with multiple pins on PortC, E, F, G, H,
    // and J.  The actual port and pins used may be different on your part,
    // consult the data sheet for more information.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    //
    // This step configures the internal pin muxes to set the EPI pins for use
    // with EPI.  This step is only required because the default function of
    // these pins may not be to function in EPI mode.  Please reference the
    // datasheet for more information about pin muxing.  Note that EPI0S27:20
    // are not used for the EPI SDRAM implementation.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PH3_EPI0S0);
    GPIOPinConfigure(GPIO_PH2_EPI0S1);
    GPIOPinConfigure(GPIO_PC4_EPI0S2);
    GPIOPinConfigure(GPIO_PC5_EPI0S3);
    GPIOPinConfigure(GPIO_PC6_EPI0S4);
    GPIOPinConfigure(GPIO_PC7_EPI0S5);
    GPIOPinConfigure(GPIO_PH0_EPI0S6);
    GPIOPinConfigure(GPIO_PH1_EPI0S7);
    GPIOPinConfigure(GPIO_PE0_EPI0S8);
    GPIOPinConfigure(GPIO_PE1_EPI0S9);
    GPIOPinConfigure(GPIO_PH4_EPI0S10);
    GPIOPinConfigure(GPIO_PH5_EPI0S11);
    GPIOPinConfigure(GPIO_PF4_EPI0S12);
    GPIOPinConfigure(GPIO_PG0_EPI0S13);
    GPIOPinConfigure(GPIO_PG1_EPI0S14);
    GPIOPinConfigure(GPIO_PF5_EPI0S15);
    GPIOPinConfigure(GPIO_PJ0_EPI0S16);
    GPIOPinConfigure(GPIO_PJ1_EPI0S17);
    GPIOPinConfigure(GPIO_PJ2_EPI0S18);
    GPIOPinConfigure(GPIO_PJ3_EPI0S19);
    GPIOPinConfigure(GPIO_PJ4_EPI0S28);
    GPIOPinConfigure(GPIO_PJ5_EPI0S29);
    GPIOPinConfigure(GPIO_PJ6_EPI0S30);
    GPIOPinConfigure(GPIO_PG7_EPI0S31);

    //
    // Configure the GPIO pins for EPI mode.  All the EPI pins require 8mA
    // drive strength in push-pull operation.  This step also gives control of
    // pins to the EPI module.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeEPI(GPIO_PORTC_BASE, EPI_PORTC_PINS);
    GPIOPinTypeEPI(GPIO_PORTE_BASE, EPI_PORTE_PINS);
    GPIOPinTypeEPI(GPIO_PORTF_BASE, EPI_PORTF_PINS);
    GPIOPinTypeEPI(GPIO_PORTG_BASE, EPI_PORTG_PINS);
    GPIOPinTypeEPI(GPIO_PORTH_BASE, EPI_PORTH_PINS);
    GPIOPinTypeEPI(GPIO_PORTJ_BASE, EPI_PORTJ_PINS);
    
    //
    // Now set the EPI operating mode for the daughter board detected.  We need
    // to determine some timing information based on the ID block we have and
    // also the current system clock.
    //
    ulClk = ROM_SysCtlClockGet();
    ulNsPerTick = 1000000000 / ulClk;
    //
    // Set the EPI clock divider to ensure a basic EPI clock rate no faster
    // than defined via the ucRate0nS and ucRate1nS fields in the info
    // structure.
    //
    EPIDividerSet(EPI0_BASE, CalcEPIDivider(ulNsPerTick));

//    //
//    // Sets the clock divider for the EPI module.  In this case set the
//    // divider to 0, making the EPIClock = SysClk.
//    //
//    EPIDividerSet(EPI0_BASE, 0);

    //
    // Sets the usage mode of the EPI module.  For this example we will use
    // the SDRAM mode to talk to the external 8MB SDRAM daughter card.
    //
    EPIModeSet(EPI0_BASE, EPI_MODE_SDRAM);

    //
    // Configure the SDRAM mode.  We configure the SDRAM according to our core
    // clock frequency, in this case we are in the 15 MHz < clk <= 30 MHz
    // range (i.e 16Mhz crystal).  We will use the normal (or full power)
    // operating state which means we will not use the low power self-refresh
    // state.  Set the SDRAM size to 8MB (or 64Mb) with a refresh counter of
    // 1024 clock ticks.
    // TODO: change this to select the proper clock frequency and SDRAM
    // refresh counter.
    //
    ulRefresh = 0;
    ulConfig = SDRAMConfigGet(ulClk, &ulRefresh);
    EPIConfigSDRAMSet(EPI0_BASE, ulConfig | EPI_SDRAM_FULL_POWER | EPI_SDRAM_SIZE_64MBIT, 1024);

    //
    // Set the address map.  The EPI0 is mapped from 0x60000000 to 0xCFFFFFFF.
    // For this example, we will start from a base address of 0x60000000 with
    // a size of 16MB.  We use 16MB so we have the ability to access the
    // entire 8MB SDRAM daughter card.  Since there is no 8MB option, so we
    // use the next closest one.  If you attempt to access an address higher
    // than 4Meg (since SDRAM mode uses 16-bit data, you have 4Meg of
    // of addresses by 16-bits of data) a fault will not occur since we
    // configured the EPI for 16MB addressability.  In the case that you do
    // access an address higher than 0x3FFFFF, the MSb of the address gets
    // ignored.
    //
    EPIAddressMapSet(EPI0_BASE, EPI_ADDR_RAM_SIZE_16MB | EPI_ADDR_RAM_BASE_6);

    //
    // Wait for the SDRAM wake-up to complete by polling the SDRAM
    // initialization sequence bit.  This bit is true when the SDRAM interface
    // is going through the initialization and false when the SDRAM interface
    // it is not in a wake-up period.
    //
    while(HWREG(EPI0_BASE + EPI_O_STAT) &  EPI_STAT_INITSEQ)
    {
    }
}

