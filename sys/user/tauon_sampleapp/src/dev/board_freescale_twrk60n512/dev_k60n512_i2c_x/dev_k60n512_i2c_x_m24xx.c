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

#include "lib/libc/termios/termios.h"

#include "kernel/dev/arch/cortexm/k60n512/dev_k60n512_i2c_x/dev_k60n512_i2c_x.h"
/*===========================================
Global Declaration
=============================================*/
int dev_k60n512_i2c_x_m24xx_load(board_kinetis_i2c_info_t * kinetis_i2c_info);
int dev_k60n512_i2c_x_m24xx_open(desc_t desc, int o_flag, board_kinetis_i2c_info_t * kinetis_i2c_info);

//
int dev_k60n512_i2c_x_m24xx_close(desc_t desc);
int dev_k60n512_i2c_x_m24xx_seek(desc_t desc,int offset,int origin);
int dev_k60n512_i2c_x_m24xx_read(desc_t desc, char* buf,int cb);
int dev_k60n512_i2c_x_m24xx_write(desc_t desc, const char* buf,int cb);
int dev_k60n512_i2c_x_m24xx_ioctl(desc_t desc,int request,va_list ap);
int dev_k60n512_i2c_x_m24xx_seek(desc_t desc,int offset,int origin);

//
static int _kinetis_i2c_x_m24xx_read(board_kinetis_i2c_info_t * p_i2c_info, unsigned char* data,
unsigned int data_len, unsigned char addr_size);
static int _kinetis_i2c_x_m24xx_write(board_kinetis_i2c_info_t * p_i2c_info, unsigned char* data, 
unsigned int data_len, unsigned char addr_size);

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_k60n512_i2c_x_m24xx_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_m24xx_load(board_kinetis_i2c_info_t * kinetis_i2c_info){
   HAL_WRITE_UINT8(kinetis_i2c_info->i2c_base + REG_I2Cx_F,
   _kinetis_i2c_x_find_baudrate(48000000, kinetis_i2c_info->speed));
   
	return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_i2c_x_m24xx_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_m24xx_open(desc_t desc, int o_flag, board_kinetis_i2c_info_t * kinetis_i2c_info){
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
| Name:dev_k60n512_i2c_x_m24xx_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_m24xx_close(desc_t desc){
	//nothing to do
	//save code space :)
	return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_i2c_x_m24xx_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_m24xx_read(desc_t desc, char* buf,int size){
	board_kinetis_i2c_info_t * p_i2c_info = (board_kinetis_i2c_info_t*)ofile_lst[desc].p;
   unsigned int data_len = size-2-buf[1];
   
   if(!p_i2c_info) {
      return -1;
   }
   
   return _kinetis_i2c_x_m24xx_read(p_i2c_info, (unsigned char *)&buf[2], data_len, buf[1]);
}

/*-------------------------------------------
| Name:dev_k60n512_i2c_x_m24xx_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_m24xx_write(desc_t desc, const char* buf,int size){
   board_kinetis_i2c_info_t * p_i2c_info = (board_kinetis_i2c_info_t*)ofile_lst[desc].p;
   unsigned int data_len = size-2;
   
   if(!p_i2c_info) {
      return -1;
   }
   
   return _kinetis_i2c_x_m24xx_write(p_i2c_info, (unsigned char *)&buf[2], data_len, buf[1]);
}

/*-------------------------------------------
| Name:dev_k60n512_i2c_x_m24xx_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_m24xx_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_i2c_x_m24xx_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_i2c_x_m24xx_ioctl(desc_t desc,int request,va_list ap) {
   return 0;
}

/*-------------------------------------------
| Name:_kinetis_i2c_x_m24xx_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kinetis_i2c_x_m24xx_read(board_kinetis_i2c_info_t * p_i2c_info, unsigned char* data,
unsigned int data_len, unsigned char addr_size) {
   volatile unsigned char reg_val = 0;
   unsigned int i=0;
   
   //enable i2c
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, REG_I2Cx_C1_IICEN_MASK);
   
   // send Slave Address
   _kinetis_i2c_x_start_transmission(p_i2c_info, M_WR_S_RD);
   __i2c_x_wait(p_i2c_info);
   
   // write Register Address => buf[0] : register addr
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_D, data[i++]);
   __i2c_x_wait(p_i2c_info);
   
   HAL_DELAY_US(30);
    
   // second register addr
   if(addr_size > 1) {
      HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_D, data[i++]);
      __i2c_x_wait(p_i2c_info);      
   }
   
   HAL_DELAY_US(30);
   
   // do a repeated start
   HAL_READ_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, reg_val);
   reg_val |= REG_I2Cx_C1_RSTA_MASK;
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, reg_val);
   
   // send Slave Address
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_D, 
   (p_i2c_info->slave_id << 1) | M_RD_S_WR);
   __i2c_x_wait(p_i2c_info);
      
   HAL_DELAY_US(30);
   
   // put in Rx Mode
   HAL_READ_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, reg_val);
   reg_val &= ~REG_I2Cx_C1_TX_MASK;
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, reg_val);
   
   HAL_DELAY_US(30);
   
   //dummy read
   HAL_READ_UINT8(p_i2c_info->i2c_base + REG_I2Cx_D, reg_val);
   __i2c_x_wait(p_i2c_info);
   HAL_DELAY_US(30);
   
   //how much data to read
   for(i=2; i<(2+data_len); i++) {
      HAL_READ_UINT8(p_i2c_info->i2c_base + REG_I2Cx_S, reg_val);
      
      if(reg_val & REG_I2Cx_S_RXAK_MASK) {
         __i2c_x_stop(p_i2c_info);
         __i2c_x_wait_busy(p_i2c_info);
         
         HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, 0);
         return -1;
      }
      else {
         HAL_READ_UINT8(p_i2c_info->i2c_base + REG_I2Cx_D, data[i]);
         __i2c_x_wait(p_i2c_info);
         HAL_DELAY_US(30);
      }
   }

   //send nack
   HAL_READ_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, reg_val);
   reg_val |= REG_I2Cx_C1_TXAK_MASK;
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, reg_val);
   
   //read last
   HAL_READ_UINT8(p_i2c_info->i2c_base + REG_I2Cx_D, data[i]);
   __i2c_x_wait(p_i2c_info);
   
   HAL_DELAY_US(25);
    
   __i2c_x_stop(p_i2c_info);
   __i2c_x_wait_busy(p_i2c_info);
   
   //disable i2c
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, 0);
   
   return data_len;
}

/*-------------------------------------------
| Name:_kinetis_i2c_x_m24xx_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kinetis_i2c_x_m24xx_write(board_kinetis_i2c_info_t * p_i2c_info, unsigned char* data, 
unsigned int data_len,unsigned char addr_size) {
   unsigned int i=0;
   unsigned char reg_val = 0;
   
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, REG_I2Cx_C1_IICEN_MASK);
   
   // send slave addr
   _kinetis_i2c_x_start_transmission(p_i2c_info, M_WR_S_RD);
   __i2c_x_wait(p_i2c_info);
   HAL_DELAY_US(30);
   
   for(i=0; i<data_len; i++) {
      HAL_READ_UINT8(p_i2c_info->i2c_base + REG_I2Cx_S, reg_val);
      
      if(reg_val & REG_I2Cx_S_RXAK_MASK) {
         __i2c_x_stop(p_i2c_info);
         __i2c_x_pause(10);
         
         HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, 0);
         return -1;
      }
      else {
         //transmit data
         HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_D, data[i]);
         __i2c_x_wait(p_i2c_info);
         HAL_DELAY_US(30);   
      }
   }
      
   __i2c_x_stop(p_i2c_info);
   __i2c_x_pause(10);
   
   HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, 0);
   return data_len;
}
/*============================================
| End of Source  : dev_k60n512_i2c_x_m24xx.c
==============================================*/
