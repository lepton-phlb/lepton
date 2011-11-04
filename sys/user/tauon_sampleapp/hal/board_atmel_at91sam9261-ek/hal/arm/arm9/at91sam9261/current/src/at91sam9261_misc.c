/*==========================================================================
//
//      at91sam9261_misc.c
//
//      HAL misc board support code for Atmel AT91SAM9261
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Nick Garnett <nickg@calivar.com>
// Copyright (C) 2006 eCosCentric Ltd
// Copyright (C) 2006 Andrew Lunn <andrew.lunn@ascom.ch>
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    ilko iliev
// Contributors: gthomas, jskov, nickg, tkoeller, Oliver Munz, Andrew Lunn
// Date:         2006-03-09
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // necessary?
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_if.h>             // calling interface
#include <cyg/hal/hal_misc.h>           // helper functions
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#include <cyg/hal/drv_api.h>            // HAL ISR support
#endif


// -------------------------------------------------------------------------
// Hardware init
void plf_hardware_init (void) 
{
	unsigned i;

    // Reset all interrupts
    //HAL_WRITE_UINT32(AT91C_AIC_IDCR, 0xFFFFFFFF);
    HAL_WRITE_UINT32(*AT91C_AIC_IDCR, 0xFFFFFFFF);
    // Flush internal priority level stack
    for (i = 0; i < 8; ++i)
        HAL_WRITE_UINT32(AT91C_AIC_EOICR, 0xFFFFFFFF);

  	// Setup the Reset controller. Allow user resets 
  	//==============================================
	AT91C_BASE_RSTC->RSTC_RMR = 0xA5000000 | AT91C_RSTC_URSTEN | AT91C_RSTC_ERSTL;


//#ifndef CYGPKG_IO_WATCHDOG
	// Disable the watchdog. The eCos philosophy is that the watchdog is
    // disabled unless the watchdog driver is used to enable it.
	// Whoever if we disable it here we cannot re-enable it in the
	// watchdog driver, hence the conditional compilation.
	//==================================================================
	//AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;
//#endif

	// USB device
	//===========
	//AT91F_UDP_CfgPMC();
	
	hal_clock_initialize(CYGNUM_HAL_RTC_PERIOD);
}


void hal_clock_initialize(cyg_uint32 period)
{
  cyg_uint32 sr;
  
  CYG_ASSERT(CYGNUM_HAL_INTERRUPT_RTC == CYGNUM_HAL_INTERRUPT_PITC,
             "Invalid timer interrupt");
  
  //Set Period Interval timer and enable interrupt
  AT91C_BASE_PITC->PITC_PIMR = 
                   (period - 1) |  
                   AT91C_PITC_PITEN |
                   AT91C_PITC_PITIEN;
  
  // Read the status register to clear any pending interrupt
  //HAL_READ_UINT32(AT91C_BASE_PITC->PITC_PISR, sr);
  sr = AT91C_BASE_PITC->PITC_PISR;
}


// Calculate the baud value to be programmed into the serial port baud
// rate generators. This function will determine what the clock speed
// is that is driving the generator so it can be used in situations
// when the application dynamically changes the clock speed. 
cyg_uint32 hal_at91sam9261_us_baud(cyg_uint32 baud_rate)
{
cyg_uint32 val;
cyg_uint32 main_clock = 0;
cyg_uint32 baud_value = 0;

  	val = AT91C_BASE_PMC->PMC_MCKR;
  	switch (val & 0x03) 
  	{
    	/* Slow clock */
    	case AT91C_PMC_CSS_SLOW_CLK /*AT91_PMC_MCKR_SLOW_CLK*/:
      		main_clock = CYGNUM_HAL_ARM_AT91SAM9261_SLOW_CLOCK;
      		break;
      
      	/* Main clock */
    	case AT91C_PMC_CSS_MAIN_CLK /*AT91_PMC_MCKR_MAIN_CLK*/:
      		main_clock = CYGNUM_HAL_ARM_AT91SAM9261_CLOCK_OSC_MAIN;
      		break;
      		
      	/* PLL */
    	case AT91C_PMC_CSS_PLLA_CLK /*AT91_PMC_MCKR_PLL_CLK*/:
