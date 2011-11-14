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
| Includes
==============================================*/
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
#include "kernel/dev/arch/arm9/at91sam9260/dev_at91sam9260_uart/dev_at91sam9260_uart_x.h"
#include "kernel/dev/arch/arm9/at91sam9260/common/dev_at91sam9260_common_uart.h"

// eCos
#include "cyg/hal/at91sam9260.h"
#include "cyg/hal/hal_platform_ints.h"
#include <string.h>
#include <stdlib.h>


/*============================================
| Global Declaration
==============================================*/
const char dev_at91sam9260_dbgu_name[]="ttydbgu\0";

int dev_at91sam9260_dbgu_load(void);
int dev_at91sam9260_dbgu_open(desc_t desc, int o_flag);
int dev_at91sam9260_dbgu_close(desc_t desc);
int dev_at91sam9260_dbgu_isset_read(desc_t desc);
int dev_at91sam9260_dbgu_isset_write(desc_t desc);
int dev_at91sam9260_dbgu_read(desc_t desc, char* buf,int size);
int dev_at91sam9260_dbgu_write(desc_t desc, const char* buf,int size);
int dev_at91sam9260_dbgu_seek(desc_t desc,int offset,int origin);
int dev_at91sam9260_dbgu_ioctl(desc_t desc,int request,va_list ap);

void dev_at91sam9260_dbgu_fifo_rcv(desc_t desc);
void dev_at91sam9260_dbgu_fifo_pool_init(desc_t desc);
void dev_at91sam9260_dbgu_snd (desc_t desc);
void dev_at91sam9260_dbgu_timer_callback(alrm_hdl_t alarm_handle, cyg_addrword_t data);

//ISR and DSR
cyg_uint32 dev_at91sam9260_dbgu_isr(cyg_vector_t vector, cyg_addrword_t data);
void dev_at91sam9260_dbgu_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

static desc_t desc_dbgu;

static cyg_handle_t _at91sam9260_dbgu_handle;
static cyg_interrupt _at91sam9260_dbgu_it;
static board_inf_uart_t *p_board_inf_dbgu;   // ADU

// uart_dbg Functions pointers
dev_map_t dev_at91sam9260_dbgu_map={
   dev_at91sam9260_dbgu_name,
   S_IFCHR,
   dev_at91sam9260_dbgu_load,
   dev_at91sam9260_dbgu_open,
   dev_at91sam9260_dbgu_close,
   dev_at91sam9260_dbgu_isset_read,
   dev_at91sam9260_dbgu_isset_write,
   dev_at91sam9260_dbgu_read,
   dev_at91sam9260_dbgu_write,
   dev_at91sam9260_dbgu_seek,
   dev_at91sam9260_dbgu_ioctl
};


