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

#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)
   #include <atmel/ioat91sam9261.h>
#else
   #include "cyg/hal/at91sam9261.h"
   #include <string.h>
   #include <stdlib.h>
#endif


/*============================================
| Global Declaration
==============================================*/

extern AT91PS_SYS AT91_SYS;
static dev_io_info_t dev_io_info;

/*===========================================
Implementation
=============================================*/

#if defined(__KERNEL_UCORE_ECOS)
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

//
//#if defined(__KERNEL_UCORE_EMBOS) || defined(__KERNEL_UCORE_FREERTOS) 
static void dev_at91sam9261_eth_dm9000a_interrupt_isr(void){
   // 
   __hw_enter_interrupt();
   //   
   *AT91C_AIC_IVR = 0; // Debug variant of vector read, protected mode is used.
   *AT91C_AIC_ICCR = 1 << dev_io_info.irq_no; // Clears INT1 interrupt.
   if(dev_io_info.irq_no==AT91C_ID_PIOC){
      if(!(AT91_SYS->PIOC_ISR&AT91C_PIO_PC11)){
          //
          *AT91C_AIC_EOICR = 0; // Signal end of interrupt to AIC.
         //
         __hw_leave_interrupt();
         return;
      }
   }
   //
   dev_eth_dm9000a_interrupt_isr();
   //
   *AT91C_AIC_EOICR = 0; // Signal end of interrupt to AIC.
   //
   __hw_leave_interrupt();
}
//#endif

/*--------------------------------------------
| Name:        dev_at91sam9261_eth_dm9000a_load
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_at91sam9261_eth_dm9000a_load(dev_io_info_t* p_dev_io_info){
   int error;
  
   //
   if((error=dev_eth_dm9000a_load(p_dev_io_info))<0)
      return error;
   //
#if defined(__KERNEL_UCORE_ECOS)
   //
   static cyg_handle_t _eth_dm9000_handle;
   static cyg_interrupt _eth_dm9000_it;
   
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
   //
   cyg_interrupt_create(
      (cyg_vector_t)p_dev_io_info->irq_no, (cyg_priority_t)p_dev_io_info->irq_prio, 0,
      (p_dev_io_info->p_fct_isr ? p_dev_io_info->p_fct_isr : &
       dev_eth_dm9000a_interrupt_isr),
      &dev_eth_dm9000a_interrupt_dsr,
      &_eth_dm9000_handle, &_eth_dm9000_it);
   //
   cyg_interrupt_configure(p_dev_io_info->irq_no, 0 /*edge*/, 1 /*rising*/ );
   //Liaison entre l'IT crée et le vecteur d'IT
   cyg_interrupt_attach(_eth_dm9000_handle);
   //
#endif
   
#if defined(__KERNEL_UCORE_EMBOS) ||defined(__KERNEL_UCORE_FREERTOS)
   if(p_dev_io_info->irq_no==AT91C_ID_PIOC){
      AT91_SYS->PMC_PCER |= (1 << (4)); //PIOC ID = 4 // Switch on clock to portC
      AT91_SYS->PIOC_PER |= (AT91C_PIO_PC11); //enable PC11 as IO: default Input
      AT91_SYS->PIOB_PPUDR |= (AT91C_PIO_PC11); //disable pull-up on PC11
      //AT91_SYS->PIOC_IDR |= (AT91C_PIO_PC11); //disable PC11 on gpio as source of interrupt. use FIQ.
      AT91_SYS->PIOC_IER |= (AT91C_PIO_PC11); //enable PC11 on gpio as source of interrupt. don't use FIQ.
   }
   // IRQ1  interrupt vector.
   AT91C_AIC_SVR[p_dev_io_info->irq_no] = (unsigned long)&dev_at91sam9261_eth_dm9000a_interrupt_isr;
   // SRCTYPE=3, PRIOR=3. INT1 interrupt positive edge-triggered at prio 3.
   AT91C_AIC_SMR[p_dev_io_info->irq_no] = 0x63;

#endif
   //
   memcpy(&dev_io_info,p_dev_io_info,sizeof(dev_io_info_t));
   //
   return error; 
}

/*--------------------------------------------
| Name:        dev_at91sam9261_eth_dm9000a_open
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_at91sam9261_eth_dm9000a_open(desc_t desc, int o_flag){
   int error;
   //
   error=dev_eth_dm9000a_open(desc,o_flag);
   //
   if(!error && !ofile_lst[desc].nb_reader && !ofile_lst[desc].nb_writer) {  
      //
      #if defined(__KERNEL_UCORE_EMBOS) ||defined(__KERNEL_UCORE_FREERTOS)
         *AT91C_AIC_ICCR = 1 <<  dev_io_info.irq_no;    // Clears INT1 interrupt.
         *AT91C_AIC_IECR = 1 <<  dev_io_info.irq_no;    // Enable INT1 interrupt.
      #elif defined(__KERNEL_UCORE_ECOS)
         cyg_vector_t _ecos_vector =  dev_io_info.irq_no;
         cyg_interrupt_acknowledge(_ecos_vector);
         cyg_interrupt_unmask(_ecos_vector);
      #endif
   }
   
   return error;
}

/*--------------------------------------------
| Name:        dev_at91sam9261_eth_dm9000a_close
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int dev_at91sam9261_eth_dm9000a_close(desc_t desc, int o_flag){
   // 
   if(!ofile_lst[desc].nb_writer
      &&!ofile_lst[desc].nb_reader) {
      *AT91C_AIC_IDCR = 1 << dev_io_info.irq_no; // Disable INT1 interrupt.
      *AT91C_AIC_ICCR = 1 << dev_io_info.irq_no; // Clears INT1 interrupt.
   }
   //
   return  dev_eth_dm9000a_close(desc);
}

/*============================================
| End of Source  : dev_at91sam9261_eth_dm9000a.c
==============================================*/
