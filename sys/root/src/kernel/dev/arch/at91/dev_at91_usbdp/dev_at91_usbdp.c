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
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/cpu.h"
#include "kernel/core/ioctl_usb.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "dev_at91_usbdp.h"
#include "dev_at91_common_usbdp.h"

#include "kernel/dev/arch/all/usb/usbcore.h"
#include "kernel/dev/arch/all/usb/usbslave.h"

#include <string.h>
#include <stdlib.h>

#if (__tauon_cpu_device__ == __tauon_cpu_device_arm9_at91sam9260__)
   #include "cyg/hal/at91sam9260.h"
#elif (__tauon_cpu_device__ == __tauon_cpu_device_arm9_at91sam9261__)
   #include "cyg/hal/at91sam9261.h"

   #define AT91_PMC_PCER_UDP  (1 <<10)
   #define AT91_PMC_SCER_UDP  (1 << 7)

#endif

/*============================================
| Global Declaration
==============================================*/
int dev_at91_usbdp_load(board_inf_usb_t * p_inf_usb);
int dev_at91_usbdp_isset_read(desc_t desc);
int dev_at91_usbdp_isset_write(desc_t desc);
int dev_at91_usbdp_open(desc_t desc, int o_flag);
int dev_at91_usbdp_close(desc_t desc);
int dev_at91_usbdp_seek(desc_t desc,int offset,int origin);
int dev_at91_usbdp_read(desc_t desc, char* buf,int cb);
int dev_at91_usbdp_write(desc_t desc, const char* buf,int cb);
int dev_at91_usbdp_ioctl(desc_t desc,int request,va_list ap);

#if defined(USE_ECOS)
static cyg_uint32 dev_at91_usbdp_isr(cyg_vector_t vector, cyg_addrword_t data);
static void dev_at91_usbdp_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
static cyg_interrupt dev_at91_intr_it;
static cyg_handle_t dev_at91_intr_handle;
#endif

