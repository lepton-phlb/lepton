#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H

/*=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd
// Copy

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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   ilko iliev
// Contributors:gthomas, asl
// Date:        2006-03-09
// Purpose:     AT91SAM7S platform specific support routines
// Description:
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <cyg/hal/var_io.h>
#include <cyg/hal/plf_io.h>


//#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)
#if 1

   #define PLATFORM_SETUP1 _platform_setup1
//#define CYGHWR_HAL_ARM_HAS_MMU
   #define CYGSEM_HAL_ROM_RESET_USES_JUMP
   #define UNMAPPED(name)    name

// This macro represents the initial startup code for the platform
.macro _platform_setup1
ldr sp,=0x328000                                // set temporary a small stack in internal SRAM

         movs r3,pc,lsr # 20       // If ROM startup, PC < 0x100000
cmp r3, # 0
bne             10f

ldr r0,=hardware_init
         ldr r1,=0x10000000
                  orr r0, r0, r1
ldr lr, =label_1
          orr lr, lr, r1
bx r0

10 :
//copy .vectors from sdram to sram before remap
ldr r0,=0x20000200         //adresse source
         ldr r1,=0x00300000 //adresse destination
                  ldr r2,=0x20000240 //adress fin copie

loop:
                           ldmia r0 !, {r3-r11} //chargement de 32 bytes
stmia r1 !, {r3-r11}        // stockage des 32 bytes
cmp r0, r2         // adresse de fin atteinte ?
blo loop

bl hardware_init
label_1:
.endm

#else // defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)
   #define PLATFORM_SETUP1

#endif


//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
