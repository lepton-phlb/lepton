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


/*============================================
| Compiler Directive
==============================================*/
#ifndef __DEV_CORTEXM_K60N512_RTC_H__
#define __DEV_CORTEXM_K60N512_RTC_H__


/*============================================
| Includes
==============================================*/

/*============================================
| Declaration
==============================================*/
#define REG_RTC_TSR              (0x0) //Time Seconds Register
#define REG_RTC_TPR              (0x4) //Time Prescaler Register
#define REG_RTC_TAR              (0x8) //Time Alarm Register
#define REG_RTC_TCR              (0xc) //Time Compensation Register
#define REG_RTC_CR               (0x10) //Control Register
#define REG_RTC_SR               (0x14) //Status Register
#define REG_RTC_LR               (0x18) //Lock Register
#define REG_RTC_IER              (0x1c) //Interrupt Enable Register
#define REG_RTC_WAR              (0x800) //Write Access Register
#define REG_RTC_RAR              (0x804) //Interrupt Enable Register

//
#define REG_RTC_TCR_TCR_MASK                         0xFFu
#define REG_RTC_TCR_TCR_SHIFT                        0
#define REG_RTC_TCR_TCR(x)                           (((uint32_t)(((uint32_t)(x))<< \
                                                                  REG_RTC_TCR_TCR_SHIFT))& \
                                                      REG_RTC_TCR_TCR_MASK)
#define REG_RTC_TCR_CIR_MASK                         0xFF00u
#define REG_RTC_TCR_CIR_SHIFT                        8
#define REG_RTC_TCR_CIR(x)                           (((uint32_t)(((uint32_t)(x))<< \
                                                                  REG_RTC_TCR_CIR_SHIFT))& \
                                                      REG_RTC_TCR_CIR_MASK)
#define REG_RTC_TCR_TCV_MASK                         0xFF0000u
#define REG_RTC_TCR_TCV_SHIFT                        16
#define REG_RTC_TCR_TCV(x)                           (((uint32_t)(((uint32_t)(x))<< \
                                                                  REG_RTC_TCR_TCV_SHIFT))& \
                                                      REG_RTC_TCR_TCV_MASK)
#define REG_RTC_TCR_CIC_MASK                         0xFF000000u
#define REG_RTC_TCR_CIC_SHIFT                        24
#define REG_RTC_TCR_CIC(x)                           (((uint32_t)(((uint32_t)(x))<< \
                                                                  REG_RTC_TCR_CIC_SHIFT))& \
                                                      REG_RTC_TCR_CIC_MASK)

//
#define REG_RTC_CR_SWR_MASK                          0x1u
#define REG_RTC_CR_SWR_SHIFT                         0
#define REG_RTC_CR_WPE_MASK                          0x2u
#define REG_RTC_CR_WPE_SHIFT                         1
#define REG_RTC_CR_SUP_MASK                          0x4u
#define REG_RTC_CR_SUP_SHIFT                         2
#define REG_RTC_CR_UM_MASK                           0x8u
#define REG_RTC_CR_UM_SHIFT                          3
#define REG_RTC_CR_OSCE_MASK                         0x100u
#define REG_RTC_CR_OSCE_SHIFT                        8
#define REG_RTC_CR_CLKO_MASK                         0x200u
#define REG_RTC_CR_CLKO_SHIFT                        9
#define REG_RTC_CR_SC16P_MASK                        0x400u
#define REG_RTC_CR_SC16P_SHIFT                       10
#define REG_RTC_CR_SC8P_MASK                         0x800u
#define REG_RTC_CR_SC8P_SHIFT                        11
#define REG_RTC_CR_SC4P_MASK                         0x1000u
#define REG_RTC_CR_SC4P_SHIFT                        12
#define REG_RTC_CR_SC2P_MASK                         0x2000u
#define REG_RTC_CR_SC2P_SHIFT                        13

//
#define REG_RTC_SR_TIF_MASK                          0x1u
#define REG_RTC_SR_TIF_SHIFT                         0
#define REG_RTC_SR_TOF_MASK                          0x2u
#define REG_RTC_SR_TOF_SHIFT                         1
#define REG_RTC_SR_TAF_MASK                          0x4u
#define REG_RTC_SR_TAF_SHIFT                         2
#define REG_RTC_SR_TCE_MASK                          0x10u
#define REG_RTC_SR_TCE_SHIFT                         4

typedef struct board_kinetis_rtc_info_st {
   unsigned int rtc_base;

   int desc_r;
} board_kinetis_rtc_info_t;
#endif //__DEV_CORTEXM_K60N512_RTC_H__