//      		HAL_READ_UINT32((AT91_PMC+AT91_PMC_PLLR), pll);
//      		main_clock = CYGNUM_HAL_ARM_AT91_CLOCK_OSC_MAIN * (((pll & 0x7FF0000) >> 16) + 1) / (pll & 0xFF);
		main_clock = CYGNUM_HAL_ARM_AT91SAM9261_CLOCK_SPEED;
      		break;
  }
  
  // Process prescale
  val = (val & 0x1C) >> 2;
  main_clock = main_clock >> val;

  /* Define the baud rate divisor register, (round) */
  baud_value = (main_clock/(8*baud_rate)+1)/2;
  
  return baud_value;
}

//
int hal_get_board_remap(void) {
	unsigned int *remap = (unsigned int *) 0;
  unsigned int *ram = (unsigned int *) AT91SAM9261_SRAM_PHYS_BASE;

  // Try to write in 0 and see if this affects the RAM
  unsigned int temp = *ram;
  *ram = temp + 1;
  if (*remap == *ram) {
    *ram = temp;
    return 1; //remap is already done for SDRAM
  }
  else {
    *ram = temp;
    return 0; //remap is already done for CS0
  }
}


//==================================================================================
//						hardware_init()
//						---------------
//
//===================================================================================
void hardware_init(void)
{
register int value;

	// Setup the Reset controller. Allow user resets 
  	//==============================================
	AT91C_BASE_RSTC->RSTC_RMR = 0xA5000000 | AT91C_RSTC_URSTEN | AT91C_RSTC_ERSTL;


	// initialite CS0, 16 bit access, 0x10000000
	//==========================================
/*	AT91C_BASE_SMC->SMC_SETUP0 = 10*AT91C_SMC_NWESETUP +			// NWE setup length
								 10*AT91C_SMC_NCSSETUPWR +			// NCS setup length in write access
								 10*AT91C_SMC_NRDSETUP + 			// NRD setup length
								 10*AT91C_SMC_NCSSETUPRD;			// NCS setup length in read access
								 
	AT91C_BASE_SMC->SMC_PULSE0 = 11*AT91C_SMC_NWEPULSE +			// NWE pulse length
								 11*AT91C_SMC_NCSPULSEWR +			// NCS pulse length in write access
								 11*AT91C_SMC_NRDPULSE +			// NRD pulse length
								 11*AT91C_SMC_NCSPULSERD;			// NCS pulse length in read access
								 
	AT91C_BASE_SMC->SMC_CYCLE0 = 22*AT91C_SMC_NWECYCLE +			// Total Write cycle length
								 22*AT91C_SMC_NRDCYCLE;				// Total Read cycle length
	
	AT91C_BASE_SMC->SMC_CTRL0 = AT91C_SMC_READMODE +
								AT91C_SMC_WRITEMODE +				
								AT91C_SMC_NWAITM_NWAIT_DISABLE +	// NWAIT disabled	
								AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS +	// 16-bit bus
								6*AT91C_SMC_TDF +					// data float time
								AT91C_SMC_TDFEN;					// TDF optimization enabled
*/
  
  // initialite CS0, 16 bit access, 0x10000000
	//==========================================
	// valeur en hexa renter dans le regsitre SMC_SETUP0 0x00000002
	AT91C_BASE_SMC->SMC_SETUP0 = 2*AT91C_SMC_NWESETUP +			// NWE setup length 2 coup de clock
								 0*AT91C_SMC_NCSSETUPWR +			// NCS setup length in write access 0 coup de clock
								 0*AT91C_SMC_NRDSETUP + 			// NRD setup length 0 coup de clock
								 0*AT91C_SMC_NCSSETUPRD;			// NCS setup length in read access 0 coup de clock

	//valeur en hexa rentrer dans le registre SMC_PULSE0 	0x0E0E0E06
	AT91C_BASE_SMC->SMC_PULSE0 = 6*AT91C_SMC_NWEPULSE +			// NWE pulse length
								 14*AT91C_SMC_NCSPULSEWR +			// NCS pulse length in write access
								 14*AT91C_SMC_NRDPULSE +			// NRD pulse length
								 14*AT91C_SMC_NCSPULSERD;			// NCS pulse length in read access

	//valeur en hexa rentrer dans le registre SMC_CYCLE0 0x000E000E
	AT91C_BASE_SMC->SMC_CYCLE0 = 14*AT91C_SMC_NWECYCLE +			// Total Write cycle length
								 14*AT91C_SMC_NRDCYCLE;				// Total Read cycle length

	//valeur en hexa renter dans le registre SMC_CTRL0 0x00011003
	AT91C_BASE_SMC->SMC_CTRL0 = AT91C_SMC_READMODE +
								AT91C_SMC_WRITEMODE +
								AT91C_SMC_NWAITM_NWAIT_DISABLE +	// NWAIT disabled
								AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS +	// 16-bit bus
								1*AT91C_SMC_TDF ;//1 tdf
	
  /*AT91C_BASE_SMC->SMC_SETUP0 = 0x00000002;
  AT91C_BASE_SMC->SMC_PULSE0 = 0x0E0E0E06;
  AT91C_BASE_SMC->SMC_CYCLE0 = 0x000E000E;
  AT91C_BASE_SMC->SMC_CTRL0  = (AT91C_SMC_READMODE
                                  | AT91C_SMC_WRITEMODE
                                  | AT91C_SMC_NWAITM_NWAIT_DISABLE
                                  | ((0x1 << 16) & AT91C_SMC_TDF));

  AT91C_BASE_SMC->SMC_CTRL0 |= AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;*/
  
        // initialite CS2, 16 bit access
	//=============================
	/*
	AT91C_BASE_SMC->SMC_SETUP2 = 3*AT91C_SMC_NWESETUP +				// NWE setup length
								 3*AT91C_SMC_NCSSETUPWR +			// NCS setup length in write access
								 3*AT91C_SMC_NRDSETUP + 			// NRD setup length
								 3*AT91C_SMC_NCSSETUPRD;			// NCS setup length in read access
								 
	AT91C_BASE_SMC->SMC_PULSE2 = 4*AT91C_SMC_NWEPULSE +				// NWE pulse length
								 4*AT91C_SMC_NCSPULSEWR +			// NCS pulse length in write access
								 4*AT91C_SMC_NRDPULSE +				// NRD pulse length
								 4*AT91C_SMC_NCSPULSERD;			// NCS pulse length in read access
								 
	AT91C_BASE_SMC->SMC_CYCLE2 = 8*AT91C_SMC_NWECYCLE +				// Total Write cycle length
								 8*AT91C_SMC_NRDCYCLE;				// Total Read cycle length
	
	AT91C_BASE_SMC->SMC_CTRL2 = AT91C_SMC_READMODE +
								AT91C_SMC_WRITEMODE +				
								AT91C_SMC_NWAITM_NWAIT_DISABLE +	// NWAIT disabled	
								AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS +	// 16-bit bus
								6*AT91C_SMC_TDF +					// data float time
								AT91C_SMC_TDFEN;					// TDF optimization enabled
        */


	// set PLLA
	//=========
	if ( (AT91C_BASE_PMC->PMC_MCKR & AT91C_PMC_CSS) == 0 )
	{
		// Enable the main oscillator
		//===========================
		AT91C_BASE_CKGR->CKGR_MOR = (0x20 << 8) + AT91C_CKGR_MOSCEN;
	
		if ( (AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKA) == 0 )
		{
			value = AT91C_BASE_CKGR->CKGR_PLLAR;
			value &= ~AT91C_CKGR_DIVA;
			value &= ~AT91C_CKGR_OUTA;
			
			value |= (CYGNUM_HAL_ARM_AT91SAM9261_PLLA_DIVIDER);
			value |= ( CYGNUM_HAL_ARM_AT91SAM9261_CPU_CLOCK_SPEED > 180*1000*1000 ) ? AT91C_CKGR_OUTA_2 : AT91C_CKGR_OUTA_0;
	
			value |= AT91C_CKGR_SRCA;
			
			AT91C_BASE_CKGR->CKGR_PLLAR = value;
		
			value = AT91C_BASE_CKGR->CKGR_PLLAR;
			value &= ~AT91C_CKGR_MULA;
			
			value |= ((CYGNUM_HAL_ARM_AT91SAM9261_PLLA_MULTIPLIER-1) <<16);	
		
			AT91C_BASE_CKGR->CKGR_PLLAR = value;
		
			// wait for lock
			//==============
			while (!( AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKA) );
		}
	
		// change divider = 3, pres = 1
		//=============================
		value = AT91C_BASE_PMC->PMC_MCKR;
		value &= ~AT91C_PMC_MDIV;
	 	
		value |= AT91C_PMC_MDIV_2;
	
		value &= ~AT91C_PMC_PRES;
		value |= AT91C_PMC_PRES_CLK;
		AT91C_BASE_PMC->PMC_MCKR = value;
	
		// wait for update
		//================
		while ( !(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY) ) ;
	
		// change CSS = PLLA
		//===================
		value &= ~AT91C_PMC_CSS;
		value |= AT91C_PMC_CSS_PLLA_CLK;
		AT91C_BASE_PMC->PMC_MCKR = value;
	
		// wait for update
		//================
		while ( !( AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY) );
	}

	// USB clock 48MHz
	//================
  	AT91C_BASE_PMC->PMC_SCER = AT91C_PMC_UDP;
	AT91C_BASE_PMC->PMC_PCER = AT91C_ID_UDP;

	if ( (AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKB) == 0 )
	{
		value = AT91C_BASE_CKGR->CKGR_PLLBR;
		value &= ~AT91C_CKGR_DIVB;
		value &= ~AT91C_CKGR_OUTB;
		
		value |= (CYGNUM_HAL_ARM_AT91SAM9261_PLLB_DIVIDER);
		value |= ( CYGNUM_HAL_ARM_AT91SAM9261_PLLB_CLOCK > 180*1000*1000 ) ? AT91C_CKGR_OUTB_2 : AT91C_CKGR_OUTB_0;

		if ( CYGNUM_HAL_ARM_AT91SAM9261_USB_CLOCK_DIVISION == 1 )
			value |= AT91C_CKGR_USBDIV_0;							// divided by 1	
		else	
		if ( CYGNUM_HAL_ARM_AT91SAM9261_USB_CLOCK_DIVISION == 2 )
			value |= AT91C_CKGR_USBDIV_1;							// divided by 2
		else	
			value |= AT91C_CKGR_USBDIV_2;							// divided by 4
		
		AT91C_BASE_CKGR->CKGR_PLLBR = value;
	
		// multiplyer
		//===========
		value = AT91C_BASE_CKGR->CKGR_PLLBR;
		value &= ~AT91C_CKGR_MULB;
		
		value |= ((CYGNUM_HAL_ARM_AT91SAM9261_PLLB_MULTIPLIER-1) << 16);				// PLL_CLOCK = (CLOCK_IN/DIVIDER)*(MUL+1)
	
		AT91C_BASE_CKGR->CKGR_PLLBR = value;
	
		// wait for lock
		//==============
		while (!( AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKB) );
	}

	// setup SDRAM access
	// EBI chip-select register (CS1 = SDRAM controller)
	// 9 col, 13row, 4 bank, CAS2
	// write recovery = 2 (Twr)
	// row cycle = 5 (Trc)
	// precharge delay = 2 (Trp)
	// row to col delay 2 (Trcd)
	// active to precharge = 4 (Tras)
	// exit self refresh to active = 6 (Txsr)
	//=========================================
	if ( AT91C_BASE_SDRAMC->SDRAMC_TR == 0 )
	{
		// CS1 is for SDRAMC
		//==================
	    AT91C_BASE_MATRIX->MATRIX_EBICSA = 0x10A;  // |= AT91C_MATRIX_CS1A_SDRAMC;
	
		// Enable Peripheral clock in PMC for  SDRAMC
		//===========================================
		AT91C_BASE_PMC->PMC_PCER = (unsigned int)(1 << AT91C_ID_SYS);
		
		#define SDRAMC_SIGNALS			   ((unsigned int) AT91C_PC20_D20      | \
											(unsigned int) AT91C_PC21_D21      | \
											(unsigned int) AT91C_PC30_D30      | \
											(unsigned int) AT91C_PC22_D22      | \
											(unsigned int) AT91C_PC31_D31      | \
											(unsigned int) AT91C_PC23_D23      | \
											(unsigned int) AT91C_PC16_D16      | \
											(unsigned int) AT91C_PC24_D24      | \
											(unsigned int) AT91C_PC17_D17      | \
											(unsigned int) AT91C_PC25_D25      | \
											(unsigned int) AT91C_PC18_D18      | \
											(unsigned int) AT91C_PC26_D26      | \
											(unsigned int) AT91C_PC19_D19      | \
											(unsigned int) AT91C_PC27_D27      | \
											(unsigned int) AT91C_PC28_D28      | \
											(unsigned int) AT91C_PC29_D29      )
	
		// Configure PIO controllers to drive SDRAMC signals
		//==================================================
		AT91C_BASE_PIOC->PIO_ASR = SDRAMC_SIGNALS;
		AT91C_BASE_PIOC->PIO_BSR = 0;
		AT91C_BASE_PIOC->PIO_PDR   = SDRAMC_SIGNALS;
		AT91C_BASE_PIOC->PIO_PPUDR = SDRAMC_SIGNALS;

	
		AT91C_BASE_SDRAMC->SDRAMC_CR =
			AT91C_SDRAMC_NC_9 |
			AT91C_SDRAMC_NR_13 |
			AT91C_SDRAMC_NB_4_BANKS |
			AT91C_SDRAMC_CAS_2 |
			AT91C_SDRAMC_TWR_2 |
			AT91C_SDRAMC_TRC_5 |
			AT91C_SDRAMC_TRP_2 |
			AT91C_SDRAMC_TRCD_2 |
			AT91C_SDRAMC_TRAS_4 |
			AT91C_SDRAMC_TXSR_6;
	
	
		AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_DBW_32_BITS | AT91C_SDRAMC_MODE_PRCGALL_CMD;
		*(volatile unsigned int*)AT91C_EBI_SDRAM = 0;
	
		AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_DBW_32_BITS | AT91C_SDRAMC_MODE_RFSH_CMD;
		*(volatile unsigned int*)AT91C_EBI_SDRAM = 0;
		*(volatile unsigned int*)AT91C_EBI_SDRAM = 0;
		*(volatile unsigned int*)AT91C_EBI_SDRAM = 0;
		*(volatile unsigned int*)AT91C_EBI_SDRAM = 0;
		*(volatile unsigned int*)AT91C_EBI_SDRAM = 0;
		*(volatile unsigned int*)AT91C_EBI_SDRAM = 0;
		*(volatile unsigned int*)AT91C_EBI_SDRAM = 0;
		*(volatile unsigned int*)AT91C_EBI_SDRAM = 0;
	
		AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_DBW_32_BITS | AT91C_SDRAMC_MODE_LMR_CMD;
		*(volatile unsigned int*)AT91C_EBI_SDRAM = 0;
	
		AT91C_BASE_SDRAMC->SDRAMC_TR = 7 * CYGNUM_HAL_ARM_AT91SAM9261_CLOCK_SPEED / 1000000;
	
		*(volatile unsigned int*)AT91C_EBI_SDRAM = 0;
	
		AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_DBW_32_BITS | AT91C_SDRAMC_MODE_NORMAL_CMD;
		*(volatile unsigned int*)AT91C_EBI_SDRAM = 0;
	}

  //0 remap is done for CS0
  //1 remap is done for internal ram
  if(!hal_get_board_remap()) {
    AT91C_BASE_MATRIX->MATRIX_MCFG = AT91C_MATRIX_RCA926I | AT91C_MATRIX_RCA926D;
  }
	//test mmu
	hal_mmu_init();
}

