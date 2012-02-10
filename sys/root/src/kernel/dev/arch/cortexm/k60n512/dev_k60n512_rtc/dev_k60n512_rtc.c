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

#include "dev_k60n512_rtc.h"
#include "kernel/dev/arch/cortexm/k60n512/common/dev_k60n512_sim.h"
/*===========================================
Global Declaration
=============================================*/
const char dev_k60n512_rtc_name[]="rtt0\0";

static int dev_k60n512_rtc_load(void);
static int dev_k60n512_rtc_open(desc_t desc, int o_flag);
static int dev_k60n512_rtc_close(desc_t desc);
static int dev_k60n512_rtc_read(desc_t desc, char* buf,int cb);
static int dev_k60n512_rtc_write(desc_t desc, const char* buf,int cb);

dev_map_t dev_k60n512_rtc_map={
   dev_k60n512_rtc_name,
   S_IFBLK,
   dev_k60n512_rtc_load,
   dev_k60n512_rtc_open,
   dev_k60n512_rtc_close,
   __fdev_not_implemented,
   __fdev_not_implemented,
   dev_k60n512_rtc_read,
   dev_k60n512_rtc_write,
   __fdev_not_implemented,
   __fdev_not_implemented
};

//#define RTC_STABILIZATION_DELAY     (0x600000)
#define RTC_STABILIZATION_DELAY     (0x600000/200)

//
#define KINETIS_RTC_VECTOR_PRIORITY             3
#define KINETIS_RTC_IRQ            67 //66

#if defined(USE_ECOS)
static cyg_uint32 _kinetis_rtc_isr(cyg_vector_t vector, cyg_addrword_t data);
static void _kinetis_rtc_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
#endif

//
board_kinetis_rtc_info_t kinetis_rtc_info = {
   rtc_base : 0x4003d000,
   desc_r : -1
};

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:dev_k60n512_rtc_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_rtc_load(void){
   volatile unsigned int reg_val = 0;

   //enable clock gating (SIM->SCGC6 |= SIM_SCGC6_RTC_MASK)
   HAL_READ_UINT32(REG_SIM_SCGC6_ADDR, reg_val);
   reg_val |= REG_SIM_SCGC6_RTC_MASK;
   HAL_WRITE_UINT32(REG_SIM_SCGC6_ADDR, reg_val);

   HAL_READ_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_SR, reg_val);

   if(reg_val & REG_RTC_SR_TIF_MASK) {
      //software reset
      HAL_WRITE_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_CR,
                       REG_RTC_CR_SWR_MASK);
      HAL_READ_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_CR, reg_val);
      reg_val &= ~REG_RTC_CR_SWR_MASK;
      HAL_WRITE_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_CR, reg_val);

      //write a 01/01/2012 as default date
      HAL_WRITE_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_TSR, 0x4effa200);
   }

   //enable oscillation and wait stabilization
   HAL_READ_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_CR, reg_val);
   reg_val |= REG_RTC_CR_OSCE_MASK;
   HAL_WRITE_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_CR, reg_val);

   for(reg_val=0; reg_val<RTC_STABILIZATION_DELAY; reg_val++) ;

   //set timer compensation
   HAL_WRITE_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_TCR,
                    REG_RTC_TCR_CIR(0) | REG_RTC_TCR_TCR(0));

   //continue counting
   HAL_WRITE_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_TAR, 0);

   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_rtc_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_rtc_open(desc_t desc, int o_flag) {
   cyg_handle_t irq_handle;
   cyg_interrupt irq_it;
   volatile unsigned int reg_val = 0;

   if(o_flag & O_RDONLY) {
      if(kinetis_rtc_info.desc_r<0) {
         kinetis_rtc_info.desc_r = desc;
      }
      else {
         ofile_lst[desc].p = (void *)&kinetis_rtc_info;
         return 0;                //already open
      }
   }

   if(ofile_lst[desc].p)
      return -1;

   ofile_lst[desc].p = (void *)&kinetis_rtc_info;

   //enable counting
   HAL_READ_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_SR, reg_val);
   reg_val |= REG_RTC_SR_TCE_MASK;
   HAL_WRITE_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_SR, reg_val);

   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_rtc_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_rtc_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_rtc_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_rtc_read(desc_t desc, char* buf,int size){
   volatile unsigned int reg_val;

   if(size!=sizeof(time_t))
      return -1;

   HAL_READ_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_TSR, reg_val);
   memcpy((void *)buf, (void *)&reg_val, size);

   return size;
}

/*-------------------------------------------
| Name:dev_k60n512_rtc_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_rtc_write(desc_t desc, const char* buf,int size){
   volatile unsigned int reg_val = 0;
   volatile unsigned int time_val = 0;

   if(size!=sizeof(time_t))
      return -1;

   memcpy((void *)&time_val, (void *)buf, 4);

   //disable counter and write new value
   HAL_READ_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_SR, reg_val);
   reg_val &= ~REG_RTC_SR_TCE_MASK;
   HAL_WRITE_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_SR, reg_val);

   HAL_WRITE_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_TSR, time_val);
   HAL_WRITE_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_TPR, 0);

   HAL_READ_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_SR, reg_val);
   reg_val |= REG_RTC_SR_TCE_MASK;
   HAL_WRITE_UINT32(kinetis_rtc_info.rtc_base + REG_RTC_SR, reg_val);

   return size;
}

/*============================================
| End of Source  : dev_k60n512_rtc.c
==============================================*/
