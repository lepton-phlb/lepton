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
| Compiler Directive
==============================================*/
#ifndef _USBCLASS_SERIAL_H_
#define _USBCLASS_SERIAL_H_
/*============================================
| Includes
==============================================*/

/*============================================
| Declaration
==============================================*/

//Use endpoint 3 with 64 for cdc class
#define USB_ENDPOINT_DESCRIPTOR_CDC_FIFO_LENGHT_LO       0x40
#define USB_ENDPOINT_DESCRIPTOR_CDC_FIFO_LENGHT_HI       0x00

//use endpoint 3 for interrupt
#define USB_ENDPOINT_DESCRIPTOR_CDC_NO                   3

#define USB_USE_SMALL_BUFFER

#ifdef USB_USE_SMALL_BUFFER
//use endpoint 1 for IN and endpoint 2 for OUT
   #define USB_ENDPOINT_DESCRIPTOR_DC_RX_NO                 1
   #define USB_ENDPOINT_DESCRIPTOR_DC_TX_NO                 2

//Use endpoint 1 and 2 with 64 bytes for data class
   #define USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_LO        0x40
   #define USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_HI        0x00

#else
//use endpoint 1 for IN and endpoint 2 for OUT
   #define USB_ENDPOINT_DESCRIPTOR_DC_RX_NO                 4
   #define USB_ENDPOINT_DESCRIPTOR_DC_TX_NO                 5

//Use endpoint 4 and 5 with 256 bytes for data class
   #define USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_LO        0x00
   #define USB_ENDPOINT_DESCRIPTOR_DC_FIFO_LENGHT_HI        0x01

#endif //USB_USE_SMALL_BUFFER

//interface for communication class
#define USB_INTERFACE_DESCRIPTOR_COMM_CLASS              0x02
#define USB_INTERFACE_DESCRIPTOR_COMM_SUBCLASS           0x02
#define USB_INTERFACE_DESCRIPTOR_COMM_PROTOCOL           0x01

//for XP
#define USB_CLASS_ACM
//for Vista
#define USB_CLASS_ACM_CDC

//
#ifdef USB_CLASS_ACM
   #define USB_INTERFACE_DESCRIPTOR_DATA_CLASS              0x0A
   #define USB_DEVICE_DESCRIPTOR_SERIAL_CLASS_VENDOR        0x02
   #define USB_SERIAL_INTERFACE_NUMBER                      2
   #define USB_SERIAL_ENDPOINT_NUMBER                       3
//configuration descriptor have
//2 interface
//+3 endpoint
//+ 4 functional headers = 67 bytes
   #ifdef USB_CLASS_ACM_CDC
      #define USB_SERIAL_CONFIGURATION_DESCRIPTOR_SIZE         67
   #else
//configuration descriptor have
//2 interface
//+3 endpoint
// = 67 bytes
      #define USB_SERIAL_CONFIGURATION_DESCRIPTOR_SIZE         48
   #endif

#else
   #define USB_INTERFACE_DESCRIPTOR_DATA_CLASS              0xFF
   #define USB_DEVICE_DESCRIPTOR_SERIAL_CLASS_VENDOR        0x00
   #define USB_SERIAL_INTERFACE_NUMBER                      1
   #define USB_SERIAL_ENDPOINT_NUMBER                       2

//configuration descriptor have
//1 interface
//+2 endpoint
//
   #define USB_SERIAL_CONFIGURATION_DESCRIPTOR_SIZE         32

#endif

#define USB_INTERFACE_DESCRIPTOR_DATA_SUBCLASS           0x00
#define USB_INTERFACE_DESCRIPTOR_DATA_PROTOCOL           0x00

//
#define USB_DEVICE_DESCRIPTOR_SERIAL_SUBCLASS_VENDOR     0x00
#define USB_DEVICE_DESCRIPTOR_SERIAL_PROTOCOL_VENDOR     0x00

//
#define BYTE0_32(word32)                                 ((unsigned char) ((word32) & 0xFF))
#define BYTE1_32(word32)                                 ((unsigned char) (((word32) >>  8) & 0xFF))
#define BYTE2_32(word32)                                 ((unsigned char) (((word32) >> 16) & 0xFF))
#define BYTE3_32(word32)                                 ((unsigned char) (((word32) >> 24) & 0xFF))

