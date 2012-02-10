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
#include "kernel/core/ioctl.h"
#include "kernel/core/ioctl_usb.h"

#include "kernel/core/core_rttimer.h"
#include "lib/libc/termios/termios.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "usbserial.h"
#include "kernel/dev/arch/all/usb/usbcore.h"
#include "kernel/dev/arch/all/usb/usbslave.h"

#include <string.h>
#include <stdlib.h>

#ifdef USB_CLASS_ACM_CDC
   #define  USBS_CDI_ENDPOINT_OFFSET   (USB_INTERFACE_DESCRIPTOR_LENGTH+ \
                                        USBS_CDC_HEADER_SIZE+ \
                                        USBS_CDC_CALL_MANAGEMENT_DESCRIPTOR_SIZE+ \
                                        USBS_CDC_ABSTRACT_CTRL_MANAGEMENT_DESCRIPTOR_SIZE+ \
                                        USBS_CDC_UNION_DESCRIPTOR)
#else
   #define  USBS_CDI_ENDPOINT_OFFSET   (USB_INTERFACE_DESCRIPTOR_LENGTH)
#endif

/*============================================
| Global Declaration
==============================================*/
static const char dev_usbserial_name[]="usbttys\0";

static int dev_usbserial_load(void);
static int dev_usbserial_isset_read(desc_t desc);
static int dev_usbserial_isset_write(desc_t desc);
static int dev_usbserial_open(desc_t desc, int o_flag);
static int dev_usbserial_close(desc_t desc);
static int dev_usbserial_seek(desc_t desc,int offset,int origin);
static int dev_usbserial_read(desc_t desc, char* buf,int cb);
static int dev_usbserial_write(desc_t desc, const char* buf,int cb);
static int dev_usbserial_ioctl(desc_t desc,int request,va_list ap);

//need to define again for new underlying micro-kernel
static void  dev_usbserial_timer_callback(alrm_hdl_t alarm_handle, cyg_addrword_t data);
static int termios2ttys(struct termios* termios_p);
//
dev_map_t dev_usbserial_map={
   dev_usbserial_name,
   S_IFCHR,
   dev_usbserial_load,
   dev_usbserial_open,
   dev_usbserial_close,
   dev_usbserial_isset_read,
   dev_usbserial_isset_write,
   dev_usbserial_read,
   dev_usbserial_write,
   dev_usbserial_seek,
   dev_usbserial_ioctl
};

//
static const usb_configuration_descriptor usb_configuration_serial = {
   length:             USB_CONFIGURATION_DESCRIPTOR_LENGTH,
   type:               USB_CONFIGURATION_DESCRIPTOR_TYPE,
   total_length_lo :   USB_SERIAL_CONFIGURATION_DESCRIPTOR_SIZE,
   total_length_hi :   0,
   number_interfaces:  USB_SERIAL_INTERFACE_NUMBER,
   configuration_id:   1,
   configuration_str:  0,
   attributes:         (USB_CONFIGURATION_DESCRIPTOR_ATTR_REQUIRED |
                        USB_CONFIGURATION_DESCRIPTOR_ATTR_SELF_POWERED),
   max_power:          0 //50
};

