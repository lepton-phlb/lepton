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

/**
 * \addtogroup lepton_dev
 * @{
 *
 */

/**
 * \addtogroup hard_dev_at91sam9261
 * @{
 *
 */

/**
 * \defgroup dev_uart_arm9_c Les uarts de l'arm9 at91sam9261
 * @{
 *
 * Les pilotes de p�riph�riques pour les uarts de l'arm9 at91sam9261
 * et supporte le timeout (VTIME) pour le mode bloquant.
 *
 */


/**
 * \file
 * implementation du pilote de p�riph�rique Uart generique de l'arm9 at91sam9261
 * a partir de l'uart_s0 d'origine (auteur philippe le boulanger)
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


#include "kernel/dev/arch/arm9/at91sam9261/common/dev_at91sam9261_common_uart.h"

#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)
#include <ioat91sam9261.h>
#include <intrinsic.h>
#else
#include "cyg/hal/at91sam9261.h"
#endif


#include "dev_at91sam9261_uart_x.h"
/*===========================================
Global Declaration
=============================================*/
// Prototypes
int dev_at91sam9261_uart_x_load       (board_inf_uart_t *);
int dev_at91sam9261_uart_x_open       (desc_t, int);
int dev_at91sam9261_uart_x_close      (desc_t);
int dev_at91sam9261_uart_x_read       (desc_t, char *, int);
int dev_at91sam9261_uart_x_write      (desc_t, const char*, int);
int dev_at91sam9261_uart_x_isset_read (desc_t);
int dev_at91sam9261_uart_x_isset_write (desc_t);
int dev_at91sam9261_uart_x_seek       (desc_t, int, int);
int dev_at91sam9261_uart_x_ioctl      (desc_t, int, va_list ap);
void dev_at91sam9261_uart_x_snd       (desc_t);



void dev_at91sam9261_uart_x_interrupt   (board_inf_uart_t * p_board_inf_uart);

#if defined(USE_SEGGER)
   void  dev_at91sam9261_uart_x_timer_callback (board_inf_uart_t * p_board_inf_uart);
#elif defined(USE_ECOS)
   cyg_uint32 dev_at91sam9261_uart_isr(cyg_vector_t vector, cyg_addrword_t data);
   void dev_at91sam9261_uart_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
   void dev_at91sam9261_uart_x_timer_callback(alrm_hdl_t alarm_handle, cyg_addrword_t data);
#endif


static int termios2ttys              (desc_t, struct termios *);

static s2s_t const s2s[] = {
        { B0,                0 },
        { B50,              50 },
        { B75,              75 },
        { B110,            110 },
        { B134,            134 },
        { B150,            150 },
        { B200,            200 },
        { B300,            300 },
        { B600,            600 },
        { B1200,          1200 },
        { B1800,          1800 },
        { B2400,          2400 },
        { B4800,          4800 },
        { B9600,          9600 },
        { B19200,        19200 },
        { B38400,        38400 },
        { B57600,        57600 },
        { B115200,      115200 },
        { B230400,      230400 },
        { B460800,      460800 }
};


#if defined(USE_ECOS)
	#define __set_flag_fire_i_int(__flag_i_int__) (__flag_i_int__=1)
	#define __set_flag_fire_o_int(__flag_o_int__) (__flag_o_int__=1)

	#define __unset_flag_fire_i_int(__flag_i_int__) (__flag_i_int__=0)
	#define __unset_flag_fire_o_int(__flag_o_int__) (__flag_o_int__=0)
#endif

#if defined(USE_ECOS)
extern cyg_uint32 hal_at91sam9261_us_baud(cyg_uint32 baud_rate);
#endif

/*===========================================
Implementation
=============================================*/

/*--------------------------------------------
| Name       :  dev_at91sam9261_uart_x_fifo_pool_init
| Description:  Multi-buffering initialization
|               Called by dev_at91sam9261_uart_x_open
| Parameters :  desc : descriptor
| Return Type:  None
| Comments   :  -
| See        :  -
----------------------------------------------*/
static void dev_at91sam9261_uart_x_fifo_pool_init (desc_t desc)
{
   board_inf_uart_t *p_inf_uart;
   AT91_REG *p_adr;
   int i=0;

   p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
   p_adr      = (AT91_REG *)p_inf_uart->base_adr;

   uchar8_t* p = (uchar8_t *)&p_inf_uart->fifo_input_buffer[0];

   // fifo pointers initialization
   for (i=0; i < MAX_POOL ; i++)
   {
      p_inf_uart->fifo_buf_pool[i].cb = 0;  // bytes number
      p_inf_uart->fifo_buf_pool[i].p  = p;  // pointer
      p += (MAX_POOL_BUF_SZ);
   }

   p_inf_uart->buf_in_dma_no = 0;           // dma number in reception
   p_inf_uart->buf_in_rcv_no = 0;

   // flush input register
   uchar8_t  c = *(p_adr+US_RHR);

   *(p_adr+US_RTOR)     = (20 * 4 * 10);  // Time_out delay period
   *(p_adr+US_CR)       = US_STTTO ;      // Resets the status bit TIMOUT

   // Initialize Receive Pointer/Counter DMA controller Registers
   *(p_adr+US_PDC_RCR)  = MAX_POOL_BUF_SZ ;
   *(p_adr+US_PDC_RPR)  = (unsigned long)
                         p_inf_uart->fifo_buf_pool[p_inf_uart->buf_in_dma_no].p;
   // RXTEN/PDC Enable
   *(p_adr+US_PDC_PTCR)  = 0x01;
}

