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

#include "dev_k60n512_i2c_x.h"
#include "kernel/dev/arch/cortexm/k60n512/common/dev_k60n512_sim.h"
/*===========================================
Global Declaration
=============================================*/
static const unsigned int baudrate_mult[] = { 1, 2, 4 };
static const unsigned int baudrate_icr[]= {  20, 22, 24, 26, 28, 30, 34, 40, 28, 32, 
    36, 40, 44, 48, 56, 68, 48, 56, 64, 72, 80, 88, 104, 128, 80, 96, 112, 128,
    144, 160, 192, 240, 160, 192, 224, 256, 288, 320, 384, 480, 320, 384, 448, 
    512, 576, 640, 768, 960, 640, 768, 896, 1024, 1152, 1280, 1536, 1920, 1280, 
    1536, 1792, 2048, 2304, 2560, 3072, 2840 };

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:_kinetis_i2c_x_start_transmission
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int _kinetis_i2c_x_start_transmission(board_kinetis_i2c_info_t * kinetis_i2c_info, unsigned char mode) {
   unsigned char slave_id;
   
   // shift ID in right position
   slave_id = (unsigned char) kinetis_i2c_info->slave_id << 1;

   // Set R/W bit at end of Slave Address
   slave_id |= (unsigned char)mode;

   // send start signal
   __i2c_x_start(kinetis_i2c_info);

   // send ID with W/R bit
   HAL_WRITE_UINT8(kinetis_i2c_info->i2c_base + REG_I2Cx_D, slave_id);
}

/*-------------------------------------------
| Name:_kinetis_i2c_x_find_baudrate
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned int _kinetis_i2c_x_find_baudrate(unsigned int clock,unsigned int baudrate) {
   unsigned int mult, icr, min, minmult = 0, minicr = 0;
   unsigned int  val;

   min = (unsigned int)-1;
   /* We will find the nearest smallest diference in desired and real baudrate.
   ** This is transformed to find smallest clock diference.
   ** IIC baud rate = bus speed (Hz)/(mul * SCL divider) =>
   ** IIC baud rate * mul * SCL divider <= clock
   */
   for(mult = 0; mult < 3; mult++) {
      for(icr = 0; icr < MAX_ICR; icr++) {
         val = baudrate_mult[mult] * baudrate_icr[icr] * baudrate - clock;
         if(val < 0)
            val = -val;
         if(min > val) {
             min = val;
             minmult = mult;
             minicr = icr;
         }
      }
   }
   
   return REG_I2Cx_F_MULT(minmult) | REG_I2Cx_F_ICR(minicr);
}

/*============================================
| End of Source  : low_level_k60n512_i2c_x.c
==============================================*/
