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
#include "dev_at91_common_usbdp.h"
#include "dev_at91_usbdp.h"

#include "kernel/dev/arch/all/usb/usbcore.h"
#include "kernel/dev/arch/all/usb/usbslave.h"

#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"

#if (__tauon_cpu_device__ == __tauon_cpu_device_arm9_at91sam9260__)
   #include "cyg/hal/at91sam9260.h"
#elif (__tauon_cpu_device__ == __tauon_cpu_device_arm9_at91sam9261__)
   #include "cyg/hal/at91sam9261.h"
#endif

#include <string.h>
#include <stdlib.h>
/*============================================
| Global Declaration
==============================================*/

static unsigned char _at91_usbdp_ep_isr_tx(board_inf_usb_t *p_inf_usb, unsigned char epn);
static unsigned char _at91_usbdp_ep_isr_rx(board_inf_usb_t *p_inf_usb, unsigned char epn);
//
static ep0_low_level_status_t _at91_usbdp_control_error(board_inf_usb_t *p_inf_usb,
                                                        ep0_low_level_status_t status);
static ep0_low_level_status_t _at91_usbdp_control_data_recv(board_inf_usb_t *p_inf_usb,
                                                            ep0_low_level_status_t status);
static ep0_low_level_status_t _at91_usbdp_control_data_sent(board_inf_usb_t *p_inf_usb,
                                                            ep0_low_level_status_t status);
static ep0_low_level_status_t _at91_usbdp_control_setup(board_inf_usb_t *p_inf_usb,
                                                        ep0_low_level_status_t status);
static ep0_low_level_status_t _at91_usbdp_control_setup_get_status(board_inf_usb_t *p_inf_usb);
static void _at91_usbdp_control_setup_send_ack(board_inf_usb_t *p_inf_usb);
static ep0_low_level_status_t _at91_usbdp_control_setup_set_feature(board_inf_usb_t *p_inf_usb);
static ep0_low_level_status_t _at91_usbdp_control_setup_clear_feature(board_inf_usb_t *p_inf_usb);
static usbs_control_return _at91_usbdp_parse_host_get_command (usbs_control_endpoint * pcep);
/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:        _at91_usbdp_pep_to_number
| Description:
| Parameters:
| Return Type:
| Comments:    Convert an endpoint pointer to an endpoint number, using the array of endpoint structures
| See:
---------------------------------------------*/
int _at91_usbdp_pep_to_number(board_inf_usb_t *p_inf_usb,const usbs_data_endpoint * pep) {
   int epn;

   if(!p_inf_usb) {
      return 0;
   }

   for(epn=0; epn < p_inf_usb->ep_no; epn++) {
      if (pep == p_inf_usb->ep_tbl[epn])
         return epn;
   }
   return 0;
}

/*-------------------------------------------
| Name:        _at91_usbdp_ep_interrupt_enable
| Description:
| Parameters:
| Return Type:
| Comments:    Enable/Disable interrupts for a specific endpoint.
| See:
---------------------------------------------*/
void _at91_usbdp_ep_interrupt_enable (board_inf_usb_t *p_inf_usb, unsigned char epn,
                                      unsigned char enable) {
   if(!p_inf_usb) {
      return;
   }

   if (enable) {
      USBDP_WRITE_UINT32 ((p_inf_usb->base_addr + UDP_IER), 1 << epn);
   }
   else {
      USBDP_WRITE_UINT32 ((p_inf_usb->base_addr + UDP_IDR), 1 << epn);
   }
}

/*-------------------------------------------
| Name:        _at91_usbdp_ep_set_halted
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _at91_usbdp_ep_set_halted(board_inf_usb_t *p_inf_usb, int epn, unsigned char new_value) {
   unsigned int pCSR = 0;
   usbs_data_endpoint * pep = NULL;

   if(!p_inf_usb) {
      return;
   }

   pCSR = (unsigned int)pCSRn((p_inf_usb->base_addr + UDP_CSRx),epn); //unsigned int pCSR = (unsigned int)pCSRn(epn);
   pep = (usbs_data_endpoint *)(p_inf_usb->ep_tbl[epn]);


   __clr_irq(); //__atomic_in();

   if (pep->halted != new_value) {
      //there is something is to do
      pep->halted = new_value;

      if(new_value && BITS_ARE_SET((unsigned int)(p_inf_usb->base_addr + UDP_IMR), 1 << epn)) {
         //ready to transmit
         if (pep->complete_fn) {
            (*pep->complete_fn)(pep->complete_data, -1);
         }
         SET_BITS (pCSR, AT91C_UDP_FORCESTALL);
         _at91_usbdp_ep_interrupt_enable (p_inf_usb, epn, USB_TRUE);
      }
      else {
         CLEAR_BITS (pCSR, AT91C_UDP_FORCESTALL);
      }
   }
   __set_irq(); //__atomic_out();
}

/*-------------------------------------------
| Name:        _at91_usbdp_ep_init
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _at91_usbdp_ep_init(board_inf_usb_t *p_inf_usb, usbs_data_endpoint * pep,
                         unsigned char endpoint_type,
                         bool_t enable) {
   int epn = _at91_usbdp_pep_to_number(p_inf_usb, pep);
   unsigned int pCSR = 0;

   if(!p_inf_usb) {
      return;
   }
   pCSR = (unsigned int)pCSRn((p_inf_usb->base_addr + UDP_CSRx),epn);

   //
   _at91_usbdp_ep_interrupt_enable(p_inf_usb, epn, USB_FALSE);

   //reset endpoint
   USBDP_WRITE_UINT32((p_inf_usb->base_addr + UDP_RST_EP), 1 << epn);
   USBDP_WRITE_UINT32((p_inf_usb->base_addr + UDP_RST_EP), 0);

   pep->halted = USB_FALSE;

   /* Type | In */
   USBDP_WRITE_UINT32 (pCSR, (((((unsigned int) endpoint_type) & 0x03) << 8) |
                              ((((unsigned int) endpoint_type) & 0x80) << 3)));

   p_inf_usb->ep_bytes_in_fifo[epn] = 0;
   p_inf_usb->ep_bytes_received[epn] = THERE_IS_A_NEW_PACKET_IN_THE_UDP;
   p_inf_usb->ep_bank1_attr[epn] = USB_FALSE;

   if (enable) {
      SET_BITS (pCSR, AT91C_UDP_EPEDS);
   }
}