#ifdef USB_CLASS_ACM
static const usb_interface_descriptor usb_interface_Communication_Class_Interface[] = {
   USB_INTERFACE_DESCRIPTOR(
      USB_INTERFACE_DESCRIPTOR_LENGTH,
      USB_INTERFACE_DESCRIPTOR_TYPE,
      0,
      0,
      1,
      USB_INTERFACE_DESCRIPTOR_COMM_CLASS,
      USB_INTERFACE_DESCRIPTOR_COMM_SUBCLASS,
      USB_INTERFACE_DESCRIPTOR_COMM_PROTOCOL,      //just test it//0,
      0x00
      ),

   #ifdef USB_CLASS_ACM_CDC
   CDC_HEADER_DESCRIPTOR (
      USBS_CDC_HEADER_SIZE,
      USBS_CDC_GENERIC_INTERFACE,
      USBS_CDC_GENERIC_HEADER,
      USBS_CDC_GENERIC_BCD_VERSION_LO,
      USBS_CDC_GENERIC_BCD_VERSION_HI
      ),
   CDC_ABSTRACT_CONTROL_MANAGEMENT_DESCRIPTOR(
      USBS_CDC_ABSTRACT_CTRL_MANAGEMENT_DESCRIPTOR_SIZE,
      USBS_CDC_GENERIC_INTERFACE,
      USBS_CDC_GENERIC_ACM,
      USBS_CDC_GENERIC_CRTL_MANAGEMENT      //just test it//0
      ),
   CDC_UNION_DESCRIPTOR(
      USBS_CDC_UNION_DESCRIPTOR,
      USBS_CDC_GENERIC_INTERFACE,
      USBS_CDC_GENERIC_UNION,
      0,
      1
      ),
   CDC_CALL_MANAGEMENT_DESCRIPTOR(
      USBS_CDC_CALL_MANAGEMENT_DESCRIPTOR_SIZE,
      USBS_CDC_GENERIC_INTERFACE,
      USBS_CDC_GENERIC_CALLMANAGEMENT,
      USBS_CDC_GENERIC_SELFMANAGEMENT,
      1      //0
      ),
   #endif //USB_CLASS_ACM_CDC
   USB_ENDPOINT_DESCRIPTOR(
      USB_ENDPOINT_DESCRIPTOR_LENGTH,
      USB_ENDPOINT_DESCRIPTOR_TYPE,
      USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN | USB_ENDPOINT_DESCRIPTOR_CDC_NO,
      USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT,
      USB_ENDPOINT_DESCRIPTOR_CDC_FIFO_LENGHT_LO,
      USB_ENDPOINT_DESCRIPTOR_CDC_FIFO_LENGHT_HI,
      10      //255
      )
};
#endif //USB_CLASS_ACM

static const usb_interface_descriptor usb_interface_Data_Class_Interface[] = {
   USB_INTERFACE_DESCRIPTOR(
      USB_INTERFACE_DESCRIPTOR_LENGTH,
      USB_INTERFACE_DESCRIPTOR_TYPE,
      1,
      0,
      2,
      USB_INTERFACE_DESCRIPTOR_DATA_CLASS,
      USB_INTERFACE_DESCRIPTOR_DATA_SUBCLASS,
      USB_INTERFACE_DESCRIPTOR_DATA_PROTOCOL,
      0x00
      ),
   // Tx (Bulk IN) Endpoint Descriptor (data class)
   USB_ENDPOINT_DESCRIPTOR(
      USB_ENDPOINT_DESCRIPTOR_LENGTH,
      USB_ENDPOINT_DESCRIPTOR_TYPE,
      USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN | USB_ENDPOINT_DESCRIPTOR_DC_TX_NO,
      USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
      USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_LO,
      USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_HI,
      0
      ),
   // Rx (Bulk OUT) Endpoint Descriptor (data class)
   USB_ENDPOINT_DESCRIPTOR(
      USB_ENDPOINT_DESCRIPTOR_LENGTH,
      USB_ENDPOINT_DESCRIPTOR_TYPE,
      USB_ENDPOINT_DESCRIPTOR_ENDPOINT_OUT | USB_ENDPOINT_DESCRIPTOR_DC_RX_NO,
      USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
      USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_LO,
      USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_HI,
      0
      )
};

static const usb_interface_descriptor * usb_interface_serial [] ={
#ifdef USB_CLASS_ACM
   usb_interface_Communication_Class_Interface,
#endif
   usb_interface_Data_Class_Interface
};

//from usbstring.c
extern const unsigned char g_usb_language_id_str[];
extern const unsigned char g_usb_manufacturer_id_str[];
extern const unsigned char g_usb_product_id_str[];
extern const unsigned char g_usb_serial_number_str[];

//
static const unsigned char* usb_string_serial[] = {
   g_usb_language_id_str,
   g_usb_manufacturer_id_str,
   g_usb_product_id_str,
   g_usb_serial_number_str
};

