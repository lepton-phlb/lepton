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

#include "dev_k60n512_pit.h"
#include "kernel/dev/arch/cortexm/k60n512/common/dev_k60n512_sim.h"
/*===========================================
Global Declaration
=============================================*/

static unsigned char _pit_enable;
static unsigned int pit_base = 0x40037000;

#define  PIT_PERIOD_NS     20
/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:_pit_start
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _pit_start(unsigned char pit_no, unsigned int m_sec) {
   unsigned int reg_val = 0;

   //enable PIT
   if(!_pit_enable) {
      //enable clock gating (SIM->SCGC6 |= SIM_SCGC6_PIT_MASK)
      HAL_READ_UINT32(REG_SIM_SCGC6_ADDR, reg_val);
      reg_val |= REG_SIM_SCGC6_PIT_MASK;
      HAL_WRITE_UINT32(REG_SIM_SCGC6_ADDR, reg_val);

      //
      HAL_WRITE_UINT32(pit_base + REG_PIT_MCR, 0);
      _pit_enable = 1;
   }

   if(!m_sec)
      return;

   //calculate value
   reg_val = (1000000-1)*m_sec/PIT_PERIOD_NS;
   //reg_val = 100000;//500;
   HAL_WRITE_UINT32(pit_base + REG_PIT_LDVALx + 16*pit_no, reg_val);

   //start timer
   HAL_WRITE_UINT32(pit_base + REG_PIT_TCTRLx + 16*pit_no,
                    REG_PIT_TCTRL_TEN_MASK /*| REG_PIT_TCTRL_TIE_MASK*/);
}

/*-------------------------------------------
| Name:_pit_clear_flag
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _pit_clear_flag(unsigned char pit_no) {
   HAL_WRITE_UINT32(pit_base + REG_PIT_TFLGx + 16*pit_no, REG_PIT_TFLG_TIF_MASK);
}

/*============================================
| End of Source  : dev_k60n512_pit.c
==============================================*/