/*--------------------------------------------
| Name       : dev_at91sam9261_uart_fifo_pool_rcv
| Description: Receive fifo
| Parameters : desc : descriptor
| Return Type: none
| Comments   : -
| See        : -
----------------------------------------------*/
void dev_at91sam9261_uart_x_fifo_pool_rcv (desc_t desc)
{
   board_inf_uart_t *p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
   long *p_adr = (long *)p_inf_uart->base_adr;

   int sz=0;
   int prev_buf_in_dma_no = p_inf_uart->buf_in_dma_no; // save current

   // Stop dma
   *(p_adr+US_CR)         = US_STTTO; // Resets the status bit TIMOUT in US_CSR

   // Assign Counter DMA controller Register
   *(p_adr+US_PDC_RCR)    = 0;

   // Get the DMA buffer size
   sz = *(p_adr+US_PDC_RPR) -
         ((unsigned long)(p_inf_uart->fifo_buf_pool[p_inf_uart->buf_in_dma_no].p));


   //nothing to read => nothing to do
   if (!sz) {
      // prepare dma operation
      *(p_adr+US_PDC_RPR)  = (unsigned long)(p_inf_uart->fifo_buf_pool[p_inf_uart->buf_in_dma_no].p);
      // restart dma
      *(p_adr+US_PDC_RCR)  = (MAX_POOL_BUF_SZ);
      p_inf_uart->inter_char_timer = p_inf_uart->ttys_termios.c_cc[VTIME];
      return;
   }

   // Next DMA buffer
   p_inf_uart->buf_in_dma_no = ((p_inf_uart->buf_in_dma_no+1)&(~MAX_POOL));

   //error overrun
   if (p_inf_uart->buf_in_dma_no == p_inf_uart->buf_in_rcv_no) {
      p_inf_uart->flag_overrun=1;

      //pb overrun le le buf_in_dma_no doit etre remis a
      //la reception est deconnectee. elle sera reconnectee qd le micro aura reussit à rattraper son retard
      //disable receive
      *(p_adr+US_CR)=AT91C_US_RXDIS;
   }

   // prepare dma operation
   *(p_adr+US_PDC_RPR)  = (unsigned long)(p_inf_uart->fifo_buf_pool[
                                                                    p_inf_uart->buf_in_dma_no].p);
   // restart dma
   *(p_adr+US_PDC_RCR)  = (MAX_POOL_BUF_SZ);

   p_inf_uart->inter_char_timer = p_inf_uart->ttys_termios.c_cc[VTIME];

   p_inf_uart->fifo_buf_pool[prev_buf_in_dma_no].cb = sz;

   if ((p_inf_uart->desc_rd >=0) && (p_inf_uart->input_r == p_inf_uart->input_w))
   {
      //empty to not empty
#ifdef USE_SEGGER
      __fire_io_int(ofile_lst[p_inf_uart->desc_rd].owner_pthread_ptr_read);
#elif defined(USE_ECOS)
      __set_flag_fire_i_int(p_inf_uart->flag_i_int);
#endif
   }

   // re-calculated index
   p_inf_uart->input_w = ((p_inf_uart->input_w+sz)&(~UART_FIFO_INPUT_BUFFER_SZ));
}