/*-------------------------------------------
| Name:        _at91_usbdp_reset_device
| Description:
| Parameters:
| Return Type:
| Comments:    reset all endpoints
| See:
---------------------------------------------*/
void _at91_usbdp_reset_device(board_inf_usb_t *p_inf_usb) {
   int epn;

   if(!p_inf_usb) {
      return;
   }

   _at91_usbdp_end_all_transfers(p_inf_usb, -1);

   USBDP_WRITE_UINT32((p_inf_usb->base_addr + UDP_IDR), 0xffffffff);
   USBDP_WRITE_UINT32((p_inf_usb->base_addr + UDP_ICR), 0xffffffff);
   USBDP_WRITE_UINT32((p_inf_usb->base_addr + UDP_RST_EP), 0xffffffff);
   USBDP_WRITE_UINT32((p_inf_usb->base_addr + UDP_RST_EP), 0x00000000);

   USBDP_WRITE_UINT32((p_inf_usb->base_addr + UDP_FADDR), AT91C_UDP_FEN);
   USBDP_WRITE_UINT32(pCSRn((p_inf_usb->base_addr + UDP_CSRx),
                            0),AT91C_UDP_EPEDS | AT91C_UDP_EPTYPE_CTRL);
   USBDP_WRITE_UINT32((p_inf_usb->base_addr + UDP_IER), AT91_UDP_ALLOWED_IRQs);

   for (epn=1; epn < p_inf_usb->ep_no; epn++) {
      _at91_usbdp_ep_init(p_inf_usb, (usbs_data_endpoint *)(p_inf_usb->ep_tbl[epn]), 0, USB_FALSE);
   }
}

/*-------------------------------------------
| Name:        _at91_usbdp_handle_reset
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _at91_usbdp_handle_reset(board_inf_usb_t *p_inf_usb) {
   const usb_endpoint_descriptor *usb_endpoint;
   int i;
   unsigned char endpoint_type=0,endpoint_number=0;

   if(!p_inf_usb) {
      return;
   }

   //
   _at91_usbdp_reset_device(p_inf_usb);

   //
   for(i=0;; i++) {
      if(!p_inf_usb->class_ep_tbl[i]) break;

      //
      usb_endpoint = (usb_endpoint_descriptor *) p_inf_usb->class_ep_tbl[i];
      endpoint_type = (usb_endpoint->attributes |
                       (usb_endpoint->endpoint &
                        USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN ?
                        USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN :
                        USB_ENDPOINT_DESCRIPTOR_ENDPOINT_OUT));
      endpoint_number = usb_endpoint->endpoint & ~(USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN);
      //
      if(endpoint_number < p_inf_usb->ep_no ) {
         _at91_usbdp_ep_init(p_inf_usb, (usbs_data_endpoint *)p_inf_usb->ep_tbl[endpoint_number],
                             endpoint_type,
                             USB_TRUE);
      }
   }
}

/*-------------------------------------------
| Name:        _at91_usbdp_end_all_transfers
| Description:
| Parameters:
| Return Type:
| Comments:    Stop all transfers that are currently active.
| See:
---------------------------------------------*/
void _at91_usbdp_end_all_transfers(board_inf_usb_t *p_inf_usb, usbs_control_return returncode) {
   unsigned int epn;
   usbs_data_endpoint *pep = NULL;

   if(!p_inf_usb) {
      return;
   }

   //
   for(epn = 1; epn < p_inf_usb->ep_no; epn++) {
      if (BITS_ARE_SET ((unsigned int)(p_inf_usb->base_addr + UDP_IMR), 1 << epn)) {
         // If the end point is transmitting, call the complete function
         // to terminate to transfer
         pep = (usbs_data_endpoint *) (p_inf_usb->ep_tbl[epn]);

         if (pep->complete_fn) {
            (*pep->complete_fn)(pep->complete_data, returncode);
         }
         // Disable interrupts from the endpoint
         _at91_usbdp_ep_interrupt_enable(p_inf_usb, epn, USB_FALSE);
      }
   }
}

/*-------------------------------------------
| Name:        _at91_usbdp_set_pullup
| Description:
| Parameters:
| Return Type:
| Comments:    Tell the host that the device is ready to start communication
| See:
---------------------------------------------*/
void _at91_usbdp_set_pullup(unsigned char set) {
   //use matrix pullup instead
   if(set) {
      SET_BITS(AT91C_MATRIX_USBPCR,AT91C_MATRIX_USBPCR_PUON);
   }
   else {
      CLEAR_BITS(AT91C_MATRIX_USBPCR, AT91C_MATRIX_USBPCR_PUON);
   }
}

/*-------------------------------------------
| Name:        _at91_usbdp_read_power
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned char _at91_usbdp_read_power(void) {
   return USB_TRUE;
}

/*-------------------------------------------
| Name:        _at91_usbdp_ep0_start
| Description:
| Parameters:
| Return Type:
| Comments:    Start Control EP
| See:
---------------------------------------------*/
void _at91_usbdp_ep0_start(board_inf_usb_t *p_inf_usb, usbs_control_endpoint * endpoint) {
   _at91_usbdp_handle_reset(p_inf_usb);
   _at91_usbdp_set_pullup (USB_TRUE);
}

