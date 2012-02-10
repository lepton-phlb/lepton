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
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "kernel/core/cpu.h"
#include "kernel/core/core_rttimer.h"
#include "kernel/fs/vfs/vfsdev.h"

#include "dev_k60n512_dspi_x.h"
#include "kernel/dev/arch/cortexm/k60n512/common/dev_k60n512_sim.h"
/*===========================================
Global Declaration
=============================================*/
const char dev_k60n512_dspi_0_name[]="dspi0\0";

static int dev_k60n512_dspi_0_load(void);
static int dev_k60n512_dspi_0_open(desc_t desc, int o_flag);

extern int dev_k60n512_dspi_x_load(board_kinetis_dspi_info_t * kinetis_dspi_info);
extern int dev_k60n512_dspi_x_open(desc_t desc, int o_flag,
                                   board_kinetis_dspi_info_t * kinetis_dspi_info);
extern int dev_k60n512_dspi_x_close(desc_t desc);
extern int dev_k60n512_dspi_x_read(desc_t desc, char* buf,int cb);
extern int dev_k60n512_dspi_x_write(desc_t desc, const char* buf,int cb);
extern int dev_k60n512_dspi_x_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_k60n512_dspi_0_map={
   dev_k60n512_dspi_0_name,
   S_IFCHR,
   dev_k60n512_dspi_0_load,
   dev_k60n512_dspi_0_open,
   dev_k60n512_dspi_x_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_k60n512_dspi_x_read,
   dev_k60n512_dspi_x_write,
   __fdev_not_implemented,
   dev_k60n512_dspi_x_ioctl
};

//
#define KINETIS_DSPI0_VECTOR_PRIORITY           3
#define KINETIS_DSPI0_IRQ            27 //26
//#define KINETIS_DSPI0_VECTOR       42

//
#define KINETIS_DSPI0_SPEED      (10000000) //Mhz

//
static unsigned char kinetis_dspi_0_input_buffer[SPI_RX_BUFFER_SIZE];
static unsigned char kinetis_dspi_0_output_buffer[SPI_TX_BUFFER_SIZE];

//
static board_kinetis_dspi_info_t kinetis_dspi_0 = {
   dspi_base : 0x4002c000,
   desc_r : -1,
   desc_w : -1,
   irq_no : KINETIS_DSPI0_IRQ,
   irq_prio : KINETIS_DSPI0_VECTOR_PRIORITY,
   input_buffer : kinetis_dspi_0_input_buffer,
   input_r : -1,
   input_w : -1,
   output_buffer : kinetis_dspi_0_output_buffer,
   output_r : -1,
   output_w : -1,
   clk : KINETIS_DSPI0_SPEED,
   clk_phase : SPI_CTAR_CLK_POL_PHA_MODE0,
   mode : SPI_MODE_MASTER,
   cs : 0,
   irq_handle : 0
};

//
#define DSPI0_CS CYGHWR_HAL_KINETIS_PIN(A, 14, 2, 0)
#define DSPI0_CLK CYGHWR_HAL_KINETIS_PIN(A, 15, 2, 0)
#define DSPI0_MOSI CYGHWR_HAL_KINETIS_PIN(A, 16, 2, 0)
#define DSPI0_MISO CYGHWR_HAL_KINETIS_PIN(A, 17, 2, 0)

static void _kinetis_dspi0_configure_pins(void);
/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_k60n512_dspi_0_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_dspi_0_load(void) {
   volatile unsigned int reg_val = 0;
   _kinetis_dspi0_configure_pins();

   //enable clock gating (SIM->SCGC6 |= SIM_SCGC6_DSPI0_MASK)
   HAL_READ_UINT32(REG_SIM_SCGC6_ADDR, reg_val);
   reg_val |= REG_SIM_SCGC6_DSPI0_MASK;
   HAL_WRITE_UINT32(REG_SIM_SCGC6_ADDR, reg_val);

   return dev_k60n512_dspi_x_load(&kinetis_dspi_0);
}

/*-------------------------------------------
| Name:dev_k60n512_dspi_0_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_dspi_0_open(desc_t desc, int o_flag) {
   return dev_k60n512_dspi_x_open(desc, o_flag, &kinetis_dspi_0);
}

/*-------------------------------------------
| Name:_kinetis_dspi0_configure_pins
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _kinetis_dspi0_configure_pins(void) {
   hal_set_pin_function(DSPI0_CS);
   hal_set_pin_function(DSPI0_CLK);
   hal_set_pin_function(DSPI0_MOSI);
   hal_set_pin_function(DSPI0_MISO);
}

/*============================================
| End of Source  : dev_k60n512_dspi_0.c
==============================================*/