/*-------------------------------------------
| Name       : dev_at91sam9261_uart_x_snd
| Description: Send Data
| Parameters : desc : descriptor
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
void dev_at91sam9261_uart_x_snd (desc_t desc)
{
   unsigned char    snd_data;
   board_inf_uart_t *p_inf_uart;
   AT91_REG         *p_adr;

   p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
   p_adr      = (AT91_REG *)p_inf_uart->base_adr;

   if (p_inf_uart->output_r < 0)
      return;

   p_inf_uart->output_r++;

   // Byte to send?
   if (p_inf_uart->output_r < p_inf_uart->output_w)
   {
      snd_data = p_inf_uart->output_buffer[p_inf_uart->output_r];
      *(p_adr+US_THR) = snd_data;
      *(p_adr+US_IER) = 2;
   }
   // Not empty --> empty
   else if ( (p_inf_uart->desc_wr  >= 0) &&
             (p_inf_uart->output_r == p_inf_uart->output_w) )
   {
      while (!(*(p_adr+US_CSR) & (0x00000200)));  // Wait until TX shift register empty

      // There are not characters anymore to be sent -> Prevent the calling thread
#ifdef USE_SEGGER
      __fire_io_int(ofile_lst[p_inf_uart->desc_wr].owner_pthread_ptr_write);
#elif defined(USE_ECOS)
      __set_flag_fire_o_int(p_inf_uart->flag_o_int);
#endif
      *(p_adr+US_IDR)   = 2;
      p_inf_uart->XMIT  = 0;
   }
}

/*-------------------------------------------
| Name       : dev_at91sam9261_uart_x_dma_snd
| Description: Send Data into DMA
| Parameters : desc : descriptor
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
void dev_at91sam9261_uart_x_dma_snd (desc_t desc)
{
   unsigned char    snd_data;
   board_inf_uart_t *p_inf_uart;
   AT91_REG         *p_adr;

   p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
   p_adr      = (AT91_REG *)p_inf_uart->base_adr;

   p_inf_uart->output_r = p_inf_uart->output_w; // all data are sent

   // Not empty --> empty
   if (p_inf_uart->desc_wr  >= 0)
   {
      // There are not characters anymore to be sent -> Prevent the calling thread
#ifdef USE_SEGGER
      __fire_io_int(ofile_lst[p_inf_uart->desc_wr].owner_pthread_ptr_write);
#elif defined(USE_ECOS)
      __set_flag_fire_o_int(p_inf_uart->flag_o_int);
#endif
      *(p_adr+US_IDR)   = 0x10;   //ENDTX
      p_inf_uart->XMIT  = 0;
   }
}


/*-------------------------------------------
| Name       : dev_at91sam9261_uart_x_timer_callback
| Description: Generic timer callback function
| Parameters : board_inf_uart_t pointer
| Return Type: -
| Comments   : -
| See        : -
---------------------------------------------*/

