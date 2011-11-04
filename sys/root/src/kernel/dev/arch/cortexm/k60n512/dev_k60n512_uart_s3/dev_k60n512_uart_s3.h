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
#ifndef __DEV_CORTEXM_UART_S3_H__
#define __DEV_CORTEXM_UART_S3_H__


/*============================================
| Includes 
==============================================*/

/*============================================
| Declaration  
==============================================*/
enum {
    REG_UART_BDH,         // UART Baud Rate Register High
    REG_UART_BDL,         // UART Baud Rate Register Low
    REG_UART_C1,          // UART Control Register 1
    REG_UART_C2,          // UART Control Register 2
    REG_UART_S1,          // UART Status Register 1
    REG_UART_S2,          // UART Status Register 2
    REG_UART_C3,          // UART Control Register 3
    REG_UART_D,           // UART Data Register
    REG_UART_MA1,         // UART Match Address Registers 1
    REG_UART_MA2,         // UART Match Address Registers 2
    REG_UART_C4,          // UART Control Register 4
    REG_UART_C5,          // UART Control Register 5
    REG_UART_ED,          // UART Extended Data Register
    REG_UART_MODEM,       // UART Modem Register
    REG_UART_IR,          // UART Infrared Register
    REG_UART_Res_0,
    REG_UART_PFIFO,       // UART FIFO Parameters
    REG_UART_CFIFO,       // UART FIFO Control Register
    REG_UART_SFIFO,       // UART FIFO Status Register
    REG_UART_TWFIFO,      // UART FIFO Transmit Watermark
    REG_UART_TCFIFO,      // UART FIFO Transmit Count
    REG_UART_RWFIFO,      // UART FIFO Receive Watermark
    REG_UART_RCFIFO,      // UART FIFO Receive Count
    REG_UART_Res_1,
    REG_UART_C7816,       // UART 7816 Control Register
    REG_UART_IE7816,      // UART 7816 Interrupt Enable Register
    REG_UART_IS7816,      // UART 7816 Interrupt Status Register
    REG_UART_WP7816_T0T1, // UART 7816 Wait Parameter Register
    REG_UART_WN7816,      // UART 7816 Wait N Register
    REG_UART_WF7816,      // UART 7816 Wait FD Register
    REG_UART_ET7816,      // UART 7816 Error Threshold Register
    REG_UART_TL7816       // UART 7816 Transmit Length Register
};

//
#define REG_UART_C1_LOOPS     (0x80)
#define REG_UART_C1_UARTSWAI  (0x40)
#define REG_UART_C1_RSRC      (0x20)
#define REG_UART_C1_M         (0x10)
#define REG_UART_C1_WAKE      (0x08)
#define REG_UART_C1_ILT       (0x04)
#define REG_UART_C1_PE        (0x02)
#define REG_UART_C1_PT        (0x01)

#define REG_UART_C2_TIE       (0x80)
#define REG_UART_C2_TCIE      (0x40)
#define REG_UART_C2_RIE       (0x20)
#define REG_UART_C2_ILIE      (0x10)
#define REG_UART_C2_TE        (0x08)
#define REG_UART_C2_RE        (0x04)
#define REG_UART_C2_RWU       (0x02)
#define REG_UART_C2_SBK       (0x01)

#define REG_UART_S1_TDRE      (0x80)
#define REG_UART_S1_TC        (0x40)
#define REG_UART_S1_RDRF      (0x20)
#define REG_UART_S1_IDLE      (0x10)
#define REG_UART_S1_OR        (0x08)
#define REG_UART_S1_NF        (0x04)
#define REG_UART_S1_FE        (0x02)
#define REG_UART_S1_PF        (0x01)

#define REG_UART_S2_LBKDIF    (0x80)
#define REG_UART_S2_RXEDGIF   (0x40)
#define REG_UART_S2_MSBF      (0x20)
#define REG_UART_S2_RXINV     (0x10)
#define REG_UART_S2_RWUID     (0x08)
#define REG_UART_S2_BRK13     (0x04)
#define REG_UART_S2_LBKDE     (0x02)
#define REG_UART_S2_RAF       (0x01)

#define REG_UART_C3_R8        (0x80)
#define REG_UART_C3_T8        (0x40)
#define REG_UART_C3_TXDIR     (0x20)
#define REG_UART_C3_TXINV     (0x10)
#define REG_UART_C3_ORIE      (0x08)
#define REG_UART_C3_NEIE      (0x04)
#define REG_UART_C3_FEIE      (0x02)
#define REG_UART_C3_PEIE      (0x01)

#endif //__DEV_CORTEXM_UART_S3_H__