static board_inf_usb_t * p_g_board_usb;
/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name       : dev_at91_usbdp_isr
| Description:
| Parameters :
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
cyg_uint32 dev_at91_usbdp_isr(cyg_vector_t vector, cyg_addrword_t data) {
   unsigned char n;
   bool_t need_dsr = false;
   unsigned int IMR;
   unsigned int ISR;
   board_inf_usb_t * p_inf_usb = (board_inf_usb_t *)data;

   if(!p_inf_usb) {
      return CYG_ISR_HANDLED;
   }

   //warning!!! we not mask USBDP vector
   cyg_interrupt_mask(vector);

   USBDP_READ_UINT32((p_inf_usb->base_addr + UDP_IMR), IMR);
   USBDP_READ_UINT32((p_inf_usb->base_addr + UDP_ISR), ISR);

   for(n = 1; n < p_inf_usb->ep_no; n++) {
      //do any data endpoint need a data transfer ?
      if (IMR & ISR & (1 << n)) {
         need_dsr = _at91_usbdp_ep_isr(p_inf_usb, n) || need_dsr;
      }
   }
   //if we don't need any DSR re-enable interrupts and finish
   if(BITS_ARE_CLEARED ((unsigned int)(p_inf_usb->base_addr +
                                       UDP_ISR), AT91_UDP_ALLOWED_IRQs & 0xffffff01)
      && !need_dsr) {
      cyg_interrupt_acknowledge(vector);
      cyg_interrupt_unmask(vector);
      return CYG_ISR_HANDLED;
   }

   //call the DSR
   cyg_interrupt_acknowledge(vector);

   return (CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}

/*-------------------------------------------
| Name       : dev_at91_usbdp_dsr
| Description:
| Parameters :
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
void dev_at91_usbdp_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
   unsigned char n;
   board_inf_usb_t * p_inf_usb = (board_inf_usb_t *)data;
   usbs_control_endpoint * at91_usbdp_ep0 = NULL;

   if(!p_inf_usb) {
      cyg_interrupt_unmask(vector);
      return;
   }

   at91_usbdp_ep0 = (usbs_control_endpoint *)p_inf_usb->ep_tbl[0];
   //!!!warning no field available at bit number 5
   //CLEAR_BITS (AT91_UDP + AT91_UDP_GLB_STATE, 0x10);

   if(BITS_ARE_SET ((unsigned int)(p_inf_usb->base_addr + UDP_ISR), AT91C_UDP_WAKEUP)) {
      at91_usbdp_ep0->state = USBS_STATE_DEFAULT;
      _at91_usbdp_state_notify(p_inf_usb, at91_usbdp_ep0);
      //Enable Transceiver
      CLEAR_BITS((unsigned int)(p_inf_usb->base_addr + UDP_TXVC), AT91C_UDP_TXVDIS);
      //
      USBDP_WRITE_UINT32((unsigned int)(p_inf_usb->base_addr + UDP_ICR), AT91C_UDP_WAKEUP);
   }

   // RESET UDP
   if(BITS_ARE_SET ((unsigned int)(p_inf_usb->base_addr + UDP_ISR), AT91C_UDP_ENDBUSRES)) {
      at91_usbdp_ep0->state = USBS_STATE_POWERED;
      _at91_usbdp_state_notify(p_inf_usb, at91_usbdp_ep0);
      _at91_usbdp_handle_reset(p_inf_usb);

      //
      CLEAR_BITS((unsigned int)(p_inf_usb->base_addr + UDP_TXVC), AT91C_UDP_TXVDIS);

      //loop
      USBDP_WRITE_UINT32((unsigned int)(p_inf_usb->base_addr + UDP_CSRx),
                         AT91C_UDP_EPEDS | AT91C_UDP_EPTYPE_CTRL);
      USBDP_WRITE_UINT32((unsigned int)(p_inf_usb->base_addr + UDP_IER), AT91C_UDP_EPINT0);

      at91_usbdp_ep0->state = USBS_STATE_DEFAULT;
      _at91_usbdp_state_notify(p_inf_usb, at91_usbdp_ep0);

      USBDP_WRITE_UINT32((unsigned int)(p_inf_usb->base_addr + UDP_ICR), AT91C_UDP_ENDBUSRES);
   }

   if(BITS_ARE_SET ((unsigned int)(p_inf_usb->base_addr + UDP_ISR), AT91C_UDP_RXRSM)) {
      at91_usbdp_ep0->state = at91_usbdp_ep0->state & ~USBS_STATE_SUSPENDED;
      _at91_usbdp_state_notify(p_inf_usb, at91_usbdp_ep0);
      USBDP_WRITE_UINT32((unsigned int)(p_inf_usb->base_addr + UDP_ICR), AT91C_UDP_RXRSM);
   }

   if(BITS_ARE_SET ((unsigned int)(p_inf_usb->base_addr + UDP_ISR), AT91C_UDP_RXSUSP)) {
      at91_usbdp_ep0->state = at91_usbdp_ep0->state | USBS_STATE_SUSPENDED;
      _at91_usbdp_state_notify(p_inf_usb, at91_usbdp_ep0);
      //
      SET_BITS((unsigned int)(p_inf_usb->base_addr + UDP_TXVC), AT91C_UDP_TXVDIS);
      //
      USBDP_WRITE_UINT32((unsigned int)(p_inf_usb->base_addr + UDP_ICR), AT91C_UDP_RXSUSP);
   }

   if(BITS_ARE_SET ((unsigned int)(p_inf_usb->base_addr + UDP_ISR), AT91C_UDP_EPINT0)) {
      _at91_usbdp_control_dsr(p_inf_usb);
   }

   for (n = 1; n < p_inf_usb->ep_no; n++) {
      //if (*(unsigned int *) pIMR & (1 << n)) {
      if (*(unsigned int *) (p_inf_usb->base_addr + UDP_IMR) & (1 << n)) {
         _at91_usbdp_ep_dsr(p_inf_usb, n);
      }
   }

   /*   if (BITS_ARE_SET ((unsigned int)pISR, AT91C_UDP_SOFINT)) {
   USBDP_WRITE_UINT32 (pICR, AT91C_UDP_SOFINT);
   }*/

   /*   if (BITS_ARE_SET ((unsigned int)pISR, AT91C_UDP_EXTRSM)) {
   usbs_at91_ep0.state = usbs_at91_ep0.state & ~USBS_STATE_SUSPENDED;
   usbs_state_notify (&usbs_at91_ep0);
   USBDP_WRITE_UINT32 (pICR, AT91C_UDP_EXTRSM);
   }
   */

   //
   cyg_interrupt_unmask(vector);
}