#if defined(USE_SEGGER)
void  dev_at91sam9261_uart_x_timer_callback(board_inf_uart_t * p_board_inf_uart) {
#elif defined(USE_ECOS)
void  dev_at91sam9261_uart_x_timer_callback(alrm_hdl_t alarm_handle, cyg_addrword_t data){
	board_inf_uart_t *p_board_inf_uart = (board_inf_uart_t *)data;
#endif

   if( (p_board_inf_uart->desc_rd >= 0)           		   &&
       (p_board_inf_uart->ttys_termios.c_cc[VTIME] != 0) &&
       (p_board_inf_uart->inter_char_timer)       			&&
      !(--p_board_inf_uart->inter_char_timer) )
   {
      if (p_board_inf_uart->input_w == p_board_inf_uart->input_r)
      {
         __fire_io_int(ofile_lst[p_board_inf_uart->desc_rd].owner_pthread_ptr_read);
      }
   }

   //OS_RetriggerTimer(&p_board_inf_uart->timer);
    rttmr_restart(&p_board_inf_uart->timer);
}

/*--------------------------------------------
| Name       : dev_at91sam9261_uart_x_interrupt
| Description: Generic Interrupt function for
|              reading/writing bytes
| Parameters : descriptor  (desc_t)
| Return Type: none
| Comments   : -
| See        : -
----------------------------------------------*/

void dev_at91sam9261_uart_x_interrupt(board_inf_uart_t * p_inf_uart)
{
   unsigned int usart_csr;
   unsigned int recv_intr_cond  = 0;
   unsigned int trans_intr_cond = 0;

   // board_inf_uart_t *p_inf_uart;
   AT91_REG         *p_adr;

   desc_t desc_r;
   desc_t desc_w;

   //p_inf_uart = p_board_inf_uart;
   p_adr      = (AT91_REG *)p_inf_uart->base_adr;

   desc_r = p_inf_uart->desc_r;
   desc_w = p_inf_uart->desc_w;

   // if device is loaded
   if (p_inf_uart->loaded)
   {
      do
      {
         usart_csr = *(p_adr+US_CSR);
         if( (desc_r>=0) && (ofile_lst[desc_r].oflag & O_RDONLY) )
         {
            // Overrun Error
            if ((usart_csr & 0x00000020) == 0x00000020)
            {
               // Reset status bits, reset rx/tx.
               *(p_adr+US_CR)  = AT91C_US_RSTSTA; // old value 0x00000100;
            }

            // Receiver TIMEOUT or ENDRX (End of Receiver Tranfer)
            if ((usart_csr & 0x00000108))
            {
               //receive fifo
               dev_at91sam9261_uart_x_fifo_pool_rcv(p_inf_uart->desc_rd);
            }

            recv_intr_cond  = usart_csr & *(p_adr+US_IMR) & (0x00000108);
         }


         if( (desc_w>=0) && (ofile_lst[desc_w].oflag & O_WRONLY) )
         {
#if defined (USE_DMA_UART_SND)
            // ENDTX
            if (((usart_csr & 0x00000010) == 0x00000010) && (p_inf_uart->XMIT))
            {
               dev_at91sam9261_uart_x_dma_snd(p_inf_uart->desc_wr);
            }

            trans_intr_cond = ( (p_inf_uart->XMIT) &&
                  (usart_csr & *(p_adr+US_IMR) & (0x00000010)));

#else
            // TXRDY
            if (((usart_csr & 0x00000002) == 0x00000002) && (p_inf_uart->XMIT))
            {
               //Transmission (per byte)
               dev_at91sam9261_uart_x_snd(p_inf_uart->desc_wr);
            }

            trans_intr_cond = ( (p_inf_uart->XMIT) &&
                  (usart_csr & *(p_adr+US_IMR) & (0x00000002)));
#endif

         }

      } while ( recv_intr_cond || trans_intr_cond );
   }

   // Timer Value initialization for Read
   p_inf_uart->inter_char_timer=p_inf_uart->ttys_termios.c_cc[VTIME];
#if defined(USE_SEGGER)
   *AT91C_AIC_EOICR = 0; // Signal end of interrupt to AIC.
#endif

}
#if defined(USE_ECOS)
cyg_uint32 dev_at91sam9261_uart_isr(cyg_vector_t vector, cyg_addrword_t data) {

	cyg_interrupt_mask(vector);

	dev_at91sam9261_uart_x_interrupt((board_inf_uart_t*)data);

	cyg_interrupt_acknowledge(vector);
   //Informe kernel d'exécuter DSR
   return(CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}

void dev_at91sam9261_uart_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
   board_inf_uart_t * p_inf_uart = (board_inf_uart_t *)data;
   //

   if(p_inf_uart->flag_i_int) {
      __fire_io_int(ofile_lst[p_inf_uart->desc_rd].owner_pthread_ptr_read);
      __unset_flag_fire_i_int(p_inf_uart->flag_i_int);

   }

   if (p_inf_uart->flag_o_int) {
      __fire_io_int(ofile_lst[p_inf_uart->desc_wr].owner_pthread_ptr_write);
      __unset_flag_fire_o_int(p_inf_uart->flag_o_int);
   }
   //autorise à nouveau les IT
   cyg_interrupt_unmask(vector);
}
#endif
/*-------------------------------------------
| Name       :dev_at91sam9261_uart_x_load
| Description: Load device
| Parameters : p_board_inf_uart : info uart pointer
| Return Type: integer -> devive loaded (=0)
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam9261_uart_x_load (board_inf_uart_t *p_board_inf_uart)
{
   pthread_mutexattr_t  mutex_attr=0;

   //already loaded?
   if (p_board_inf_uart->loaded)
      return 0;//yes

   p_board_inf_uart->desc_rd = -1;
   p_board_inf_uart->desc_wr = -1;

   cfmakeraw   (&p_board_inf_uart->ttys_termios);
   cfsetispeed (&p_board_inf_uart->ttys_termios,B9600);
   cfsetospeed (&p_board_inf_uart->ttys_termios,B9600);
   
   p_board_inf_uart->ttys_termios.c_iflag &= ~(IXOFF|IXON); // xon/xoff disable
   p_board_inf_uart->ttys_termios.c_cc[VTIME]   = 0;        // no timeout, blocking call

#ifdef USE_SEGGER
   //VTIME timer in units of 0.1 seconds (posix specification).
   OS_CreateTimer (&p_board_inf_uart->timer,
                   p_board_inf_uart->f_timer_call_back,
                   100);  // 100ms

#elif defined(USE_ECOS)
   p_board_inf_uart->timer_attr.tm_msec = 100;
   p_board_inf_uart->timer_attr.func = dev_at91sam9261_uart_x_timer_callback;

   p_board_inf_uart->timer_attr.data = (cyg_addrword_t)p_board_inf_uart;
   rttmr_create(&p_board_inf_uart->timer,&p_board_inf_uart->timer_attr);
#endif
   // Device loaded flag
   p_board_inf_uart->loaded = 1;

   // initializations
   p_board_inf_uart->XMIT             = 0;  // transmission flag
   p_board_inf_uart->inter_char_timer = 0;

   p_board_inf_uart->flag_overrun = 0;

   kernel_pthread_mutex_init(&p_board_inf_uart->mutex, &mutex_attr);

   return 0;
}

/*-------------------------------------------
| Name       : dev_at91sam9261_uart_x_open
| Description: Open device
| Parameters : desc   : descriptor
|              o_flag : integer (O_RDONLY or/and O_WRONLY device type)
| Return Type: integer (-1) already open
|                      (0) OK
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam9261_uart_x_open (desc_t desc, int o_flag)
{
   board_inf_uart_t *p_inf_uart;
   AT91_REG         *p_adr;

   p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
   p_adr      = (AT91_REG *)p_inf_uart->base_adr;

   unsigned int baud_value=0;

   // if device not opened (read/write)
   if ( (p_inf_uart->desc_rd < 0) && (p_inf_uart->desc_wr < 0) )
   {
      // Disable all USART interrupts.
      *(p_adr+US_IDR)   = 0xffffffff;

      // Enable peripheral clock for selected USART
      *AT91C_PMC_PCER = 1 << (p_inf_uart->periph_id);

      // PIO Contoller C
      // Assign the I/O line to the Uart function
      *AT91C_PIOC_ASR = ( (1 << (p_inf_uart->ind_pio_rxd) ) |
                         (1 << (p_inf_uart->ind_pio_txd) ));

      // Disable PIO control of PIOC_/TXDx and PIOC_/RXDy.
      *AT91C_PIOC_PDR = ( (1 << (p_inf_uart->ind_pio_rxd) ) |
                         (1 << (p_inf_uart->ind_pio_txd) ));

      *(p_adr+US_CR)  =   AT91C_US_RSTRX |         /* Reset Receiver      */
                          AT91C_US_RSTTX |         /* Reset Transmitter   */
                          AT91C_US_RXDIS |         /* Receiver Disable    */
                          AT91C_US_TXDIS;          /* Transmitter Disable */

      *(p_adr+US_MR)   =  AT91C_US_USMODE_NORMAL |  /* Normal Mode */
                          AT91C_US_CLKS_CLOCK    |  /* Clock = MCK */
                          AT91C_US_CHRL_8_BITS   |  /* 8-bit Data  */
                          AT91C_US_PAR_NONE      |  /* No Parity   */
                          AT91C_US_NBSTOP_1_BIT;    /* 1 Stop Bit  */

#if defined(USE_SEGGER)
      // Set baud rate.
      //*(p_adr+US_BRGR)  = __KERNEL_CPU_FREQ / OS_RS232_BAUDRATE / 16;
       baud_value = ((__KERNEL_PERIPHERAL_FREQ*10)/(OS_RS232_BAUDRATE * 16));

      if ((baud_value % 10) >= 5)
      	baud_value = (baud_value / 10) + 1;
      else
      	baud_value /= 10;
#elif defined(USE_ECOS)
      baud_value = hal_at91sam9261_us_baud(OS_RS232_BAUDRATE); //see hal/arm/at91/at91sam9261-ek/current/src/at91sam9261_misc.c
#endif

        *(p_adr+US_BRGR) = baud_value;

      // Reset status bits, reset rx/tx.
      *(p_adr+US_CR)  = AT91C_US_RSTSTA;
      *(p_adr+US_CR)  = AT91C_US_RXEN  |          /* Receiver Enable     */
                        AT91C_US_TXEN;            /* Tranfsmitter Enable  */


      // SRCTYPE=3, PRIOR=3. USART 0 interrupt positive edge-triggered at prio 3.
      AT91C_AIC_SMR[p_inf_uart->periph_id] = 0x63;

      *AT91C_AIC_ICCR = (1 << p_inf_uart->periph_id); // Clears usart x interrupt
      *AT91C_AIC_IECR = (1 << p_inf_uart->periph_id); // Enable usart x interrupt
   }

   // Read only Device opening
   if (o_flag & O_RDONLY)
   {
      char _rcv_buf;
      if (p_inf_uart->desc_rd >= 0)
         return -1;  //already open: exclusive resource.

      p_inf_uart->input_r = 0; // used for Application layer
      p_inf_uart->input_w = 0; // used in interrupt
      p_inf_uart->desc_rd = desc;

      //pool multi-buffering
      dev_at91sam9261_uart_x_fifo_pool_init(desc);

      p_inf_uart->ttys_termios.c_cc[VTIME]   = 0;// no timeout, blocking call
      p_inf_uart->inter_char_timer           = 0;    

      // Interrupt on DMA
       *(p_adr+US_IER) = IER_MASK_RCV;
   }

   // Write only Device opening
   if (o_flag & O_WRONLY)
   {
      if (p_inf_uart->desc_wr >= 0)
         return -1;  //already open: exclusive resource.

      p_inf_uart->output_r = -1;
      p_inf_uart->output_w = 0;
      p_inf_uart->desc_wr  = desc;

#if defined (USE_DMA_UART_SND)
     *(p_adr+US_PDC_PTCR) = AT91C_PDC_TXTEN; // TXTEN/PDC Enable
#else
      // Interrupt on TXRDY
      *(p_adr+US_IER) =   IER_MASK_SND;
#endif
   }

   p_inf_uart->o_flag |= o_flag; // set o_flag


   return 0;
}

