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
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/cpu.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/ioctl_usb.h"
#include "kernel/core/ioctl_eth.h"

#include "kernel/core/core_rttimer.h"
#include "lib/libc/termios/termios.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "usbeth.h"
#include "kernel/dev/arch/all/usb/usbcore.h"
#include "kernel/dev/arch/all/usb/usbslave.h"

#include <string.h>
#include <stdlib.h>

#define  USBS_CDI_ENDPOINT_OFFSET   (USB_INTERFACE_DESCRIPTOR_LENGTH+ \
                                     USBS_ETH_HEADER_SIZE + USBS_ETH_UNION_DESCRIPTOR_SIZE+ \
                                     USBS_ETH_NETWORKING_DESCRIPTOR_SIZE)

/*============================================
| Global Declaration
==============================================*/
static const char dev_usbeth_name[]="usbeth\0";

static int dev_usbeth_load(void);
static int dev_usbeth_isset_read(desc_t desc);
static int dev_usbeth_isset_write(desc_t desc);
static int dev_usbeth_open(desc_t desc, int o_flag);
static int dev_usbeth_close(desc_t desc);
static int dev_usbeth_seek(desc_t desc,int offset,int origin);
static int dev_usbeth_read(desc_t desc, char* buf,int cb);
static int dev_usbeth_write(desc_t desc, const char* buf,int cb);
static int dev_usbeth_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_usbeth_map = {
   dev_usbeth_name,
   S_IFCHR,
   dev_usbeth_load,
   dev_usbeth_open,
   dev_usbeth_close,
   dev_usbeth_isset_read,
   dev_usbeth_isset_write,
   dev_usbeth_read,
   dev_usbeth_write,
   dev_usbeth_seek,
   dev_usbeth_ioctl
};

//
static const usb_configuration_descriptor usb_configuration_eth = {
   length:             USB_CONFIGURATION_DESCRIPTOR_LENGTH,
   type:               USB_CONFIGURATION_DESCRIPTOR_TYPE,
   total_length_lo :   USB_ETH_CONFIGURATION_DESCRIPTOR_SIZE,
   total_length_hi :   0,
   number_interfaces:  USB_ETH_INTERFACE_NUMBER,
   configuration_id:   1,
   configuration_str:  0,
   attributes:         (USB_CONFIGURATION_DESCRIPTOR_ATTR_REQUIRED |
                        USB_CONFIGURATION_DESCRIPTOR_ATTR_SELF_POWERED),
   max_power:          0   //50
};

static const usb_interface_descriptor usb_interface_Communication_Class_Interface[] = {
   USB_INTERFACE_DESCRIPTOR(
      USB_INTERFACE_DESCRIPTOR_LENGTH,
      USB_INTERFACE_DESCRIPTOR_TYPE,
      0,
      0,
      1,
      USB_INTERFACE_DESCRIPTOR_ETH_COMM_CLASS,
      USB_INTERFACE_DESCRIPTOR_ETH_COMM_SUBCLASS,
      USB_INTERFACE_DESCRIPTOR_ETH_COMM_PROTOCOL,      //just test it//0,
      0x00
      ),

   ETH_HEADER_DESCRIPTOR (
      USBS_ETH_HEADER_SIZE,
      USBS_ETH_GENERIC_INTERFACE,
      USBS_ETH_GENERIC_HEADER,
      USBS_ETH_GENERIC_BCD_VERSION_LO,
      USBS_ETH_GENERIC_BCD_VERSION_HI
      ),
   ETH_UNION_DESCRIPTOR(
      USBS_ETH_UNION_DESCRIPTOR_SIZE,
      USBS_ETH_GENERIC_INTERFACE,
      USBS_ETH_GENERIC_UNION,
      0,
      1
      ),
   ETH_NETWORKING_DESCRIPTOR(
      USBS_ETH_NETWORKING_DESCRIPTOR_SIZE,
      USBS_ETH_GENERIC_INTERFACE,
      USBS_ETH_GENERIC,
      4,      //index in string descriptor
      0,0,0,0,
      USBS_ETH_SEG_SIZE_LO,
      USBS_ETH_SEG_SIZE_HI,
      0,0,
      0
      ),
   USB_ENDPOINT_DESCRIPTOR(
      USB_ENDPOINT_DESCRIPTOR_LENGTH,
      USB_ENDPOINT_DESCRIPTOR_TYPE,
      USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN | USB_ENDPOINT_DESCRIPTOR_ETH_NO,
      USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT,
      USB_ENDPOINT_DESCRIPTOR_ETH_FIFO_LENGHT_LO,
      USB_ENDPOINT_DESCRIPTOR_ETH_FIFO_LENGHT_HI,
      10      //255
      )
};

