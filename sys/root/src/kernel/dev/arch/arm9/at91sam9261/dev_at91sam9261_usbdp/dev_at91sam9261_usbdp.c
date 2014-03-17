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
#include "kernel/dev/arch/at91/dev_at91_usbdp/dev_at91_common_usbdp.h"
#include "kernel/dev/arch/at91/dev_at91_usbdp/dev_at91_usbdp.h"

#include "kernel/dev/arch/all/usb/usbcore.h"
#include "kernel/dev/arch/all/usb/usbslave.h"

#include "kernel/core/ioctl_usb.h"
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/system.h"

#include "kernel/fs/vfs/vfsdev.h"

#if defined(USE_ECOS)
   #include "cyg/hal/at91sam9261.h"
#endif
/*============================================
| Global Declaration
==============================================*/
//
extern int dev_at91_usbdp_load(board_inf_usb_t * p_inf_usb);
extern int dev_at91_usbdp_open(desc_t desc, int o_flag);
extern int dev_at91_usbdp_close(desc_t desc);
extern int dev_at91_usbdp_isset_read(desc_t desc);
extern int dev_at91_usbdp_isset_write(desc_t desc);
extern int dev_at91_usbdp_read(desc_t desc, char* buf,int cb);
extern int dev_at91_usbdp_write(desc_t desc, const char* buf,int cb);
extern int dev_at91_usbdp_seek(desc_t desc,int offset,int origin);
extern int dev_at91_usbdp_ioctl(desc_t desc,int request,va_list ap);

//
static const char dev_at91sam9261_usbdp_name[]="usbdp0\0";
static int dev_at91sam9261_usbdp_load(void);

//
dev_map_t dev_at91sam9261_usbdp_map={
   dev_at91sam9261_usbdp_name,
   S_IFCHR,
   dev_at91sam9261_usbdp_load,
   dev_at91_usbdp_open,
   dev_at91_usbdp_close,
   dev_at91_usbdp_isset_read,
   dev_at91_usbdp_isset_write,
   dev_at91_usbdp_read,
   dev_at91_usbdp_write,
   dev_at91_usbdp_seek,
   dev_at91_usbdp_ioctl
};


//
#define  AT91SAM9261_RCV_BUFFER        (128*1024) //(96*1024)//(64*1024)//(4*1024)//
#define  AT91SAM9261_ENDPOINT_COUNT    6

#define  CYGNUM_HAL_INT_USBDP          10 //(see cyg/hal/hal_platform_ints.h)
#define  CYGNUM_HAL_INT_USBDP_PRIOR    6

//we explicitely use endpoint 1 for read (endpoint 4 can have its buffer)
static unsigned char _at91sam9261_endpoint_rcv_buffer[AT91SAM9261_RCV_BUFFER] __attribute__ ((
                                                                                                section (
                                                                                                   ".no_cache")))
   ={0};

//size of fifos endpoint
static const unsigned short _at91sam9261_endpoint_fifo_size[AT91SAM9261_ENDPOINT_COUNT] = {
   8,
   64,
   64,
   64,
   256,
   256
};

//is double buffering supported by associated endpoints
static const unsigned char _at91sam9261_endpoint_pingpong[AT91SAM9261_ENDPOINT_COUNT] = {
   USB_FALSE,
   USB_TRUE,
   USB_TRUE,
   USB_FALSE,
   USB_TRUE,
   USB_TRUE
};

//data pointers per each endpoints (read)
static unsigned char *_at91sam9261_ep_input_pbegin[AT91SAM9261_ENDPOINT_COUNT] __attribute__ ((
                                                                                                 section (
                                                                                                    ".no_cache")))
   = {
   0,
   _at91sam9261_endpoint_rcv_buffer,
   0,
   0,
   0,
   0
   };
static unsigned char *_at91sam9261_ep_input_pend[AT91SAM9261_ENDPOINT_COUNT] __attribute__ ((
                                                                                               section (
                                                                                                  ".no_cache")))
   =  {
   0,
   _at91sam9261_endpoint_rcv_buffer,
   0,
   0,
   0,
   0
   };

//data pointers per each endpoints (read)
static unsigned char *_at91sam9261_ep_buffer_pbegin[AT91SAM9261_ENDPOINT_COUNT] __attribute__ ((
                                                                                                  section (
                                                                                                     ".no_cache")))
   = {
   0,
   _at91sam9261_endpoint_rcv_buffer,
   0,
   0,
   0,
   0
   };
static unsigned char *_at91sam9261_ep_buffer_pend[AT91SAM9261_ENDPOINT_COUNT] __attribute__ ((
                                                                                                section (
                                                                                                   ".no_cache")))
   =  {
   0,
   _at91sam9261_endpoint_rcv_buffer,
   0,
   0,
   0,
   0
   };


//which bank are using for current data
static unsigned char _at91sam9261_ep_bank1[AT91SAM9261_ENDPOINT_COUNT] __attribute__ ((section (
                                                                                          ".no_cache")))
   = {
   USB_FALSE,
   USB_FALSE,
   USB_FALSE,
   USB_FALSE,
   USB_FALSE,
   USB_FALSE
   };

//how many bytes are used per each endpoints
static unsigned short _at91sam9261_ep_bytes_in_fifo[AT91SAM9261_ENDPOINT_COUNT] __attribute__ ((
                                                                                                  section (
                                                                                                     ".no_cache")))
   = {
   0,
   0,
   0,
   0,
   0,
   0
   };