/*-------------------------------------------
| Name       : dev_at91sam9261_uart_x_close
| Description: Called by the kernel
| Parameters : desc : descriptor
| Return Type: integer
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam9261_uart_x_close(desc_t desc)
{
   board_inf_uart_t *p_inf_uart;
   AT91_REG *p_adr;

   p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
   p_adr      = (AT91_REG *)p_inf_uart->base_adr;

   if(ofile_lst[desc].oflag & O_RDONLY)
   {
      // If several process are created by the kernel
      if(!ofile_lst[desc].nb_reader)
      {
         // Disable Interrupt on RXRDY
         *(p_adr+US_IDR) = IER_MASK_RCV;

         //stop inter char timer
        // OS_StopTimer(&p_inf_uart->timer);
         rttmr_stop(&p_inf_uart->timer);

         p_inf_uart->ttys_termios.c_cc[VTIME]     = 0; // no timeout, blocking call
         p_inf_uart->inter_char_timer = 0;

         p_inf_uart->desc_rd = -1;
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY)
   {
      // If several process are created by the kernel
      if(!ofile_lst[desc].nb_writer)
      {
         // Disable Interrupt on TXRDY
         *(p_adr+US_IDR)= IER_MASK_SND;

         p_inf_uart->desc_wr = -1;
      }
   }

   //close all
   if ( (p_inf_uart->desc_wr < 0) && (p_inf_uart->desc_rd < 0) )
   {
      *(p_adr+US_IDR) = 0xffffffff; // Disable all USART interrupts
      *AT91C_AIC_ICCR = (1 << p_inf_uart->periph_id); // Clears usart x interrupt
      *(p_adr+US_CR)  = 0x000000a0; // Disable receiver, disable transmitter
      *(p_adr+US_CR)  = 0x0000010c; // Reset status bits, reset rx/tx

      // disable peripheral clock for selected USART0
      *AT91C_PMC_PCDR =  1 << p_inf_uart->periph_id;
   }


   return 0;
}

/*-------------------------------------------
| Name       :dev_at91sam9261_uart_x_isset_read
| Description: Called by Read Posix interface
| Parameters : desc  : descriptor
| Return Type: integer (0)  : wait incomming data
|                      (-1) : end of waiting
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam9261_uart_x_isset_read (desc_t desc)
{
   board_inf_uart_t *p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
   AT91_REG         *p_adr      = (AT91_REG *)p_inf_uart->base_adr;

   // if existing data
   if ( (p_inf_uart->input_w != p_inf_uart->input_r) ||
        (p_inf_uart->ttys_termios.c_cc[VTIME] && !p_inf_uart->inter_char_timer) )
   {
      p_inf_uart->inter_char_timer = p_inf_uart->ttys_termios.c_cc[VTIME]; // load value
      return (0);  
   }
   else
      return (-1);  //wait incomming data
}

/*-------------------------------------------
| Name       : dev_at91sam9261_uart_x_isset_write
| Description: Called by write function
| Parameters : desc  : descriptor
| Return Type: int
| Comments   :
| See        :
---------------------------------------------*/
int dev_at91sam9261_uart_x_isset_write (desc_t desc)
{
   board_inf_uart_t *p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
   AT91_REG         *p_adr      = (AT91_REG *)p_inf_uart->base_adr;

   if (p_inf_uart->output_w == p_inf_uart->output_r)
   {
      p_inf_uart->output_r = -1;
      return 0;
   }
   else
      return -1;
}

