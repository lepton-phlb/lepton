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
#ifndef __MANAGE_ORDER_H__
#define __MANAGE_ORDER_H__

/*============================================
| Includes
==============================================*/
#include <cyg/kernel/kapi.h>
#include <string.h>
#include <cyg/hal/hal_io.h>

/*============================================
| Declaration
==============================================*/
typedef unsigned int u_int32;

typedef struct {
   unsigned char sens;      //'R' ou 'W'
   u_int32 reg;
   u_int32 value;
} ecos_virtual_order;


//définition des num de registres
enum enum_registers {
   AIC_IS = 0x00000000,      //Interrupt source Registers
   US_CR,      //USART0 control register (1)
   US_MR,      //USART0 mode register (2)
   US_IER,      //USART0 interrupt enable register (3)
   US_IDR,      //USART0 interrupt disable register (4)
   US_IMR,      //USART0 interrupt mask register (5)
   US_CSR,      //USART0 channel status register (6)
   US_RHR,      //USART0 receiver holding register (7)
   US_THR,      //USART0 transmitter holding register (8)
   US_BRGR,      //USART baudrate generator register (9)
   US_RTOR,
   US_TTGR,
   NB_REG
};

/*gestion ordre*/
int send_order_w(unsigned char s, u_int32 reg, u_int32 val);
int send_order_r(unsigned char s, u_int32 reg, u_int32 * val);

#endif
/*
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.1  2009/03/30 11:16:09  jjp
| First import of tauon
|
|---------------------------------------------*/
