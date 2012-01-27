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


/*============================================
| Compiler Directive   
==============================================*/
#ifndef __DEV_CORTEXM_K60N512_PIT_H__
#define __DEV_CORTEXM_K60N512_PIT_H__


/*============================================
| Includes 
==============================================*/

/*============================================
| Declaration  
==============================================*/
#define REG_PIT_MCR           (0x0) //Module Control Register
#define REG_PIT_LDVALx        (0x100)//Timer Load Value
#define REG_PIT_CVALx         (0x104)//Current Timer Value
#define REG_PIT_TCTRLx        (0x108)//Timer Control
#define REG_PIT_TFLGx         (0x10c)//Timer Control

//
#define REG_PIT_MCR_FRZ_MASK                         0x1u
#define REG_PIT_MCR_FRZ_SHIFT                        0
#define REG_PIT_MCR_MDIS_MASK                        0x2u
#define REG_PIT_MCR_MDIS_SHIFT                       1

//
#define REG_PIT_LDVAL_TSV_MASK                       0xFFFFFFFFu
#define REG_PIT_LDVAL_TSV_SHIFT                      0
#define REG_PIT_LDVAL_TSV(x)                         (((uint32_t)(((uint32_t)(x))<<REG_PIT_LDVAL_TSV_SHIFT))&REG_PIT_LDVAL_TSV_MASK)

//
#define REG_PIT_CVAL_TVL_MASK                        0xFFFFFFFFu
#define REG_PIT_CVAL_TVL_SHIFT                       0
#define REG_PIT_CVAL_TVL(x)                          (((uint32_t)(((uint32_t)(x))<<REG_PIT_CVAL_TVL_SHIFT))&REG_PIT_CVAL_TVL_MASK)

//
#define REG_PIT_TCTRL_TEN_MASK                       0x1u
#define REG_PIT_TCTRL_TEN_SHIFT                      0
#define REG_PIT_TCTRL_TIE_MASK                       0x2u
#define REG_PIT_TCTRL_TIE_SHIFT                      1

//
#define REG_PIT_TFLG_TIF_MASK                        0x1u
#define REG_PIT_TFLG_TIF_SHIFT                       0

//
void _pit_start(unsigned char pit_no, unsigned int m_sec);
void _pit_clear_flag(unsigned char pit_no);
#endif //__DEV_CORTEXM_K60N512_PIT_H__