/*-------------------------------------------
| Name       : dev_at91sam9261_uart_x_read
| Description: Called by Read Posix interface
| Parameters : desc : descriptor
|              buf  : pointer on read buffer
|              size : size
| Return Type: integer cb: number of bytes readed
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam9261_uart_x_read(desc_t desc, char* buf,int size)
{
   board_inf_uart_t *p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
   AT91_REG         *p_adr      = (AT91_REG *)p_inf_uart->base_adr;

   int r              = p_inf_uart->input_r;
   int _buf_in_rcv_no = p_inf_uart->buf_in_rcv_no;
   int _buf_in_dma_no = -1;
   int cb             = 0, w;

   kernel_pthread_mutex_lock (&p_inf_uart->mutex);

#if defined(USE_SEGGER)
   OS_DI();
#elif defined(USE_ECOS)
   __clr_irq();
#endif
   _buf_in_dma_no = p_inf_uart->buf_in_dma_no;//p_inf_uart->buf_in_rcv_no;

#if defined(USE_SEGGER)
   OS_EI();
#elif defined(USE_ECOS)
   __set_irq();
#endif

   do
   {
      w=0;

      if ( !p_inf_uart->fifo_buf_pool[_buf_in_rcv_no].cb ||
           cb == size)
         break;
      w = (size < (cb + p_inf_uart->fifo_buf_pool[_buf_in_rcv_no].cb)
           ? (size-cb)
           : p_inf_uart->fifo_buf_pool[_buf_in_rcv_no].cb);

      memcpy (buf+cb,
              p_inf_uart->fifo_buf_pool[_buf_in_rcv_no].p,
              w);

      cb += w;

      p_inf_uart->fifo_buf_pool[_buf_in_rcv_no].cb -= w;
      p_inf_uart->fifo_buf_pool[_buf_in_rcv_no].p  += w;

      if (p_inf_uart->fifo_buf_pool[_buf_in_rcv_no].cb <= 0)
      {
         p_inf_uart->fifo_buf_pool[_buf_in_rcv_no].p =
                (uchar8_t *)&p_inf_uart->fifo_input_buffer[
                                                _buf_in_rcv_no*MAX_POOL_BUF_SZ];
         _buf_in_rcv_no = ((_buf_in_rcv_no+1)&(~MAX_POOL));
      }

   } while( _buf_in_rcv_no != _buf_in_dma_no);

   //si tout  à été lu reconnecter la reception si au cas elle à été deconnecté
   if((_buf_in_rcv_no==_buf_in_dma_no)&&p_inf_uart->fifo_buf_pool[_buf_in_rcv_no].cb==0)
   {
	   *(p_adr+US_CR)  = AT91C_US_RXEN;
	   p_inf_uart->flag_overrun = 0;
   }

#if defined(USE_SEGGER)
   OS_DI();
#elif defined(USE_ECOS)
   __clr_irq();
#endif
   p_inf_uart->input_r = ((p_inf_uart->input_r+cb) & (~UART_FIFO_INPUT_BUFFER_SZ));
   p_inf_uart->buf_in_rcv_no = _buf_in_rcv_no;
#if defined(USE_SEGGER)
   OS_EI();
#elif defined(USE_ECOS)
   __set_irq();
#endif


   kernel_pthread_mutex_unlock (&p_inf_uart->mutex);

   return cb;
}

/*-------------------------------------------
| Name       :dev_at91sam9261_uart_x_write
| Description: Write buffer
|              Called by Write Posix Interface
| Parameters : descriptor (desc_t)
|              buffer adress (char *)
|              buffer size
| Return Type: Integer
| Comments   : Use DMA or not (byte per byte)
| See        : -
---------------------------------------------*/
int dev_at91sam9261_uart_x_write(desc_t desc, const char* buf,int size)
{
   unsigned char snd_data;

   //
   board_inf_uart_t *p_inf_uart;
   AT91_REG *p_adr;

   p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
   p_adr      = (AT91_REG *)p_inf_uart->base_adr;

   kernel_pthread_mutex_lock  (&p_inf_uart->mutex);

   // truncated size
   if (size >= UART_OUTPUT_BUFFER_SIZE)
         size = UART_OUTPUT_BUFFER_SIZE-1;

   memcpy(p_inf_uart->output_buffer, buf, size);

   //buffer ready
   snd_data = p_inf_uart->output_buffer[0];

   while (!(*(p_adr+US_CSR) & (0x00000002)));    // Wait until THR empty

   p_inf_uart->output_r = 0;
   p_inf_uart->output_w = size;

#if defined(USE_SEGGER)
   OS_DI();
#elif defined(USE_ECOS)
   __clr_irq();
#endif

   p_inf_uart->XMIT = 1;

#if defined (USE_DMA_UART_SND)
   // TXTEN/PDC Enable
   *(p_adr+US_IER)      = 0x10; // ENDTX
   *(p_adr+US_PDC_TPR)  = (unsigned long)&p_inf_uart->output_buffer[0];
   *(p_adr+US_PDC_TCR)  = size;
#else
   // Byte per byte
   *(p_adr+US_THR) = snd_data;
   *(p_adr+US_IER) = 0x02;
#endif

#if defined(USE_SEGGER)
   OS_EI();
#elif defined(USE_ECOS)
   __set_irq();
#endif

   kernel_pthread_mutex_unlock(&p_inf_uart->mutex);

   return size;
}

