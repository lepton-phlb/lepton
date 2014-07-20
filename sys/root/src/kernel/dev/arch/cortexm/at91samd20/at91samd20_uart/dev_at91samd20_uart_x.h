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
#ifndef __DEV_AT91SAMD20_UART_X_H__
#define __DEV_AT91SAMD20_UART_X_H__


/*============================================
| Includes
==============================================*/


/*============================================
| Declaration
==============================================*/
#define MAX_AT91SAMD20_RX_BUFFER 64
#define MAX_AT91SAMD20_TX_BUFFER 64

typedef struct board_at91samd20_uart_info_st {
   struct usart_module usart_instance;
   
   //lepton
   uchar8_t com_no; 
   //
   int desc_r;
   int desc_w;
   
   //
   uint16_t rx_buffer_r;
   uint16_t rx_buffer_w;
   //
   uint16_t tx_buffer_r;
   uint16_t tx_buffer_w;
   //
   uchar8_t internal_buffer_rx[MAX_AT91SAMD20_RX_BUFFER];
   uchar8_t internal_buffer_tx[MAX_AT91SAMD20_TX_BUFFER];
   
   
} board_at91samd20_uart_info_t;


#endif //__DEV_STM32F4XX_UART_H__