static const usb_interface_descriptor usb_interface_Data_Class_Interface_default[] = {
   USB_INTERFACE_DESCRIPTOR(
      USB_INTERFACE_DESCRIPTOR_LENGTH,
      USB_INTERFACE_DESCRIPTOR_TYPE,
      1,                          //0,
      0,
      0,
      USB_INTERFACE_DESCRIPTOR_ETH_DATA_CLASS,
      USB_INTERFACE_DESCRIPTOR_ETH_DATA_SUBCLASS,
      USB_INTERFACE_DESCRIPTOR_ETH_DATA_PROTOCOL,
      0x00
      ),
};

static const usb_interface_descriptor usb_interface_Data_Class_Interface_io[] = {
   USB_INTERFACE_DESCRIPTOR(
      USB_INTERFACE_DESCRIPTOR_LENGTH,
      USB_INTERFACE_DESCRIPTOR_TYPE,
      1,
      1,      //0,
      2,
      USB_INTERFACE_DESCRIPTOR_ETH_DATA_CLASS,
      USB_INTERFACE_DESCRIPTOR_ETH_DATA_SUBCLASS,
      USB_INTERFACE_DESCRIPTOR_ETH_DATA_PROTOCOL,
      0x00
      ),
   // Tx (Bulk IN) Endpoint Descriptor (data class)
   USB_ENDPOINT_DESCRIPTOR(
      USB_ENDPOINT_DESCRIPTOR_LENGTH,
      USB_ENDPOINT_DESCRIPTOR_TYPE,
      USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN | USB_ENDPOINT_DESCRIPTOR_ETH_TX_NO,
      USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
      USB_ENDPOINT_DESCRIPTOR_ETH_FIFO_LENGHT_LO,
      USB_ENDPOINT_DESCRIPTOR_ETH_FIFO_LENGHT_HI,
      0
      ),
   // Rx (Bulk OUT) Endpoint Descriptor (data class)
   USB_ENDPOINT_DESCRIPTOR(
      USB_ENDPOINT_DESCRIPTOR_LENGTH,
      USB_ENDPOINT_DESCRIPTOR_TYPE,
      USB_ENDPOINT_DESCRIPTOR_ENDPOINT_OUT | USB_ENDPOINT_DESCRIPTOR_ETH_RX_NO,
      USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
      USB_ENDPOINT_DESCRIPTOR_ETH_FIFO_LENGHT_LO,
      USB_ENDPOINT_DESCRIPTOR_ETH_FIFO_LENGHT_HI,
      0
      )
};

static const usb_interface_descriptor * usb_interface_eth [] ={
   usb_interface_Communication_Class_Interface,
   usb_interface_Data_Class_Interface_default,
   usb_interface_Data_Class_Interface_io
};

//from usbstring.c
extern const unsigned char g_usb_language_id_str[];
extern const unsigned char g_usb_manufacturer_id_str[];
extern const unsigned char g_usb_product_id_str[];
extern const unsigned char g_usb_serial_number_str[];

