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

#include "lib/libc/termios/termios.h"
#include "kernel/dev/arch/cortexm/k60n512/dev_k60n512_i2c_x/dev_k60n512_i2c_x.h"
/*===========================================
Global Declaration
=============================================*/
int dev_k60n512_i2c_x_mma7660_load(board_kinetis_i2c_info_t * kinetis_i2c_info);
int dev_k60n512_i2c_x_mma7660_open(desc_t desc, int o_flag, board_kinetis_i2c_info_t * kinetis_i2c_info);

//
int dev_k60n512_i2c_x_mma7660_close(desc_t desc);
int dev_k60n512_i2c_x_mma7660_seek(desc_t desc,int offset,int origin);
int dev_k60n512_i2c_x_mma7660_read(desc_t desc, char* buf,int cb);
int dev_k60n512_i2c_x_mma7660_write(desc_t desc, const char* buf,int cb);
int dev_k60n512_i2c_x_mma7660_ioctl(desc_t desc,int request,va_list ap);
int dev_k60n512_i2c_x_mma7660_seek(desc_t desc,int offset,int origin);

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_k60n512_i2c_x_mma7660_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_mma7660_load(board_kinetis_i2c_info_t * kinetis_i2c_info){
   HAL_WRITE_UINT8(kinetis_i2c_info->i2c_base + REG_I2Cx_F,
   _kinetis_i2c_x_find_baudrate(48000000, kinetis_i2c_info->speed));
   
   HAL_WRITE_UINT8(kinetis_i2c_info->i2c_base + REG_I2Cx_C1, REG_I2Cx_C1_IICEN_MASK);
   
	return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_i2c_x_mma7660_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_mma7660_open(desc_t desc, int o_flag, board_kinetis_i2c_info_t * kinetis_i2c_info){
   volatile unsigned short reg_val = 0;
   
	if(o_flag & O_RDONLY){
		if(kinetis_i2c_info->desc_r<0) {
			kinetis_i2c_info->desc_r = desc;
		}
		else
			return -1; //already open
	}

	if(o_flag & O_WRONLY){
		if(kinetis_i2c_info->desc_w<0) {
			kinetis_i2c_info->desc_w = desc;
		}
		else
			return -1; //already open
	}
	
	if(!ofile_lst[desc].p)
		ofile_lst[desc].p=kinetis_i2c_info;

	return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_i2c_x_mma7660_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_mma7660_close(desc_t desc){
	//nothing to do
	//save code space :)
	return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_i2c_x_mma7660_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_mma7660_read(desc_t desc, char* buf,int size){
	board_kinetis_i2c_info_t * p_i2c_info = (board_kinetis_i2c_info_t*)ofile_lst[desc].p;
   volatile unsigned char reg_val = 0;
   
   if(!p_i2c_info) {
      return -1;
   }
   
   // Send Slave Address
   _kinetis_i2c_x_start_transmission(p_i2c_info, M_WR_S_RD);
   __i2c_x_wait(p_i2c_info);

   // Write Register Address => buf[0] : register addr
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_D, buf[0]);
   __i2c_x_wait(p_i2c_info);

   // Do a repeated start
   HAL_READ_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, reg_val);
   reg_val |= REG_I2Cx_C1_RSTA_MASK;
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, reg_val);
   
   // Send Slave Address
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_D, 
   (p_i2c_info->slave_id << 1) | 0x1);
   __i2c_x_wait(p_i2c_info);

   // Put in Rx Mode
   HAL_READ_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, reg_val);
   reg_val &= ~REG_I2Cx_C1_TX_MASK;
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, reg_val);

   // Turn off ACK since this is second to last byte being read
   HAL_READ_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, reg_val);
   reg_val |= REG_I2Cx_C1_TXAK_MASK;
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, reg_val);
   
   // Dummy read
   HAL_READ_UINT8(p_i2c_info->i2c_base + REG_I2Cx_D, reg_val);
   __i2c_x_wait(p_i2c_info);

   // Send stop since about to read last byte
   __i2c_x_stop(p_i2c_info);

   //Read byte
   HAL_READ_UINT8(p_i2c_info->i2c_base + REG_I2Cx_D, buf[1]);

   return size;
}

/*-------------------------------------------
| Name:dev_k60n512_i2c_x_mma7660_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_mma7660_write(desc_t desc, const char* buf,int size){
   board_kinetis_i2c_info_t * p_i2c_info = (board_kinetis_i2c_info_t*)ofile_lst[desc].p;
   
   if(!p_i2c_info) {
      return -1;
   }
   
   // send data to slave
   _kinetis_i2c_x_start_transmission(p_i2c_info, M_WR_S_RD);
   __i2c_x_wait(p_i2c_info);

   //buf[0] : register addr
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_D, buf[0]);
   __i2c_x_wait(p_i2c_info);

   //buf[1] : data
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_D, buf[1]);
   __i2c_x_wait(p_i2c_info);

   __i2c_x_stop(p_i2c_info);
   __i2c_x_pause(10);
   
   return size;
}

/*-------------------------------------------
| Name:dev_k60n512_i2c_x_mma7660_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_mma7660_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_i2c_x_mma7660_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_mma7660_ioctl(desc_t desc,int request,va_list ap) {
   return 0;
}

/*============================================
| End of Source  : dev_k60n512_i2c_x_mma7660.c
==============================================*/
