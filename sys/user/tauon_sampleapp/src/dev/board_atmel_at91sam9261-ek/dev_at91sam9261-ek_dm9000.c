/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Philippe Le Boulanger.
Portions created by Philippe Le Boulanger are Copyright (C) 2011 <lepton.phlb@gmail.com>.
All Rights Reserved.

Contributor(s): Jean-Jacques Pitrolle <lepton.jjp@gmail.com>.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license 
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable 
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under 
the MPL, indicate your decision by deleting  the provisions above and replace 
them with the notice and other provisions required by the [eCos GPL] License. 
If you do not delete the provisions above, a recipient may use your version of this file under 
either the MPL or the [eCos GPL] License."
*/

/*============================================
| Includes
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/kernel_pthread.h"
#define USE_IO_LINUX
#include "kernel/core/io.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"

#include "lib/libc/termios/termios.h"

#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/core/ioctl_eth.h"

#include "kernel/dev/arch/all/eth/dev_eth_dm9000a/dev_eth_dm9000a.h"
#include "kernel/dev/arch/arm9/at91sam9261/dev_at91sam9261_eth_dm9000a/dev_at91sam9261_eth_dm9000a.h"


#if defined(USE_ECOS)
   #include "cyg/hal/at91sam9261.h"
#endif

/*============================================
| Global Declaration
==============================================*/
//#define USE_BOARD_A25400
/*
#define USE_BOARD_AT91SAM9261_EK
#if defined(USE_BOARD_AT91SAM9261_EK)
   #define DM9KS_DATA_IO    0x04
   #define DM9KS_IRQ_NO    CYGNUM_HAL_INTERRUPT_PIOC //(voir cyg/hal/hal_platform_ints.h)
#elif defined(USE_BOARD_A25400)
   #define DM9KS_DATA_IO    0x1000
   #define DM9KS_IRQ_NO    CYGNUM_HAL_INTERRUPT_IRQ1 //(voir cyg/hal/hal_platform_ints.h)
#endif
*/

extern AT91PS_SYS  AT91_SYS;

#define DM9KS_MIN_IO    0x30000000
#define DM9KS_DATA_IO   0x04
#define CYGNUM_HAL_H_PRIOR 3

static int dev_at91sam9261_ek_eth_dm9000_load(void);

dev_map_t dev_at91sam9261_ek_eth_dm9000_map={
   dev_eth_dm9000a_name,
   S_IFCHR,
   dev_at91sam9261_ek_eth_dm9000_load,
   dev_eth_dm9000a_open,
   dev_eth_dm9000a_close,
   dev_eth_dm9000a_isset_read,
   dev_eth_dm9000a_isset_write,
   dev_eth_dm9000a_read,
   dev_eth_dm9000a_write,
   dev_eth_dm9000a_seek,
   dev_eth_dm9000a_ioctl //ioctl
};
/*===========================================
Implementation
=============================================*/

/*--------------------------------------------
| Name:        dev_at91sam9261_ek_eth_dm9000_load
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int dev_at91sam9261_ek_eth_dm9000_load(void){

   dev_io_info_t dev_io_info;
   dev_io_info.io_addr  = DM9KS_MIN_IO;
   dev_io_info.io_data  = DM9KS_MIN_IO+DM9KS_DATA_IO;
   dev_io_info.irq_no   = CYGNUM_HAL_INTERRUPT_PIOC;//(voir cyg/hal/hal_platform_ints.h)
   dev_io_info.irq_prio = CYGNUM_HAL_H_PRIOR;

   // init Chip Select (CS2) for Ethernet controler DM9000A
   AT91C_BASE_SMC->SMC_CTRL2 = AT91C_SMC_READMODE | AT91C_SMC_WRITEMODE | AT91C_SMC_NWAITM_NWAIT_DISABLE | AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;
   AT91C_BASE_SMC->SMC_CYCLE2 = 0x000F000F; // RD cycle = WR cycle = 15 clock
   AT91C_BASE_SMC->SMC_SETUP2 = 0x03030303; // RD setup = WR setup = 3 clock
   AT91C_BASE_SMC->SMC_PULSE2 = 0x03030303; // RD pulse = WR pulse = 3 clock

   //disable hard reset on dm9000 chip for atmel at91sam9261-ek board.
   AT91_SYS->PMC_PCER |= (1 << (4)); //PIOC ID = 4 // Switch on clock to portC
   AT91_SYS->PIOC_PER |= (AT91C_PIO_PC10); //enable PC10 as IO
   AT91_SYS->PIOC_OER |= (AT91C_PIO_PC10); //enable PC10 as Output
   AT91_SYS->PIOC_CODR|= (AT91C_PIO_PC10); //clear PC10 line. reset is inactive.
   
   return dev_at91sam9261_eth_dm9000a_load(&dev_io_info);
}


/*============================================
| End of Source  : dev_at91sam9261-ek_dm9000.c
==============================================*/
