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
#ifndef _DEV_AT91SAM9260_UART_X_H
#define _DEV_AT91SAM9260_UART_X_H

#include "kernel/dev/arch/arm9/at91sam9260/common/dev_at91sam9260_common_uart.h"

/*============================================
| Includes
==============================================*/
#include "cyg/hal/at91sam9260.h"
#include "kernel/core/core_rttimer.h"

/*============================================
| Declaration
==============================================*/
//input fifo multibuffering
#define MAX_POOL        16
#define MAX_POOL_BUF_SZ 512
#define UART_FIFO_INPUT_BUFFER_SZ (MAX_POOL*MAX_POOL_BUF_SZ)

//output buffer
#define UART_OUTPUT_BUFFER_SIZE 400

// Mask for interrupt
#define IER_MASK_RCV     0x00000021
#define IER_MASK_SND     0x00000002

#define US_STTTO         0x0800      /* Start Time-out */

// Uart Generic base Information for RS232
//
typedef struct{
   AT91_REG        *base_adr;
   unsigned char   periph_id;   // peripheric ID

   int   ind_pio_rxd;
   int   ind_pio_txd;

   int             o_flag;
   desc_t          desc_r;
   desc_t          desc_w;

   void            (* f_timer_call_back)(void);
   int             loaded;

   kernel_pthread_mutex_t   mutex;

   fifo_buf_pool_t fifo_buf_pool[MAX_POOL];
   char            fifo_input_buffer[UART_FIFO_INPUT_BUFFER_SZ+1];
   volatile int             buf_in_dma_no;
   volatile int             buf_in_rcv_no;
   char            output_buffer[UART_OUTPUT_BUFFER_SIZE+1];
   volatile signed int      input_r;
   volatile signed int      input_w;
   volatile signed int      output_r;
   volatile signed int      output_w;
   desc_t          desc_rd;
   desc_t          desc_wr;

   //VTIME timer in units of 0.1 seconds (posix specification).
   tmr_t timer;
   rttmr_attr_t timer_attr;
   char            inter_char_timer;
   unsigned char   XMIT;
	int rcv_flag;
   struct termios ttys_termios;

}board_inf_uart_t;

/*============================================
| Prototypes
==============================================*/
extern void dev_at91sam9260_uart_x_fifo_rcv(desc_t desc);
extern void dev_at91sam9260_uart_x_snd (desc_t desc);
extern int dev_at91sam9260_uart_x_load(board_inf_uart_t *);
extern int dev_at91sam9260_uart_x_open(desc_t, int );
extern int dev_at91sam9260_uart_x_close(desc_t);
extern int dev_at91sam9260_uart_x_isset_read(desc_t);
extern int dev_at91sam9260_uart_x_isset_write(desc_t);
extern int dev_at91sam9260_uart_x_read(desc_t, char *,int);
extern int dev_at91sam9260_uart_x_write(desc_t, const char *, int);
extern int dev_at91sam9260_uart_x_seek(desc_t, int, int);
extern int dev_at91sam9260_uart_x_ioctl(desc_t, int, va_list);
extern int dev_at91sam9260_uart_x_interrupt(desc_t);

#endif

/*============================================
| End of Source  : dev_at91sam9260_uart_x.h
==============================================*/