/*-------------------------------------------
| Name       :dev_at91sam9261_uart_x_seek
| Description: -
| Parameters : -
| Return Type: -
| Comments   : N/A
| See        : -
---------------------------------------------*/
int dev_at91sam9261_uart_x_seek(desc_t desc,int offset,int origin)
{
   return 0;
}

/*-------------------------------------------
| Name       :termios2ttys
| Description: called by dev_at91sam9261_uart_x_ioctl
| Parameters : descriptor
|              struct termios *
| Return Type: integer
| Comments   : -
| See        : -
---------------------------------------------*/
static int termios2ttys(desc_t desc, struct termios* termios_p)
{
   const struct s2s *sp;
   long         n_speed;
   speed_t      speed;

   //
   board_inf_uart_t *p_inf_uart;
   AT91_REG    *p_adr;

   p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
   p_adr      = (AT91_REG *)p_inf_uart->base_adr;
   //

   //speed
   speed = cfgetospeed(termios_p);
   for (sp = s2s; sp < s2s + (sizeof(s2s) / sizeof(s2s[0])); sp++)
   {
      if (sp->ts == speed)
      {
         n_speed = sp->ns;
         // Set baud rate    
#if defined(USE_SEGGER)
         *(p_adr+US_BRGR) = (__KERNEL_PERIPHERAL_FREQ / n_speed / 16);
#elif defined(USE_ECOS)
         speed = hal_at91sam9261_us_baud(n_speed);
         *(p_adr+US_BRGR) = speed;
#endif
         break;
      }
   }

   //timeout read in blocking call
   if (p_inf_uart->desc_rd >= 0) 
   {     
     if(termios_p->c_cc[VTIME])
     {
#if defined(USE_SEGGER)
       OS_StopTimer (&p_inf_uart->timer);
 #elif defined(USE_ECOS)
	   rttmr_stop(&p_inf_uart->timer);
 #endif
       p_inf_uart->inter_char_timer = termios_p->c_cc[VTIME];
#ifdef USE_SEGGER      
		OS_RetriggerTimer (&p_inf_uart->timer);
#elif defined(USE_ECOS)
		rttmr_restart(&p_inf_uart->timer);
#endif                        	  
     }
     else
     {
#ifdef USE_SEGGER      
		OS_StopTimer (&p_inf_uart->timer);
#elif defined(USE_ECOS)
		rttmr_stop(&p_inf_uart->timer);
#endif                	   
     }
   }
   //   
   memcpy (&p_inf_uart->ttys_termios,termios_p, sizeof(struct termios));
   return 0;
}

