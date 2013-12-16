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
 * \defgroup dev_uart_arm9_c Les uart de l'arm9 at91sam9261.
 * @{
 *
 * Les pilotes de p�riph�riques pour les uart de l'arm9 at91sam9261
 * Ces pilotes de p�riph�riques g�re les vitesses de 50 � 38400 bauds
 * et supporte le timeout (VTIME) pour le mode bloquant.
 *
 */


/**
 * \file
 * implementation du pilote de p�riph�rique pour l'uart 1 de l'arm9 at91sam9261
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

#include "dev_at91sam9261_uart_1.h"

#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)
   #include <atmel/ioat91sam9261.h>
#else
   #include "cyg/hal/at91sam9261.h"
   #include <string.h>
   #include <stdlib.h>
#endif

/*===========================================
Global Declaration
=============================================*/
const char dev_at91sam9261_uart_1_name[]="ttys1\0";

// extern
extern int dev_at91sam9261_uart_x_load        (board_inf_uart_t *);
extern int dev_at91sam9261_uart_x_open        (desc_t, int );
extern int dev_at91sam9261_uart_x_close       (desc_t);
extern int dev_at91sam9261_uart_x_isset_read  (desc_t);
extern int dev_at91sam9261_uart_x_isset_write (desc_t);
extern int dev_at91sam9261_uart_x_read        (desc_t, char *,int);
extern int dev_at91sam9261_uart_x_write       (desc_t, const char *, int);
extern int dev_at91sam9261_uart_x_seek        (desc_t, int, int);
extern int dev_at91sam9261_uart_x_ioctl       (desc_t, int, va_list);
//extern int dev_at91sam9261_uart_x_interrupt   (desc_t);
extern int termios2ttys                       (struct termios *);

#if defined(__KERNEL_UCORE_EMBOS)

extern void dev_at91sam9261_uart_x_interrupt   (/*desc_t*/ board_inf_uart_t *);
extern void dev_at91sam9261_uart_x_timer_callback(board_inf_uart_t *);

static void dev_at91sam9261_uart_1_interrupt  (void);

#elif defined(__KERNEL_UCORE_ECOS)
extern cyg_uint32 dev_at91sam9261_uart_isr(cyg_vector_t vector, cyg_addrword_t data);
extern void dev_at91sam9261_uart_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
extern void dev_at91sam9261_uart_x_timer_callback(alrm_hdl_t alarm_handle, cyg_addrword_t data);

   #define CYGNUM_HAL_H_PRIOR 3
static cyg_handle_t _at91sam9261_uart_handle;
static cyg_interrupt _at91sam9261_uart_it;
extern struct termios ttys_termios;

#endif

// statics
static int dev_at91sam9261_uart_1_load        (void);
int dev_at91sam9261_uart_1_open        (desc_t, int);

desc_t desc_uart_1;

static volatile board_inf_uart_t board_inf_uart_1 __attribute__ ((section (".no_cache")));
static board_inf_uart_t *p_board_inf_uart_1=(void*)0;   // ADU

// uart_1 Functions pointers
dev_map_t dev_at91sam9261_uart_1_map={
   dev_at91sam9261_uart_1_name,
   S_IFCHR,
   dev_at91sam9261_uart_1_load,
   dev_at91sam9261_uart_1_open,
   dev_at91sam9261_uart_x_close,
   dev_at91sam9261_uart_x_isset_read,
   dev_at91sam9261_uart_x_isset_write,
   dev_at91sam9261_uart_x_read,
   dev_at91sam9261_uart_x_write,
   dev_at91sam9261_uart_x_seek,
   dev_at91sam9261_uart_x_ioctl
};


/*===========================================
Implementation
=============================================*/
#ifdef __KERNEL_UCORE_EMBOS
/*-------------------------------------------
| Name       : dev_at91sam9261_uart_1_interrupt
| Description: Call generic interrupt function
| Parameters : None
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
void dev_at91sam9261_uart_1_interrupt(void)
{
   dev_at91sam9261_uart_x_interrupt(p_board_inf_uart_1);
}

/*-------------------------------------------
| Name       : dev_at91sam9261_uart_1_timer_callback
| Description: Call generic timer callback function
| Parameters : None
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
void dev_at91sam9261_uart_1_timer_callback(void)
{
   dev_at91sam9261_uart_x_timer_callback(p_board_inf_uart_1);
}
#endif

/*-------------------------------------------
| Name       : dev_at91sam9261_uart_1_load
| Description: Call generic load function
| Parameters : None
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam9261_uart_1_load (void)
{
   return dev_at91sam9261_uart_1_load_ex(&board_inf_uart_1);
}

/*-------------------------------------------
| Name       : dev_at91sam9261_uart_1_load_ex
| Description: Call generic load function
| Parameters : None
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam9261_uart_1_load_ex(board_inf_uart_t* p_board_inf_uart){

   if(!p_board_inf_uart)
      p_board_inf_uart_1  = (board_inf_uart_t *)malloc(sizeof(board_inf_uart_t));
   else
      p_board_inf_uart_1  = p_board_inf_uart;

#if defined(__KERNEL_UCORE_EMBOS)
   p_board_inf_uart_1->f_timer_call_back = dev_at91sam9261_uart_1_timer_callback;
#else
   p_board_inf_uart_1->f_timer_call_back = (void*)0;
#endif

   p_board_inf_uart_1->loaded = 0; // Not loaded

   return dev_at91sam9261_uart_x_load (p_board_inf_uart_1);
}



/*-------------------------------------------
| Name       : dev_at91sam9261_uart_1_open
| Description: Opening uart device
|              call by Open Posix interface
| Parameters : desc_t desc -> descriptor
|
| Return Type:
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam9261_uart_1_open (desc_t desc, int o_flag)
{
#if defined(__KERNEL_UCORE_ECOS)
   cyg_vector_t serial_vector = CYGNUM_HAL_INTERRUPT_USART1;
   cyg_priority_t serial_prior = CYGNUM_HAL_H_PRIOR;
#endif

   // Uart1 informations
   ofile_lst[desc].p = (board_inf_uart_t *)p_board_inf_uart_1;

   // Specific fields
   ((board_inf_uart_t *)ofile_lst[desc].p)->base_adr    = (AT91_REG *)AT91C_US1_CR;
   ((board_inf_uart_t *)ofile_lst[desc].p)->periph_id   = AT91C_ID_US1;
   ((board_inf_uart_t *)ofile_lst[desc].p)->ind_pio_rxd = 13; // PC13/RXD1
   ((board_inf_uart_t *)ofile_lst[desc].p)->ind_pio_txd = 12; // PC12/TXD1

   // Save o_flag
   ((board_inf_uart_t *)ofile_lst[desc].p)->o_flag |= o_flag;

   // Switch on o_flag type and save it
   if (o_flag & O_RDONLY)
   {
      ((board_inf_uart_t *)ofile_lst[desc].p)->desc_r = desc;
   }

   if (o_flag & O_WRONLY)
   {
      ((board_inf_uart_t *)ofile_lst[desc].p)->desc_w = desc;
   }

   // save descriptor
   desc_uart_1 = desc;

   // call uart common Api open
   int ret = dev_at91sam9261_uart_x_open(desc, o_flag);

   //create interrupt a first time
   if((ofile_lst[desc].nb_reader==0)&&(ofile_lst[desc].nb_writer==0))
   {
#ifdef __KERNEL_UCORE_EMBOS
      // Usart 1 interrupt vector.
      AT91C_AIC_SVR[AT91C_ID_US1] = (unsigned long)&dev_at91sam9261_uart_1_interrupt;
#elif defined(__KERNEL_UCORE_ECOS)
      //Primitive de creation de l'IT au chargement du driver
      cyg_interrupt_create(serial_vector, serial_prior, (cyg_addrword_t)ofile_lst[desc].p,
                           &dev_at91sam9261_uart_isr, &dev_at91sam9261_uart_dsr,
                           &_at91sam9261_uart_handle, &_at91sam9261_uart_it);
      //Liaison entre l'IT crée et le vecteur d'IT
      cyg_interrupt_attach(_at91sam9261_uart_handle);
      cyg_interrupt_unmask(serial_vector);
#endif
   }

   return 0;
}
/** @} */
/** @} */
/** @} */

/*============================================
| End of Source  : dev_at91sam9261_uart_1.c
==============================================*/
