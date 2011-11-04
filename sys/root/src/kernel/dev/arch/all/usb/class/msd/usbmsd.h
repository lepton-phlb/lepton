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
| Compiler Directive
==============================================*/
#ifndef _USBCLASS_MASS_STORAGE_H_
#define _USBCLASS_MASS_STORAGE_H_
/*============================================
| Includes
==============================================*/
#include "msd.h"
#include "lun.h"
#include "kernel/core/kernel_sem.h"

/*============================================
| Declaration
==============================================*/
#define USB_DEVICE_DESCRIPTOR_MSD_CLASS_VENDOR				0
#define USB_DEVICE_DESCRIPTOR_MSD_SUBCLASS_VENDOR			0
#define USB_DEVICE_DESCRIPTOR_MSD_PROTOCOL_VENDOR			0
        
//use endpoint 1 for IN and endpoint 2 for OUT
#define USB_ENDPOINT_DESCRIPTOR_DC_RX_NO                 1
#define USB_ENDPOINT_DESCRIPTOR_DC_TX_NO                 2

//Use endpoint 1 and 2 with 64 bytes for data class
#define USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_LO        0x40
#define USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_HI        0x00

//interface for communication class
#define USB_INTERFACE_DESCRIPTOR_MSD_CLASS              	0x08
#define USB_INTERFACE_DESCRIPTOR_MSD_SUBCLASS_SCSI       0x06
#define USB_INTERFACE_DESCRIPTOR_MSD_PROTOCOL_BULKO      0x50

//
#define USB_MSD_INTERFACE_NUMBER									1
#define USB_MSD_ENDPOINT_NUMBER                       	2
//configuration descriptor have
//1 interface
//+2 endpoint
//
#define USB_MSD_CONFIGURATION_DESCRIPTOR_SIZE         	32

//special request for ms class
//return the maximum LUN number supported by the device.
#define USBS_MSD_GET_MAX_LUN										0xFE
//reset the mass storage %device and its associated interface.
#define USBS_MSD_BULK_ONLY_RESET									0xFF

////
#define USBS_MSD_MAX_LUN											0			
#define USBS_MSD_MAX_LUN_LENGTH									1

//general information
typedef struct inf_usb_mass_st {
   int desc_rd;
   int desc_wr;
   
   int rx_endpoint;
   int tx_endpoint;
}inf_usb_mass_t;

//
typedef struct usb_mass_state_st {
	unsigned char 	state; //mass storage machine state
	
	kernel_sem_t	sem_read; //synchronisation for read
	kernel_sem_t	sem_write; //synchronisation for write
	
	MSCbw				cbw; //Command Block Wrapper
	MSCsw				csw; //Command Status Wrapper
	
	MSDLun         *luns; //Logical UNit array
	
	unsigned char	wait_reset_recovery;
	
	unsigned char	cmd_state; //current state of executing command
	unsigned int	cmd_length;
	unsigned char 	post_process;
	
}usb_mass_state_t;

int usb_mass_read_payload(int desc, char* buf,int cb);
int usb_mass_write_payload(int desc, char* buf,int cb);

#endif /*_USBCLASS_MASS_STORAGE_H_*/
