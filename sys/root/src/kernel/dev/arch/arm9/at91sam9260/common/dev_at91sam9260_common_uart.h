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
#ifndef _DEV_AT91SAM9260_COMMON_UART_H_
#define _DEV_AT91SAM9260_COMMON_UART_H_

/*============================================
| Includes
==============================================*/
#include "dev_at91sam9260_common.h"

/*============================================
| Declaration
==============================================*/

enum PIO_CONTROLLER_NUMBER
{
   PIO_CONTROLLER_A=0,
   PIO_CONTROLLER_B,
   PIO_CONTROLLER_C
};

enum PIO_PERIPHERAL_NUMBER
{
   PIO_PERIPHERAL_A=0,
   PIO_PERIPHERAL_B
};


typedef struct s2s {
   speed_t ts;
   long ns;
}s2s_t;

// Pool buffer
typedef struct {
   uint16_t cb;
   uchar8_t *p;
}fifo_buf_pool_t;


#define OS_RS232_BAUDRATE 9600L

#endif /*DEV_ATSAM9260_COMMON_UART_H_*/

/*============================================
| End of Source  : dev_at91sam9260_common_uart.h
==============================================*/