/*-------------------------------------------
| Name:        _at91_usbdp_read_fifo_uint8
| Description:
| Parameters:
| Return Type:
| Comments:    Read bytes from endpoint fifo
| See:
---------------------------------------------*/
unsigned char * _at91_usbdp_read_fifo_uint8(unsigned char  * pdest, unsigned int psource,
                                            unsigned int size) {
   unsigned char *preqbyte = pdest;
   unsigned reqbyte;
   //
   while (size--) {
      USBDP_READ_UINT8 (psource, reqbyte);
      *preqbyte = reqbyte;
      preqbyte++;
   }
   //
   return preqbyte;
}

/*-------------------------------------------
| Name:        _at91_usbdp_write_fifo_uint8
| Description:
| Parameters:
| Return Type:
| Comments:    Write bytes to endpoint fifo
| See:
---------------------------------------------*/
unsigned char * _at91_usbdp_write_fifo_uint8(unsigned int pdest, unsigned char * psource,
                                             unsigned char * psource_end) {
   unsigned char *preqbyte;

   for (preqbyte = psource; preqbyte < psource_end; preqbyte++) {
      USBDP_WRITE_UINT8 (pdest, (*preqbyte));
   }

   return preqbyte;
}

/*-------------------------------------------
| Name:        _at91_usbdp_ep_isr
| Description:
| Parameters:
| Return Type:
| Comments:    ISR for an endpoint. Handle receive and transmit interrupts.
| See:
---------------------------------------------*/
unsigned char _at91_usbdp_ep_isr(board_inf_usb_t *p_inf_usb, unsigned char epn) {
   unsigned int pCSR = (unsigned int)pCSRn((p_inf_usb->base_addr + UDP_CSRx),epn); //pCSRn(epn);

   //IN : tx_endpoint
   if (BITS_ARE_SET (pCSR, /*AT91C_UDP_EPTYPE_BULK_IN*/ AT91C_UDP_TXCOMP)) {
      return _at91_usbdp_ep_isr_tx(p_inf_usb,epn);
   }
   //OUT: rx_endpoint
   else {
      if (!BITS_ARE_CLEARED (pCSR, AT91C_UDP_RX_DATA_BK0) ||
          !BITS_ARE_CLEARED (pCSR, AT91C_UDP_RX_DATA_BK1 )) {
         return _at91_usbdp_ep_isr_rx(p_inf_usb,epn);
      }
   }

   return USB_FALSE;
}

/*-------------------------------------------
| Name:        _at91_usbdp_ep_isr_tx
| Description:
| Parameters:
| Return Type:
| Comments:    Perform transmit handling on an endpoint
| See:
---------------------------------------------*/
static int dummy_isr_tx=0, dummy_isr_tx_err=0;
unsigned char _at91_usbdp_ep_isr_tx(board_inf_usb_t *p_inf_usb, unsigned char epn) {
   unsigned int pCSR = (unsigned int)pCSRn((p_inf_usb->base_addr + UDP_CSRx),epn);
   unsigned int pFDR = (unsigned int)pFDRn((p_inf_usb->base_addr + UDP_FDRx),epn);
   unsigned char **ppbegin = &p_inf_usb->ep_buffer_pbegin[epn];
   unsigned char **ppend = &p_inf_usb->ep_buffer_pend[epn];
   unsigned short endpoint_size = p_inf_usb->ep_fifo_size[epn];
   unsigned short space = 0;

   CLEAR_BITS (pCSR, AT91C_UDP_TXCOMP);

   if (BITS_ARE_CLEARED (pCSR, AT91C_UDP_TXPKTRDY )) {
      /* Ready to transmit ? */
      if (*ppend > *ppbegin) {
         /* Something to send */
         space = (unsigned int) *ppend - (unsigned int) *ppbegin;

         /*if (space == endpoint_size) {
            *ppend = *ppbegin;            //Send zero-packet
         }*/

         //
         *ppbegin =
            _at91_usbdp_write_fifo_uint8 (pFDR, *ppbegin,
                                          (unsigned char *) ((unsigned int) *ppbegin +
                                                             MIN (space,
                                                                  endpoint_size)));
         SET_BITS (pCSR, AT91C_UDP_TXPKTRDY);

         if (*ppend == *ppbegin) {       //Last packet ?
            *ppend = *ppbegin - 1;        //The packet isn't sent yet
         }
      }
      else {
         if (*ppend + 1 == *ppbegin) {
            //
            dummy_isr_tx++;
            //
            *ppend = *ppbegin;    /* Flag for DSR */
            return USB_TRUE;
         }
         else {
            *ppend = *ppbegin - 1;        /* Flag for zero-packet */
            SET_BITS (pCSR, AT91C_UDP_TXPKTRDY);       /* Send no data */
            dummy_isr_tx_err++;
         }
      }
   }

   return USB_FALSE;
}