//
static usbs_enumeration_data usbs_enumeration_serial = {
   {
      length:                 USB_DEVICE_DESCRIPTOR_LENGTH,
      type:                   USB_DEVICE_DESCRIPTOR_TYPE,
      usb_spec_lo:            USB_DEVICE_DESCRIPTOR_USB20_LO,  //USB_DEVICE_DESCRIPTOR_USB11_LO,
      usb_spec_hi:            USB_DEVICE_DESCRIPTOR_USB20_HI,  //USB_DEVICE_DESCRIPTOR_USB11_HI,
      device_class:           USB_DEVICE_DESCRIPTOR_SERIAL_CLASS_VENDOR,
      device_subclass:        USB_DEVICE_DESCRIPTOR_SERIAL_SUBCLASS_VENDOR,
      device_protocol:        USB_DEVICE_DESCRIPTOR_SERIAL_PROTOCOL_VENDOR,
      max_packet_size:        8,
      vendor_lo:              USB_DEVICE_DESCRIPTOR_VENDOR_LO,
      vendor_hi:              USB_DEVICE_DESCRIPTOR_VENDOR_HI,
      product_lo:             USB_DEVICE_DESCRIPTOR_PRODUCT_LO,
      product_hi:             USB_DEVICE_DESCRIPTOR_PRODUCT_HI,
      device_lo:              USB_DEVICE_DESCRIPTOR_DEVICE_LO,
      device_hi:              USB_DEVICE_DESCRIPTOR_DEVICE_HI,
      manufacturer_str:       1,
      product_str:            2,
      serial_number_str:      3,
      number_configurations:  1
   },
   {
      length :                                        USB_QUALIFIER_DESCRIPTOR_LENGTH,
      type :                                          USB_QUALIFIER_DESCRIPTOR_TYPE,
      bcd_low :                                       USB_DEVICE_DESCRIPTOR_USB20_LO,
      bcd_high :                                      USB_DEVICE_DESCRIPTOR_USB20_HI,
      device_class:           USB_DEVICE_DESCRIPTOR_SERIAL_CLASS_VENDOR,
      device_subclass:        USB_DEVICE_DESCRIPTOR_SERIAL_SUBCLASS_VENDOR,
      device_protocol:        USB_DEVICE_DESCRIPTOR_SERIAL_PROTOCOL_VENDOR,
      max_packet_size:        8,
      number_configurations:    1,
      reserved:                                 0

   },
   total_number_interfaces:    USB_SERIAL_INTERFACE_NUMBER,
   total_number_endpoints:     USB_SERIAL_ENDPOINT_NUMBER,
   total_number_strings:       4,
   configurations:             &usb_configuration_serial,
   interfaces:                 usb_interface_serial,
   strings:                    usb_string_serial
};

//
typedef usbs_enumeration_data * pusb_serial_t;
pusb_serial_t pusb_serial = &usbs_enumeration_serial;

//
static rttmr_attr_t usbserial_timer_attr={
   100,
   dev_usbserial_timer_callback
#if defined(USE_ECOS)
   ,(unsigned int)0
#endif
};

static inf_usb_serial_t g_inf_usb_serial = {
   desc_rd : -1,
   desc_wr : -1,
   dev_usbserial_timer : 0,
   dev_usbserial_timer_attr: &usbserial_timer_attr,
   ttyusb_termios : 0,
   inter_char_timer : 0,
   {
      dter_rate : 115200,
      char_format : USBS_CDC_LINECODING_ONESTOPBIT,
      parity_type : USBS_CDC_LINECODING_NOPARITY,                   //None
      data_bits : 8
   },
   0
};
//
static usbs_control_return  usbs_serial_acm_class_handler(usbs_control_endpoint* ep0, void* data);
static void usbserial_complete_fn_write(void *data, int err);
static void usbserial_complete_fn_read(void *data, int err);
static void usbserial_complete_fn_interrupt(void *data, int err);
//
static usbs_control_return usbserial_complete_fn_control(usbs_control_endpoint *ctrl_ep, int err);

//
static unsigned char * usb_serial_virtual_ep_tbl[4] = {0};
static void fill_virtual_endpoint_table(void);