//
// This routine is called during a clock interrupt.
void hal_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
  cyg_uint32 reg;
  cyg_uint32 pimr;
  
  CYG_ASSERT(period < AT91C_PITC_PIV, "Invalid HAL clock configuration");
  
  // Check that the PIT has the right period.
  //HAL_READ_UINT32(AT91C_BASE_PITC->PITC_PIMR, pimr);
  pimr = AT91C_BASE_PITC->PITC_PIMR;
  if ((pimr & AT91C_PITC_PIV) != (period - 1)) {
    AT91C_BASE_PITC->PITC_PIMR = 
                     (period - 1) |  
                     AT91C_PITC_PITEN |
                     AT91C_PITC_PITIEN;
  }

  /* Read the value register so that we clear the interrupt */
  //HAL_READ_UINT32(AT91C_BASE_PITC->PITC_PIVR, reg);
  reg = AT91C_BASE_PITC->PITC_PIVR;
}


//
// Delay for some number of micro-seconds
// PIT is clocked at MCLK / 16
//
void hal_delay_us(cyg_int32 usecs)
{
  cyg_int64 ticks;
  cyg_uint32 val1, val2;
  cyg_uint32 piv;
  
  // Calculate how many PIT ticks the required number of microseconds
  // equate to. We do this calculation in 64 bit arithmetic to avoid
  // overflow.
  ticks = (((cyg_uint64)usecs) * 
           ((cyg_uint64)CYGNUM_HAL_ARM_AT91SAM9261_CLOCK_SPEED))/16/1000000LL;
  
  // Calculate the wrap around period. 
  //HAL_READ_UINT32(AT91C_BASE_PITC->PITC_PIMR, piv);
  piv = AT91C_BASE_PITC->PITC_PIMR;
  piv = (piv & AT91C_PITC_PIV) - 1; 
  
  hal_clock_read(&val1);
  while (ticks > 0) {
    hal_clock_read(&val2);
    if (val2 < val1)
      ticks -= ((piv + val2) - val1); //overflow occurred
    else 
      ticks -= (val2 - val1);
    val1 = val2;
   }
}