/*-------------------------------------------
| Name:        _at91_usbdp_ep_isr_rx
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned char _at91_usbdp_ep_isr_rx(board_inf_usb_t *p_inf_usb, unsigned char epn) {
   unsigned int pCSR = (unsigned int) pCSRn((p_inf_usb->base_addr + UDP_CSRx),epn);
   unsigned int pFDR = (unsigned int) pFDRn((p_inf_usb->base_addr + UDP_FDRx),epn);
   unsigned short *pinfifo = &p_inf_usb->ep_bytes_in_fifo[epn];
   unsigned char **ppbegin = &p_inf_usb->ep_buffer_pbegin[epn];
   unsigned char **ppend = &p_inf_usb->ep_buffer_pend[epn];
   unsigned short *preceived = &p_inf_usb->ep_bytes_received[epn];
   unsigned short endpoint_size = p_inf_usb->ep_fifo_size[epn];
   unsigned int index=0;

   if(*preceived == THERE_IS_A_NEW_PACKET_IN_THE_UDP) {
      /* There is a new packet */
      *preceived = ((*(unsigned int *)pCSR) >> 16) & 0x7ff;
      *pinfifo = *preceived;
   }

   //while data are available in fifo
   while(*pinfifo) {
      /* If we have buffer-space AND data in the FIFO */
      USBDP_READ_UINT8(pFDR, **ppend);
      index=(unsigned int) *ppend;

      //INC
      if((++index) == (unsigned int)(p_inf_usb->rcv_buffer+p_inf_usb->rcv_buffer_size)) {
         index=(unsigned int)p_inf_usb->rcv_buffer;
      }
      //

      *ppend=(unsigned char *)index;
      //
      (*pinfifo)--;
   }
   //
   if(*pinfifo == 0) {
      //If the FIFO is empty, then we can release it
      //if(usbs_at91_endpoint_pingpong[epn]) {
      if(p_inf_usb->ep_pingpong_attr[epn]) {
         //Time to clear the interrupt flag
         //if(usbs_at91_endpoint_bank1[epn]) {
         if(p_inf_usb->ep_bank1_attr[epn]) {
            CLEAR_BITS (pCSR, AT91C_UDP_RX_DATA_BK1);
         }
         else {
            CLEAR_BITS (pCSR, AT91C_UDP_RX_DATA_BK0);
         }
         p_inf_usb->ep_bank1_attr[epn] = !p_inf_usb->ep_bank1_attr[epn]; //usbs_at91_endpoint_bank1[epn] = !usbs_at91_endpoint_bank1[epn];
      }
      else {
         CLEAR_BITS (pCSR, AT91C_UDP_RX_DATA_BK0);
      }

      //set usbs_at91_endpoint_pend with new value for next read
      p_inf_usb->ep_buffer_pend[epn] = *ppend;
      *preceived = THERE_IS_A_NEW_PACKET_IN_THE_UDP;
      return USB_TRUE;
   }

   return USB_FALSE;
}

/*-------------------------------------------
| Name:        _at91_usbdp_ep_dsr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static unsigned int dummy_dsr_tx, dummy_dsr_rx;
void _at91_usbdp_ep_dsr(board_inf_usb_t *p_inf_usb, unsigned char epn) {
   unsigned int pCSRx = (unsigned int)pCSRn((p_inf_usb->base_addr + UDP_CSRx),epn);
   usbs_data_endpoint *pep = (usbs_data_endpoint *) p_inf_usb->ep_tbl[epn];
   unsigned char **ppbegin = &p_inf_usb->ep_buffer_pbegin[epn];
   unsigned char **ppend = &p_inf_usb->ep_buffer_pend[epn];

   //transmit endpoint (bulk in (0x600) now)
   //TODO add iso and interrupt
   if (BITS_ARE_SET (pCSRx, AT91C_UDP_EPTYPE_BULK_IN)
       || BITS_ARE_SET (pCSRx,AT91C_UDP_EPTYPE_INT_IN)) {
      //
      if (*ppend == *ppbegin
          && *ppend) {
         pep->buffer_size = (unsigned int) *ppbegin - (unsigned int) pep->buffer;
         if (pep->buffer_size && pep->complete_fn) {
            if (!pep->halted) {
               //
               dummy_dsr_tx++;
               //
               //fire_io_int is set by usbs_set_complete_fn
               (*pep->complete_fn)(pep->buffer, pep->buffer_size);
            }
            else {
               //fire_io_int is set by usbs_set_complete_fn
               (*pep->complete_fn)(pep->complete_data, -1);
            }
         }
         //reset it
         pep->buffer=*ppbegin;
      }
   }
   //received endpoint (bulk out (0x200) now)
   //TODO add iso and interrupt
   else if (BITS_ARE_SET (pCSRx, AT91C_UDP_EPTYPE_BULK_OUT)
            /*|| BITS_ARE_SET (pCSRx, AT91C_UDP_EPTYPE_INT_OUT)*/) {
      //
      if (*ppend != *ppbegin) {
         unsigned char **input_ppend = &p_inf_usb->ep_input_pend[epn];

         if (pep->complete_fn) {
            if (!pep->halted) {
               //
               dummy_dsr_rx++;
               //
               //fire_io_int is set by usbs_set_complete_fn
               (*pep->complete_fn)(pep->buffer, *ppend-*ppbegin);
            }
            else {
               //fire_io_int is set by usbs_set_complete_fn
               (*pep->complete_fn)(pep->complete_data, -1);
            }
         }
         //reset
         *ppbegin=*ppend;
         //for user ptr
         *input_ppend=*ppbegin;
      }
   }

   ///AT91C_UDP_ISOERROR same as STALLSENT
   if(BITS_ARE_SET (pCSRx, AT91C_UDP_ISOERROR)) {
      CLEAR_BITS(pCSRx, AT91C_UDP_ISOERROR);
      CLEAR_BITS(pCSRx, AT91C_UDP_FORCESTALL);

/*    if(pep->halted) {
         pep->halted = 0;
         CLEAR_BITS(pCSRx, AT91C_UDP_FORCESTALL);
      }*/
   }
}

