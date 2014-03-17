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
#ifndef _USBCLASS_ETH_H_
#define _USBCLASS_ETH_H_
/*============================================
| Includes
==============================================*/

/*============================================
| Declaration
==============================================*/

//Use endpoint 3 with 64 for cdc class
#define USB_ENDPOINT_DESCRIPTOR_ETH_FIFO_LENGHT_LO                      0x40
#define USB_ENDPOINT_DESCRIPTOR_ETH_FIFO_LENGHT_HI                      0x00

//use endpoint 3 for interrupt
#define USB_ENDPOINT_DESCRIPTOR_ETH_NO                                                  3


//use endpoint 1 for IN and endpoint 2 for OUT
#define USB_ENDPOINT_DESCRIPTOR_ETH_RX_NO                                               1
#define USB_ENDPOINT_DESCRIPTOR_ETH_TX_NO                                               2

//Use endpoint 1 and 2 with 64 bytes for data class
#define USB_ENDPOINT_DESCRIPTOR_ETH_FIFO_LENGHT_LO                      0x40
#define USB_ENDPOINT_DESCRIPTOR_ETH_FIFO_LENGHT_HI                      0x00

//interface for communication class
#define USB_INTERFACE_DESCRIPTOR_ETH_COMM_CLASS                         0x02
#define USB_INTERFACE_DESCRIPTOR_ETH_COMM_SUBCLASS                      0x06
#define USB_INTERFACE_DESCRIPTOR_ETH_COMM_PROTOCOL                      0x00

#define USB_INTERFACE_DESCRIPTOR_ETH_DATA_CLASS                         0x0A
#define USB_DEVICE_DESCRIPTOR_ETH_CLASS_VENDOR                          0x02
#define USB_ETH_INTERFACE_NUMBER \
   2
#define USB_ETH_ENDPOINT_NUMBER                                                                 3
//configuration descriptor have
//3 interfaces (9+9+9)
//+3 endpoint (7+7+7)
//+ 4 functional headers (5+5+13)(see USBS_ETH_HEADER_SIZE ...)
// = 72 bytes + configuration size
#define USB_ETH_CONFIGURATION_DESCRIPTOR_SIZE                           80

#define USB_INTERFACE_DESCRIPTOR_ETH_DATA_SUBCLASS                      0x00
#define USB_INTERFACE_DESCRIPTOR_ETH_DATA_PROTOCOL                      0x00

//
#define USB_DEVICE_DESCRIPTOR_ETH_SUBCLASS_VENDOR                       0x00
#define USB_DEVICE_DESCRIPTOR_ETH_PROTOCOL_VENDOR                       0x00

//special request for ETH class
#define USBS_ETH_NETWORK_CONNECTION \
   0x00
#define USBS_ETH_RESPONSE_AVAILABLE \
   0x01
#define USBS_ETH_CONNECTION_SPEED_CHANGE                                                0x2a

#define USBS_ETH_SET_MULTICAST_FILTER                                                   0x40
#define USBS_ETH_SET_PWR_MANAGEMENT \
   0x41
#define USBS_ETH_GET_PWR_MANAGEMENT \
   0x42
#define USBS_ETH_PACKET_FILTER \
   0x43
#define USBS_ETH_STATISTIC \
   0x44

///DON'T USE SPECIAL STRUCT FOR FUNCTIONNAL HEADER
///JUST USE MACRO TO HANDLE
#define ETH_HEADER_DESCRIPTOR(lenght,desc_type,desc_subtype,bcd_cdc_hi,bcd_cdc_lo) \
   lenght,desc_type,desc_subtype,bcd_cdc_hi,bcd_cdc_lo

#define  USBS_ETH_HEADER_SIZE                            5
#define  USBS_ETH_GENERIC_INTERFACE                      0x24
#define  USBS_ETH_GENERIC_HEADER                         0x00
#define  USBS_ETH_GENERIC_BCD_VERSION_HI                 0x01
#define  USBS_ETH_GENERIC_BCD_VERSION_LO                 0x10

//
#define  ETH_UNION_DESCRIPTOR(lenght,desc_type,desc_subtype,master_interface,slave_interface) \
   lenght,desc_type,desc_subtype,master_interface,slave_interface

#define USBS_ETH_UNION_DESCRIPTOR_SIZE                                                  5
#define USBS_ETH_GENERIC_UNION \
   0x06

//
//#define	ETH_COUNTRY_SELECTION_DESCRIPTOR(lenght,desc_type,desc_subtype,country_code_reldate,country_code_0) \
// // // //			lenght,desc_type,desc_subtype,country_code_reldate,country_code_0

//#define	USBS_ETH_COUNTRY_SELECTION_SIZE							6

//
//imac_addr_index index in string descriptors
#define ETH_NETWORKING_DESCRIPTOR(lenght,desc_type,desc_subtype,imac_addr_index, \
                                  eth_stat0,eth_stat1,eth_stat2,eth_stat3, \
                                  max_seg_size_lo,max_seg_size_hi, \
                                  numb_mc_filt_lo,numb_mc_filt_hi, \
                                  numb_pow_filt) \
   lenght,desc_type,desc_subtype,imac_addr_index, \
   eth_stat0,eth_stat1,eth_stat2,eth_stat3, \
   max_seg_size_lo,max_seg_size_hi, \
   numb_mc_filt_lo,numb_mc_filt_hi, \
   numb_pow_filt

#define USBS_ETH_NETWORKING_DESCRIPTOR_SIZE                                             13 //(1+1+1+1+4+2+2+1)
#define USBS_ETH_GENERIC \
   0x0f
//1514 bytes
#define USBS_ETH_SEG_SIZE_LO \
   0xea
#define USBS_ETH_SEG_SIZE_HI \
   0x05

//general information
typedef struct inf_usb_eth_st {
   int desc_rd;
   int desc_wr;
   const unsigned char * mac_addr;
}inf_usb_eth_t;

#endif /*_USBCLASS_SERIAL_H_*/
