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
#ifndef _DEV_AT91_COMMON_USB_X_H
#define _DEV_AT91_COMMON_USB_X_H

/*============================================
| Includes
==============================================*/


/*============================================
| Declaration
==============================================*/
enum USB_offset_register {
   UDP_FRM_NUM    = 0x0000,
   UDP_GLB_STAT   = UDP_FRM_NUM + 0x0001,    //0x0004
   UDP_FADDR      = UDP_FRM_NUM + 0x0002,    //0x0008
   UDP_IER        = UDP_FRM_NUM + 0x0004,    //0x0010
   UDP_IDR        = UDP_FRM_NUM + 0x0005,    //0x0014
   UDP_IMR        = UDP_FRM_NUM + 0x0006,    //0x0018
   UDP_ISR        = UDP_FRM_NUM + 0x0007,    //0x001C
   UDP_ICR        = UDP_FRM_NUM + 0x0008,    //0x0020
   UDP_RST_EP     = UDP_FRM_NUM + 0x000a,    //0x0028

   UDP_CSRx       = UDP_FRM_NUM + 0x000c,   //0x000b, //0x002c
   UDP_FDRx       = UDP_FRM_NUM + 0x0014,   //0x0010, //0x004c

   UDP_TXVC       = UDP_FRM_NUM + 0x001d,    //0x0074
};

//IRQ
#define  UDP_EPINT0     ((unsigned int) 0x1 <<  0) // (UDP) Endpoint 0 Interrupt
#define  UDP_EPINT1     ((unsigned int) 0x1 <<  1) // (UDP) Endpoint 0 Interrupt
#define  UDP_EPINT2     ((unsigned int) 0x1 <<  2) // (UDP) Endpoint 2 Interrupt
#define  UDP_EPINT3     ((unsigned int) 0x1 <<  3) // (UDP) Endpoint 3 Interrupt
#define  UDP_EPINT4     ((unsigned int) 0x1 <<  4) // (UDP) Endpoint 4 Interrupt
#define  UDP_EPINT5     ((unsigned int) 0x1 <<  5) // (UDP) Endpoint 5 Interrupt
#define  UDP_RXSUSP     ((unsigned int) 0x1 <<  8) // (UDP) USB Suspend Interrupt
#define  UDP_RXRSM      ((unsigned int) 0x1 <<  9) // (UDP) USB Resume Interrupt
#define  UDP_EXTRSM     ((unsigned int) 0x1 << 10) // (UDP) USB External Resume Interrupt
#define  UDP_SOFINT     ((unsigned int) 0x1 << 11) // (UDP) USB Start Of frame Interrupt
#define  UDP_ENDBUSRES  ((unsigned int) 0x1 << 12) // (UDP) USB End Of Bus Reset Interrupt
#define  UDP_WAKEUP     ((unsigned int) 0x1 << 13) // (UDP) USB Resume Interrupt

#define AT91_UDP_ALLOWED_IRQs \
   ( UDP_WAKEUP | UDP_ENDBUSRES | UDP_EXTRSM | \
     UDP_RXRSM | UDP_RXSUSP | UDP_EPINT0 | \
     UDP_EPINT1 | UDP_EPINT2 | UDP_EPINT3 )


#define pCSRn(__base__, __no__)   (__base__ + __no__)
#define pFDRn(__base__, __no__)   (__base__ + __no__)

//
#ifndef MIN
   #define MIN(__a__,__b__) (((__a__) < (__b__)) ? (__a__) : (__b__))
#endif

#define     USB_FALSE      0
#define     USB_TRUE       1

//
#define THERE_IS_A_NEW_PACKET_IN_THE_UDP 0xffff

///

//from install/cyg/hal/hal_io.h
#define USBDP_READ_UINT8( __register__, __value__ ) \
   ((__value__) = *((volatile unsigned char *)(__register__)))

#define USBDP_WRITE_UINT8( __register__, __value__ ) \
   (*((volatile unsigned char *)(__register__)) = (__value__))

#define USBDP_READ_UINT32( __register__, __value__ ) \
   ((__value__) = *((volatile unsigned int *)(__register__)))

#define USBDP_WRITE_UINT32( __register__, __value__ ) \
   (*((volatile unsigned int *)(__register__)) = (__value__))

//from install/cyg/hal/var_io.h
// Given a pin description, determine which PIO controller it is on
#define USBDP_PIO_CTRL(__pin__) ((__pin__ >> 16) & 0xff)

// Given a pin description, determine which PIO bit controls this pin
#define USBDP_PIO_BIT(__pin__) (1 << (__pin__ & 0xff))

//from $(ECOS_REPOSITORY)/devs/usb/at91/current/src/bitsops.h
//eCos types are define in install/include/cyg/infra/cyg_type.h
// Set the given bits in a device register
#define SET_BITS(__register__, __bits__) { \
   unsigned int _value_; \
   USBDP_READ_UINT32(__register__, _value_); \
   _value_ |= __bits__; \
   USBDP_WRITE_UINT32(__register__, _value_); \
}

// Clear the given bits in a device register
#define CLEAR_BITS(__register__, __bits__) { \
   unsigned int _value_; \
   USBDP_READ_UINT32(__register__, _value_); \
   _value_ &= ~__bits__; \
   USBDP_WRITE_UINT32(__register__, _value_); \
}

//
#define BITS_ARE_SET(__register__, __bits__) bits_are_set(__register__, __bits__)
#define BITS_ARE_CLEARED(__register__, __bits__) bits_are_cleared(__register__, __bits__)

//
static inline int bits_are_set (unsigned int addr, unsigned int bits) {
   unsigned int read;
   USBDP_READ_UINT32 (addr, read);
   return (read & bits) == bits;
}

//
static inline int bits_are_cleared (unsigned int addr, unsigned int bits) {
   unsigned int read;
   USBDP_READ_UINT32 (addr, read);
   return (read | ~bits) == ~bits;
}

#endif //_DEV_AT91_COMMON_USB_X_H
