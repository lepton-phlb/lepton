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
#ifndef _USBCORE_H_
#define _USBCORE_H_
/*============================================
| Includes
==============================================*/


/*============================================
| Declaration
==============================================*/
//This file comes from $(ECOS_REPOSITORY)/io/usb/common/current/include/usb.h

// USB device requests, the setup packet.
//
// The structure is defined entirely in terms of bytes, eliminating
// any confusion about who is supposed to swap what when. This avoids
// endianness-related portability problems, and eliminates any need
// to worry about alignment. Also for some requests the value field
// is split into separate bytes anyway.
typedef struct usb_devreq {
   unsigned char type;
   unsigned char request;
   unsigned char value_lo;
   unsigned char value_hi;
   unsigned char index_lo;
   unsigned char index_hi;
   unsigned char length_lo;
   unsigned char length_hi;
} __attribute__((packed)) usb_devreq;

// Encoding of the request_type
#define USB_DEVREQ_DIRECTION_OUT        0
#define USB_DEVREQ_DIRECTION_IN         (1 << 7)
#define USB_DEVREQ_DIRECTION_MASK       (1 << 7)

#define USB_DEVREQ_TYPE_STANDARD        0
#define USB_DEVREQ_TYPE_CLASS           (0x1 << 5)
#define USB_DEVREQ_TYPE_VENDOR          (0x2 << 5)
#define USB_DEVREQ_TYPE_RESERVED        (0x3 << 5)
#define USB_DEVREQ_TYPE_MASK            (0x3 << 5)

#define USB_DEVREQ_RECIPIENT_DEVICE     0x00
#define USB_DEVREQ_RECIPIENT_INTERFACE  0x01
#define USB_DEVREQ_RECIPIENT_ENDPOINT   0x02
#define USB_DEVREQ_RECIPIENT_OTHER      0x03
#define USB_DEVREQ_RECIPIENT_MASK       0x1F

// The standard request codes.
#define USB_DEVREQ_GET_STATUS            0
#define USB_DEVREQ_CLEAR_FEATURE         1
#define USB_DEVREQ_SET_FEATURE           3
#define USB_DEVREQ_SET_ADDRESS           5
#define USB_DEVREQ_GET_DESCRIPTOR        6
#define USB_DEVREQ_SET_DESCRIPTOR        7
#define USB_DEVREQ_GET_CONFIGURATION     8
#define USB_DEVREQ_SET_CONFIGURATION     9
#define USB_DEVREQ_GET_INTERFACE        10
#define USB_DEVREQ_SET_INTERFACE        11
#define USB_DEVREQ_SYNCH_FRAME          12

// Descriptor types. These are placed in value_hi for the
// GET_DESCRIPTOR and SET_DESCRIPTOR requests, with an index
// in value_lo. They also go into the type fields of the
// various descriptor structures.
#define USB_DEVREQ_DESCRIPTOR_TYPE_DEVICE               1
#define USB_DEVREQ_DESCRIPTOR_TYPE_CONFIGURATION        2
#define USB_DEVREQ_DESCRIPTOR_TYPE_STRING               3
#define USB_DEVREQ_DESCRIPTOR_TYPE_INTERFACE            4
#define USB_DEVREQ_DESCRIPTOR_TYPE_ENDPOINT             5
#define USB_DEVREQ_DESCRIPTOR_TYPE_QUALIFIER            6

// Feature selectors. These go into value_lo for the CLEAR_FEATURE and
// SET_FEATURE requests, and in the first response byte for
// GET_STATUS.
#define USB_DEVREQ_FEATURE_DEVICE_REMOTE_WAKEUP         1
#define USB_DEVREQ_FEATURE_ENDPOINT_HALT                0

// Index decoding. When the CLEAR_FEATURE, SET_FEATURE and GET_STATUS
// requests is applied to an endpoint (as per the recipient field in
// the type field) index_lo identifies the endpoint.
#define USB_DEVREQ_INDEX_DIRECTION_OUT                  0
#define USB_DEVREQ_INDEX_DIRECTION_IN                   (1 << 7)
#define USB_DEVREQ_INDEX_DIRECTION_MASK                 (1 << 7)
#define USB_DEVREQ_INDEX_ENDPOINT_MASK                  0x0F

// Descriptors for the GET_DESCRIPTOR and SET_DESCRIPTOR requests.
typedef struct usb_device_descriptor {
   unsigned char length;                        // USB_DEVICE_DESCRIPTOR_LENGTH == 18
   unsigned char type;                          // USB_DEVREQ_DESCRIPTOR_TYPE
   unsigned char usb_spec_lo;
   unsigned char usb_spec_hi;
   unsigned char device_class;
   unsigned char device_subclass;
   unsigned char device_protocol;
   unsigned char max_packet_size;
   unsigned char vendor_lo;
   unsigned char vendor_hi;
   unsigned char product_lo;
   unsigned char product_hi;
   unsigned char device_lo;
   unsigned char device_hi;
   unsigned char manufacturer_str;
   unsigned char product_str;
   unsigned char serial_number_str;
   unsigned char number_configurations;
} __attribute__((packed)) usb_device_descriptor;