/*-------------------------------------------
| Name:        _at91_usbdp_state_notify
| Description:
| Parameters:
| Return Type:
| Comments:    There has been a change in state. Update the end point.
| See:
---------------------------------------------*/
void _at91_usbdp_state_notify(board_inf_usb_t *p_inf_usb, usbs_control_endpoint * pcep) {
   static int old_state = USBS_STATE_CHANGE_POWERED;
   int state = pcep->state & USBS_STATE_MASK;

   if (pcep->state != old_state) {
      _at91_usbdp_end_all_transfers (p_inf_usb,-1);
      switch (state) {
      case USBS_STATE_DETACHED:
      case USBS_STATE_ATTACHED:
      case USBS_STATE_POWERED:
         // Nothing to do
         break;
      case USBS_STATE_DEFAULT:
         USBDP_WRITE_UINT32((p_inf_usb->base_addr + UDP_GLB_STAT),0);
         break;
      case USBS_STATE_ADDRESSED:
         USBDP_WRITE_UINT32((p_inf_usb->base_addr + UDP_GLB_STAT),AT91C_UDP_FADDEN);
         break;
      case USBS_STATE_CONFIGURED:
         USBDP_WRITE_UINT32((p_inf_usb->base_addr + UDP_GLB_STAT),AT91C_UDP_CONFG);
         //dummy allow others IRQ when all things are configured
         USBDP_WRITE_UINT32((p_inf_usb->base_addr + UDP_IER),AT91_UDP_ALLOWED_IRQs);
         break;
      default:
         break;
      }
      //
      if (pcep->state_change_fn) {
         (*pcep->state_change_fn)(pcep, 0, pcep->state, old_state);
      }
      old_state = pcep->state;
   }
}

