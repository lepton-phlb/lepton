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
 * \defgroup dev_uart_arm9_c Les uart de l'arm9 at91sam9261.
 * @{
 *
 * Les pilotes de priphriques pour les uart de l'arm9 at91sam9261
 * Ces pilotes de priphriques gre les vitesses de 50  38400 bauds
 * et supporte le timeout (VTIME) pour le mode bloquant.
 *
 */


/**
 * \file
 * implementation du pilote de priphrique pour l'uart 0 de l'arm9 at91sam9261
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
#include "kernel/dev/arch/arm9/at91sam9261/dev_at91sam9261_uart_x/dev_at91sam9261_uart_x.h"
#include "kernel/dev/arch/arm9/at91sam9261/common/dev_at91sam9261_common_uart.h"


#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)
   #include <atmel/ioat91sam9261.h>
#else
   #include "cyg/hal/at91sam9261.h"
   #include <string.h>
   #include <stdlib.h>

   #define CYGNUM_HAL_INT_SERIAL 38 //6 //ou 7 (USART1) ou 8 (USART2)(voir cyg/hal/hal_platform_ints.h)
   #define CYGNUM_HAL_H_PRIOR 3

#endif

// define

/*===========================================
Global Declaration
=============================================*/
const char dev_at91sam9261_uart_dbg_name[]="ttydbgu\0"; // ADU

// extern
extern int dev_at91sam9261_uart_dbg_x_load        (board_inf_uart_t *);
extern int dev_at91sam9261_uart_dbg_x_open        (desc_t desc, int o_flag);
extern int dev_at91sam9261_uart_dbg_x_close       (desc_t desc);
extern int dev_at91sam9261_uart_dbg_x_isset_read  (desc_t desc);
extern int dev_at91sam9261_uart_dbg_x_isset_write (desc_t desc);
extern int dev_at91sam9261_uart_dbg_x_read        (desc_t desc, char* buf,int size);
extern int dev_at91sam9261_uart_dbg_x_write       (desc_t desc, const char* buf,int size);
extern int dev_at91sam9261_uart_dbg_x_seek        (desc_t desc,int offset,int origin);
extern int dev_at91sam9261_uart_dbg_x_ioctl       (desc_t desc,int request,va_list ap);
extern int dev_at91sam9261_uart_dbg_x_interrupt   (desc_t desc);
extern int termios2ttys                      (struct termios* termios_p);

#if defined(__KERNEL_UCORE_EMBOS) || defined(__KERNEL_UCORE_FREERTOS)
extern void dev_at91sam9261_uart_dbg_x_timer_callback (board_inf_uart_t * p_board_inf_uart);
#else
extern void dev_at91sam9261_uart_dbg_x_timer_callback(alrm_hdl_t alarm_handle, cyg_addrword_t data);
extern void dev_at91sam9261_uart_dbg_x_snd       (desc_t);
extern void dev_at91sam9261_uart_dbg_x_fifo_rcv  (desc_t desc);
#endif

// statics
static int dev_at91sam9261_uart_dbg_load        (void);
static int dev_at91sam9261_uart_dbg_open        (desc_t desc, int o_flag);
static void dev_at91sam9261_uart_dbg_interrupt  (void);

desc_t desc_uart_dbg=-1;

#if defined(__KERNEL_UCORE_EMBOS) || defined(__KERNEL_UCORE_FREERTOS)
// fct pointer for debug interrupt
extern void (*g_p_fct_dbg_interrupt)(void);

#elif defined(__KERNEL_UCORE_ECOS)
static cyg_handle_t _at91sam9261_uart_dbg_handle;
static cyg_interrupt _at91sam9261_uart_dbg_it;
int rcv_flag=0;
extern struct termios ttys_termios;
#endif

static board_inf_uart_t *p_board_inf_uart_dbg;

dev_map_t dev_at91sam9261_uart_dbg_map={
   dev_at91sam9261_uart_dbg_name,
   S_IFCHR,
   dev_at91sam9261_uart_dbg_load,
   dev_at91sam9261_uart_dbg_open,
   dev_at91sam9261_uart_dbg_x_close,
   dev_at91sam9261_uart_dbg_x_isset_read,
   dev_at91sam9261_uart_dbg_x_isset_write,
   dev_at91sam9261_uart_dbg_x_read,
   dev_at91sam9261_uart_dbg_x_write,
   dev_at91sam9261_uart_dbg_x_seek,
   dev_at91sam9261_uart_dbg_x_ioctl
};

#if defined(__KERNEL_UCORE_EMBOS) || defined(__KERNEL_UCORE_FREERTOS)
void dev_at91sam9261_uart_dbg_interrupt(void)
{
   // interrupt for dbg Uart ?
   board_inf_uart_t *p_inf_uart;
   AT91_REG         *p_adr;
    //
   __hw_enter_interrupt();
   //
   p_inf_uart = (board_inf_uart_t *)ofile_lst[desc_uart_dbg].p;
   p_adr      = (AT91_REG *)p_inf_uart->base_adr;

   dev_at91sam9261_uart_dbg_x_interrupt(desc_uart_dbg);
   //
   __hw_leave_interrupt();
   //
}


void dev_at91sam9261_uart_dbg_timer_callback(void)
{
   dev_at91sam9261_uart_dbg_x_timer_callback(p_board_inf_uart_dbg);
}
#elif defined(__KERNEL_UCORE_ECOS)
cyg_uint32 dev_at91sam9261_uart_dbg_isr(cyg_vector_t vector, cyg_addrword_t data) {
   cyg_interrupt_mask(vector);
   unsigned int usart_csr;
   unsigned int recv_intr_cond  = 0;
   unsigned int trans_intr_cond = 0;

   board_inf_uart_t *p_inf_uart;
   AT91_REG         *p_adr;

   desc_t desc_r;
   desc_t desc_w;

   p_inf_uart = (board_inf_uart_t *)ofile_lst[desc_uart_dbg].p;
   p_adr      = (AT91_REG *)p_inf_uart->base_adr;

   desc_r = p_inf_uart->desc_r;
   desc_w = p_inf_uart->desc_w;

   // if device is loaded
   if (p_inf_uart->loaded) {
      do {
         usart_csr = *(p_adr+US_CSR);

         if( (desc_r>=0) && (ofile_lst[desc_r].oflag & O_RDONLY) ) {
            // Overrun Error
            if ((usart_csr & 0x00000020) == 0x00000020) {
               // Reset status bits, reset rx/tx.
               *(p_adr+US_CR)  = AT91C_US_RSTSTA;                          // old value 0x00000100;
            }

            // Receiver byte per byte
            if ((usart_csr & 0x01) == 0x01)                                  // RXRDY
            {
               //receive fifo
               dev_at91sam9261_uart_dbg_x_fifo_rcv(desc_uart_dbg);
            }
            recv_intr_cond  = usart_csr & *(p_adr+US_IMR) & (0x00000001);
         }
         //
         if( (desc_w>=0) && (ofile_lst[desc_w].oflag & O_WRONLY) ) {
            // TXRDY
            if (((usart_csr & 0x00000002) == 0x00000002) && (p_inf_uart->XMIT)) {
               //Transmission (per byte)
               //dev_at91sam9261_uart_x_snd(desc);
               dev_at91sam9261_uart_dbg_x_snd(desc_uart_dbg);
            }
            //
            trans_intr_cond = ( (p_inf_uart->XMIT) &&
                                (usart_csr & *(p_adr+US_IMR) & (0x00000002)));
         }
      } while ( recv_intr_cond || trans_intr_cond );
   }
   // Timer Value initialization for Read
   p_inf_uart->inter_char_timer=ttys_termios.c_cc[VTIME];

   //*AT91C_AIC_EOICR = 0; // Signal end of interrupt to AIC.
   //ACK de l'IT au CPU
   cyg_interrupt_acknowledge(vector);
   //Informe kernel d'exécuter DSR
   return(CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}


void dev_at91sam9261_uart_dbg_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
   board_inf_uart_t * p_inf_uart = (board_inf_uart_t *)ofile_lst[desc_uart_dbg].p;
   //
   if(rcv_flag) {
      __fire_io_int(ofile_lst[p_inf_uart->desc_rd].owner_pthread_ptr_read);
      rcv_flag = 0;
   }
   #if defined(USE_DMA_UART_SND)
   if (p_inf_uart->desc_wr  >= 0) {
      __fire_io_int(ofile_lst[p_inf_uart->desc_wr].owner_pthread_ptr_write);
   }
   #else
   if ( (p_inf_uart->desc_wr  >= 0) && (p_inf_uart->output_r == p_inf_uart->output_w) ) {
      __fire_io_int(ofile_lst[p_inf_uart->desc_wr].owner_pthread_ptr_write);
   }
   #endif
   //autorise à nouveau les IT
   cyg_interrupt_unmask(vector);
}
#endif

int dev_at91sam9261_uart_dbg_load (void)
{
   p_board_inf_uart_dbg                    = (board_inf_uart_t *)malloc(sizeof(board_inf_uart_t));
   memset(p_board_inf_uart_dbg,0,sizeof(board_inf_uart_t));
#if defined(__KERNEL_UCORE_EMBOS) || defined(__KERNEL_UCORE_FREERTOS)
   p_board_inf_uart_dbg->f_timer_call_back = dev_at91sam9261_uart_dbg_timer_callback;
#endif
   p_board_inf_uart_dbg->loaded = 0;

   dev_at91sam9261_uart_dbg_x_load (p_board_inf_uart_dbg);
}

/*-------------------------------------------
| Name:dev_at91sam9261_uart_dbg_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_at91sam9261_uart_dbg_open(desc_t desc, int o_flag)
{
#if defined(__KERNEL_UCORE_ECOS)
   cyg_vector_t serial_vector = CYGNUM_HAL_INT_SERIAL;
   cyg_priority_t serial_prior = CYGNUM_HAL_H_PRIOR;
#endif

   int ret;
   // Uart1 informations
   ofile_lst[desc].p = (board_inf_uart_t *)p_board_inf_uart_dbg;

   ((board_inf_uart_t *)ofile_lst[desc].p)->base_adr    = (AT91_REG *)AT91C_DBGU_CR; //0xFFFFF200
   ((board_inf_uart_t *)ofile_lst[desc].p)->periph_id   = AT91C_ID_SYS; // system
   ((board_inf_uart_t *)ofile_lst[desc].p)->ind_pio_rxd = 9; // PA9/DRXD
   ((board_inf_uart_t *)ofile_lst[desc].p)->ind_pio_txd = 10; // PA10/DTXD

   // Switch on descriptor type (R/W) and save it
   if (o_flag & O_RDONLY)
   {
      ((board_inf_uart_t *)ofile_lst[desc].p)->desc_r = desc;
   }

   if (o_flag & O_WRONLY)
   {
      ((board_inf_uart_t *)ofile_lst[desc].p)->desc_w = desc;
   }

   // save
   if(desc_uart_dbg<0)
      desc_uart_dbg = desc;

   // call uart common Api open
   ret = dev_at91sam9261_uart_dbg_x_open(desc, o_flag);

#if defined(__KERNEL_UCORE_EMBOS) || defined(__KERNEL_UCORE_FREERTOS)
   // Usart debug interrupt function initialization
   g_p_fct_dbg_interrupt = dev_at91sam9261_uart_dbg_interrupt;
#elif defined(__KERNEL_UCORE_ECOS)
   //Primitive de creation de l'IT au chargement du driver
   cyg_interrupt_create(serial_vector, serial_prior, 0,
                        &dev_at91sam9261_uart_dbg_isr, &dev_at91sam9261_uart_dbg_dsr,
                        &_at91sam9261_uart_dbg_handle, &_at91sam9261_uart_dbg_it);
   //Liaison entre l'IT crée et le vecteur d'IT
   cyg_interrupt_attach(_at91sam9261_uart_dbg_handle);
   cyg_interrupt_unmask(serial_vector);
#endif

   return 0;
}

/** @} */
/** @} */
/** @} */

/*============================================
| End of Source  : dev_at91sam9261_uart_dbg.c
==============================================*/