//
static unsigned short _at91sam9261_ep_bytes_received[AT91SAM9261_ENDPOINT_COUNT] __attribute__ ((
                                                                                                   section (
                                                                                                      ".no_cache")))
   = {
   THERE_IS_A_NEW_PACKET_IN_THE_UDP,
   THERE_IS_A_NEW_PACKET_IN_THE_UDP,
   THERE_IS_A_NEW_PACKET_IN_THE_UDP,
   THERE_IS_A_NEW_PACKET_IN_THE_UDP,
   THERE_IS_A_NEW_PACKET_IN_THE_UDP,
   THERE_IS_A_NEW_PACKET_IN_THE_UDP
   };

// Endpoint 0, the control endpoint, structure.
static usbs_control_endpoint _at91sam9261_ep0 __attribute__ ((section (".no_cache"))) = {
   // The hardware does not distinguish  between detached, attached and powered.
   state:                  USBS_STATE_POWERED,
   enumeration_data:       (usbs_enumeration_data *) 0,
   control_buffer:         {0, 0, 0, 0, 0, 0, 0, 0},
   state_change_fn:        (void (*)(usbs_control_endpoint *,
                                     void *, usbs_state_change, int)) 0,
   state_change_data:      (void *) 0,
   standard_control_fn:    (usbs_control_return (*)
                               (usbs_control_endpoint *, void *)) 0,
   standard_control_data:  (void *) 0,
   class_control_fn:       (usbs_control_return (*)
                               (usbs_control_endpoint *, void *)) 0,
   class_control_data:     (void *) 0,
   vendor_control_fn:      (usbs_control_return (*)
                               (usbs_control_endpoint *, void *)) 0,
   vendor_control_data:    (void *) 0,
   reserved_control_fn:    (usbs_control_return (*)
                               (usbs_control_endpoint *, void *)) 0,
   reserved_control_data:  (void *) 0,
   class_clear_feature_fn: (usbs_control_return (*)
                               (usbs_control_endpoint *, void *)) 0,
   buffer:                 (unsigned char *) 0,
   buffer_size:            0,
   fill_buffer_fn:         (void (*)(usbs_control_endpoint *)) 0,
   fill_data:              (void *) 0,
   fill_index:             0,
   complete_fn:            (usbs_control_return (*)(usbs_control_endpoint *,
                                                    int)) 0
};

//general endpoint halt function
//void usbs_at91_endpoint_set_halted (int epn, bool_t new_value);

// Endpoint 1
static usbs_data_endpoint _at91sam9261_ep1 __attribute__ ((section (".no_cache"))) = {0};
// Endpoint 2
static usbs_data_endpoint _at91sam9261_ep2 __attribute__ ((section (".no_cache"))) = {0};
// Endpoint 3
static usbs_data_endpoint _at91sam9261_ep3 __attribute__ ((section (".no_cache"))) = {0};
// Endpoint 4
static usbs_data_endpoint _at91sam9261_ep4 __attribute__ ((section (".no_cache"))) = {0};
// Endpoint 5
static usbs_data_endpoint _at91sam9261_ep5 __attribute__ ((section (".no_cache"))) = {0};

//table for representing other endpoint
// Array of end points. Used for translating end point pointer to an
// end point number
static const void *_at91sam9261_endpoints[AT91SAM9261_ENDPOINT_COUNT] __attribute__ ((section (
                                                                                         ".no_cache")))
   = {
   (void *) &_at91sam9261_ep0,
   (void *) &_at91sam9261_ep1,
   (void *) &_at91sam9261_ep2,
   (void *) &_at91sam9261_ep3,
   (void *) &_at91sam9261_ep4,
   (void *) &_at91sam9261_ep5
   };


static board_inf_usb_t g_inf_board_usb __attribute__ ((section (".no_cache"))) = {
   base_addr : (volatile unsigned int *)0xfffa4000,
   irq_no : CYGNUM_HAL_INT_USBDP,
   irq_prio : CYGNUM_HAL_INT_USBDP_PRIOR,
   ep_fifo_size : _at91sam9261_endpoint_fifo_size,
   ep_pingpong_attr : _at91sam9261_endpoint_pingpong,
   ep_input_pbegin : _at91sam9261_ep_input_pbegin,
   ep_input_pend : _at91sam9261_ep_input_pend,
   ep_buffer_pbegin : _at91sam9261_ep_buffer_pbegin,
   ep_buffer_pend : _at91sam9261_ep_buffer_pend,
   rcv_buffer : _at91sam9261_endpoint_rcv_buffer,
   rcv_buffer_size : AT91SAM9261_RCV_BUFFER,
   ep_bank1_attr : _at91sam9261_ep_bank1,
   ep_bytes_in_fifo : _at91sam9261_ep_bytes_in_fifo,
   ep_bytes_received : _at91sam9261_ep_bytes_received,
   ep_tbl : _at91sam9261_endpoints,
   ep_no : AT91SAM9261_ENDPOINT_COUNT,
   class_ep_tbl : NULL,
   opened : 0
};
/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name       : dev_at91sam9261_usbdp_load
| Description:
| Parameters :
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam9261_usbdp_load(void) {
   return dev_at91_usbdp_load(&g_inf_board_usb);
}
/*============================================
| End of Source  : dev_at91sam9261_usbdp.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