/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_isr
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
cyg_uint32 dev_at91sam9260_dbgu_isr(cyg_vector_t vector, cyg_addrword_t data)
{
	cyg_interrupt_mask(vector);
	unsigned int usart_csr;
	unsigned int recv_intr_cond  = 0;
	unsigned int trans_intr_cond = 0;

	board_inf_uart_t *p_inf_uart;
	AT91S_DBGU         *p_adr;

	p_inf_uart = (board_inf_uart_t *)ofile_lst[desc_dbgu].p;
	p_adr      = (AT91S_DBGU *)p_inf_uart->base_adr;

	// if device is loaded
	if (p_inf_uart->loaded)	{
		do {
			usart_csr = p_adr->DBGU_CSR;

			if (p_inf_uart->o_flag & O_RDONLY) {
				// Overrun Error
				if ((usart_csr & 0x00000020) == 0x00000020) {
					// Reset status bits, reset rx/tx.
					p_adr->DBGU_CR = AT91C_US_RSTSTA; // old value 0x00000100;
				}

				// Receiver byte per byte
				if ((usart_csr & 0x01) == 0x01) {        // RXRDY
					//receive fifo
					dev_at91sam9260_dbgu_fifo_rcv(desc_dbgu);
				}
				recv_intr_cond  = usart_csr & (p_adr->DBGU_IMR) & (0x00000001);
			}
			//
			if (p_inf_uart->o_flag & O_WRONLY) {
				// TXRDY
				if (((usart_csr & 0x00000002) == 0x00000002) && (p_inf_uart->XMIT)) {
					//Transmission (per byte)
					dev_at91sam9260_dbgu_snd(desc_dbgu);
				}
            trans_intr_cond = ( (p_inf_uart->XMIT) && (usart_csr & (p_adr->DBGU_IMR) & (0x00000002)));
			}
		} while ( recv_intr_cond || trans_intr_cond );
	}
	// Timer Value initialization for Read
	p_inf_uart->inter_char_timer = p_inf_uart->ttys_termios.c_cc[VTIME];

	//ACK de l'IT au CPU
	cyg_interrupt_acknowledge(vector);
	//Informe kernel d'executer DSR
	return(CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}


/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_seek
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
void dev_at91sam9260_dbgu_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
	board_inf_uart_t * p_inf_uart = (board_inf_uart_t *)ofile_lst[desc_dbgu].p;

	if(p_inf_uart->rcv_flag) {
		__fire_io_int(ofile_lst[p_inf_uart->desc_rd].owner_pthread_ptr_read);
		p_inf_uart->rcv_flag = 0;
	}
	if ( (p_inf_uart->desc_wr  >= 0) && (p_inf_uart->output_r == p_inf_uart->output_w) ) {
		__fire_io_int(ofile_lst[p_inf_uart->desc_wr].owner_pthread_ptr_write);
	}
	//autorise �nouveau les IT
	cyg_interrupt_unmask(vector);
}

/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_seek
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
void dev_at91sam9260_dbgu_timer_callback(alrm_hdl_t alarm_handle, cyg_addrword_t data)
{
	board_inf_uart_t *p_board_inf_uart = (board_inf_uart_t *)data;

	if( (p_board_inf_uart->desc_rd >= 0)               &&
       (p_board_inf_uart->ttys_termios.c_cc[VTIME])   &&
       (p_board_inf_uart->inter_char_timer)           &&
      !(--p_board_inf_uart->inter_char_timer) )
   {
      if (p_board_inf_uart->input_w == p_board_inf_uart->input_r) {
         __fire_io_int(ofile_lst[p_board_inf_uart->desc_rd].owner_pthread_ptr_read);
      }
   }

	rttmr_restart(&p_board_inf_uart->timer);
}


/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_seek
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
int dev_at91sam9260_dbgu_load(void)
{
	pthread_mutexattr_t  mutex_attr = 0;

	p_board_inf_dbgu  = (board_inf_uart_t *)malloc(sizeof(board_inf_uart_t));
	p_board_inf_dbgu->loaded = 0; // Not loaded

   // Disable all USART Debug interrupts.
   *(AT91C_DBGU_IDR) = 0xffffffff;

	//already loaded?
	if (p_board_inf_dbgu->loaded)
		return 0;//yes

	p_board_inf_dbgu->desc_rd = -1;
	p_board_inf_dbgu->desc_wr = -1;

	cfmakeraw   (&p_board_inf_dbgu->ttys_termios);
	cfsetispeed (&p_board_inf_dbgu->ttys_termios,B9600);
	cfsetospeed (&p_board_inf_dbgu->ttys_termios,B9600);

	p_board_inf_dbgu->ttys_termios.c_iflag &= ~(IXOFF|IXON); // xon/xoff disable
	p_board_inf_dbgu->ttys_termios.c_cc[VTIME]   = 0;        // no timeout, blocking call

	p_board_inf_dbgu->timer_attr.tm_msec = 100;
	p_board_inf_dbgu->timer_attr.func = dev_at91sam9260_dbgu_timer_callback;
	p_board_inf_dbgu->timer_attr.data = (cyg_addrword_t)&p_board_inf_dbgu;

	rttmr_create(&p_board_inf_dbgu->timer,&p_board_inf_dbgu->timer_attr);

   // Device loaded flag
	p_board_inf_dbgu->loaded = 1;
	p_board_inf_dbgu->o_flag = 0;

	// initializations
	p_board_inf_dbgu->XMIT             = 0;  // transmission flag
	p_board_inf_dbgu->inter_char_timer = 0;
   p_board_inf_dbgu->rcv_flag = 0;

	kernel_pthread_mutex_init(&p_board_inf_dbgu->mutex, &mutex_attr);
	return 0;
}

