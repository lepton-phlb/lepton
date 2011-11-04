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
#ifndef _DEV_AT91_RTT_H
#define _DEV_AT91_RTT_H

/*============================================
| Includes
==============================================*/


/*============================================
| Declaration
==============================================*/
enum RTT_offset_register {
      RTT_MR      = 0x0000,
      RTT_AR      = RTT_MR + 0x0001, //0x0004
      RTT_VR      = RTT_MR + 0x0002, //0x0008
      RTT_SR      = RTT_MR + 0x0003, //0x000C
};

//merge it with USBDP_READ_* and USBDP_WRITE_*
//from install/cyg/hal/hal_io.h
#define RTT_READ_UINT8( __register__, __value__ ) \
      ((__value__) = *((volatile unsigned char *)(__register__)))

#define RTT_WRITE_UINT8( __register__, __value__ ) \
      (*((volatile unsigned char *)(__register__)) = (__value__))

#define RTT_READ_UINT32( __register__, __value__ ) \
      ((__value__) = *((volatile unsigned int *)(__register__)))

#define RTT_WRITE_UINT32( __register__, __value__ ) \
      (*((volatile unsigned int *)(__register__)) = (__value__))

//
typedef struct board_inf_rtt_st {
   volatile unsigned int * base_addr;
   volatile unsigned int * bckup_reg_addr;
} board_inf_rtt_t;


#endif //_DEV_AT91_RTT_H