#define USB_DEVICE_DESCRIPTOR_LENGTH             18
#define USB_DEVICE_DESCRIPTOR_TYPE               USB_DEVREQ_DESCRIPTOR_TYPE_DEVICE
//for usb 1.1
#define USB_DEVICE_DESCRIPTOR_USB11_LO           0x10
#define USB_DEVICE_DESCRIPTOR_USB11_HI           0x01
//for usb 2.0
#define USB_DEVICE_DESCRIPTOR_USB20_LO           0x00
#define USB_DEVICE_DESCRIPTOR_USB20_HI           0x02

#define USB_DEVICE_DESCRIPTOR_CLASS_INTERFACE    0x00
#define USB_DEVICE_DESCRIPTOR_CLASS_VENDOR       0x00FF
#define USB_DEVICE_DESCRIPTOR_SUBCLASS_INTERFACE 0x00
#define USB_DEVICE_DESCRIPTOR_SUBCLASS_VENDOR    0x00FF
#define USB_DEVICE_DESCRIPTOR_PROTOCOL_INTERFACE 0x00
#define USB_DEVICE_DESCRIPTOR_PROTOCOL_VENDOR    0x00FF

//
typedef struct usb_qualifier_descriptor {
   unsigned char length;     // Size of the descriptor in bytes.
   unsigned char type; // Descriptor type (USBDESC_DEVICE_QUALIFIER or "USB device types").
   unsigned char bcd_low; // USB specification release number (in BCD format).
   unsigned char bcd_high;
   unsigned char device_class; // Device class code.
   unsigned char device_subclass; // Device subclass code.
   unsigned char device_protocol; // Device protocol code.
   unsigned char max_packet_size; // Maximum packet size of endpoint 0.
   unsigned char number_configurations; /// Number of possible configurations for the device.
   unsigned char reserved; // Reserved.
} __attribute__((packed)) usb_qualifier_descriptor;

#define USB_QUALIFIER_DESCRIPTOR_LENGTH     10
#define USB_QUALIFIER_DESCRIPTOR_TYPE       USB_DEVREQ_DESCRIPTOR_TYPE_QUALIFIER
//
typedef struct usb_configuration_descriptor {
   unsigned char length;
   unsigned char type;
   unsigned char total_length_lo;
   unsigned char total_length_hi;
   unsigned char number_interfaces;
   unsigned char configuration_id;
   unsigned char configuration_str;
   unsigned char attributes;
   unsigned char max_power;
} __attribute__((packed)) usb_configuration_descriptor;

#define USB_CONFIGURATION_DESCRIPTOR_LENGTH     9
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       USB_DEVREQ_DESCRIPTOR_TYPE_CONFIGURATION
#define USB_CONFIGURATION_DESCRIPTOR_ATTR_REQUIRED      (1 << 7)
#define USB_CONFIGURATION_DESCRIPTOR_ATTR_SELF_POWERED  (1 << 6)
#define USB_CONFIGURATION_DESCRIPTOR_ATTR_REMOTE_WAKEUP (1 << 5)

///NOW WE DON'T USE INTERFACE AND ENDPOINT DESCRIPTOR
///WE PREFER unsigned char array BECAUSE OF Functionnal Header for CDC/ACM
typedef unsigned char usb_interface_descriptor;
#define  USB_INTERFACE_DESCRIPTOR(lenght,type,interface_id,alternate_setting,number_endpoint, \
                                  interface_class,interface_subclass,interface_protocol, \
                                  interface_str) \
   lenght,type,interface_id,alternate_setting,number_endpoint,interface_class,interface_subclass, \
   interface_protocol,interface_str

//offset
#define  INTERFACE_LENGHT_OFFSET                0
#define  INTERFACE_TYPE_OFFSET                  (INTERFACE_LENGHT_OFFSET+1)
#define  INTERFACE_INTERFACEID_OFFSET           (INTERFACE_TYPE_OFFSET+1)
#define  INTERFACE_ALTERSETTING_OFFSET          (INTERFACE_INTERFACEID_OFFSET+1)
#define  INTERFACE_NUMEP_OFFSET                 (INTERFACE_ALTERSETTING_OFFSET+1)
#define  INTERFACE_INTERCLASS_OFFSET            (INTERFACE_NUMEP_OFFSET+1)
#define  INTERFACE_INTERSUBCLASS_OFFSET         (INTERFACE_INTERCLASS_OFFSET+1)
#define  INTERFACE_PROTOCOL_OFFSET              (INTERFACE_INTERSUBCLASS_OFFSET+1)
#define  INTERFACE_STR_OFFSET                   (INTERFACE_PROTOCOL_OFFSET+1)
//
#define USB_INTERFACE_DESCRIPTOR_LENGTH          9
#define USB_INTERFACE_DESCRIPTOR_TYPE            USB_DEVREQ_DESCRIPTOR_TYPE_INTERFACE
#define USB_INTERFACE_DESCRIPTOR_CLASS_VENDOR    0x00FF
#define USB_INTERFACE_DESCRIPTOR_SUBCLASS_VENDOR 0x00FF
#define USB_INTERFACE_DESCRIPTOR_PROTOCOL_VENDOR 0x00FF