///
void hal_at91sam9261_reset_cpu() {
	//do nothing now
}

// Read the current value of the clock, returning the number of hardware
// "ticks" that have occurred (i.e. how far away the current value is from
// the start)
void
hal_clock_read(cyg_uint32 *pvalue)
{
  cyg_uint32 ir;
  cyg_uint32 pimr;
  
  // Check that the PIT is running. If not start it.
  //HAL_READ_UINT32((AT91_PITC + AT91_PITC_PIMR),pimr);
  pimr = AT91C_BASE_PITC->PITC_PIMR;
  if (!(pimr & AT91C_PITC_PITEN)) {
    //HAL_WRITE_UINT32((AT91_PITC + AT91_PITC_PIMR), 
    //                 AT91_PITC_VALUE_MASK | AT91_PITC_PIMR_PITEN);
    AT91C_BASE_PITC->PITC_PIMR = 0xfffff | AT91C_PITC_PITEN;
  }
  
  //HAL_READ_UINT32(AT91_PITC + AT91_PITC_PIIR, ir);
  ir = AT91C_BASE_PITC->PITC_PIIR;
  *pvalue = ir & 0xfffff;
}


// -------------------------------------------------------------------------

// Decode a system interrupt. Not all systems have all interrupts. So
// code will only be generated for those interrupts which have a
// defined value.
static int sys_irq_handler(void)
{
  cyg_uint32 sr, mr;

#ifdef CYGNUM_HAL_INTERRUPT_PITC
  // Periodic Interrupt Timer Controller
  //HAL_READ_UINT32(AT91C_BASE_PITC->PITC_PISR, sr);
  sr = AT91C_BASE_PITC->PITC_PISR;
  if (sr & AT91C_PITC_PITS) {
    return CYGNUM_HAL_INTERRUPT_PITC;
  }
#endif

#ifdef CYGNUM_HAL_INTERRUPT_DBG
  // Debug Unit
  //HAL_READ_UINT32(AT91C_BASE_DBGU->DBGU_CSR, sr);
  sr = AT91C_BASE_DBGU->DBGU_CSR;
  //HAL_READ_UINT32(AT91C_BASE_DBGU->DBGU_IMR, mr);
  mr = AT91C_BASE_DBGU->DBGU_IMR;
  if (sr & mr) {
    return CYGNUM_HAL_INTERRUPT_DBG;
  }
#endif

#ifdef CYGNUM_HAL_INTERRUPT_RTTC
  /* Real Time Timer. Check the interrupt is enabled, not that just
     the status indicates there is an interrupt. It takes a while for
     the status bit to clear. */
  //HAL_READ_UINT32(AT91C_BASE_RTTC->RTTC_RTSR, sr);
  sr = AT91C_BASE_RTTC->RTTC_RTSR;
  //HAL_READ_UINT32(AT91C_BASE_RTTC->RTTC_RTMR, mr);
  mr = AT91C_BASE_RTTC->RTTC_RTMR;
  if (((mr & AT91C_RTTC_ALMIEN) &&
       (sr & AT91C_RTTC_ALMS)) ||
      ((mr & AT91C_RTTC_RTTINCIEN) &&
       (sr & AT91C_RTTC_RTTINC))) {
    return CYGNUM_HAL_INTERRUPT_RTTC;
  }
#endif

#ifdef CYGNUM_HAL_INTERRUPT_PMC
  // Power Management Controller
  //HAL_READ_UINT32(AT91C_BASE_PMC->PMC_IMR, mr);
  mr = AT91C_BASE_PMC->PMC_IMR;
  //HAL_READ_UINT32(AT91C_BASE_PMC->PMC_SR, sr);
  sr = AT91C_BASE_PMC->PMC_SR;
  if ((sr & mr) & 
      (AT91C_PMC_MOSCS   |
       AT91C_PMC_LOCKA   |
       AT91C_PMC_LOCKB   |
       AT91C_PMC_MCKRDY  |
       AT91C_PMC_PCK0RDY |
       AT91C_PMC_PCK1RDY )) {
    return CYGNUM_HAL_INTERRUPT_PMC;
  }
#endif

#ifdef CYGNUM_HAL_INTERRUPT_SDRAMC
  // Memory controller
  //HAL_READ_UINT32(AT91C_BASE_SDRAMC->SDRAMC_IMR, mr);
  mr = AT91C_BASE_SDRAMC->SDRAMC_IMR;
  //HAL_READ_UINT32(AT91C_BASE_SDRAMC->SDRAMC_ISR, sr);
  sr = AT91C_BASE_SDRAMC->SDRAMC_ISR;
  if ((sr & mr) & 
      (AT91C_SDRAMC_RES)) {
    return CYGNUM_HAL_INTERRUPT_SDRAMC;
  }
#endif

#ifdef CYGNUM_HAL_INTERRUPT_WDTC
  // Watchdog Timer Controller
  //HAL_READ_UINT32(AT91C_BASE_WDTC->WDTC_WDSR, sr);
  sr = AT91C_BASE_WDTC->WDTC_WDSR;
  //HAL_READ_UINT32(AT91C_BASE_WDTC->WDTC_WDMR, mr);
  mr = AT91C_BASE_WDTC->WDTC_WDMR;
  if ((mr & AT91C_WDTC_WDFIEN) &&
      sr & (AT91C_WDTC_WDUNF |
            AT91C_WDTC_WDERR)) {
    return CYGNUM_HAL_INTERRUPT_WDTC;
  }
#endif

#ifdef CYGNUM_HAL_INTERRUPT_RSTC
  // Reset Controller
  //HAL_READ_UINT32(AT91C_BASE_RSTC->RSTC_RSR, sr);
  sr = AT91C_BASE_RSTC->RSTC_RSR;
  //HAL_READ_UINT32(AT91C_BASE_RSTC->RSTC_RMR, mr);
  mr = AT91C_BASE_RSTC->RSTC_RSR;
  if (((mr & AT91C_RSTC_URSTEN) && (sr & AT91C_RSTC_ERSTL)) ||  
      ((mr & AT91C_RSTC_URSTIEN)))
    return CYGNUM_HAL_INTERRUPT_RSTC;
#endif
  
  return CYGNUM_HAL_INTERRUPT_NONE;
}

