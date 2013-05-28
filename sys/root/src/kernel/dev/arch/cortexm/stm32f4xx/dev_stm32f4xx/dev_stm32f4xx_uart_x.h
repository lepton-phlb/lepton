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
#ifndef __DEV_STM32F4XX_UART_H__
#define __DEV_STM32F4XX_UART_H__


/*============================================
| Includes
==============================================*/
#include "kernel/dev/arch/cortexm/stm32f4xx/target.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/gpio.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/uart.h"

/*============================================
| Declaration
==============================================*/

typedef struct board_stm32f4xx_uart_info_st {
   unsigned int com_no; 
   //
   int desc_r;
   int desc_w;
   //
   _Uart_Ctrl uart_ctrl;
   _Uart_Descriptor uart_descriptor;
   
   //
   kernel_pthread_mutex_t mutex;
} board_stm32f4xx_uart_info_t;

#endif //__DEV_STM32F4XX_UART_H__
