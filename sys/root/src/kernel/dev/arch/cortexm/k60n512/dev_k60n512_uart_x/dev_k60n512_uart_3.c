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
#include "kernel/core/core_rttimer.h"
#include "kernel/fs/vfs/vfsdev.h"

#include "dev_k60n512_uart_x.h"
#include "kernel/dev/arch/cortexm/k60n512/common/dev_k60n512_sim.h"
/*===========================================
Global Declaration
=============================================*/
const char dev_k60n512_uart_3_name[]="ttys3\0\0";

static int dev_k60n512_uart_3_load(void);
static int dev_k60n512_uart_3_open(desc_t desc, int o_flag);

extern int dev_k60n512_uart_x_load(board_kinetis_uart_info_t * kinetis_uart_info);
extern int dev_k60n512_uart_x_open(desc_t desc, int o_flag,
                                   board_kinetis_uart_info_t * kinetis_uart_info);
extern int dev_k60n512_uart_x_close(desc_t desc);
extern int dev_k60n512_uart_x_read(desc_t desc, char* buf,int cb);
extern int dev_k60n512_uart_x_write(desc_t desc, const char* buf,int cb);
extern int dev_k60n512_uart_x_ioctl(desc_t desc,int request,va_list ap);
extern int dev_k60n512_uart_x_isset_read(desc_t desc);
extern int dev_k60n512_uart_x_isset_write(desc_t desc);
extern int dev_k60n512_uart_x_seek(desc_t desc,int offset,int origin);

///
dev_map_t dev_k60n512_uart_s3_map={
//dev_map_t dev_k60n512_uart_s3_ko_map={
   dev_k60n512_uart_3_name,
   S_IFCHR,
   dev_k60n512_uart_3_load,
   dev_k60n512_uart_3_open,
   dev_k60n512_uart_x_close,
   dev_k60n512_uart_x_isset_read,
   dev_k60n512_uart_x_isset_write,
   dev_k60n512_uart_x_read,
   dev_k60n512_uart_x_write,
   dev_k60n512_uart_x_seek,
   dev_k60n512_uart_x_ioctl
};

//
//
#define KINETIS_UART_IRQ_NO                             52
#define KINETIS_UART_IRQ_PRIORITY               3

//
#define KINETIS_UART_SPEED      (38400)

//
static unsigned char kinetis_uart_3_input_buffer[UART_RX_BUFFER_SIZE];

//
static board_kinetis_uart_info_t kinetis_uart_3 = {
   uart_base : 0x4006d000,
   irq_no : KINETIS_UART_IRQ_NO,
   irq_prio: KINETIS_UART_IRQ_PRIORITY,
   desc_r : -1,
   desc_w : -1,
   input_buffer : kinetis_uart_3_input_buffer,
   input_r : 0,
   input_w : 0,
   output_buffer : NULL,
   output_r : -1,
   output_w : -1,
   xmit : 0,
   speed: KINETIS_UART_SPEED,
   irq_handle : 0
};

//
#define UART3_RX CYGHWR_HAL_KINETIS_PIN(C, 16, 3, 0)
#define UART3_TX CYGHWR_HAL_KINETIS_PIN(C, 17, 3, 0)

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_k60n512_uart_3_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_3_load(void) {
   volatile unsigned int reg_val = 0;

   //configure PINS
   hal_set_pin_function(UART3_RX);
   hal_set_pin_function(UART3_TX);

   //enable clock gating (SIM->SCGC4 |= SIM_SCGC4_UART3_MASK)
   HAL_READ_UINT32(REG_SIM_SCGC4_ADDR, reg_val);
   reg_val |= REG_SIM_SCGC4_UART3_MASK;
   HAL_WRITE_UINT32(REG_SIM_SCGC4_ADDR, reg_val);

   return dev_k60n512_uart_x_load(&kinetis_uart_3);
}

/*-------------------------------------------
| Name:dev_k60n512_uart_3_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_uart_3_open(desc_t desc, int o_flag) {
   return dev_k60n512_uart_x_open(desc, o_flag, &kinetis_uart_3);
}

/*============================================
| End of Source  : dev_k60n512_uart_3.c
==============================================*/