//
static const unsigned char usb_eth_mac_addr_str [] = {
   USB_STRING_DESC_LENGTH(12),
   USB_DEVREQ_DESCRIPTOR_TYPE_STRING,
   USB_STRING_DESC_UNICODE('0'),USB_STRING_DESC_UNICODE('0'),USB_STRING_DESC_UNICODE('1'),
   USB_STRING_DESC_UNICODE('F'),USB_STRING_DESC_UNICODE('A'),USB_STRING_DESC_UNICODE('B'),
   USB_STRING_DESC_UNICODE('A'),USB_STRING_DESC_UNICODE('B'),USB_STRING_DESC_UNICODE('A'),
   USB_STRING_DESC_UNICODE('B'),USB_STRING_DESC_UNICODE('A'),USB_STRING_DESC_UNICODE('B')
};

//
static const unsigned char* usb_string_eth[] = {
   g_usb_language_id_str,
   g_usb_manufacturer_id_str,
   g_usb_product_id_str,
   g_usb_serial_number_str,
   usb_eth_mac_addr_str
};

//
static usbs_enumeration_data usbs_enumeration_eth = {
   {
      length:                 USB_DEVICE_DESCRIPTOR_LENGTH,
      type:                   USB_DEVICE_DESCRIPTOR_TYPE,
      usb_spec_lo:            USB_DEVICE_DESCRIPTOR_USB20_LO,
      usb_spec_hi:            USB_DEVICE_DESCRIPTOR_USB20_HI,
      device_class:           USB_DEVICE_DESCRIPTOR_ETH_CLASS_VENDOR,
      device_subclass:        USB_DEVICE_DESCRIPTOR_ETH_SUBCLASS_VENDOR,
      device_protocol:        USB_DEVICE_DESCRIPTOR_ETH_PROTOCOL_VENDOR,
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
      device_class:           USB_DEVICE_DESCRIPTOR_ETH_CLASS_VENDOR,
      device_subclass:        USB_DEVICE_DESCRIPTOR_ETH_SUBCLASS_VENDOR,
      device_protocol:        USB_DEVICE_DESCRIPTOR_ETH_PROTOCOL_VENDOR,
      max_packet_size:        8,
      number_configurations:    1,
      reserved:                                 0

   },
   total_number_interfaces:    3, //USB_ETH_INTERFACE_NUMBER,
   total_number_endpoints:     USB_ETH_ENDPOINT_NUMBER,
   total_number_strings:       5,
   configurations:             &usb_configuration_eth,
   interfaces:                 usb_interface_eth,
   strings:                    usb_string_eth
};

//
typedef usbs_enumeration_data * pusb_eth_t;
pusb_eth_t pusb_eth = &usbs_enumeration_eth;

//
static const unsigned char usbeth_mac_address[] = {0x00,0x1F,0xAB,0xAB,0xAB,0xAB};
static inf_usb_eth_t g_inf_usb_eth = {
   desc_rd : -1,
   desc_wr : -1,
   mac_addr : usbeth_mac_address
};

//
static usbs_control_return  usbs_eth_class_handler(usbs_control_endpoint* ep0, void* data);
static void usbeth_complete_fn_write(void *data, int err);
static void usbeth_complete_fn_read(void *data, int err);
static void usbeth_complete_fn_interrupt(void *data, int err);
//
static unsigned char * usb_eth_virtual_ep_tbl[4] = {0};
static void fill_virtual_endpoint_table(void);