// This routine is called to respond to a hardware interrupt (IRQ).  It
// should interrogate the hardware and return the IRQ vector number.
int hal_IRQ_handler(void)
{
    cyg_uint32 irq_num;
    cyg_uint32 ivr;
//#ifdef CYGHWR_HAL_ARM_AT91_FIQ
    // handle fiq interrupts as irq 
    cyg_uint32 ipr,imr;

    //HAL_READ_UINT32(AT91C_BASE_AIC->AIC_IPR, ipr);
    ipr = AT91C_BASE_AIC->AIC_IPR;
    //HAL_READ_UINT32(AT91C_BASE_AIC->AIC_IMR, imr);
    imr = AT91C_BASE_AIC->AIC_IMR;
    if (imr & ipr & (1 << CYGNUM_HAL_INTERRUPT_FIQ)) {
      AT91C_BASE_AIC->AIC_ICCR = (1 << CYGNUM_HAL_INTERRUPT_FIQ);
      return CYGNUM_HAL_INTERRUPT_FIQ;
    }
//#endif
    // Calculate active interrupt (updates ISR)
    //HAL_READ_UINT32(AT91C_BASE_AIC->AIC_IVR, ivr);
    ivr = AT91C_BASE_AIC->AIC_IVR;
    //HAL_READ_UINT32(AT91C_BASE_AIC->AIC_ISR, irq_num);
    irq_num = AT91C_BASE_AIC->AIC_ISR;

    if (irq_num == CYGNUM_HAL_INTERRUPT_SYS) {
      // determine the source of the system interrupt
      irq_num = sys_irq_handler();
    }

    // An invalid interrupt source is treated as a spurious interrupt    
    if (irq_num < CYGNUM_HAL_ISR_MIN || irq_num > CYGNUM_HAL_ISR_MAX)
      irq_num = CYGNUM_HAL_INTERRUPT_NONE;
    
    return irq_num;
}

