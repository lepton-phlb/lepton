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

#include "kernel/dev/arch/cortexm/k60n512/dev_k60n512_i2c_x/dev_k60n512_i2c_x.h"
#include "kernel/dev/arch/cortexm/k60n512/common/dev_k60n512_sim.h"
/*===========================================
Global Declaration
=============================================*/
const char dev_twrk60n512_i2c_0_mma7660_name[]="i2c1\0";

static int dev_twrk60n512_i2c_0_mma7660_load(void);
static int dev_twrk60n512_i2c_0_mma7660_open(desc_t desc, int o_flag);

extern int dev_k60n512_i2c_x_mma7660_load(board_kinetis_i2c_info_t * kinetis_i2c_info);
extern int dev_k60n512_i2c_x_mma7660_open(desc_t desc, int o_flag, board_kinetis_i2c_info_t * kinetis_i2c_info);
extern int dev_k60n512_i2c_x_mma7660_close(desc_t desc);
extern int dev_k60n512_i2c_x_mma7660_read(desc_t desc, char* buf,int cb);
extern int dev_k60n512_i2c_x_mma7660_write(desc_t desc, const char* buf,int cb);
extern int dev_k60n512_i2c_x_mma7660_ioctl(desc_t desc,int request,va_list ap);

dev_map_t dev_twrk60n512_i2c_0_mma7660_map={
   dev_twrk60n512_i2c_0_mma7660_name,
   S_IFCHR,
   dev_twrk60n512_i2c_0_mma7660_load,
   dev_twrk60n512_i2c_0_mma7660_open,
   dev_k60n512_i2c_x_mma7660_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_k60n512_i2c_x_mma7660_read,
   dev_k60n512_i2c_x_mma7660_write,
   __fdev_not_implemented,
   dev_k60n512_i2c_x_mma7660_ioctl
};

//slave address
#define MMA7660_I2C_ADDRESS      0x4C

//
static board_kinetis_i2c_info_t kinetis_i2c_0_mma7660 = {
   i2c_base : 0x40066000,
   desc_r : -1,
   desc_w : -1,
   speed : 800000, //Hz
   slave_id : MMA7660_I2C_ADDRESS
};

#define I2C0_SCL CYGHWR_HAL_KINETIS_PIN(D, 8, 2, 0)
#define I2C0_SDA CYGHWR_HAL_KINETIS_PIN(D, 9, 2, 0)

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_k60n512_a25410_dspi_0_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_twrk60n512_i2c_0_mma7660_load(void) {
   volatile unsigned int reg_val = 0;
   
   hal_set_pin_function(I2C0_SCL);
   hal_set_pin_function(I2C0_SDA);
   
   //enable clock gating (SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK)
   HAL_READ_UINT32(REG_SIM_SCGC4_ADDR, reg_val);
   reg_val |= REG_SIM_SCGC4_I2C0_MASK;
   HAL_WRITE_UINT32(REG_SIM_SCGC4_ADDR, reg_val);
   
   return dev_k60n512_i2c_x_mma7660_load(&kinetis_i2c_0_mma7660);
}

/*-------------------------------------------
| Name:dev_k60n512_a25410_dspi_0_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_twrk60n512_i2c_0_mma7660_open(desc_t desc, int o_flag) {
   return dev_k60n512_i2c_x_mma7660_open(desc, o_flag, &kinetis_i2c_0_mma7660);
}

/*============================================
| End of Source  : dev_k60n512_i2c_0_mma7660.c
==============================================*/