/*-------------------------------------------
| Name       :dev_at91sam9261_uart_s1_ioctl
| Description: Called by ioctl
| Parameters :
|             descriptor desc_t  desc
|             int     request // $BM
|             va_list ap
|
| Return Type: int (-1 :)
| Comments   :
| See        :
---------------------------------------------*/
int dev_at91sam9261_uart_x_ioctl(desc_t desc, int request, va_list ap)
{
   struct termios* termios_p = (struct termios*)0;

   //
   board_inf_uart_t *p_inf_uart;
   AT91_REG *    p_adr;

   p_inf_uart = (board_inf_uart_t *)ofile_lst[desc].p;
   p_adr      = (AT91_REG *)p_inf_uart->base_adr;

   switch (request)
   {
      case TIOCSSERIAL:{
                         unsigned long speed = va_arg( ap, unsigned long);
                         if (speed==0) return -1;
                         // Set baud rate.
                  //       *(p_adr+US_BRGR) = (__KERNEL_PERIPHERAL_FREQ / speed / 16);
#if defined(USE_SEGGER)
         *(p_adr+US_BRGR) = (__KERNEL_PERIPHERAL_FREQ / speed / 16);
#elif defined(USE_ECOS)
         speed = hal_at91sam9261_us_baud(speed);
         *(p_adr+US_BRGR) = speed;
#endif
                       }
            break;

      /* If optional_actions is TCSANOW, the change will occur immediately.   */
      /* If optional_actions is TCSADRAIN, the change will occur after all
            output written to fildes is transmitted.
     This function should be used when changing parameters that affect output.*/
      /* If optional_actions is TCSAFLUSH, the change will occur after all
         output written to fildes is transmitted, and all input so far received
         but not read will be discarded before the change is made.
         return 0;*/
      case TCSETS   :
      case TCSAFLUSH:
      case TCSADRAIN:
      case TCSANOW  :
                      termios_p = va_arg( ap, struct termios*);
                      if (!termios_p) return -1;
                      termios2ttys(desc,termios_p);
           break;
      case TCGETS:
                      termios_p = va_arg( ap, struct termios*);
                      if(!termios_p) return -1;
                      memcpy(termios_p,&p_inf_uart->ttys_termios,sizeof(struct termios));
           break;
      //
      case  TIOCM_RTS :*(p_adr+US_MR)|= 0x02;
      		break;
      				
      default:
           return -1;
   }

   return 0;
}

/** @} */
/** @} */
/** @} */

/*============================================
| End of Source  : dev_at91sam9261_uart_x.c
==============================================*/
