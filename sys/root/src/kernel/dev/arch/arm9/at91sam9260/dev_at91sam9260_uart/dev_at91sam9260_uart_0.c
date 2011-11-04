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


/**
 * \addtogroup lepton_dev
 * @{
 *
 */

/**
 * \addtogroup hard_dev_at91sam9260
 * @{
 *
 */

/**
 * \defgroup dev_uart_arm9_c Les uart de l'arm9 at91sam9260.
 * @{
 *
 * Les pilotes de p�riph�riques pour les uart de l'arm9 at91sam9260
 * Ces pilotes de p�riph�riques g�re les vitesses de 50 � 38400 bauds
 * et supporte le timeout (VTIME) pour le mode bloquant.
 *
 */


/**
 * \file
 * implementation du pilote de p�riph�rique pour l'uart 0 de l'arm9 at91sam9260
 * \author bruno mollo
 */

/*===========================================
Includes
=============================================*/
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/cpu.h"
#include "lib/libc/termios/termios.h"
#include "kernel/fs/vfs/vfsdev.h"

// drivers
#include "dev_at91sam9260_uart_x.h"
#include "kernel/dev/arch/arm9/at91sam9260/common/dev_at91sam9260_common_uart.h"

// eCos
#include "cyg/hal/at91sam9260.h"
#include "cyg/hal/hal_platform_ints.h"
#include <string.h>
#include <stdlib.h>


/*===========================================
Global Declaration
=============================================*/
const char dev_at91sam9260_uart_0_name[]="ttys0\0";

// statics
static int dev_at91sam9260_uart_0_load(void);
static int dev_at91sam9260_uart_0_open(desc_t, int);
static void dev_at91sam9260_uart_0_interrupt(void);

desc_t desc_uart0;

static cyg_handle_t _at91sam9260_uart_0_handle;
static cyg_interrupt _at91sam9260_uart_0_it;
static board_inf_uart_t *p_board_inf_uart_0;   // ADU