/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_seek
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
int dev_at91sam9260_dbgu_open(desc_t desc, int o_flag)
{
	cyg_vector_t serial_vector = CYGNUM_HAL_INTERRUPT_DBG;
	cyg_priority_t serial_prior = 4;
	board_inf_uart_t *p_inf_uart;
	AT91S_DBGU         *p_adr;

	ofile_lst[desc].p = (board_inf_uart_t *)p_board_inf_dbgu;

	// Specific fields
	((board_inf_uart_t *)ofile_lst[desc].p)->base_adr    = (AT91_REG *)AT91C_BASE_DBGU;
	((board_inf_uart_t *)ofile_lst[desc].p)->periph_id   = AT91C_ID_SYS;
	((board_inf_uart_t *)ofile_lst[desc].p)->ind_pio_rxd = AT91C_PB14_DRXD;
	((board_inf_uart_t *)ofile_lst[desc].p)->ind_pio_txd = AT91C_PB15_DTXD;

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
	desc_dbgu = desc;

	p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
	p_adr      = (AT91S_DBGU *)p_inf_uart->base_adr;

	// if device not opened (read/write)
	if ( (p_inf_uart->desc_rd < 0) && (p_inf_uart->desc_wr < 0) ) {
		// Disable all USART interrupts.
		p_adr->DBGU_IDR = 0xffffffff;

		// Enable peripheral clock for selected USART
		*AT91C_PMC_PCER = 1 << (p_inf_uart->periph_id);

		// Assign the I/O line to the Uart function
		*AT91C_PIOB_ASR = p_inf_uart->ind_pio_rxd | p_inf_uart->ind_pio_txd;

		// Disable PIO control of PIOA_/TXDx and PIOA_/RXDy.
		*AT91C_PIOB_PDR = p_inf_uart->ind_pio_rxd | p_inf_uart->ind_pio_txd;

		//
		p_adr->DBGU_CR = AT91C_US_RSTRX |         /* Reset Receiver      */
		AT91C_US_RSTTX |         /* Reset Transmitter   */
		AT91C_US_RXDIS |         /* Receiver Disable    */
		AT91C_US_TXDIS;          /* Transmitter Disable */
		//
		p_adr->DBGU_MR = AT91C_US_USMODE_NORMAL |  /* Normal Mode */
		AT91C_US_CLKS_CLOCK    |  /* Clock = MCK */
		AT91C_US_CHRL_8_BITS   |  /* 8-bit Data  */
		AT91C_US_PAR_NONE      |  /* No Parity   */
		AT91C_US_NBSTOP_1_BIT;    /* 1 Stop Bit  */

		// Set baud rate.
		p_adr->DBGU_BRGR = AT91_US_BAUD(OS_RS232_BAUDRATE); //see at91sam9260_misc.c

		// Reset status bits, reset rx/tx.
		p_adr->DBGU_CR = AT91C_US_RSTSTA;
		p_adr->DBGU_CR = AT91C_US_RXEN  |          /* Receiver Enable     */
		AT91C_US_TXEN;            /* Transmitter Enable  */

      p_adr->DBGU_IER = IER_MASK_RCV;
	}
	// Read only Device opening
	if (o_flag & O_RDONLY) {
		if (p_inf_uart->desc_rd >= 0)
			return -1;  //already open: exclusive resource.

		p_inf_uart->input_r = 0; // used for Application layer
		p_inf_uart->input_w = 0; // used in interrupt
		p_inf_uart->desc_rd = desc;
		p_inf_uart->ttys_termios.c_cc[VTIME] = 0;// no timeout, blocking call
		p_inf_uart->inter_char_timer = 0;
	}

	// Write only Device opening
	if (o_flag & O_WRONLY) {
		if (p_inf_uart->desc_wr >= 0)
			return -1;  //already open: exclusive resource.

		p_inf_uart->output_r = -1;
		p_inf_uart->output_w = 0;
		p_inf_uart->desc_wr  = desc;
   }
   p_inf_uart->o_flag |= o_flag; // set o_flag
	//Primitive de creation de l'IT au chargement du driver
	cyg_interrupt_create(serial_vector, serial_prior, 0,
			&dev_at91sam9260_dbgu_isr, &dev_at91sam9260_dbgu_dsr,
			&_at91sam9260_dbgu_handle, &_at91sam9260_dbgu_it);
	/*Configuration IT en edge-trigerred sur front montant*/
	//Liaison entre l'IT crée et le vecteur d'IT
	cyg_interrupt_attach(_at91sam9260_dbgu_handle);
	cyg_interrupt_unmask(serial_vector);

	return 0;
}