////
typedef struct usb_endpoint_descriptor {
   unsigned char length;
   unsigned char type;
   unsigned char endpoint;
   unsigned char attributes;
   unsigned char max_packet_lo;
   unsigned char max_packet_hi;
   unsigned char interval;
} __attribute__((packed)) usb_endpoint_descriptor;

#define  USB_ENDPOINT_DESCRIPTOR(lenght,type,endpoint,attributes,max_packet_lo,max_packet_hi, \
                                 interval) \
   lenght,type,endpoint,attributes,max_packet_lo,max_packet_hi,interval

//offset
#define  ENDPOINT_LENGHT_OFFSET                 0
#define  ENDPOINT_TYPE_OFFSET                   (ENDPOINT_LENGHT_OFFSET+1)
#define  ENDPOINT_ENDPOINT_OFFSET               (ENDPOINT_TYPE_OFFSET+1)
#define  ENDPOINT_ATTRIBUTES_OFFSET             (ENDPOINT_ENDPOINT_OFFSET+1)
#define  ENDPOINT_MAXPACKETLO_OFFSET            (ENDPOINT_ATTRIBUTES_OFFSET+1)
#define  ENDPOINT_MAXPACKETHI_OFFSET            (ENDPOINT_MAXPACKETHI_OFFSET+1)
#define  ENDPOINT_INTERVAL_OFFSET               (ENDPOINT_MAXPACKETHI_OFFSET+1)
//

#define USB_ENDPOINT_DESCRIPTOR_LENGTH           7
#define USB_ENDPOINT_DESCRIPTOR_TYPE             USB_DEVREQ_DESCRIPTOR_TYPE_ENDPOINT
#define USB_ENDPOINT_DESCRIPTOR_ENDPOINT_OUT     0
#define USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN      (1 << 7)
#define USB_ENDPOINT_DESCRIPTOR_ATTR_CONTROL     0x00
#define USB_ENDPOINT_DESCRIPTOR_ATTR_ISOCHRONOUS 0x01
#define USB_ENDPOINT_DESCRIPTOR_ATTR_BULK        0x02
#define USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT   0x03

// String descriptors. If these are used at all then string 0
// must be a table of supported LANGID codes. For a simple device
// which only supports US English, the following sequence of
// four bytes should suffice for string 0. In practice string
// constants tend to be used which makes the use of these
// #define's difficult.
#define USB_STRING_DESCRIPTOR_STRING0_LENGTH    4
#define USB_STRING_DESCRIPTOR_STRING0_TYPE      USB_DEVREQ_DESCRIPTOR_TYPE_STRING
#define USB_STRING_DESCRIPTOR_STRING0_LANGID_LO 0x09
#define USB_STRING_DESCRIPTOR_STRING0_LANGID_HI 0x04

// For subsequent strings the length and data will have to be
// determined by the application developer or by a suitable tool.
#define USB_STRING_DESCRIPTOR_TYPE              USB_DEVREQ_DESCRIPTOR_TYPE_STRING

// Utility macros to calculate the total_length fields in a
// configuration descriptor.
#define USB_CONFIGURATION_DESCRIPTOR_TOTAL_LENGTH_LO(interfaces, endpoints) \
   (USB_CONFIGURATION_DESCRIPTOR_LENGTH +            \
    (interfaces * USB_INTERFACE_DESCRIPTOR_LENGTH) + \
    (endpoints  * USB_ENDPOINT_DESCRIPTOR_LENGTH)) % 256

#define USB_CONFIGURATION_DESCRIPTOR_TOTAL_LENGTH_HI(interfaces, endpoints) \
   (USB_CONFIGURATION_DESCRIPTOR_LENGTH +            \
    (interfaces * USB_INTERFACE_DESCRIPTOR_LENGTH) + \
    (endpoints  * USB_ENDPOINT_DESCRIPTOR_LENGTH)) / 256

//
//Atmel Vendor (0x03EB)
#define USB_DEVICE_DESCRIPTOR_VENDOR_LO               0xEB
#define USB_DEVICE_DESCRIPTOR_VENDOR_HI               0x03

//Atmel Product ID (0x6119)
#define USB_DEVICE_DESCRIPTOR_PRODUCT_LO              0x19
#define USB_DEVICE_DESCRIPTOR_PRODUCT_HI              0x61

//Version 1.0
#define USB_DEVICE_DESCRIPTOR_DEVICE_LO               0x00
#define USB_DEVICE_DESCRIPTOR_DEVICE_HI               0x01

//
//from atmel see USBStringDescriptor.h
// Language ID for US english.
#define USB_STRING_DESC_ENGLISH_US                                      0x09, 0x04
#define USB_STRING_DESC_LENGTH(length)                                          ((length) * 2 + 2)
#define USB_STRING_DESC_UNICODE(ascii)                                          (ascii), 0

#if defined(__KERNEL_UCORE_ECOS)
//for true and false
   #include <cyg/hal/hal_io.h>
#endif

#endif /*_USBCORE_H_*/