// uart_0 Functions pointers
dev_map_t dev_at91sam9260_uart0_map={
   dev_at91sam9260_uart_0_name,
   S_IFCHR,
   dev_at91sam9260_uart_0_load,
   dev_at91sam9260_uart_0_open,
   dev_at91sam9260_uart_x_close,
   dev_at91sam9260_uart_x_isset_read,
   dev_at91sam9260_uart_x_isset_write,
   dev_at91sam9260_uart_x_read,
   dev_at91sam9260_uart_x_write,
   dev_at91sam9260_uart_x_seek,
   dev_at91sam9260_uart_x_ioctl
};


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name       : dev_at91sam9260_uart0_isr
| Description:
| Parameters : None
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
cyg_uint32 dev_at91sam9260_uart0_isr(cyg_vector_t vector, cyg_addrword_t data)
{
	cyg_interrupt_mask(vector);
	unsigned int usart_csr;
	unsigned int recv_intr_cond  = 0;
	unsigned int trans_intr_cond = 0;

	board_inf_uart_t *p_inf_uart;
	AT91S_USART        *p_adr;


	p_inf_uart = (board_inf_uart_t *)ofile_lst[desc_uart0].p;
	p_adr      = (AT91S_USART*)p_inf_uart->base_adr;

	// if device is loaded
	if (p_inf_uart->loaded)	{
		do {
			usart_csr = p_adr->US_CSR;

			if (p_inf_uart->o_flag & O_RDONLY) {
				// Overrun Error
				if ((usart_csr & 0x00000020) == 0x00000020) {
					// Reset status bits, reset rx/tx.
                                        p_adr->US_CR = AT91C_US_RSTSTA;
				}

				// Receiver byte per byte
				if ((usart_csr & 0x01) == 0x01) {             // RXRDY
					//receive fifo
					dev_at91sam9260_uart_x_fifo_rcv(desc_uart0);
				}
                                recv_intr_cond  = usart_csr & (p_adr->US_IMR) & (0x00000001);
			}
			//
			if (p_inf_uart->o_flag & O_WRONLY) {
				// TXRDY
				if (((usart_csr & 0x00000002) == 0x00000002) && (p_inf_uart->XMIT)) {
					//Transmission (per byte)
					dev_at91sam9260_uart_x_snd(desc_uart0);
				}
            trans_intr_cond = ( (p_inf_uart->XMIT) && (usart_csr & (p_adr->US_IMR) & (0x00000002)));
			}
		} while ( recv_intr_cond || trans_intr_cond );
	}
	// Timer Value initialization for Read
	p_inf_uart->inter_char_timer = p_inf_uart->ttys_termios.c_cc[VTIME];

	//ACK de l'IT au CPU
	cyg_interrupt_acknowledge(vector);
	//Informe kernel d'ex�cuter DSR
	return(CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}


/*-------------------------------------------
| Name       : dev_at91sam9260_uart0_dsr
| Description:
| Parameters : None
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
void dev_at91sam9260_uart0_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
	board_inf_uart_t * p_inf_uart = (board_inf_uart_t *)ofile_lst[desc_uart0].p;

	if(p_inf_uart->rcv_flag) {
		__fire_io_int(ofile_lst[p_inf_uart->desc_rd].owner_pthread_ptr_read);
		p_inf_uart->rcv_flag = 0;
	}
	if ( (p_inf_uart->desc_wr  >= 0) && (p_inf_uart->output_r == p_inf_uart->output_w) ) {
		__fire_io_int(ofile_lst[p_inf_uart->desc_wr].owner_pthread_ptr_write);
	}
	//autorise � nouveau les IT
	cyg_interrupt_unmask(vector);
}


/*-------------------------------------------
| Name       : dev_at91sam9260_uart_0_load
| Description: Call generic load function
| Parameters : None
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam9260_uart_0_load (void)
{
   p_board_inf_uart_0  = (board_inf_uart_t *)malloc(sizeof(board_inf_uart_t));
   p_board_inf_uart_0->loaded = 0; // Not loaded

   // Disable all USART interrupts.
   *(AT91C_US0_IDR) = 0xffffffff;

   return dev_at91sam9260_uart_x_load(p_board_inf_uart_0);
}


/*-------------------------------------------
| Name       : dev_at91sam9260_uart_0_open
| Description: Opening uart device
|              call by Open Posix interface
| Parameters : desc_t desc -> descriptor
|
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam9260_uart_0_open(desc_t desc, int o_flag)
{
   cyg_vector_t serial_vector = CYGNUM_HAL_INTERRUPT_USART0;
   cyg_priority_t serial_prior = 3;
   int ret;

   // Uart0 informations
   ofile_lst[desc].p = (board_inf_uart_t *)p_board_inf_uart_0;

   // Specific fields
   ((board_inf_uart_t *)ofile_lst[desc].p)->base_adr    = (AT91_REG *)AT91C_BASE_US0;
   ((board_inf_uart_t *)ofile_lst[desc].p)->periph_id   = AT91C_ID_US0 ;
   ((board_inf_uart_t *)ofile_lst[desc].p)->ind_pio_rxd = AT91C_PB5_RXD0;
   ((board_inf_uart_t *)ofile_lst[desc].p)->ind_pio_txd = AT91C_PB4_TXD0;

   // Save o_flag
   ((board_inf_uart_t *)ofile_lst[desc].p)->o_flag |= o_flag;

   // Switch on o_flag type and save it
   if (o_flag & O_RDONLY) {
      ((board_inf_uart_t *)ofile_lst[desc].p)->desc_r = desc;
   }

   if (o_flag & O_WRONLY) {
      ((board_inf_uart_t *)ofile_lst[desc].p)->desc_w = desc;
   }

   // save descriptor
   desc_uart0 = desc;

   // call uart common Api open
   ret = dev_at91sam9260_uart_x_open(desc, o_flag);

   //Primitive de creation de l'IT au chargement du driver
   cyg_interrupt_create(serial_vector, serial_prior, 0,
			&dev_at91sam9260_uart0_isr, &dev_at91sam9260_uart0_dsr,
			&_at91sam9260_uart_0_handle, &_at91sam9260_uart_0_it);
   /*Configuration IT en edge-trigerred sur front montant*/
   //Liaison entre l'IT cr�e et le vecteur d'IT
   cyg_interrupt_attach(_at91sam9260_uart_0_handle);
   cyg_interrupt_unmask(serial_vector);

   return ret;
}


/** @} */
/** @} */
/** @} */

/*============================================
| End of Source  : dev_at91sam9260_uart_0.c
==============================================*/