/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_seek
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
int dev_at91sam9260_dbgu_close(desc_t desc)
{
	board_inf_uart_t *p_inf_uart;
	AT91S_DBGU *p_adr;

	p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
	p_adr      = (AT91S_DBGU *)p_inf_uart->base_adr;

	if(ofile_lst[desc].oflag & O_RDONLY) {
		// If several process are created by the kernel
		if(!ofile_lst[desc].nb_reader) {
			// Disable Interrupt on RXRDY
			p_adr->DBGU_IDR = IER_MASK_RCV;
			//stop inter char timer
			rttmr_stop(&p_inf_uart->timer);
			p_inf_uart->ttys_termios.c_cc[VTIME] = 0; // no timeout, blocking call
			p_inf_uart->inter_char_timer = 0;
			p_inf_uart->desc_rd = -1;
		}
	}

	if(ofile_lst[desc].oflag & O_WRONLY) {
		// If several process are created by the kernel
		if(!ofile_lst[desc].nb_writer) {
			// Disable Interrupt on TXRDY
			p_adr->DBGU_IDR = IER_MASK_SND;
			p_inf_uart->desc_wr = -1;
		}
	}

	//close all
	if ( (p_inf_uart->desc_wr < 0) && (p_inf_uart->desc_rd < 0) ) {
      p_adr->DBGU_CR = AT91C_US_TXDIS | AT91C_US_RXDIS; // Disable receiver, disable transmitter
      p_adr->DBGU_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RSTSTA; // Reset status bits, reset rx/tx

		// disable peripheral clock for selected USART
		*AT91C_PMC_PCDR =  1 << p_inf_uart->periph_id;
	}
	return 0;
}

/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_seek
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
int dev_at91sam9260_dbgu_isset_read(desc_t desc)
{
	board_inf_uart_t *p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;

	// if existing data
	if ( (p_inf_uart->input_w != p_inf_uart->input_r) ||
		  (p_inf_uart->ttys_termios.c_cc[VTIME] && !p_inf_uart->inter_char_timer) ) {
		p_inf_uart->inter_char_timer = p_inf_uart->ttys_termios.c_cc[VTIME]; // load value
		return 0;  //wait incomming data
	} else {
		return -1;
   }
}

/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_seek
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
int dev_at91sam9260_dbgu_isset_write(desc_t desc)
{
	board_inf_uart_t *p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;

	if (p_inf_uart->output_w == p_inf_uart->output_r) {
		p_inf_uart->output_r = -1;
		return 0;
	} else {
		return -1;
   }
}

/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_seek
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
int dev_at91sam9260_dbgu_read(desc_t desc, char* buf,int size)
{
	board_inf_uart_t *p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
	int r   = p_inf_uart->input_r;
	int w   = p_inf_uart->input_w;
	int cb  = 0;

	kernel_pthread_mutex_lock (&p_inf_uart->mutex);

	for(cb=0;((r!=w) && (cb<size));cb++) {
		buf[cb] = p_inf_uart->fifo_input_buffer[r];

		if(++r == UART_FIFO_INPUT_BUFFER_SZ)
			r=0;
	}
	p_inf_uart->input_r = r;

	kernel_pthread_mutex_unlock (&p_inf_uart->mutex);
	return cb;
}