/*-------------------------------------------
| Name       : dev_at91_usbdp_load
| Description:
| Parameters :
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91_usbdp_load(board_inf_usb_t * p_inf_usb) {
   p_g_board_usb = p_inf_usb;
   return 0;
}


/*-------------------------------------------
| Name       : dev_at91_usbdp_isset_read
| Description:
| Parameters :
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91_usbdp_isset_read(desc_t desc) {
   unsigned char **ppbegin = NULL;
   unsigned char **ppend = NULL;

   if(!p_g_board_usb) {
      return -1;
   }

   ppbegin = &p_g_board_usb->ep_input_pbegin[desc];
   ppend = &p_g_board_usb->ep_input_pend[desc];

   if(*ppbegin != *ppend) {
      return 0;
   }
   return -1;
}


/*-------------------------------------------
| Name       : dev_at91_usbdp_isset_write
| Description:
| Parameters :
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91_usbdp_isset_write(desc_t desc) {
   unsigned char **ppbegin = NULL;
   unsigned char **ppend = NULL;

   if(!p_g_board_usb) {
      return -1;
   }

   ppbegin = &p_g_board_usb->ep_buffer_pbegin[0]; //&p_g_board_usb->ep_output_pbegin[desc];//&usbs_at91_endpoint_pbegin[desc];
   ppend =  &p_g_board_usb->ep_buffer_pend[0]; //&p_g_board_usb->ep_output_pend[desc];//&usbs_at91_endpoint_pend[desc];

   if(*ppbegin == *ppend) {
      return 0;
   }
   return -1;
}


/*-------------------------------------------
| Name       : dev_at91_usbdp_open
| Description:
| Parameters :
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91_usbdp_open(desc_t desc, int o_flag) {
   //p_g_board_usb = (board_inf_usb_t *)ofile_lst[desc].p;

   if(!p_g_board_usb) {
      return -1;
   }

   //already open
   if(p_g_board_usb->opened) {
      return -1;
   }
   else {
      //enable USB clock
      USBDP_WRITE_UINT32(AT91C_PMC_SCER, AT91_PMC_SCER_UDP);
      USBDP_WRITE_UINT32(AT91C_PMC_PCER, AT91_PMC_PCER_UDP);

      //set pullup
      _at91_usbdp_set_pullup(USB_FALSE);
      //reset device and start endpoint0
      _at91_usbdp_reset_device(p_g_board_usb);

      //create ISR and DSR
      cyg_interrupt_create((cyg_vector_t)p_g_board_usb->irq_no,
                           (cyg_priority_t)p_g_board_usb->irq_prio, (cyg_addrword_t)p_g_board_usb,
                           &dev_at91_usbdp_isr, &dev_at91_usbdp_dsr,
                           &dev_at91_intr_handle,
                           &dev_at91_intr_it);
      //attach it
      cyg_interrupt_attach(dev_at91_intr_handle);
      ofile_lst[desc].offset = 0;
      p_g_board_usb->opened = 1;
   }
   return 0;
}


/*-------------------------------------------
| Name       : dev_at91_usbdp_close
| Description:
| Parameters :
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91_usbdp_close(desc_t desc) {
   return 0;
}


/*-------------------------------------------
| Name       : dev_at91_usbdp_seek
| Description:
| Parameters :
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91_usbdp_seek(desc_t desc,int offset,int origin) {
   return 0;
}


/*-------------------------------------------
| Name       : dev_at91_usbdp_read
| Description:
| Parameters :
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91_usbdp_read(desc_t desc, char* buf,int cb) {
   unsigned char **ppbegin =&p_g_board_usb->ep_input_pbegin[desc];
   unsigned char **ppend = &p_g_board_usb->ep_input_pend[desc];
   int r;
   unsigned int index;

   __clr_irq(); //__atomic_in();

   r=0;
   while((*ppbegin!=*ppend) && cb--) {
      buf[r++] = **ppbegin;
      index = (unsigned int)*ppbegin;

      //INC
      if((++index) == (unsigned int)(p_g_board_usb->rcv_buffer+p_g_board_usb->rcv_buffer_size)) {
         index=(unsigned int)p_g_board_usb->rcv_buffer;
      }

      *ppbegin = (unsigned char *)index;
   }

   __set_irq(); //__atomic_out();

   return r;
}


/*-------------------------------------------
| Name       : dev_at91_usbdp_write
| Description:
| Parameters :
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91_usbdp_write(desc_t desc, const char* buf,int cb) {
   volatile unsigned int pCSRx = (unsigned int)pCSRn((p_g_board_usb->base_addr + UDP_CSRx),desc); //(unsigned int)pCSRn(desc);
   volatile unsigned int pFDRx = (unsigned int)pFDRn((p_g_board_usb->base_addr + UDP_FDRx),desc); //(unsigned int)pFDRn(desc);

   volatile unsigned short space = 0;
   volatile unsigned short endpoint_size = p_g_board_usb->ep_fifo_size[desc]; //usbs_at91_endpoint_fifo_size[desc];
   unsigned char **ppbegin = &p_g_board_usb->ep_buffer_pbegin[desc]; //&p_g_board_usb->ep_output_pbegin[desc];//&usbs_at91_endpoint_pbegin[desc];
   unsigned char **ppend = &p_g_board_usb->ep_buffer_pend[desc]; //&p_g_board_usb->ep_output_pend[desc];//&usbs_at91_endpoint_pend[desc];
   volatile usbs_data_endpoint * pep = (usbs_data_endpoint *) p_g_board_usb->ep_tbl[desc]; //usbs_at91_endpoints[desc];

   if(pep->halted) {
      return 0;
   }
   //
   if(cb<0) {
      return -1;
   }

   //set buffer
   pep->buffer = (unsigned char *)buf;
   //pep->buffer_size = (cb>endpoint_size)?endpoint_size:cb;
   pep->buffer_size = cb;

   //set the working pointers
   __clr_irq();
   *ppbegin = pep->buffer;
   *ppend = (unsigned char *) ((unsigned int) *ppbegin + pep->buffer_size);

   //wait fifo become available
   while(BITS_ARE_SET(pCSRx, AT91C_UDP_TXPKTRDY)) ;

   //
   space = (unsigned int) *ppend - (unsigned int) *ppbegin;

   //send zero-packet
   if (space == endpoint_size) {
      *ppend = *ppbegin;
   }

   *ppbegin = _at91_usbdp_write_fifo_uint8 ((unsigned int)pFDRx, *ppbegin,
                                            (unsigned char *) ((unsigned int) *ppbegin +
                                                               MIN (space, endpoint_size)));
   SET_BITS (pCSRx, AT91C_UDP_TXPKTRDY);

   //last packet
   if (*ppend == *ppbegin) {
      //the packet hasn't been sent yet
      *ppend = *ppbegin - 1;
   }

   _at91_usbdp_ep_interrupt_enable(p_g_board_usb,desc,USB_TRUE); //usbs_at91_endpoint_interrupt_enable (desc, true);
   //
   __set_irq();

   return (cb>pep->buffer_size) ? pep->buffer_size : cb;
}


/*-------------------------------------------
| Name       : dev_at91_usbdp_ioctl
| Description:
| Parameters :
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91_usbdp_ioctl(desc_t desc,int request,va_list ap) {
   usbs_control_endpoint * at91_usbdp_ep0 = (usbs_control_endpoint *)p_g_board_usb->ep_tbl[0];

   switch(request) {
   case USB_START_CONTROL_ENDPOINT: {
      //really begin when enumeration data is set
      cyg_vector_t usbdp_vector = (cyg_vector_t)p_g_board_usb->irq_no;
      _at91_usbdp_ep0_start(p_g_board_usb, at91_usbdp_ep0);
      //change state
      at91_usbdp_ep0->state = USBS_STATE_POWERED;
      _at91_usbdp_state_notify(p_g_board_usb, at91_usbdp_ep0);
      //unmask ISR
      cyg_interrupt_unmask(usbdp_vector);
      return 0;
   }
   break;

   //get enumeration data for endpoint 0
   case USB_SET_ENUMERATION_DATA: {
      usbs_enumeration_data * penum_data = va_arg(ap, usbs_enumeration_data *);
      if(penum_data) {
         at91_usbdp_ep0->enumeration_data = penum_data;
         return 0;
      }
   }
   break;

   //put class handler
   case USB_SET_CLASS_HANDLER: {
      void * class_control_fn = (void *)ap;
      if(class_control_fn) {
         at91_usbdp_ep0->class_control_fn = class_control_fn;
         return 0;
      }
   }
   break;

   //set endpoint table
   case USB_SET_ENDPOINT_TABLE: {
      unsigned char ** endpoint_table = (unsigned char **)ap;
      //
      if(endpoint_table) {
         p_g_board_usb->class_ep_tbl = (unsigned char **)endpoint_table;
         return 0;
      }
   }
   break;

   //set completion function
   case USB_SET_COMPLETION_FUNCTION: {
      void * complete_fn = va_arg(ap, void *);
      unsigned int epn = va_arg(ap, int);
      if(complete_fn && epn) {
         usbs_data_endpoint * pep = (usbs_data_endpoint *) p_g_board_usb->ep_tbl[epn];   //usbs_at91_endpoints[epn];
         pep->complete_fn = complete_fn;
         return 0;
      }
   }
   break;

   //get endpoint state
   case USB_GET_CONTROL_ENDPOINT_STATE: {
      int *usb_state =  (int *)ap;
      //maybe mutex
      *usb_state = at91_usbdp_ep0->state;
      return 0;
   }
   break;

   //halt endpoint
   case USB_HALT_ENDPOINT: {
      int epn = va_arg(ap, int);

      if(epn<0 || epn>p_g_board_usb->ep_no)
         return -1;

      _at91_usbdp_ep_set_halted(p_g_board_usb, epn, 1);
      return 0;
   }
   break;

   //is halt endpoint
   case USB_IS_HALTED_ENDPOINT: {
      usbs_data_endpoint_halted_req * ep_req= (usbs_data_endpoint_halted_req *)ap;
      usbs_data_endpoint * pep = NULL;

      if(ep_req->epn<0 || ep_req->epn>p_g_board_usb->ep_no)
         return -1;

      __clr_irq();
      pep = (usbs_data_endpoint *) p_g_board_usb->ep_tbl[ep_req->epn];   //usbs_at91_endpoints[ep_req->epn];
      ep_req->halted=pep->halted;
      __set_irq();
      return 0;
   }
   break;

   //
   case USB_SET_CLEAR_FEATURE_FN: {
      void * clear_feature_fn = (void *)ap;
      if(clear_feature_fn) {
         at91_usbdp_ep0->class_clear_feature_fn = clear_feature_fn;
         return 0;
      }
   }
   break;
   //
   default:
      break;
   }
   return -1;
}


/*============================================
| End of Source  : dev_at91_usbdp.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
