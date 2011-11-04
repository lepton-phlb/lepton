/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license 
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable 
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under 
the MPL, indicate your decision by deleting  the provisions above and replace 
them with the notice and other provisions required by the [eCos GPL] License. 
If you do not delete the provisions above, a recipient may use your version of this file under 
either the MPL or the [eCos GPL] License."
*/
#ifndef _DEV_SDCARD_ARCH_H
#define _DEV_SDCARD_ARCH_H


/*===========================================
Includes
=============================================*/
#include "kernel/core/kernelconf.h"
#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/core/kernel.h"
#include "kernel/core/cpu.h"

#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)
	#include <ioat91sam9261.h>
	#include <intrinsic.h>
#else
	#include "cyg/hal/at91sam9261.h"
#endif

#if defined(USE_ECOS)
	#include "pkgconf/hal_arm_at91sam9261.h"
#endif

/*===========================================
Declaration
=============================================*/

/*----------------------------------------------------------------------------*/
// Macros
/*  Fonctions du driver SPI */
// launch via open interface
#define __lowlevel_drv_init()                      
//
#define __lowlevel_drv_read(__desc__,__buffer__,__size__)		_lowlevel_drv_read(__desc__,__buffer__,__size__)
#define __lowlevel_drv_write(__desc__,__buffer__,__size__)	   _lowlevel_drv_write(__desc__,__buffer__,__size__)
                                 

/* Fonctions liees au Hardware */
#if defined(CPU_ARM7) || defined(CPU_ARM9)

   //arm9
   #if defined(CPU_ARM9)

      #define SPI_TP_DLYBS_ARM9  100

      #define FREQUENCY_SPI_SD   20000000   // 25000000   // Frequence SPI de 25 Mhz
       
	  //calcul du diviseur de clk pour obtenir la frequence d'horloge souhaitee
#ifdef CPU_ARM9
	  #define SPI_SCKDIV_ARM9    (CYGNUM_HAL_ARM_AT91SAM9261_CLOCK_SPEED/FREQUENCY_SPI_SD)
#else
	  #define SPI_SCKDIV_ARM9    (__KERNEL_PERIPHERAL_FREQ/FREQUENCY_SPI_SD)
#endif
      
      #define __lowlevel_getstatus_WE	        (((*AT91C_PIOA_PDSR)&(1<<4))>>4) //=1 => not write enable
      #define __lowlevel_getstatus_CD	        (((*AT91C_PIOA_PDSR)&(1<<5))>>5) //=1 => no sd card

      #define __lowlevel_reset_CS {\
                 int t;\
                 *AT91C_PIOA_CODR = (1<<3);\
                 for(t=0;t<SPI_TP_DLYBS_ARM9;t++);\
      }

      #define __lowlevel_set_CS {\
                 int t;\
                 for(t=0;t<SPI_TP_DLYBS_ARM9;t++);\
                 *AT91C_PIOA_SODR = (1<<3);\
      }
      
   #endif

   //arm7
   #if defined(CPU_ARM7)

      //* PIO A definitions
      #define AT91C_PIOA_PER  &__PIO_PER   /*((volatile unsigned int *) 0xfffec000)*/
      #define AT91C_PIOA_PDR  &__PIO_PDR   /*((volatile unsigned int *) 0xfffec004)*/
      #define AT91C_PIOA_SODR &__PIO_SODR  /*((volatile unsigned int *) 0xfffec030)*/
      #define AT91C_PIOA_CODR &__PIO_CODR  /*((volatile unsigned int *) 0xfffec034)*/
      #define AT91C_PIOA_PDSR &__PIO_PDSR
      //* PIO B definitions
      #define AT91C_PIOB_PDSR &__PIO_PDSRB

      //
      #define SPI_TP_DLYBS_ARM7 0

      #define FREQUENCY_SPI_SD	   8000000   // Frequence de xx
      #define SPI_SCKDIV_ARM9    (__KERNEL_PERIPHERAL_FREQ/FREQUENCY_SPI_SD)
      #define SPI_SCKDIV_ARM7       (__KERNEL_CPU_FREQ/FREQUENCY_SPI_SD)
      #define SPI_SCKDIV_ARM7_55800 (__KERNEL_CPU_FREQ/(2*FREQUENCY_SPI_SD))

      #define __lowlevel_getstatus_WE	        (((*AT91C_PIOB_PDSR)&(1<<3))>>3)   //=1 => not write enable PB3
      #define __lowlevel_getstatus_CD	        (((*AT91C_PIOA_PDSR)&(1<<12))>>12) //=1 => no sd card PA12

      #define __lowlevel_reset_CS{\
                       int t;\
                       for(t=0;t<SPI_TP_DLYBS_ARM7;t++);\
                       *AT91C_PIOA_CODR = (1<<26);\
                       for(t=0;t<SPI_TP_DLYBS_ARM7;t++);\
      }

      #define __lowlevel_set_CS{\
                       int t;\
                       for(t=0;t<SPI_TP_DLYBS_ARM7;t++);\
                       *AT91C_PIOA_SODR = (1<<26);\
                       for(t=0;t<SPI_TP_DLYBS_ARM7;t++);\
      }

   #endif


#else
   #define __lowlevel_getstatus_WE (1)
   #define __lowlevel_getstatus_CD (1)
   #define __lowlevel_reset_CS     (1)
   #define __lowlevel_set_CS       (1)

#endif






#endif
