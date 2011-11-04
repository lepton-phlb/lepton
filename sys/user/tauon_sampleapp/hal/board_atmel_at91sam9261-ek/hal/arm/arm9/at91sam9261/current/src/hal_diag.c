/*=============================================================================
//
//      hal_diag_dbg.c
//
//      HAL diagnostic output code using the debug serial port
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2006 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jskov
// Contributors:jskov, gthomas
// Date:        2001-07-12
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/hal.h>
#include CYGBLD_HAL_VARIANT_H           // Variant specific configuration
#include CYGBLD_HAL_PLATFORM_H          // Platform specific configuration

#include <cyg/infra/cyg_type.h>         // base types

#include <cyg/hal/hal_arch.h>           // SAVE/RESTORE GP macros
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_if.h>             // interface API
#include <cyg/hal/hal_intr.h>           // HAL_ENABLE/MASK/UNMASK_INTERRUPTS
#include <cyg/hal/hal_misc.h>           // Helper functions
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED
#include <cyg/hal/hal_diag.h>

//-----------------------------------------------------------------------------
typedef struct {
    cyg_int32 msec_timeout;
    int isr_vector;
    cyg_uint32 baud_rate;
} channel_data_t;

static channel_data_t at91_ser_channels[1] = {
    { 1000, CYGNUM_HAL_INTERRUPT_DBG, CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD}
};

//-----------------------------------------------------------------------------

static void cyg_hal_plf_serial_dbg_init_channel(void* __ch_data)
{
	//do nothing
}

void cyg_hal_plf_serial_dbg_putc(void* __ch_data, char c)
{
	//do nothing
}

static cyg_bool cyg_hal_plf_serial_dbg_getc_nonblock(void* __ch_data, cyg_uint8* ch)
{
	//do nothing
    return false;
}

cyg_uint8 cyg_hal_plf_serial_dbg_getc(void* __ch_data)
{
		//do nothing
    return 0;
}

static void cyg_hal_plf_serial_dbg_write(void* __ch_data, const cyg_uint8* __buf,
                             cyg_uint32 __len)
{
	//do nothing
}

static void cyg_hal_plf_serial_dbg_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
	//do nothing
}

cyg_bool cyg_hal_plf_serial_dbg_getc_timeout(void* __ch_data, cyg_uint8* ch)
{
	//do nothing
	return false;
}

static int cyg_hal_plf_serial_dbg_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
	//do nothing
	return 0;
}

static int cyg_hal_plf_serial_dbg_isr(void *__ch_data, int* __ctrlc,
                       CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{
	//do nothing
	return CYG_ISR_HANDLED;
}



static void cyg_hal_plf_serial_init(void)
{
/*    hal_virtual_comm_table_t* comm;
    int cur;

    cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

    // Init channels
    cyg_hal_plf_serial_dbg_init_channel(&at91_ser_channels[0]);

    // Setup procs in the vector table

    // Set channel 0
    CYGACC_CALL_IF_SET_CONSOLE_COMM(0);
    comm = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_COMM_IF_CH_DATA_SET(*comm, &at91_ser_channels[0]);
    CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_plf_serial_dbg_write);
    CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_plf_serial_dbg_read);
    CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_plf_serial_dbg_putc);
    CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_plf_serial_dbg_getc);
    CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_plf_serial_dbg_control);
    CYGACC_COMM_IF_DBG_ISR_SET(*comm, cyg_hal_plf_serial_dbg_isr);
    CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_plf_serial_dbg_getc_timeout);

    // Restore original console
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);*/
    //do nothing
    
}

void cyg_hal_plf_comms_init(void)
{
    static int initialized = 0;

    if (initialized)
        return;

    initialized = 1;

	//do nothing
//    cyg_hal_plf_serial_init();
}


//-----------------------------------------------------------------------------
// End of hal_diag_dbg.c
