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


/*============================================
| Includes
==============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"


#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/core/ioctl_eth.h"

#include "kernel/dev/arch/all/eth/dev_eth_ksz885x/dev_eth_ksz885x.h"

#if defined(USE_ECOS)
   #include "cyg/hal/at91sam9261.h"
#endif
/*============================================
| Global Declaration
==============================================*/
#define CONFIG_KSZ8851_BASE_ADDR        0x30000000
#define CONFIG_KSZ8851_MLL_CMD          0x00001000
#define CYGNUM_HAL_H_PRIOR             3

static int dev_at91sam9261_eth_ksz885x_load(void);
static void _at91sam9261_eth_ksz885x_gpio_init(void);

extern const char dev_eth_ksz885x_name[];
extern int dev_eth_ksz885x_load(dev_io_info_t* p_dev_io_info);
extern int dev_eth_ksz885x_open(desc_t desc, int o_flag);
extern int dev_eth_ksz885x_close(desc_t desc);
extern int dev_eth_ksz885x_isset_read(desc_t desc);
extern int dev_eth_ksz885x_isset_write(desc_t desc);
extern int dev_eth_ksz885x_read(desc_t desc, char* buf,int size);
extern int dev_eth_ksz885x_write(desc_t desc, const char* buf,int size);
extern int dev_eth_ksz885x_seek(desc_t desc,int offset,int origin);
extern int dev_eth_ksz885x_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_at91sam9261_eth_ksz885x_map={
      dev_eth_ksz885x_name,
      S_IFCHR,
      dev_at91sam9261_eth_ksz885x_load,
      dev_eth_ksz885x_open,
      dev_eth_ksz885x_close,
      dev_eth_ksz885x_isset_read,
      dev_eth_ksz885x_isset_write,
      dev_eth_ksz885x_read,
      dev_eth_ksz885x_write,
      dev_eth_ksz885x_seek,
      dev_eth_ksz885x_ioctl
};

/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        dev_at91sam9261_eth_ksz885x_load
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int dev_at91sam9261_eth_ksz885x_load(void){

   dev_io_info_t dev_io_info;
   dev_io_info.io_addr  = CONFIG_KSZ8851_BASE_ADDR;
   dev_io_info.io_data  = CONFIG_KSZ8851_MLL_CMD;
   dev_io_info.irq_no   = CYGNUM_HAL_INTERRUPT_IRQ1;//(voir cyg/hal/hal_platform_ints.h)
   dev_io_info.irq_prio = CYGNUM_HAL_H_PRIOR;

   _at91sam9261_eth_ksz885x_gpio_init();

   return dev_eth_ksz885x_load(&dev_io_info);
}

/*--------------------------------------------
| Name:        _at91sam9261_eth_ksz885x_gpio_init
| Description: Initialize the related hardware for KS8851 chip
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _at91sam9261_eth_ksz885x_gpio_init(void){
   //Enable PIO fonction IRQ1 in Input fonction
   AT91C_BASE_PIOB->PIO_PER = AT91C_PIO_PB30;
   AT91C_BASE_PIOB->PIO_ODR = AT91C_PIO_PB30;

   // Initialite Chip Select CS2, 16 bit access Chip Select
   //======================================================
   AT91C_BASE_SMC->SMC_CTRL2 = AT91C_SMC_READMODE | AT91C_SMC_WRITEMODE | AT91C_SMC_NWAITM_NWAIT_DISABLE | AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;
   AT91C_BASE_SMC->SMC_CYCLE2 = 0x000F000F;         // RD cycle = WR cycle = 15 clock
   AT91C_BASE_SMC->SMC_SETUP2 = 0x05050505;         // RD setup = WR setup = 3 clock
   AT91C_BASE_SMC->SMC_PULSE2 = 0x05050505;         // RD pulse = WR pulse = 3 clock

}

/*============================================
| End of Source  : dev_at91sam9261_ksz885x.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