/*-------------------------------------------
| Name:        _at91_usbdp_parse_host_get_command
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
usbs_control_return _at91_usbdp_parse_host_get_command(usbs_control_endpoint * pcep) {
   usbs_control_return retcode;
   unsigned char dev_req_type =
      (((usb_devreq *) pcep->control_buffer)->type) & USB_DEVREQ_TYPE_MASK;

   switch (dev_req_type) {
   case USB_DEVREQ_TYPE_STANDARD:
      if (!pcep->standard_control_fn) {
         return usbs_handle_standard_control (pcep);
      }

      retcode = (*pcep->standard_control_fn)(pcep, pcep->standard_control_data);

      if (retcode == USBS_CONTROL_RETURN_UNKNOWN) {
         return usbs_handle_standard_control (pcep);
      }
      return retcode;

   case USB_DEVREQ_TYPE_CLASS:
      if (!pcep->class_control_fn) {
         return USBS_CONTROL_RETURN_STALL;
      }
      return (*pcep->class_control_fn)(pcep, pcep->class_control_data);

   case USB_DEVREQ_TYPE_VENDOR:
      if (!pcep->class_control_fn) {
         return USBS_CONTROL_RETURN_STALL;
      }
      return (*pcep->class_control_fn)(pcep, pcep->vendor_control_data);

   case USB_DEVREQ_TYPE_RESERVED:
      if (!pcep->reserved_control_fn) {
         return USBS_CONTROL_RETURN_STALL;
      }
      return (*pcep->reserved_control_fn)(pcep, pcep->reserved_control_data);
   default:
      return USBS_CONTROL_RETURN_STALL;
   }
}

/*-------------------------------------------
| Name:        _at91_usbdp_control_dsr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _at91_usbdp_control_dsr(board_inf_usb_t *p_inf_usb) {
   static ep0_low_level_status_t status = EP0_LL_IDLE;

   while (!BITS_ARE_CLEARED((unsigned int)(p_inf_usb->base_addr + UDP_CSRx),
                            AT91C_UDP_TXCOMP  | AT91C_UDP_RX_DATA_BK0 |
                            AT91C_UDP_RXSETUP | AT91C_UDP_ISOERROR |
                            AT91C_UDP_RX_DATA_BK1)) {

      // Check and handle any error conditions
      if (BITS_ARE_SET ((unsigned int)(p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_ISOERROR)) {
         status = _at91_usbdp_control_error(p_inf_usb, status);
      }

      // Check for a setup message and handle it
      if (BITS_ARE_SET ((unsigned int)(p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_RXSETUP)) {
         status = _at91_usbdp_control_setup(p_inf_usb, status);
      }

      // Check for received data on the control endpoint
      if (BITS_ARE_SET ((unsigned int)(p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_RX_DATA_BK0)) {
         status = _at91_usbdp_control_data_recv(p_inf_usb, status);
      }

      // Check if the last packet has been sent
      if (BITS_ARE_CLEARED ((unsigned int)(p_inf_usb->base_addr + UDP_CSRx),
                            AT91C_UDP_TXPKTRDY )) {
         status = _at91_usbdp_control_data_sent(p_inf_usb,status);
      }

      // Received an ACK packet
      if (BITS_ARE_SET ((unsigned int)(p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_TXCOMP)) {
         CLEAR_BITS ((p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_TXCOMP);
      }

      if (status == EP0_LL_STALL) {
         CLEAR_BITS((p_inf_usb->base_addr + UDP_CSRx), 0x7f);
         SET_BITS((p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_FORCESTALL);
      }
   }
}

/*-------------------------------------------
| Name:        _at91_usbdp_control_error
| Description:
| Parameters:
| Return Type:
| Comments:    Handle an error condition on the control endpoint
| See:
---------------------------------------------*/
ep0_low_level_status_t _at91_usbdp_control_error(board_inf_usb_t *p_inf_usb,
                                                 ep0_low_level_status_t status) {
   unsigned char **ppbegin = &p_inf_usb->ep_buffer_pbegin[0];
   unsigned char **ppend = &p_inf_usb->ep_buffer_pend[0];
   usbs_control_endpoint * at91_usbdp_ep0 = (usbs_control_endpoint *)p_inf_usb->ep_tbl[0];


   at91_usbdp_ep0->buffer_size = 0;
   at91_usbdp_ep0->fill_buffer_fn = 0;
   at91_usbdp_ep0->complete_fn = 0;

   *ppbegin = at91_usbdp_ep0->buffer;
   *ppend = *ppbegin;

   if (status == EP0_LL_IDLE) {
      if (at91_usbdp_ep0->complete_fn) {
         (*at91_usbdp_ep0->complete_fn)(at91_usbdp_ep0,
                                        USBS_CONTROL_RETURN_STALL);
      }
   }

   status = EP0_LL_IDLE;
   CLEAR_BITS ((p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_ISOERROR | AT91C_UDP_FORCESTALL);

   return status;
}

/*-------------------------------------------
| Name:        _at91_usbdp_control_setup_get_status
| Description:
| Parameters:
| Return Type:
| Comments:    Handle a get status setup message on the control end point
| See:
---------------------------------------------*/
ep0_low_level_status_t _at91_usbdp_control_setup_get_status(board_inf_usb_t *p_inf_usb) {
   ep0_low_level_status_t status;
   usbs_control_endpoint * at91_usbdp_ep0 = (usbs_control_endpoint *)p_inf_usb->ep_tbl[0];
   usb_devreq *req = (usb_devreq *)at91_usbdp_ep0->control_buffer;
   unsigned char recipient = req->type & USB_DEVREQ_RECIPIENT_MASK;
   unsigned char **ppbegin = &p_inf_usb->ep_buffer_pbegin[0];
   unsigned char **ppend = &p_inf_usb->ep_buffer_pend[0];
   unsigned short word = 0;

   status = EP0_LL_SEND_READY;

   switch (recipient) {
   case USB_DEVREQ_RECIPIENT_DEVICE:
   case USB_DEVREQ_RECIPIENT_INTERFACE:
      // Nothing to do
      break;

   case USB_DEVREQ_RECIPIENT_ENDPOINT:
      if ((at91_usbdp_ep0->state == USBS_STATE_CONFIGURED) &&
          (req->index_lo > 0) &&
          (req->index_lo < p_inf_usb->ep_no)) {
         unsigned int CSR;

         USBDP_READ_UINT32(pCSRn((p_inf_usb->base_addr + UDP_CSRx), req->index_lo), CSR);

         if (CSR & AT91C_UDP_EPEDS) {
            word = 1;
         }
      }
      else {
         status = EP0_LL_STALL;
      }
      break;

   default:
      status = EP0_LL_STALL;
   }

   *ppbegin = (unsigned char *)&word;
   *ppend = *ppbegin + sizeof (word);

   return status;
}

/*-------------------------------------------
| Name:        _at91_usbdp_control_setup_send_ack
| Description:
| Parameters:
| Return Type:
| Comments:    Setup the begin and end pointers such that an ACK is sent
| See:
---------------------------------------------*/
void _at91_usbdp_control_setup_send_ack(board_inf_usb_t *p_inf_usb) {
   unsigned char **ppbegin = &p_inf_usb->ep_buffer_pbegin[0];
   unsigned char **ppend = &p_inf_usb->ep_buffer_pend[0];
   usbs_control_endpoint * at91_usbdp_ep0 = (usbs_control_endpoint *)p_inf_usb->ep_tbl[0];

   *ppbegin = at91_usbdp_ep0->buffer;
   *ppend = *ppbegin;
}

/*-------------------------------------------
| Name:        _at91_usbdp_control_setup_set_feature
| Description:
| Parameters:
| Return Type:
| Comments:    Handle a get status set feature message on the control endpoint
| See:
---------------------------------------------*/
ep0_low_level_status_t _at91_usbdp_control_setup_set_feature(board_inf_usb_t *p_inf_usb) {
   ep0_low_level_status_t status;
   usbs_control_endpoint * at91_usbdp_ep0 = (usbs_control_endpoint *)p_inf_usb->ep_tbl[0];
   usb_devreq *req = (usb_devreq *)at91_usbdp_ep0->control_buffer;
   unsigned char recipient = req->type & USB_DEVREQ_RECIPIENT_MASK;

   _at91_usbdp_control_setup_send_ack(p_inf_usb);
   status = EP0_LL_SEND_READY;

   switch(recipient) {
   case USB_DEVREQ_RECIPIENT_DEVICE:
      status = EP0_LL_STALL;
      break;
   case USB_DEVREQ_RECIPIENT_INTERFACE:
      // Nothing to do
      break;
   case USB_DEVREQ_RECIPIENT_ENDPOINT:
      if ((at91_usbdp_ep0->state == USBS_STATE_CONFIGURED) &&
          (req->index_lo > 0) &&
          (req->index_lo < p_inf_usb->ep_no)) {
         unsigned int CSR;

         USBDP_READ_UINT32(pCSRn((p_inf_usb->base_addr + UDP_CSRx), req->index_lo), CSR);
         if (CSR & AT91C_UDP_EPEDS) {
            /* TODO */
         }
      }
      else {
         status = EP0_LL_STALL;
      }
   default:
      status = EP0_LL_STALL;
   }
   return status;
}

/*-------------------------------------------
| Name:        _at91_usbdp_control_setup_clear_feature
| Description:
| Parameters:
| Return Type:
| Comments:    Handle a get status clear feature message on the control endpoint
| See:
---------------------------------------------*/
ep0_low_level_status_t _at91_usbdp_control_setup_clear_feature(board_inf_usb_t *p_inf_usb) {
   ep0_low_level_status_t status;
   usbs_control_endpoint * at91_usbdp_ep0 = (usbs_control_endpoint *)p_inf_usb->ep_tbl[0];
   usb_devreq *req = (usb_devreq *)at91_usbdp_ep0->control_buffer;
   unsigned char recipient = req->type & USB_DEVREQ_RECIPIENT_MASK;

   _at91_usbdp_control_setup_send_ack(p_inf_usb);
   status = EP0_LL_SEND_READY;

   switch (recipient) {
   case USB_DEVREQ_RECIPIENT_DEVICE:
      status = EP0_LL_STALL;
      break;

   case USB_DEVREQ_RECIPIENT_INTERFACE:
      // Nothing to do
      break;

   case USB_DEVREQ_RECIPIENT_ENDPOINT: {
      int epn = req->index_lo&~USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN;
      int ret=0;
      //
      if(at91_usbdp_ep0->class_clear_feature_fn) {
         ret = (*at91_usbdp_ep0->class_clear_feature_fn)(at91_usbdp_ep0, NULL);
      }
      //
      if(ret) {
         //reset endpoint rx ptr
      }
      else if((at91_usbdp_ep0->state == USBS_STATE_CONFIGURED) &&
              (epn > 0) &&
              (epn < p_inf_usb->ep_no)) {
         unsigned int pCSR = (unsigned int)pCSRn((p_inf_usb->base_addr + UDP_CSRx),epn);
         usbs_data_endpoint *pep = (usbs_data_endpoint *)p_inf_usb->ep_tbl[epn];

         //Clear FORCESTALL flag
         CLEAR_BITS (pCSR, AT91C_UDP_FORCESTALL);
         pep->halted = 0;

         //Reset Endpoint Fifos
         USBDP_WRITE_UINT32 ((p_inf_usb->base_addr + UDP_RST_EP), 1 << epn);
         USBDP_WRITE_UINT32 ((p_inf_usb->base_addr + UDP_RST_EP), 0);
      }
      else {
         status = EP0_LL_STALL;
      }
   }
   break;
   default:
      status = EP0_LL_STALL;
   }
   return status;
}

/*-------------------------------------------
| Name:        _at91_usbdp_control_setup
| Description:
| Parameters:
| Return Type:
| Comments:    Handle a setup message from the host
| See:
---------------------------------------------*/
ep0_low_level_status_t _at91_usbdp_control_setup(board_inf_usb_t *p_inf_usb,
                                                 ep0_low_level_status_t status) {
   unsigned char **ppbegin = &p_inf_usb->ep_buffer_pbegin[0];
   unsigned char **ppend = &p_inf_usb->ep_buffer_pend[0];
   usbs_control_endpoint * at91_usbdp_ep0 = (usbs_control_endpoint *)p_inf_usb->ep_tbl[0];
   usb_devreq *req = (usb_devreq *) at91_usbdp_ep0->control_buffer;
   unsigned char protocol;
   unsigned length;
   unsigned char dev_to_host;
   usbs_control_return usbcode;
   unsigned char handled = USB_FALSE;

   at91_usbdp_ep0->buffer_size = 0;
   at91_usbdp_ep0->fill_buffer_fn = 0;
   at91_usbdp_ep0->complete_fn = 0;

   _at91_usbdp_read_fifo_uint8 ((unsigned char *)req,
                                (unsigned int)(p_inf_usb->base_addr + UDP_FDRx), sizeof (usb_devreq));

   length = (req->length_hi << 8) | req->length_lo;
   dev_to_host = req->type & USB_DEVREQ_DIRECTION_IN;

   CLEAR_BITS((unsigned int)(p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_DTGLE);

   status = EP0_LL_REQUEST;

   protocol = req->type & (USB_DEVREQ_TYPE_MASK);

   // Set the next transfer direction
   if (dev_to_host) {
      //Set IN direction
      SET_BITS ((unsigned int)(p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_DIR);
   }
   else {
      //Set OUT direction
      CLEAR_BITS ((unsigned int)(p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_DIR);
   }

   if (protocol == USB_DEVREQ_TYPE_STANDARD) {
      handled = USB_TRUE;
      switch (req->request) {
      case USB_DEVREQ_GET_STATUS:
         status = _at91_usbdp_control_setup_get_status(p_inf_usb);
         break;

      case USB_DEVREQ_SET_ADDRESS:
         // Most of the hard work is done by the hardware. We just need
         // to send an ACK.
         _at91_usbdp_control_setup_send_ack(p_inf_usb);
         status = EP0_LL_SEND_READY;
         break;

      case USB_DEVREQ_SET_FEATURE:
         status = _at91_usbdp_control_setup_set_feature(p_inf_usb);
         break;

      case USB_DEVREQ_CLEAR_FEATURE:
         status =  _at91_usbdp_control_setup_clear_feature(p_inf_usb);
         break;

      default:
         handled = USB_FALSE;
         break;
      }
   }

   if ((protocol != USB_DEVREQ_TYPE_STANDARD) || !handled) {
      // Ask the layer above to process the message
      usbcode = _at91_usbdp_parse_host_get_command(at91_usbdp_ep0);
      at91_usbdp_ep0->buffer_size = MIN (at91_usbdp_ep0->buffer_size, length);

      *ppbegin = at91_usbdp_ep0->buffer;
      *ppend = *ppbegin + at91_usbdp_ep0->buffer_size; /* Ready to send... */

      if (usbcode == USBS_CONTROL_RETURN_HANDLED) { /* OK */
         if (dev_to_host) {
            status = EP0_LL_SEND_READY;
         }
         else {
            status = EP0_LL_RECEIVE_READY;
         }
      }
      else {
         status = EP0_LL_STALL;
      }
   }
   // Clear the setup bit so indicating we have processed the message
   CLEAR_BITS ((unsigned int)(p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_RXSETUP);

   return status;
}

/*-------------------------------------------
| Name:        _at91_usbdp_control_data_recv
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
ep0_low_level_status_t _at91_usbdp_control_data_recv(board_inf_usb_t *p_inf_usb,
                                                     ep0_low_level_status_t status) {
   unsigned int received = 0;
   unsigned int length;
   unsigned char **ppbegin = &p_inf_usb->ep_buffer_pbegin[0];
   unsigned char **ppend = &p_inf_usb->ep_buffer_pend[0];
   usbs_control_endpoint * at91_usbdp_ep0 = (usbs_control_endpoint *)p_inf_usb->ep_tbl[0];
   usbs_control_return usbcode;

   if (status == EP0_LL_RECEIVE_READY) {
      received = ((*(unsigned int *) (p_inf_usb->base_addr + UDP_CSRx)) >> 16) & 0x7ff;
      length = MIN (received, (unsigned int) *ppend - (unsigned int) *ppbegin);
      *ppbegin =
         _at91_usbdp_read_fifo_uint8(
            *ppbegin, (unsigned int)(p_inf_usb->base_addr + UDP_FDRx) /*(unsigned int)pFDR0*/,
            length);

      //last packet
      if (received < p_inf_usb->ep_fifo_size[0]) {
         *ppend = *ppbegin;
      }

      //all received ?
      if (*ppbegin == *ppend) {
         at91_usbdp_ep0->buffer_size =
            (unsigned int) *ppend - (unsigned int) at91_usbdp_ep0->buffer;
         usbcode = USBS_CONTROL_RETURN_STALL;

         if (at91_usbdp_ep0->complete_fn) {
            usbcode = (*at91_usbdp_ep0->complete_fn)(at91_usbdp_ep0, 0);
         }

         if (usbcode == USBS_CONTROL_RETURN_HANDLED) {
            status = EP0_LL_SEND_READY;
         } else {
            status = EP0_LL_STALL;
         }
      }
   }

   CLEAR_BITS ((unsigned int)(p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_RX_DATA_BK0);

   return status;
}

/*-------------------------------------------
| Name:        _at91_usbdp_control_data_sent
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
ep0_low_level_status_t _at91_usbdp_control_data_sent(board_inf_usb_t *p_inf_usb,
                                                     ep0_low_level_status_t status) {
   unsigned char **ppbegin = &p_inf_usb->ep_buffer_pbegin[0];
   unsigned char **ppend = &p_inf_usb->ep_buffer_pend[0];
   unsigned int bytes_to_write = 0;

   usbs_control_endpoint * at91_usbdp_ep0 = (usbs_control_endpoint *)p_inf_usb->ep_tbl[0];
   usb_devreq *req = (usb_devreq *)at91_usbdp_ep0->control_buffer;
   unsigned short value;

   switch (status) {
   //case EP0_LL_RECEIVE_READY:
   case EP0_LL_SEND_READY:
      if (*ppbegin == *ppend &&
          at91_usbdp_ep0->fill_buffer_fn == NULL) {
         // All bytes are sent, send ACK
         status = EP0_LL_ACK;
         SET_BITS ((p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_TXPKTRDY);
      }
      else {
         // We have more bytes to send
         bytes_to_write = MIN (*ppend - *ppbegin, p_inf_usb->ep_fifo_size[0]);
         *ppbegin = _at91_usbdp_write_fifo_uint8 (
            (unsigned int)(p_inf_usb->base_addr + UDP_FDRx), *ppbegin, (unsigned char *)
            ((unsigned int) *ppbegin + bytes_to_write));

         // Send next few bytes
         if (*ppbegin == *ppend) {    /* Control-Endoints don't need ACK's */
            if (at91_usbdp_ep0->fill_buffer_fn) {    // More Records ?
               (*at91_usbdp_ep0->fill_buffer_fn)(at91_usbdp_ep0);

               *ppbegin = at91_usbdp_ep0->buffer;
               *ppend = *ppbegin + at91_usbdp_ep0->buffer_size;

               /* Ready to send... */
               bytes_to_write =
                  MIN (*ppend - *ppbegin,
                       p_inf_usb->ep_fifo_size[0] - bytes_to_write);

               *ppbegin =
                  _at91_usbdp_write_fifo_uint8 (
                     (unsigned int)(p_inf_usb->base_addr + UDP_FDRx), *ppbegin, (unsigned char *)
                     ((unsigned int) *ppbegin + bytes_to_write));
            }
            else {
               if (bytes_to_write == p_inf_usb->ep_fifo_size[0]) {
                  // Last packet is full, so we need to send a zero bytes
                  // packet next time
                  status = EP0_LL_SEND_READY;
               }
               else {
                  status = EP0_LL_IDLE;
               }

            }
         }
         SET_BITS((p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_TXPKTRDY);         // Signal FIFO loaded
      }
      break;

   case EP0_LL_RECEIVE_READY:
      // All bytes are received, send ACK
      if (*ppbegin == *ppend) {
         status = EP0_LL_ACK;
         // Signal FIFO loaded
         SET_BITS((p_inf_usb->base_addr + UDP_CSRx), AT91C_UDP_TXPKTRDY);
      }
      break;

   case EP0_LL_ACK:
      // Special-processing
      if (req->request == USB_DEVREQ_SET_ADDRESS) {
         USBDP_WRITE_UINT32 ((p_inf_usb->base_addr + UDP_FADDR),
                             req->value_lo | AT91C_UDP_FEN);
         value = (req->value_hi << 8) | req->value_lo;
         if (value) {
            at91_usbdp_ep0->state = USBS_STATE_ADDRESSED;
         }
      }

      if (at91_usbdp_ep0->complete_fn) {
         (*at91_usbdp_ep0->complete_fn)(at91_usbdp_ep0,
                                        USBS_CONTROL_RETURN_HANDLED);
      }

      status = EP0_LL_IDLE;
      _at91_usbdp_state_notify(p_inf_usb, at91_usbdp_ep0);
      break;

   default:
      break;
   }
   return status;
}


/*============================================
| End of Source  : dev_at91_usbdp_lowlevel.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