//
static unsigned char rsp_buf[32]={0};
/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:usbs_eth_class_handler
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
usbs_control_return  usbs_eth_class_handler(usbs_control_endpoint* ep0, void* data) {
   usbs_control_return result = USBS_CONTROL_RETURN_UNKNOWN;
   usb_devreq      *req = (usb_devreq *) ep0->control_buffer;

   switch (req->request) {
   case USBS_ETH_SET_MULTICAST_FILTER:
   case USBS_ETH_SET_PWR_MANAGEMENT:
   case USBS_ETH_GET_PWR_MANAGEMENT:
   case USBS_ETH_STATISTIC:
      result = USBS_CONTROL_RETURN_STALL;                  //USBS_CONTROL_RETURN_HANDLED;
      break;

   //required
   case USBS_ETH_PACKET_FILTER:
      ep0->buffer = rsp_buf;
      ep0->buffer_size = 0;
      result = USBS_CONTROL_RETURN_HANDLED;
      break;

   default:
      result = USBS_CONTROL_RETURN_STALL;
      break;
   }

   return result;
}

/*-------------------------------------------
| Name:usbeth_complete_fn_interrupt
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void usbeth_complete_fn_interrupt(void *data, int err) {
}

/*-------------------------------------------
| Name:usbeth_complete_fn_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void usbeth_complete_fn_read(void *data, int err) {
   __fire_io_int(ofile_lst[g_inf_usb_eth.desc_rd].owner_pthread_ptr_read);
}

/*-------------------------------------------
| Name:usbeth_complete_fn_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void usbeth_complete_fn_write(void *data, int err) {
   __fire_io_int(ofile_lst[g_inf_usb_eth.desc_wr].owner_pthread_ptr_write);
}

/*-------------------------------------------
| Name:dev_usbeth_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbeth_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_usbeth_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbeth_isset_read(desc_t desc){
   //desc_nxt[0] is the underlying device driver read descriptor
   return ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_isset_read(
             USB_ENDPOINT_DESCRIPTOR_ETH_RX_NO);
}

/*-------------------------------------------
| Name:dev_usbeth_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbeth_isset_write(desc_t desc){
   //desc_nxt[1] is the underlying device driver write descriptor
   return ofile_lst[ofile_lst[desc].desc_nxt[1]].pfsop->fdev.fdev_isset_write(
             USB_ENDPOINT_DESCRIPTOR_ETH_TX_NO);
}

/*-------------------------------------------
| Name:dev_usbeth_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbeth_open(desc_t desc, int o_flag){
   //for fire_io_int
   if(o_flag & O_RDONLY) {
      //g_inf_usb_eth.desc_rd = desc;
   }

   if(o_flag & O_WRONLY) {
      //g_inf_usb_eth.desc_wr = desc;
   }

   return 0;
}

/*-------------------------------------------
| Name:dev_usbeth_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbeth_close(desc_t desc){
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
| Name:dev_usbeth_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbeth_read(desc_t desc, char* buf,int cb){
   //
   g_inf_usb_eth.desc_rd = desc;
   //desc_nxt[0] is the underlying device driver read descriptor
   return ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_read(
             USB_ENDPOINT_DESCRIPTOR_ETH_RX_NO,buf,cb);
}

/*-------------------------------------------
| Name:dev_usbeth_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbeth_write(desc_t desc, const char* buf,int cb){
   g_inf_usb_eth.desc_wr = desc;
   //we know corresponding endpoint to write , we replace desc by that
   //desc_nxt[1] is the underlying device driver write descriptor
   return ofile_lst[ofile_lst[desc].desc_nxt[1]].pfsop->fdev.fdev_write(
             USB_ENDPOINT_DESCRIPTOR_ETH_TX_NO,buf,cb);
}

/*-------------------------------------------
| Name:dev_usbeth_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbeth_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*-------------------------------------------
| Name:dev_usbeth_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_usbeth_ioctl(desc_t desc,int request,va_list ap) {
   switch(request) {
   //
   case I_LINK: {
      usbs_completefn_endpoint tmp;
      va_list va_ioctl;

      if(ofile_lst[desc].p)   //already set.
         return 0;

      //set usb_enumeration pointer
      ofile_lst[desc].p = (void *)pusb_eth;

      //fill endpoint table for driver
      fill_virtual_endpoint_table();
      memcpy((void *)&va_ioctl, &usb_eth_virtual_ep_tbl, sizeof(va_list));
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_ENDPOINT_TABLE,
                                                                    va_ioctl);

      //set usb enumeration to driver
      memcpy((void *)&va_ioctl, &pusb_eth, sizeof(va_list));
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_ENUMERATION_DATA,
                                                                    va_ioctl);

      //set usb special class function
      memcpy((void *)&va_ioctl, &usbs_eth_class_handler, sizeof(va_list));
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_CLASS_HANDLER,
                                                                    va_ioctl);

      //set complete function for all desire endpoints read and write
      tmp.complete_fn = usbeth_complete_fn_write;
      tmp.epn = USB_ENDPOINT_DESCRIPTOR_ETH_TX_NO;
      memcpy((void *)&va_ioctl, &tmp, sizeof(va_list));
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,
                                                                    USB_SET_COMPLETION_FUNCTION,
                                                                    va_ioctl);
      //
      tmp.complete_fn = usbeth_complete_fn_read;
      tmp.epn = USB_ENDPOINT_DESCRIPTOR_ETH_RX_NO;
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,
                                                                    USB_SET_COMPLETION_FUNCTION,
                                                                    va_ioctl);
      //
      tmp.complete_fn = usbeth_complete_fn_interrupt;
      tmp.epn = USB_ENDPOINT_DESCRIPTOR_ETH_NO;
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,
                                                                    USB_SET_COMPLETION_FUNCTION,
                                                                    va_ioctl);

      //start control endpoint and enable IRQ on EP1, EP2
      bzero(&va_ioctl, sizeof(va_list));
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_START_CONTROL_ENDPOINT,
                                                                    va_ioctl);

      ///dummy BUG no reset of ofile_lst[ofile_lst[desc].desc_nxt[0]].desc_nxt[0|1]
      /*ofile_lst[ofile_lst[desc].desc_nxt[0]].desc_nxt[0]=-1;
      ofile_lst[ofile_lst[desc].desc_nxt[0]].desc_nxt[1]=-1;*/
      ///
      return 0;
   }
   break;

   //
   case I_UNLINK:
      break;

   case USB_GET_CONTROL_ENDPOINT_STATE: {
      ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,
                                                                    USB_GET_CONTROL_ENDPOINT_STATE,
                                                                    ap);
      return 0;
   }
   break;

   //
   case ETHGETHWADDRESS: {
      unsigned char* p_eth_hwaddr = va_arg( ap, unsigned char*);
      if(!p_eth_hwaddr)
         return -1;

      p_eth_hwaddr[0] = g_inf_usb_eth.mac_addr[0];
      p_eth_hwaddr[1] = g_inf_usb_eth.mac_addr[1];
      p_eth_hwaddr[2] = g_inf_usb_eth.mac_addr[2];
      p_eth_hwaddr[3] = g_inf_usb_eth.mac_addr[3];
      p_eth_hwaddr[4] = g_inf_usb_eth.mac_addr[4];
      p_eth_hwaddr[5] = g_inf_usb_eth.mac_addr[5];
   }
   break;

   //
   default:
      return -1;

   }
   return -1;
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
   //We know all offset to get endpoint
   usb_eth_virtual_ep_tbl[0]=
      (unsigned char *)(usb_interface_Communication_Class_Interface+USBS_CDI_ENDPOINT_OFFSET);
   usb_eth_virtual_ep_tbl[1]=
      (unsigned char *)(usb_interface_Data_Class_Interface_io+USB_INTERFACE_DESCRIPTOR_LENGTH);
   usb_eth_virtual_ep_tbl[2] =
      (unsigned char *)(usb_interface_Data_Class_Interface_io+USB_INTERFACE_DESCRIPTOR_LENGTH+
                        USB_ENDPOINT_DESCRIPTOR_LENGTH);
}

/*============================================
| End of Source  : usbeth.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
