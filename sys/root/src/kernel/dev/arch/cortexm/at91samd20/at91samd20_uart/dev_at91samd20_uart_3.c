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

/*===========================================
Includes
=============================================*/
#include "kernel/core/kernelconf.h"
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/cpu.h"
#include "kernel/fs/vfs/vfsdev.h"

#include "kernel/dev/arch/cortexm/at91samd20/at91samd20_xplained_pro_board/at91samd20_xplained_pro_board.h"
#include "kernel/dev/arch/at91/asf/sam0/drivers/sercom/usart/usart.h"
#include "kernel/dev/arch/at91/asf/sam0/drivers/sercom/usart/usart_interrupt.h"
#include "dev_at91samd20_uart_x.h"

/*===========================================
Global Declaration
=============================================*/
const char dev_at91samd20_uart_3_name[]="ttys3\0";

int dev_at91samd20_uart_3_load(void);
int dev_at91samd20_uart_3_open(desc_t desc, int o_flag);

extern int dev_at91samd20_uart_x_load(board_at91samd20_uart_info_t * uart_info);
extern int dev_at91samd20_uart_x_open(desc_t desc, int o_flag, board_at91samd20_uart_info_t * uart_info);
extern int dev_at91samd20_uart_x_close(desc_t desc);
extern int dev_at91samd20_uart_x_read(desc_t desc, char* buf,int cb);
extern int dev_at91samd20_uart_x_write(desc_t desc, const char* buf,int cb);
extern int dev_at91samd20_uart_x_ioctl(desc_t desc,int request,va_list ap);
extern int dev_at91samd20_uart_x_isset_read(desc_t desc);
extern int dev_at91samd20_uart_x_isset_write(desc_t desc);
extern int dev_at91samd20_uart_x_seek(desc_t desc,int offset,int origin);

dev_map_t dev_at91samd20_uart_s3_map={
   dev_at91samd20_uart_3_name,
   S_IFCHR,
   dev_at91samd20_uart_3_load,
   dev_at91samd20_uart_3_open,
   dev_at91samd20_uart_x_close,
   dev_at91samd20_uart_x_isset_read,
   dev_at91samd20_uart_x_isset_write,
   dev_at91samd20_uart_x_read,
   dev_at91samd20_uart_x_write,
   dev_at91samd20_uart_x_seek,
   dev_at91samd20_uart_x_ioctl
};

board_at91samd20_uart_info_t board_at91samd20_uart_info_3;

/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:dev_at91samd20_uart_3_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91samd20_uart_3_load(void) {
   
	struct usart_config config_usart;
   
   board_at91samd20_uart_info_3.desc_r=-1;
   board_at91samd20_uart_info_3.desc_w=-1;
     
   //! [setup_config_defaults]
	usart_get_config_defaults(&config_usart);
   
   //! [setup_change_config]
	config_usart.baudrate    = 115200;
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
   
   //! [setup_set_config]
	while (usart_init((struct usart_module*)&board_at91samd20_uart_info_3,
			EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
	}
 
   //
   return dev_at91samd20_uart_x_load(&board_at91samd20_uart_info_3);
}
   
/*-------------------------------------------
| Name:dev_at91samd20_uart_3_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static int dev_at91samd20_uart_3_open(desc_t desc, int o_flag) {
   return dev_at91samd20_uart_x_open(desc, o_flag, &board_at91samd20_uart_info_3);
}

/*============================================
| End of Source  : dev_at91samd20_uart_3.c
==============================================*/