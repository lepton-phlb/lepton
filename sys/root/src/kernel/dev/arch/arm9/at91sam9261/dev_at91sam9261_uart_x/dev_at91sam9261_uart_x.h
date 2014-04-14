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
#ifndef _DEV_AT91SAM9261_UART_X_H
#define _DEV_AT91SAM9261_UART_X_H

#include "kernel/dev/arch/arm9/at91sam9261/common/dev_at91sam9261_common_uart.h"

/*============================================
| Includes
==============================================*/
#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)
   #include <atmel/ioat91sam9261.h>
#else
   #include "cyg/hal/at91sam9261.h"
#endif

#include "kernel/core/core_rttimer.h"
/*============================================
| Declaration
==============================================*/
//input fifo multibuffering
#define MAX_POOL        16
#define MAX_POOL_BUF_SZ 2048
#define UART_FIFO_INPUT_BUFFER_SZ (MAX_POOL*MAX_POOL_BUF_SZ)

//output buffer --> SLIP Mode
#define UART_OUTPUT_BUFFER_SIZE 2048

// Mask for interrupt
#define IER_MASK_RCV     0x00000128
#define IER_MASK_SND     0x00000020 //0x00000022

#define US_STTTO         0x0800      /* Start Time-out */

#define DEBUG_MODE       1
#define NO_DEBUG_MODE    0

//
// Use DMA for sending buffer
//#define USE_DMA_UART_SND   0
#define USE_DMA_UART_SND
//#undef USE_DMA_UART_SND
//
// Uart Generic base Information for RS232
//
typedef struct {
   AT91_REG        *base_adr;
   unsigned char periph_id;     // peripheric ID

   unsigned char ind_pio_rxd;
   unsigned char ind_pio_txd;

   int o_flag;
   desc_t desc_r;
   desc_t desc_w;

   int flag_i_int; //flag input
   int flag_o_int; //flag output

   void (* f_timer_call_back)(void);
   int loaded;

   kernel_pthread_mutex_t mutex;

   fifo_buf_pool_t fifo_buf_pool[MAX_POOL];
   char fifo_input_buffer[UART_FIFO_INPUT_BUFFER_SZ+1];
   volatile int buf_in_dma_no;
   volatile int buf_in_rcv_no;
   char output_buffer[UART_OUTPUT_BUFFER_SIZE+1];
   volatile signed int input_r;
   volatile signed int input_w;
   volatile signed int output_r;
   volatile signed int output_w;
   desc_t desc_rd;
   desc_t desc_wr;

   //VTIME timer in units of 0.1 seconds (posix specification).
#if defined(__KERNEL_UCORE_EMBOS)
   OS_TIMER timer;
#elif defined(__KERNEL_UCORE_ECOS) || defined(__KERNEL_UCORE_FREERTOS)
   tmr_t timer;
   rttmr_attr_t timer_attr;
#endif
   char inter_char_timer;
   struct termios ttys_termios;
   unsigned char XMIT;

   unsigned char flag_overrun;
}board_inf_uart_t;

#endif
