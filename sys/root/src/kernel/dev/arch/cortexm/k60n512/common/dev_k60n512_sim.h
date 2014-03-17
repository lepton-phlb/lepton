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
#ifndef __DEV_CORTEXM_K60N512_SIM_H__
#define __DEV_CORTEXM_K60N512_SIM_H__


/*============================================
| Includes 
==============================================*/

/*============================================
| Declaration  
==============================================*/
#define REG_SIM_SOPT1_ADDR       (0x40047000)//System Option Register 1
#define REG_SIM_SOPT2_ADDR       (0x40048004)//System Option Register 2
#define REG_SIM_SOPT4_ADDR       (0x4004800c)//System Option Register 4
#define REG_SIM_SOPT5_ADDR       (0x40048010)//System Option Register 5
#define REG_SIM_SOPT6_ADDR       (0x40048014)//System Option Register 6
#define REG_SIM_SOPT7_ADDR       (0x40048018)//System Option Register 7
#define REG_SIM_SDID_ADDR        (0x40048024)//System Device Id
#define REG_SIM_SCGC1_ADDR       (0x40048028)//System Clock Gating 1
#define REG_SIM_SCGC2_ADDR       (0x4004802c)//System Clock Gating 2
#define REG_SIM_SCGC3_ADDR       (0x40048030)//System Clock Gating 3
#define REG_SIM_SCGC4_ADDR       (0x40048034)//System Clock Gating 4
#define REG_SIM_SCGC5_ADDR       (0x40048038)//System Clock Gating 5
#define REG_SIM_SCGC6_ADDR       (0x4004803c)//System Clock Gating 6
#define REG_SIM_SCGC7_ADDR       (0x40048040)//System Clock Gating 7

//
#define REG_SIM_SCGC1_UART4_MASK                     0x400u
#define REG_SIM_SCGC1_UART4_SHIFT                    10
#define REG_SIM_SCGC1_UART5_MASK                     0x800u
#define REG_SIM_SCGC1_UART5_SHIFT                    11

//
#define REG_SIM_SCGC2_ENET_MASK                      0x1u
#define REG_SIM_SCGC2_ENET_SHIFT                     0
#define REG_SIM_SCGC2_DAC0_MASK                      0x1000u
#define REG_SIM_SCGC2_DAC0_SHIFT                     12
#define REG_SIM_SCGC2_DAC1_MASK                      0x2000u
#define REG_SIM_SCGC2_DAC1_SHIFT                     13

//
#define REG_SIM_SCGC3_RNGB_MASK                      0x1u
#define REG_SIM_SCGC3_RNGB_SHIFT                     0
#define REG_SIM_SCGC3_FLEXCAN1_MASK                  0x10u
#define REG_SIM_SCGC3_FLEXCAN1_SHIFT                 4
#define REG_SIM_SCGC3_SPI2_MASK                      0x1000u
#define REG_SIM_SCGC3_SPI2_SHIFT                     12
#define REG_SIM_SCGC3_SDHC_MASK                      0x20000u
#define REG_SIM_SCGC3_SDHC_SHIFT                     17
#define REG_SIM_SCGC3_FTM2_MASK                      0x1000000u
#define REG_SIM_SCGC3_FTM2_SHIFT                     24
#define REG_SIM_SCGC3_ADC1_MASK                      0x8000000u
#define REG_SIM_SCGC3_ADC1_SHIFT                     27

//
#define REG_SIM_SCGC4_EWM_MASK                       0x2u
#define REG_SIM_SCGC4_EWM_SHIFT                      1
#define REG_SIM_SCGC4_CMT_MASK                       0x4u
#define REG_SIM_SCGC4_CMT_SHIFT                      2
#define REG_SIM_SCGC4_I2C0_MASK                      0x40u
#define REG_SIM_SCGC4_I2C0_SHIFT                     6
#define REG_SIM_SCGC4_I2C1_MASK                      0x80u
#define REG_SIM_SCGC4_I2C1_SHIFT                     7
#define REG_SIM_SCGC4_UART0_MASK                     0x400u
#define REG_SIM_SCGC4_UART0_SHIFT                    10
#define REG_SIM_SCGC4_UART1_MASK                     0x800u
#define REG_SIM_SCGC4_UART1_SHIFT                    11
#define REG_SIM_SCGC4_UART2_MASK                     0x1000u
#define REG_SIM_SCGC4_UART2_SHIFT                    12
#define REG_SIM_SCGC4_UART3_MASK                     0x2000u
#define REG_SIM_SCGC4_UART3_SHIFT                    13
#define REG_SIM_SCGC4_USBOTG_MASK                    0x40000u
#define REG_SIM_SCGC4_USBOTG_SHIFT                   18
#define REG_SIM_SCGC4_CMP_MASK                       0x80000u
#define REG_SIM_SCGC4_CMP_SHIFT                      19
#define REG_SIM_SCGC4_VREF_MASK                      0x100000u
#define REG_SIM_SCGC4_VREF_SHIFT                     20
#define REG_SIM_SCGC4_LLWU_MASK                      0x10000000u
#define REG_SIM_SCGC4_LLWU_SHIFT                     28

