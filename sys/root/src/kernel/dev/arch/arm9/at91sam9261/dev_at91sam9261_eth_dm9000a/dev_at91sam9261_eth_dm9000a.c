/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

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

#if defined(USE_ECOS)
   #include "cyg/hal/at91sam9261.h"
#endif

/*============================================
| Global Declaration
==============================================*/

extern AT91PS_SYS AT91_SYS;

/*===========================================
Implementation
=============================================*/

#if defined(USE_ECOS)
static cyg_uint32 dev_at91sam9261_eth_dm9000a_interrupt_isr(cyg_vector_t vector,
                                                            cyg_addrword_t data) {
   cyg_interrupt_mask(vector);

   if(vector==CYGNUM_HAL_INTERRUPT_PIOC) { //use FIQ interrupt line as gpio irq

      if(!(AT91_SYS->PIOC_ISR&AT91C_PIO_PC11)) //PIOC_ISR must be read to re-enabled interrupt on PIOC
         return(CYG_ISR_HANDLED);

      return dev_eth_dm9000a_interrupt_isr(vector,data); //call generic dm9000 interrupt
   }
   //
   return(CYG_ISR_HANDLED);
}
#endif

/*--------------------------------------------
| Name:        dev_at91sam9261_eth_dm9000a_load
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_at91sam9261_eth_dm9000a_load(dev_io_info_t* p_dev_io_info){

   if(p_dev_io_info->irq_no==CYGNUM_HAL_INTERRUPT_PIOC) {
      //enable IRQ on PIO_PC11/FIQ pin
      AT91_SYS->PMC_PCER |= (1 << (4)); //PIOC ID = 4 // Switch on clock to portC
      AT91_SYS->PIOC_PER |= (AT91C_PIO_PC11); //enable PC11 as IO: default Input
      AT91_SYS->PIOB_PPUDR |= (AT91C_PIO_PC11); //disable pull-up on PC11
      //AT91_SYS->PIOC_IDR |= (AT91C_PIO_PC11); //disable PC11 on gpio as source of interrupt. use FIQ.
      AT91_SYS->PIOC_IER |= (AT91C_PIO_PC11); //enable PC11 on gpio as source of interrupt. don't use FIQ.

      p_dev_io_info->p_fct_isr=dev_at91sam9261_eth_dm9000a_interrupt_isr;
   }else{
      p_dev_io_info->p_fct_isr=(void*)0;
   }

   return dev_eth_dm9000a_load(p_dev_io_info);
}


/*============================================
| End of Source  : dev_at91sam9261_eth_dm9000a.c
==============================================*/

/*--------------------------------------------
| Created:  6 sept. 2010
| Revision/Date: $Revision: $  $Date: $
| Description:
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: $
|
|---------------------------------------------*/
