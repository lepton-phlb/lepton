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
#ifndef _DEV_AT91SAM7S_UART_SX_H
#define _DEV_AT91SAM7S_UART_SX_H

#include "pthread/pthread.h"
/*============================================
| Includes
==============================================*/
#include <intrinsic.h>

/*============================================
| Declaration
==============================================*/
//#define OS_FSYS 10000000L //to do: warning!!! OSFSYS redeclaration cpu clock = 10Mhz
#define OS_RS232_BAUDRATE 9600L
#define OS_RS232_BAUDDIVIDE ((OS_FSYS+OS_RS232_BAUDRATE*8L)/(OS_RS232_BAUDRATE*16L)-1)

//input fifo multibuffering
#define MAX_POOL        16
#define MAX_POOL_BUF_SZ 512
#define UART_FIFO_INPUT_BUFFER_SZ (MAX_POOL*MAX_POOL_BUF_SZ)

//output buffer
#define UART_OUTPUT_BUFFER_SIZE 400

// Mask for interrupt
#define IER_MASK_RCV     0x00000128
#define IER_MASK_SND     0x00000020 //0x00000022

#define US_STTTO         0x0800      /* Start Time-out */

//
// Use DMA for sending buffer
//#define USE_DMA         1
//

typedef struct {
   uint16_t cb;
   uchar8_t *p;
}fifo_buf_pool_t;


typedef struct s2s {
   speed_t ts;
   long ns;
}s2s_t;

//
// Uart Generic base Information for RS232
//
typedef struct {
   AT91_REG        *base_adr;
   unsigned char periph_id;     // peripheric ID

   unsigned char ind_pio_rxd;
   unsigned char ind_pio_txd;
   unsigned char ind_pio_rts;

   int o_flag;
   desc_t desc_r;
   desc_t desc_w;

   void (* f_timer_call_back)(void);
   int dev_at91sam7s_uart_sx_loaded;

   kernel_pthread_mutex_t mutex;

   fifo_buf_pool_t fifo_buf_pool[MAX_POOL];
   char at91sam7s_uart_fifo_input_buffer[UART_FIFO_INPUT_BUFFER_SZ+1];
   int buf_in_dma_no;
   int buf_in_rcv_no;
   char at91sam7s_uart_sx_output_buffer[UART_OUTPUT_BUFFER_SIZE+1];
   signed int at91sam7s_uart_sx_input_r;
   signed int at91sam7s_uart_sx_input_w;
   signed int at91sam7s_uart_sx_output_r;
   signed int at91sam7s_uart_sx_output_w;
   desc_t at91sam7s_uart_sx_desc_rd;
   desc_t at91sam7s_uart_sx_desc_wr;

   //VTIME timer in units of 0.1 seconds (posix specification).
   OS_TIMER dev_at91sam7s_uart_sx_timer;
   char inter_char_timer;
   unsigned char XMIT;

}board_inf_uart_t;

#endif
