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
 * \addtogroup hard_dev_at91sam7s 
 * @{
 *
 */

/**
 * \defgroup dev_uart_arm7_c Les uart de l'arm7 at91sam7s.
 * @{
 *
 * Les pilotes de périphériques pour les uart de l'arm7 at91sam7s..
 * Ces pilotes de périphériques gère les vitesses de 50 à 38400 bauds
 * et supporte le timeout (VTIME) pour le mode bloquant.
 *    
 */


/**
 * \file
 * implementation du pilote de périphérique pour l'uart 0 de l'arm7 at91sam7s.
 * \author philippe le boulanger
 */

/*===========================================
Includes
=============================================*/
#include "kernel/types.h"
#include "kernel/interrupt.h"
#include "kernel/kernelconf.h"
#include "kernel/kernel.h"
#include "kernel/system.h"
#include "kernel/fcntl.h"
#include "kernel/cpu.h"

#include "termios/termios.h"

#include "vfs/vfsdev.h"

#include "dev/arch/arm7/at91sam7se/dev_at91sam7s_uart_sx/dev_at91sam7s_uart_sx.h"
#include "dev/arch/arm7/at91sam7se/common/dev_at91sam7s_common_uart.h"

#include <ioat91sam7se512.h>
#include <intrinsic.h>

/*===========================================
Global Declaration
=============================================*/
const char dev_at91sam7s_uart_s0_name[]="ttys0\0";

// extern 
extern int dev_at91sam7s_uart_sx_load        (board_inf_uart_t *);
extern int dev_at91sam7s_uart_sx_open        (desc_t, int );
extern int dev_at91sam7s_uart_sx_close       (desc_t);
extern int dev_at91sam7s_uart_sx_isset_read  (desc_t);
extern int dev_at91sam7s_uart_sx_isset_write (desc_t);
extern int dev_at91sam7s_uart_sx_read        (desc_t, char *,int);
extern int dev_at91sam7s_uart_sx_write       (desc_t, const char *, int);
extern int dev_at91sam7s_uart_sx_seek        (desc_t, int, int);
extern int dev_at91sam7s_uart_sx_ioctl       (desc_t, int, va_list);
extern int dev_at91sam7s_uart_sx_interrupt   (desc_t);
extern int termios2ttys                      (struct termios *);
extern void dev_at91sam7s_uart_sx_timer_callback(board_inf_uart_t *);

// statics
static int dev_at91sam7s_uart_s0_load        (void);
static int dev_at91sam7s_uart_s0_open        (desc_t, int);
static void dev_at91sam7s_uart_s0_interrupt  (void);

desc_t desc_uart_s0;

/*static*/ board_inf_uart_t *p_board_inf_uart_s0;

// uart_s0 Functions pointers
dev_map_t dev_at91sam7s_uart_s0_map={
   dev_at91sam7s_uart_s0_name,
   S_IFCHR,
   dev_at91sam7s_uart_s0_load,
   dev_at91sam7s_uart_s0_open,
   dev_at91sam7s_uart_sx_close,
   dev_at91sam7s_uart_sx_isset_read,
   dev_at91sam7s_uart_sx_isset_write,
   dev_at91sam7s_uart_sx_read,
   dev_at91sam7s_uart_sx_write,
   dev_at91sam7s_uart_sx_seek,
   dev_at91sam7s_uart_sx_ioctl
};


/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name       : dev_at91sam7s_uart_s0_interrupt
| Description: Call generic interrupt function
| Parameters : None
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
void dev_at91sam7s_uart_s0_interrupt(void)
{
  dev_at91sam7s_uart_sx_interrupt(desc_uart_s0);
}


/*-------------------------------------------
| Name       : dev_at91sam7s_uart_s0_timer_callback
| Description: Call generic timer callback function
| Parameters : None
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
void dev_at91sam7s_uart_s0_timer_callback(void)
{
  dev_at91sam7s_uart_sx_timer_callback(p_board_inf_uart_s0);
}


/*-------------------------------------------
| Name       : dev_at91sam7s_uart_s0_load
| Description: Call generic load function
| Parameters : None
| Return Type: None
| Comments   : -
| See        : -
---------------------------------------------*/
int dev_at91sam7s_uart_s0_load (void)
{      
  p_board_inf_uart_s0  = (board_inf_uart_t *)malloc(sizeof(board_inf_uart_t));
  p_board_inf_uart_s0->f_timer_call_back = dev_at91sam7s_uart_s0_timer_callback; 
  p_board_inf_uart_s0->dev_at91sam7s_uart_sx_loaded = 0;
  
  dev_at91sam7s_uart_sx_load (p_board_inf_uart_s0); 
}
  

/*-------------------------------------------
| Name       : dev_at91sam7s_uart_s0_open
| Description: Opening uart device
|              call by Open Posix interface
| Parameters : desc_t desc -> descriptor
|
| Return Type:
| Comments   : - 
| See        : - 
---------------------------------------------*/
int dev_at91sam7s_uart_s0_open (desc_t desc, int o_flag)
{     
  // Uart0 informations    
  ofile_lst[desc].p = (board_inf_uart_t *)p_board_inf_uart_s0;
  
  // Specific fields
  ((board_inf_uart_t *)ofile_lst[desc].p)->base_adr    = (AT91_REG *)AT91C_US0_CR; 
  ((board_inf_uart_t *)ofile_lst[desc].p)->periph_id   = 6; // AT91C_ID_US0 
  ((board_inf_uart_t *)ofile_lst[desc].p)->ind_pio_rxd = 5; // PA5/RXD0
  ((board_inf_uart_t *)ofile_lst[desc].p)->ind_pio_txd = 6; // PA6/TXD0
  
  // Save o_flag
  ((board_inf_uart_t *)ofile_lst[desc].p)->o_flag = desc;
  
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
  desc_uart_s0 = desc;
  
  // call uart common Api open 
  int ret = dev_at91sam7s_uart_sx_open(desc, o_flag);
  
  // Usart 0 interrupt vector.
  AT91C_AIC_SVR[AT91C_ID_US0] = (unsigned long)&dev_at91sam7s_uart_s0_interrupt;     
  return 0;
}

/** @} */
/** @} */
/** @} */

/*============================================
| End of Source  : dev_at91sam7s_uart_s0.c
==============================================*/
