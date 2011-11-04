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
#include "kernel/core/cpu.h"
#include "kernel/core/ioctl.h"
#include "kernel/core/ioctl_usb.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "usbstd.h"
#include "kernel/dev/arch/all/usb/usbcore.h"
#include "kernel/dev/arch/all/usb/usbslave.h"

#include <string.h>
#include <stdlib.h>

/*============================================
| Global Declaration
==============================================*/
static const char dev_usbstd_name[]="usbstd\0";

static int dev_usbstd_load(void);
static int dev_usbstd_isset_read(desc_t desc);
static int dev_usbstd_isset_write(desc_t desc);
static int dev_usbstd_open(desc_t desc, int o_flag);
static int dev_usbstd_close(desc_t desc);
static int dev_usbstd_seek(desc_t desc,int offset,int origin);
static int dev_usbstd_read(desc_t desc, char* buf,int cb);
static int dev_usbstd_write(desc_t desc, const char* buf,int cb);
static int dev_usbstd_ioctl(desc_t desc,int request,va_list ap);

//
dev_map_t dev_usbstd_map={
   dev_usbstd_name,
   S_IFCHR,
   dev_usbstd_load,
   dev_usbstd_open,
   dev_usbstd_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_usbstd_read,
   dev_usbstd_write,
   dev_usbstd_seek,
   dev_usbstd_ioctl
};

//
static const usb_configuration_descriptor usb_configuration_std =  {
   length:             USB_CONFIGURATION_DESCRIPTOR_LENGTH,
   type:               USB_CONFIGURATION_DESCRIPTOR_TYPE,
   total_length_lo:    USB_STD_CONFIGURATION_DESCRIPTOR_SIZE,
   total_length_hi:    0,
   number_interfaces:  0,//1,
   configuration_id:   1,
   configuration_str:  0,
   attributes:         USB_CONFIGURATION_DESCRIPTOR_ATTR_REQUIRED |
   USB_CONFIGURATION_DESCRIPTOR_ATTR_SELF_POWERED,
   max_power:          50
};

//
static const usb_interface_descriptor usb_interface_Standard[] = {
	USB_INTERFACE_DESCRIPTOR(
   	USB_INTERFACE_DESCRIPTOR_LENGTH,
   	USB_INTERFACE_DESCRIPTOR_TYPE,
   	0,
   	0,
   	0, //excluding endpoint 0
   	0x0,
   	0x0,
   	0x0,
   	0
   )
};

static const usb_interface_descriptor * usb_interface_std [] ={
	usb_interface_Standard
};

//from usbstring.c
extern const unsigned char g_usb_language_id_str[];
extern const unsigned char g_usb_manufacturer_id_str[];
extern const unsigned char g_usb_product_id_str[];
extern const unsigned char g_usb_serial_number_str[];

//size,string descriptor,string
static const unsigned char* usb_string_std[] = {
      g_usb_language_id_str,
      g_usb_manufacturer_id_str,
      g_usb_product_id_str,
      g_usb_serial_number_str
};


