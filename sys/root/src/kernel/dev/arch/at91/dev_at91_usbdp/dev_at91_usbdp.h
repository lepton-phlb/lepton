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
| Compiler Directive
==============================================*/
#ifndef _DEV_AT91_USB_X_H
#define _DEV_AT91_USB_X_H

/*============================================
| Includes
==============================================*/
#include "kernel/dev/arch/all/usb/usbslave.h"
#include "kernel/fs/vfs/vfsdev.h"

/*============================================
| Declaration
==============================================*/

//
typedef enum ep0_low_level_status_t {
   EP0_LL_IDLE = 0,
   EP0_LL_REQUEST,
   EP0_LL_SEND_READY,
   EP0_LL_ACK,
   EP0_LL_RECEIVE_READY,
   EP0_LL_ISOERROR,
   EP0_LL_STALL,
   EP0_LL_SET_ADDRESS,
} ep0_low_level_status_t;


//global struct for USB
typedef struct board_inf_usb_st {
   volatile unsigned int * base_addr;
   unsigned char irq_no;
   unsigned char irq_prio;

   const unsigned short *  ep_fifo_size;
   const unsigned char  *  ep_pingpong_attr;

   unsigned char        ** ep_input_pbegin;
   unsigned char        ** ep_input_pend;

   unsigned char        ** ep_buffer_pbegin;
   unsigned char        ** ep_buffer_pend;

   unsigned char        *  rcv_buffer;
   unsigned int rcv_buffer_size;

   unsigned char        *  ep_bank1_attr;

   unsigned short       *  ep_bytes_in_fifo;
   unsigned short       *  ep_bytes_received;

   const void           ** ep_tbl;
   unsigned int ep_no;

   unsigned char        ** class_ep_tbl;

   unsigned char opened;
}board_inf_usb_t;

//
int _at91_usbdp_pep_to_number(board_inf_usb_t *p_inf_usb,const usbs_data_endpoint * pep);

void _at91_usbdp_reset_device(board_inf_usb_t *p_inf_usb);
void _at91_usbdp_end_all_transfers(board_inf_usb_t *p_inf_usb, usbs_control_return returncode);
void _at91_usbdp_ep_interrupt_enable (board_inf_usb_t *p_inf_usb, unsigned char epn,
                                      unsigned char enable);

void _at91_usbdp_ep_set_halted(board_inf_usb_t *p_inf_usb, int epn, unsigned char new_value);

void _at91_usbdp_ep_init(board_inf_usb_t *p_inf_usb, usbs_data_endpoint * pep,
                         unsigned char endpoint_type,
                         bool_t enable);

void _at91_usbdp_set_pullup(unsigned char set);
unsigned char _at91_usbdp_read_power(void);
void _at91_usbdp_ep0_start(board_inf_usb_t *p_inf_usb, usbs_control_endpoint * endpoint);
unsigned char * _at91_usbdp_read_fifo_uint8(unsigned char  * pdest, unsigned int psource,
                                            unsigned int size);
unsigned char * _at91_usbdp_write_fifo_uint8(unsigned int pdest, unsigned char * psource,
                                             unsigned char * psource_end);

void _at91_usbdp_handle_reset(board_inf_usb_t *p_inf_usb);
//
unsigned char _at91_usbdp_ep_isr (board_inf_usb_t *p_inf_usb, unsigned char epn);
void _at91_usbdp_ep_dsr (board_inf_usb_t *p_inf_usb, unsigned char epn);
void _at91_usbdp_control_dsr(board_inf_usb_t *p_inf_usb);
void _at91_usbdp_state_notify(board_inf_usb_t *p_inf_usb, usbs_control_endpoint * pcep);

#endif //#define _DEV_AT91_USB_X_H