//
#define REG_SIM_SCGC5_LPTIMER_MASK                   0x1u
#define REG_SIM_SCGC5_LPTIMER_SHIFT                  0
#define REG_SIM_SCGC5_REGFILE_MASK                   0x2u
#define REG_SIM_SCGC5_REGFILE_SHIFT                  1
#define REG_SIM_SCGC5_TSI_MASK                       0x20u
#define REG_SIM_SCGC5_TSI_SHIFT                      5
#define REG_SIM_SCGC5_PORTA_MASK                     0x200u
#define REG_SIM_SCGC5_PORTA_SHIFT                    9
#define REG_SIM_SCGC5_PORTB_MASK                     0x400u
#define REG_SIM_SCGC5_PORTB_SHIFT                    10
#define REG_SIM_SCGC5_PORTC_MASK                     0x800u
#define REG_SIM_SCGC5_PORTC_SHIFT                    11
#define REG_SIM_SCGC5_PORTD_MASK                     0x1000u
#define REG_SIM_SCGC5_PORTD_SHIFT                    12
#define REG_SIM_SCGC5_PORTE_MASK                     0x2000u
#define REG_SIM_SCGC5_PORTE_SHIFT                    13

//
#define REG_SIM_SCGC6_FTFL_MASK                      0x1u
#define REG_SIM_SCGC6_FTFL_SHIFT                     0
#define REG_SIM_SCGC6_DMAMUX_MASK                    0x2u
#define REG_SIM_SCGC6_DMAMUX_SHIFT                   1
#define REG_SIM_SCGC6_FLEXCAN0_MASK                  0x10u
#define REG_SIM_SCGC6_FLEXCAN0_SHIFT                 4
#define REG_SIM_SCGC6_DSPI0_MASK                     0x1000u
#define REG_SIM_SCGC6_DSPI0_SHIFT                    12
#define REG_SIM_SCGC6_SPI1_MASK                      0x2000u
#define REG_SIM_SCGC6_SPI1_SHIFT                     13
#define REG_SIM_SCGC6_I2S_MASK                       0x8000u
#define REG_SIM_SCGC6_I2S_SHIFT                      15
#define REG_SIM_SCGC6_CRC_MASK                       0x40000u
#define REG_SIM_SCGC6_CRC_SHIFT                      18
#define REG_SIM_SCGC6_USBDCD_MASK                    0x200000u
#define REG_SIM_SCGC6_USBDCD_SHIFT                   21
#define REG_SIM_SCGC6_PDB_MASK                       0x400000u
#define REG_SIM_SCGC6_PDB_SHIFT                      22
#define REG_SIM_SCGC6_PIT_MASK                       0x800000u
#define REG_SIM_SCGC6_PIT_SHIFT                      23
#define REG_SIM_SCGC6_FTM0_MASK                      0x1000000u
#define REG_SIM_SCGC6_FTM0_SHIFT                     24
#define REG_SIM_SCGC6_FTM1_MASK                      0x2000000u
#define REG_SIM_SCGC6_FTM1_SHIFT                     25
#define REG_SIM_SCGC6_ADC0_MASK                      0x8000000u
#define REG_SIM_SCGC6_ADC0_SHIFT                     27
#define REG_SIM_SCGC6_RTC_MASK                       0x20000000u
#define REG_SIM_SCGC6_RTC_SHIFT                      29

//
#define REG_SIM_SCGC7_FLEXBUS_MASK                   0x1u
#define REG_SIM_SCGC7_FLEXBUS_SHIFT                  0
#define REG_SIM_SCGC7_DMA_MASK                       0x2u
#define REG_SIM_SCGC7_DMA_SHIFT                      1
#define REG_SIM_SCGC7_MPU_MASK                       0x4u
#define REG_SIM_SCGC7_MPU_SHIFT                      2

#endif //__DEV_CORTEXM_K60N512_SIM_H__