/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_seek
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
int dev_at91sam9260_dbgu_write(desc_t desc, const char* buf,int size)
{
	unsigned char snd_data;
	board_inf_uart_t *p_inf_uart;
	AT91S_DBGU *p_adr;

	p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
	p_adr      = (AT91S_DBGU *)p_inf_uart->base_adr;

	kernel_pthread_mutex_lock  (&p_inf_uart->mutex);

	// truncated size
	if (size >= UART_OUTPUT_BUFFER_SIZE)
		size = UART_OUTPUT_BUFFER_SIZE-1;

	memcpy(p_inf_uart->output_buffer, buf, size);

	//buffer ready
	snd_data = p_inf_uart->output_buffer[0];

	while (!((p_adr->DBGU_CSR) & (0x00000002)));    // Wait until THR empty

	p_inf_uart->output_r = 0;
	p_inf_uart->output_w = size;

	__clr_irq();

	p_inf_uart->XMIT = 1;

	// Byte per byte
	p_adr->DBGU_THR = snd_data;
	p_adr->DBGU_IER = IER_MASK_SND;

	__set_irq();

	kernel_pthread_mutex_unlock(&p_inf_uart->mutex);

	return size;
}

/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_seek
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
int dev_at91sam9260_dbgu_seek(desc_t desc,int offset,int origin)
{
	return 0;
}

/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_ioctl
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
int dev_at91sam9260_dbgu_ioctl(desc_t desc,int request,va_list ap)
{
   struct termios* termios_p = (struct termios*)0;
   board_inf_uart_t *p_inf_uart;

   p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;

   switch (request)
   {
      case TCGETS:
         termios_p = va_arg( ap, struct termios*);
         if(!termios_p) return -1;
         memcpy(termios_p,&p_inf_uart->ttys_termios,sizeof(struct termios));
         break;

      default:
           return -1;
   }
	return 0;
}

/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_snd
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
void dev_at91sam9260_dbgu_snd(desc_t desc)
{
	unsigned char    snd_data;
	board_inf_uart_t *p_inf_uart;
	AT91S_DBGU        *p_adr;

	p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
	p_adr      = (AT91S_DBGU *)p_inf_uart->base_adr;

	if (p_inf_uart->output_r < 0)
		return;

	p_inf_uart->output_r++;

	// Byte to send?
	if (p_inf_uart->output_r < p_inf_uart->output_w) {
		snd_data = p_inf_uart->output_buffer[p_inf_uart->output_r];
		p_adr->DBGU_THR = snd_data;
		p_adr->DBGU_IER = IER_MASK_SND;

   // Not empty --> empty
   } else if ( (p_inf_uart->desc_wr  >= 0) &&
			(p_inf_uart->output_r == p_inf_uart->output_w) ) {
		while (!((p_adr->DBGU_CSR) & (0x00000200)));  // Wait until TX shift register empty

		// There are not characters anymore to be sent -> Prevent the calling thread
		p_adr->DBGU_IDR = IER_MASK_SND;
		p_inf_uart->XMIT = 0;
	}
}

/*-------------------------------------------
| Name       : dev_at91sam9260_dbgu_fifo_rcv
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
void dev_at91sam9260_dbgu_fifo_rcv(desc_t desc)
{
	board_inf_uart_t *p_inf_uart;
	AT91S_DBGU *p_adr;
	int           data=0;
	unsigned char rcv_data;

	p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
	p_adr      = (AT91S_DBGU *)p_inf_uart->base_adr;

	data       = p_adr->DBGU_RHR;
	rcv_data   = (unsigned char) data;

	p_inf_uart->fifo_input_buffer[ p_inf_uart->input_w] = rcv_data;

	if ( (p_inf_uart->desc_rd >=0) &&
			(p_inf_uart->input_r == p_inf_uart->input_w)) {
		//empty to not empty
		p_inf_uart->rcv_flag = 1;
	}

	if(++p_inf_uart->input_w == UART_FIFO_INPUT_BUFFER_SZ)
		p_inf_uart->input_w = 0;

	p_inf_uart->inter_char_timer = p_inf_uart->ttys_termios.c_cc[VTIME];
}

/*============================================
| End of Source  : dev_at91sam9260_dbgu.c
==============================================*/