//----------------------------------------------------------------------------
// Interrupt control

void hal_interrupt_mask(int vector)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");

    if (vector >= 32) {
      AT91C_BASE_AIC->AIC_IDCR = (1 << 1);
      return;
    }

    AT91C_BASE_AIC->AIC_IDCR = (1<<vector);
}

void hal_interrupt_unmask(int vector)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");

    if (vector >= 32) {
      hal_interrupt_configure(1, true, true);
      AT91C_BASE_AIC->AIC_IECR = (1 <<1);
      return;
    }

    AT91C_BASE_AIC->AIC_IECR = (1<<vector);
}


void hal_interrupt_acknowledge(int vector)
{
    // No check for valid vector here! Spurious interrupts
    // must be acknowledged, too.
    AT91C_BASE_AIC->AIC_EOICR = 0xFFFFFFFF;  
}


void hal_interrupt_configure(int vector, int level, int up)
{
    cyg_uint32 mode;

    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");

    if (vector >= 32) 
      return;

    if (level) {
        if (up) {
            mode = AT91C_AIC_SRCTYPE_HIGH_LEVEL;
        } else {
            mode = AT91C_AIC_SRCTYPE_EXT_LOW_LEVEL;
        }
    } else {
        if (up) {
            mode = AT91C_AIC_SRCTYPE_POSITIVE_EDGE;
        } else {
            mode = AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE;
        }
    }
    mode |= 7;  // Default priority
    AT91C_BASE_AIC->AIC_SMR[vector] = mode;
}

void hal_interrupt_set_level(int vector, int level)
{
    cyg_uint32 mode;

    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");
    CYG_ASSERT(level >= 0 && level <= 7, "Invalid level");

    if (vector >= 32) 
      return;

    //HAL_READ_UINT32(AT91C_BASE_AIC->AIC_SMR[vector], mode);
    mode = AT91C_BASE_AIC->AIC_SMR[vector];
    mode = (mode & ~AT91C_AIC_PRIOR_HIGHEST) | level;
    AT91C_BASE_AIC->AIC_SMR[vector] = mode;
}

/* Use the Reset Controller to generate a reset */
void hal_reset_cpu(void)
{
  AT91C_BASE_RSTC->RSTC_RCR =
                   AT91C_RSTC_PROCRST |                   
                   AT91C_RSTC_ICERST  |
                   AT91C_RSTC_PERRST  |
                   (0xA5<<24);
  while(1) CYG_EMPTY_STATEMENT;
}

//--------------------------------------------------------------------------
// EOF at91sam9261_misc.c