//special request for cdc class
#define USBS_SERIAL_SET_LINE_CODING                      0x20
#define USBS_SERIAL_GET_LINE_CODING                      0x21
#define USBS_SERIAL_SET_CONTROL_LINE_STATE               0x22
#define USBS_SERIAL_SEND_BREAK                           0x23

///DON'T USE SPECIAL STRUCT FOR FUNCTIONNAL HEADER
///JUST USE MACRO TO HANDLE
#define CDC_HEADER_DESCRIPTOR(lenght,desc_type,desc_subtype,bcd_cdc_hi,bcd_cdc_lo) \
   lenght,desc_type,desc_subtype,bcd_cdc_hi,bcd_cdc_lo

#define  USBS_CDC_HEADER_SIZE                            5
#define  USBS_CDC_GENERIC_INTERFACE                      0x24
#define  USBS_CDC_GENERIC_HEADER                         0x00
#define  USBS_CDC_GENERIC_BCD_VERSION_HI                 0x01
#define  USBS_CDC_GENERIC_BCD_VERSION_LO                 0x10

//
#define  CDC_CALL_MANAGEMENT_DESCRIPTOR(lenght,desc_type,desc_subtype,capabilities,data_interface) \
   lenght,desc_type,desc_subtype,capabilities,data_interface

#define  USBS_CDC_CALL_MANAGEMENT_DESCRIPTOR_SIZE        5
#define  USBS_CDC_GENERIC_CALLMANAGEMENT                 0x01
#define  USBS_CDC_GENERIC_SELFMANAGEMENT                 0 //(1<<0)

//
#define  CDC_ABSTRACT_CONTROL_MANAGEMENT_DESCRIPTOR(lenght,desc_type,desc_subtype,capabilities) \
   lenght,desc_type,desc_subtype,capabilities

#define  USBS_CDC_ABSTRACT_CTRL_MANAGEMENT_DESCRIPTOR_SIZE 4
/// Device supports the request combination of SetLineCoding, GetLineCoding and
/// SetControlLineState.
#define  USBS_CDC_GENERIC_CRTL_MANAGEMENT                   (1 << 1)
#define  USBS_CDC_GENERIC_ACM                               0x02

//
#define  CDC_UNION_DESCRIPTOR(lenght,desc_type,desc_subtype,master_interface,slave_interface) \
   lenght,desc_type,desc_subtype,master_interface,slave_interface

#define USBS_CDC_UNION_DESCRIPTOR                           5
#define USBS_CDC_GENERIC_UNION                              0x06

//
//see CDC PSTN 6.3.11
typedef struct usbs_serial_linecoding_st {
   unsigned int dter_rate;        //data terminal type in byte/sec
   unsigned char char_format;       //stop bits
   unsigned char parity_type;       //parity
   unsigned char data_bits;       //data bits
}usbs_serial_linecoding_t;

#define USBS_CDC_LINECODING_SIZE \
   7
//one stop bit.
#define USBS_CDC_LINECODING_ONESTOPBIT                                                          0
//1.5 stop bit.
#define USBS_CDC_LINECODING_ONE5STOPBIT                                                 1
//two stop bits.
#define USBS_CDC_LINECODING_TWOSTOPBITS                                                         2

// No parity checking.
#define USBS_CDC_LINECODING_NOPARITY                                                            0
// Odd parity checking.
#define USBS_CDC_LINECODING_ODDPARITY                                                           1
// Even parity checking.
#define USBS_CDC_LINECODING_EVENPARITY                                                          2
// Mark parity checking.
#define USBS_CDC_LINECODING_MARKPARITY                                                          3
// Space parity checking.
#define USBS_CDC_LINECODING_SPACEPARITY                                                 4

//general information
typedef struct inf_usb_serial_st {
   int desc_rd;
   int desc_wr;
   tmr_t dev_usbserial_timer;
   rttmr_attr_t * dev_usbserial_timer_attr;
   struct termios ttyusb_termios;
   char inter_char_timer;
   usbs_serial_linecoding_t usbs_serial_linecoding;
   unsigned char line_state;
}inf_usb_serial_t;

#endif /*_USBCLASS_SERIAL_H_*/