//
static unsigned char rsp_buf[32]={0};
/*============================================
| Implementation
==============================================*/
// ACM Class Handler
//
// For a Windows host, the device must, at least, respond to a SetLineCoding
// request (0x20), otherwise Windows will report that it's unable to open the
// port. This request normally sets the standard serial parameters:
//          baud rate, # stop bits, parity, and # data bits
// If we're just making believe that we're a serial port to communicate with
// the host via USB, then these values don't matter. So we ACK the request,
// but ignore the parameters.
// If we were actually creating a USB-serial converter, then we would need to
// read these values and configure the serial port accordingly.
//
// Similarly, the host can request the current settings through a
// GetLineCoding request (0x21). Since we're just faking it, we return some
// arbitrary values: 38400,1,N,8
/*-------------------------------------------
| Name:usbs_serial_acm_class_handler
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
usbs_control_return  usbs_serial_acm_class_handler(usbs_control_endpoint* ep0, void* data) {
   usbs_control_return result = USBS_CONTROL_RETURN_UNKNOWN;
   usb_devreq      *req = (usb_devreq *) ep0->control_buffer;

   switch (req->request) {

   case USBS_SERIAL_SET_CONTROL_LINE_STATE:
      //
      g_inf_usb_serial.line_state=req->value_lo;
      //
      ep0->buffer = rsp_buf;
      ep0->buffer_size = 0;
      result = USBS_CONTROL_RETURN_HANDLED;
      break;

   case USBS_SERIAL_SET_LINE_CODING:
      //we have to
      ep0->complete_fn = usbserial_complete_fn_control;
      //
      ep0->buffer = rsp_buf;
      ep0->buffer_size = 7;
      result = USBS_CONTROL_RETURN_HANDLED;
      break;

   case USBS_SERIAL_GET_LINE_CODING:
      rsp_buf[0] = BYTE0_32(g_inf_usb_serial.usbs_serial_linecoding.dter_rate);
      rsp_buf[1] = BYTE1_32(g_inf_usb_serial.usbs_serial_linecoding.dter_rate);
      rsp_buf[2] = BYTE2_32(g_inf_usb_serial.usbs_serial_linecoding.dter_rate);
      rsp_buf[3] = BYTE3_32(g_inf_usb_serial.usbs_serial_linecoding.dter_rate);
      rsp_buf[4] = g_inf_usb_serial.usbs_serial_linecoding.char_format; // One stop bit
      rsp_buf[5] = g_inf_usb_serial.usbs_serial_linecoding.parity_type; // No parity
      rsp_buf[6] = g_inf_usb_serial.usbs_serial_linecoding.data_bits; // 8 data bits
      ep0->buffer = rsp_buf;
      ep0->buffer_size = USBS_CDC_LINECODING_SIZE;
      result = USBS_CONTROL_RETURN_HANDLED;
      break;

   default:
      result = USBS_CONTROL_RETURN_STALL;
      break;
   }

   return result;
}

/*-------------------------------------------
| Name:usbserial_complete_fn_control
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
usbs_control_return usbserial_complete_fn_control(usbs_control_endpoint *ctrl_ep, int err) {
   //we receive that from a previous USBS_SERIAL_SET_LINE_CODING
   //just send a zero length packet
   ctrl_ep->buffer = rsp_buf;
   ctrl_ep->buffer_size = 0;
   ctrl_ep->complete_fn = 0;
   return USBS_CONTROL_RETURN_HANDLED;
}

/*-------------------------------------------
| Name:usbserial_complete_fn_interrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void usbserial_complete_fn_interrupt(void *data, int err) {
}

/*-------------------------------------------
| Name:usbserial_complete_fn_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void usbserial_complete_fn_read(void *data, int err) {
   __fire_io_int(ofile_lst[g_inf_usb_serial.desc_rd].owner_pthread_ptr_read);
}

/*-------------------------------------------
| Name:usbserial_complete_fn_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void usbserial_complete_fn_write(void *data, int err) {
   __fire_io_int(ofile_lst[g_inf_usb_serial.desc_wr].owner_pthread_ptr_write);
}

/*-------------------------------------------
| Name:dev_usbserial_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbserial_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_usbserial_timer_callback
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void  dev_usbserial_timer_callback(alrm_hdl_t alarm_handle, cyg_addrword_t data) {
   if( (g_inf_usb_serial.desc_rd>=0)
       && (g_inf_usb_serial.ttyusb_termios.c_cc[VTIME])
       && g_inf_usb_serial.inter_char_timer
       && !(--g_inf_usb_serial.inter_char_timer)) {

      if(ofile_lst[ofile_lst[g_inf_usb_serial.desc_rd].desc_nxt[0]].pfsop->fdev.fdev_isset_read(
            USB_ENDPOINT_DESCRIPTOR_DC_RX_NO)) {
         if(ofile_lst[g_inf_usb_serial.desc_rd].owner_pthread_ptr_read)
            __fire_io_int(ofile_lst[g_inf_usb_serial.desc_rd].owner_pthread_ptr_read);
      }
   }

   rttmr_restart(&g_inf_usb_serial.dev_usbserial_timer);

}

/*-------------------------------------------
| Name:dev_usbserial_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbserial_isset_read(desc_t desc){
   //desc_nxt[0] is the underlying device driver read descriptor
   if(!ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_isset_read(
         USB_ENDPOINT_DESCRIPTOR_DC_RX_NO)
      || (g_inf_usb_serial.ttyusb_termios.c_cc[VTIME] && !g_inf_usb_serial.inter_char_timer) ) {
      g_inf_usb_serial.inter_char_timer=g_inf_usb_serial.ttyusb_termios.c_cc[VTIME];
      return 0;
   }

   return -1;
}

/*-------------------------------------------
| Name:dev_usbserial_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbserial_isset_write(desc_t desc){
   //desc_nxt[1] is the underlying device driver write descriptor
   return ofile_lst[ofile_lst[desc].desc_nxt[1]].pfsop->fdev.fdev_isset_write(
             USB_ENDPOINT_DESCRIPTOR_DC_TX_NO);
}

/*-------------------------------------------
| Name:dev_usbserial_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbserial_open(desc_t desc, int o_flag){
   //for fire_io_int
   if(o_flag & O_RDONLY) {
      g_inf_usb_serial.desc_rd = desc;
      g_inf_usb_serial.ttyusb_termios.c_cc[VTIME]=0; // no timeout, blocking call
      g_inf_usb_serial.inter_char_timer = 0;
   }

   if(o_flag & O_WRONLY) {
      g_inf_usb_serial.desc_wr = desc;
   }
   //
   rttmr_create(&g_inf_usb_serial.dev_usbserial_timer,g_inf_usb_serial.dev_usbserial_timer_attr);
   return 0;
}

/*-------------------------------------------
| Name:dev_usbserial_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbserial_close(desc_t desc){
   if(ofile_lst[desc].oflag & O_RDONLY) {
      if(!ofile_lst[desc].nb_reader) {
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY) {
      if(!ofile_lst[desc].nb_writer) {
      }
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_usbserial_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbserial_read(desc_t desc, char* buf,int cb){
   //test
   while(!g_inf_usb_serial.line_state) ;
   //desc_nxt[0] is the underlying device driver read descriptor
   return ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_read(
             USB_ENDPOINT_DESCRIPTOR_DC_RX_NO,buf,cb);
}

/*-------------------------------------------
| Name:dev_usbserial_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbserial_write(desc_t desc, const char* buf,int cb){
   //test
   while(!g_inf_usb_serial.line_state) ;
   //we know corresponding endpoint to write , we replace desc by that
   //desc_nxt[1] is the underlying device driver write descriptor
   return ofile_lst[ofile_lst[desc].desc_nxt[1]].pfsop->fdev.fdev_write(
             USB_ENDPOINT_DESCRIPTOR_DC_TX_NO,buf,cb);
}

/*-------------------------------------------
| Name:dev_usbserial_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbserial_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*-------------------------------------------
| Name:dev_usbserial_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbserial_ioctl(desc_t desc,int request,va_list ap) {
   switch(request) {
   //
   case I_LINK: {
      usbs_completefn_endpoint tmp;
      if(ofile_lst[desc].p)   //already set.
         return 0;

      //set usb_enumeration pointer
      ofile_lst[desc].p = (void *)pusb_serial;

      //fill endpoint table for driver
      fill_virtual_endpoint_table();
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_ENDPOINT_TABLE,
                                                                    &usb_serial_virtual_ep_tbl);

      //set usb enumeration to driver
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_ENUMERATION_DATA,
                                                                    &pusb_serial);

      //set usb special class function
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_CLASS_HANDLER,
                                                                    &usbs_serial_acm_class_handler);

      //set complete function for all desire endpoints read and write
      tmp.complete_fn = usbserial_complete_fn_write;
      tmp.epn = USB_ENDPOINT_DESCRIPTOR_DC_TX_NO;
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,
                                                                    USB_SET_COMPLETION_FUNCTION,
                                                                    &tmp);
      //
      tmp.complete_fn = usbserial_complete_fn_read;
      tmp.epn = USB_ENDPOINT_DESCRIPTOR_DC_RX_NO;
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,
                                                                    USB_SET_COMPLETION_FUNCTION,
                                                                    &tmp);
      //
#ifdef USB_CLASS_ACM
      tmp.complete_fn = usbserial_complete_fn_interrupt;
      tmp.epn = USB_ENDPOINT_DESCRIPTOR_CDC_NO;
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,
                                                                    USB_SET_COMPLETION_FUNCTION,
                                                                    &tmp);
#endif

      //start control endpoint and enable IRQ on EP3, EP4 and EP5
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_START_CONTROL_ENDPOINT,
                                                                    0);
      return 0;
   }
   break;

   //
   case I_UNLINK:
      break;

   case USB_GET_CONTROL_ENDPOINT_STATE: {
      int *usb_serial_state = va_arg(ap, int *);
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,
                                                                    USB_GET_CONTROL_ENDPOINT_STATE,
                                                                    usb_serial_state);
      //*usb_serial_state = usbs_get_state();
      return 0;
   }
   break;

   //do serial some serial ioctl options
   case TCSETS:
   case TCSAFLUSH:
   case TCSADRAIN:
   case TCSANOW: {
      struct termios* termios_p = va_arg( ap, struct termios*);
      if(!termios_p)
         return -1;
      termios2ttys(termios_p);
      return 0;
   }
   break;

   //
   case TCGETS: {
      struct termios* termios_p = va_arg( ap, struct termios*);
      if(!termios_p)
         return -1;

      memcpy((void *)termios_p,(void *)&g_inf_usb_serial.ttyusb_termios,sizeof(struct termios));
      return 0;
   }
   break;

   //
   default:
      return -1;

   }
   return -1;
}


/*-------------------------------------------
| Name:termios2ttys
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int termios2ttys(struct termios* termios_p){
   const struct s2s *sp;
//   long n_speed;
//   speed_t speed;

   //xon/xoff
   if((termios_p->c_iflag&IXON) != (g_inf_usb_serial.ttyusb_termios.c_iflag&IXON)) {
      if(termios_p->c_iflag&IXON) {
         termios_p->c_iflag |= (IXOFF|IXON);
         //_xonoff_option = 1;
      }
      else{
         termios_p->c_iflag &= ~(IXOFF|IXON);
         //_xonoff_option = 0;

      }
   }
   else if((termios_p->c_iflag&IXOFF) != (g_inf_usb_serial.ttyusb_termios.c_iflag&IXOFF)) {
      if(termios_p->c_iflag&IXOFF) {
         termios_p->c_iflag |= (IXOFF|IXON);
         //_xonoff_option = 1;
      }
      else{
         termios_p->c_iflag &= ~(IXOFF|IXON);
         //_xonoff_option = 0;
      }
   }

   //speed

   //timeout read in blocking call
   if(termios_p->c_cc[VTIME]) {
      rttmr_stop(&g_inf_usb_serial.dev_usbserial_timer);
      g_inf_usb_serial.inter_char_timer = termios_p->c_cc[VTIME];
      rttmr_restart(&g_inf_usb_serial.dev_usbserial_timer);
   }
   else{
      rttmr_stop(&g_inf_usb_serial.dev_usbserial_timer);
   }

   //
   memcpy((void *)&g_inf_usb_serial.ttyusb_termios,(void *)termios_p,sizeof(struct termios));
   return 0;
}


/*-------------------------------------------
| Name:fill_virtual_endpoint_table
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void fill_virtual_endpoint_table(void) {
   int i=0;
   //We know all offset to get endpoint
#ifdef USB_CLASS_ACM
   usb_serial_virtual_ep_tbl[i++]=
      (unsigned char *)(usb_interface_Communication_Class_Interface+USBS_CDI_ENDPOINT_OFFSET);
#endif
   usb_serial_virtual_ep_tbl[i++]=
      (unsigned char *)(usb_interface_Data_Class_Interface+USB_INTERFACE_DESCRIPTOR_LENGTH);
   usb_serial_virtual_ep_tbl[i] =
      (unsigned char *)(usb_interface_Data_Class_Interface+USB_INTERFACE_DESCRIPTOR_LENGTH+
                        USB_ENDPOINT_DESCRIPTOR_LENGTH);
}

/*============================================
| End of Source  : usbserial.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