//fill enumration data and pass it to usb device port
static usbs_enumeration_data usbs_enumeration_std = {
      /*Device Descriptor*/
      {
            length:                 USB_DEVICE_DESCRIPTOR_LENGTH,
            type:                   USB_DEVICE_DESCRIPTOR_TYPE,
            usb_spec_lo:            USB_DEVICE_DESCRIPTOR_USB20_LO,
            usb_spec_hi:            USB_DEVICE_DESCRIPTOR_USB20_HI,
            device_class:           USB_DEVICE_DESCRIPTOR_STD_CLASS_VENDOR,
            device_subclass:        USB_DEVICE_DESCRIPTOR_STD_SUBCLASS_VENDOR,
            device_protocol:        USB_DEVICE_DESCRIPTOR_STD_PROTOCOL_VENDOR,
            max_packet_size:        8,
            vendor_lo:              USB_DEVICE_DESCRIPTOR_VENDOR_LO,
            vendor_hi:              USB_DEVICE_DESCRIPTOR_VENDOR_HI,
            product_lo:             USB_DEVICE_DESCRIPTOR_PRODUCT_LO,
            product_hi:             USB_DEVICE_DESCRIPTOR_PRODUCT_HI,
            device_lo:              USB_DEVICE_DESCRIPTOR_DEVICE_LO,
            device_hi:              USB_DEVICE_DESCRIPTOR_DEVICE_HI,
            manufacturer_str:       1,
            product_str:            2,
            serial_number_str:      0,
            number_configurations:  1
      },
      {
		length : 					USB_QUALIFIER_DESCRIPTOR_LENGTH,
		type : 						USB_QUALIFIER_DESCRIPTOR_TYPE,
		bcd_low : 					USB_DEVICE_DESCRIPTOR_USB20_LO,
		bcd_high : 					USB_DEVICE_DESCRIPTOR_USB20_HI,
		device_class:           USB_DEVICE_DESCRIPTOR_STD_CLASS_VENDOR,
      device_subclass:        USB_DEVICE_DESCRIPTOR_STD_SUBCLASS_VENDOR,
      device_protocol:        USB_DEVICE_DESCRIPTOR_STD_PROTOCOL_VENDOR,
      max_packet_size:        8,
      number_configurations:	1,
      reserved:					0
		
		},
      1,/*total number of interfaces*/
      0,/*total number of endpoints excluding 0*/
      4, /*total number of strings */
      &usb_configuration_std,/*configuration descriptor*/
      usb_interface_std,/*interface descriptor*/
      usb_string_std
};

//
typedef usbs_enumeration_data * pusb_std_t;
pusb_std_t pusb_std = &usbs_enumeration_std;
//
//
static unsigned char * usb_std_virtual_ep_tbl[1] = {0};
/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:dev_usbstd_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_usbstd_load(void){
   return 0;
}

/*-------------------------------------------
| Name:dev_usbstd_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_usbstd_isset_read(desc_t desc){
  return -1;
}

/*-------------------------------------------
| Name:dev_usbstd_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_usbstd_isset_write(desc_t desc){
   return -1;
}

/*-------------------------------------------
| Name:dev_usbstd_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_usbstd_open(desc_t desc, int o_flag){
   return 0;
}

/*-------------------------------------------
| Name:dev_usbstd_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_usbstd_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_usbstd_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_usbstd_read(desc_t desc, char* buf,int cb){
   return 0;
}

/*-------------------------------------------
| Name:dev_usbstd_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_usbstd_write(desc_t desc, const char* buf,int cb){

   return 0;
}

/*-------------------------------------------
| Name:dev_usbstd_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_usbstd_seek(desc_t desc,int offset,int origin){
   return -1;
}

/*-------------------------------------------
| Name:dev_usbstd_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_usbstd_ioctl(desc_t desc,int request,va_list ap) {
   switch(request){
      //
      case I_LINK:{
         int desc_usbdp = va_arg(ap, int);
         if(ofile_lst[desc].p)//already set.
            return 0;

			//fill endpoint table
         usb_std_virtual_ep_tbl[0] = 0x0;
         ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_ENDPOINT_TABLE,&usb_std_virtual_ep_tbl);
         
         //set usb_enumeration pointer
         ofile_lst[desc].p = (void *)pusb_std;
         //set usb enumeration to driver
         ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_SET_ENUMERATION_DATA,&pusb_std);
         
         //
         ofile_lst[ofile_lst[desc].desc_nxt[0]].pfsop->fdev.fdev_ioctl(desc,USB_START_CONTROL_ENDPOINT,0);
         return 0;
      }
      break;

      //
      case I_UNLINK:{
      }
      break;

      //
      default:
         return -1;

   }
   return -1;
}

/*============================================
| End of Source  : usbstd.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
