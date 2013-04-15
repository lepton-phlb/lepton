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
#ifndef __DEV_CORTEXM_K60N512_I2CX_H__
#define __DEV_CORTEXM_K60N512_I2CX_H__


/*============================================
| Includes 
==============================================*/

/*============================================
| Declaration  
==============================================*/
#define REG_I2Cx_A1        (0x0) //Address Register 1
#define REG_I2Cx_F         (0x1) //Frequency Divider Register
#define REG_I2Cx_C1        (0x2) //Control Register 1
#define REG_I2Cx_S         (0x3) //Status Register
#define REG_I2Cx_D         (0x4) //Data Register
#define REG_I2Cx_C2        (0x5) //Control Register 2
#define REG_I2Cx_FLT       (0x6) //Pro. Input Glitch Filter
#define REG_I2Cx_RA        (0x7) //Range Address Register
#define REG_I2Cx_SMB       (0x8) //SMBBus Control and Status
#define REG_I2Cx_A2        (0x9) //Address Register 2
#define REG_I2Cx_SLTH      (0xa) //Low Timeout Register High
#define REG_I2Cx_SLTL      (0xb) //Low Timeout Register High

//
#define REG_I2Cx_A1_AD_MASK                           0xFEu
#define REG_I2Cx_A1_AD_SHIFT                          1
#define REG_I2Cx_A1_AD(x)                             (((uint8_t)(((uint8_t)(x))<<REG_I2Cx_A1_AD_SHIFT))&REG_I2Cx_A1_AD_MASK)

//
#define REG_I2Cx_F_ICR_MASK                           0x3Fu
#define REG_I2Cx_F_ICR_SHIFT                          0
#define REG_I2Cx_F_ICR(x)                             (((uint8_t)(((uint8_t)(x))<<REG_I2Cx_F_ICR_SHIFT))&REG_I2Cx_F_ICR_MASK)
#define REG_I2Cx_F_MULT_MASK                          0xC0u
#define REG_I2Cx_F_MULT_SHIFT                         6
#define REG_I2Cx_F_MULT(x)                            (((uint8_t)(((uint8_t)(x))<<REG_I2Cx_F_MULT_SHIFT))&REG_I2Cx_F_MULT_MASK)

//
#define REG_I2Cx_C1_DMAEN_MASK                        0x1u
#define REG_I2Cx_C1_DMAEN_SHIFT                       0
#define REG_I2Cx_C1_WUEN_MASK                         0x2u
#define REG_I2Cx_C1_WUEN_SHIFT                        1
#define REG_I2Cx_C1_RSTA_MASK                         0x4u
#define REG_I2Cx_C1_RSTA_SHIFT                        2
#define REG_I2Cx_C1_TXAK_MASK                         0x8u
#define REG_I2Cx_C1_TXAK_SHIFT                        3
#define REG_I2Cx_C1_TX_MASK                           0x10u
#define REG_I2Cx_C1_TX_SHIFT                          4
#define REG_I2Cx_C1_MST_MASK                          0x20u
#define REG_I2Cx_C1_MST_SHIFT                         5
#define REG_I2Cx_C1_IICIE_MASK                        0x40u
#define REG_I2Cx_C1_IICIE_SHIFT                       6
#define REG_I2Cx_C1_IICEN_MASK                        0x80u
#define REG_I2Cx_C1_IICEN_SHIFT                       7

//
#define REG_I2Cx_S_RXAK_MASK                          0x1u
#define REG_I2Cx_S_RXAK_SHIFT                         0
#define REG_I2Cx_S_IICIF_MASK                         0x2u
#define REG_I2Cx_S_IICIF_SHIFT                        1
#define REG_I2Cx_S_SRW_MASK                           0x4u
#define REG_I2Cx_S_SRW_SHIFT                          2
#define REG_I2Cx_S_RAM_MASK                           0x8u
#define REG_I2Cx_S_RAM_SHIFT                          3
#define REG_I2Cx_S_ARBL_MASK                          0x10u
#define REG_I2Cx_S_ARBL_SHIFT                         4
#define REG_I2Cx_S_BUSY_MASK                          0x20u
#define REG_I2Cx_S_BUSY_SHIFT                         5
#define REG_I2Cx_S_IAAS_MASK                          0x40u
#define REG_I2Cx_S_IAAS_SHIFT                         6
#define REG_I2Cx_S_TCF_MASK                           0x80u
#define REG_I2Cx_S_TCF_SHIFT                          7

//
#define REG_I2Cx_D_DATA_MASK                          0xFFu
#define REG_I2Cx_D_DATA_SHIFT                         0
#define REG_I2Cx_D_DATA(x)                            (((uint8_t)(((uint8_t)(x))<<REG_I2Cx_D_DATA_SHIFT))&REG_I2Cx_D_DATA_MASK)

//
#define REG_I2Cx_C2_AD_MASK                           0x7u
#define REG_I2Cx_C2_AD_SHIFT                          0
#define REG_I2Cx_C2_AD(x)                             (((uint8_t)(((uint8_t)(x))<<REG_I2Cx_C2_AD_SHIFT))&REG_I2Cx_C2_AD_MASK)
#define REG_I2Cx_C2_RMEN_MASK                         0x8u
#define REG_I2Cx_C2_RMEN_SHIFT                        3
#define REG_I2Cx_C2_SBRC_MASK                         0x10u
#define REG_I2Cx_C2_SBRC_SHIFT                        4
#define REG_I2Cx_C2_HDRS_MASK                         0x20u
#define REG_I2Cx_C2_HDRS_SHIFT                        5
#define REG_I2Cx_C2_ADEXT_MASK                        0x40u
#define REG_I2Cx_C2_ADEXT_SHIFT                       6
#define REG_I2Cx_C2_GCAEN_MASK                        0x80u
#define REG_I2Cx_C2_GCAEN_SHIFT                       7

//
#define REG_I2Cx_FLT_FLT_MASK                         0x1Fu
#define REG_I2Cx_FLT_FLT_SHIFT                        0
#define REG_I2Cx_FLT_FLT(x)                           (((uint8_t)(((uint8_t)(x))<<REG_I2Cx_FLT_FLT_SHIFT))&REG_I2Cx_FLT_FLT_MASK)

//
#define REG_I2Cx_RA_RAD_MASK                          0xFEu
#define REG_I2Cx_RA_RAD_SHIFT                         1
#define REG_I2Cx_RA_RAD(x)                            (((uint8_t)(((uint8_t)(x))<<REG_I2Cx_RA_RAD_SHIFT))&REG_I2Cx_RA_RAD_MASK)

//
#define REG_I2Cx_SMB_SHTF2IE_MASK                     0x1u
#define REG_I2Cx_SMB_SHTF2IE_SHIFT                    0
#define REG_I2Cx_SMB_SHTF2_MASK                       0x2u
#define REG_I2Cx_SMB_SHTF2_SHIFT                      1
#define REG_I2Cx_SMB_SHTF1_MASK                       0x4u
#define REG_I2Cx_SMB_SHTF1_SHIFT                      2
#define REG_I2Cx_SMB_SLTF_MASK                        0x8u
#define REG_I2Cx_SMB_SLTF_SHIFT                       3
#define REG_I2Cx_SMB_TCKSEL_MASK                      0x10u
#define REG_I2Cx_SMB_TCKSEL_SHIFT                     4
#define REG_I2Cx_SMB_SIICAEN_MASK                     0x20u
#define REG_I2Cx_SMB_SIICAEN_SHIFT                    5
#define REG_I2Cx_SMB_ALERTEN_MASK                     0x40u
#define REG_I2Cx_SMB_ALERTEN_SHIFT                    6
#define REG_I2Cx_SMB_FACK_MASK                        0x80u
#define REG_I2Cx_SMB_FACK_SHIFT                       7

//
#define REG_I2Cx_A2_SAD_MASK                          0xFEu
#define REG_I2Cx_A2_SAD_SHIFT                         1
#define REG_I2Cx_A2_SAD(x)                            (((uint8_t)(((uint8_t)(x))<<REG_I2Cx_A2_SAD_SHIFT))&REG_I2Cx_A2_SAD_MASK)

//
#define REG_I2Cx_SLTH_SSLT_MASK                       0xFFu
#define REG_I2Cx_SLTH_SSLT_SHIFT                      0
#define REG_I2Cx_SLTH_SSLT(x)                         (((uint8_t)(((uint8_t)(x))<<REG_I2Cx_SLTH_SSLT_SHIFT))&REG_I2Cx_SLTH_SSLT_MASK)

//
#define REG_I2Cx_SLTL_SSLT_MASK                       0xFFu
#define REG_I2Cx_SLTL_SSLT_SHIFT                      0
#define REG_I2Cx_SLTL_SSLT(x)                         (((uint8_t)(((uint8_t)(x))<<REG_I2Cx_SLTL_SSLT_SHIFT))&REG_I2Cx_SLTL_SSLT_MASK)

//
#define M_WR_S_RD                   0x00  //master write
#define M_RD_S_WR                   0x01  //master read

//
#define MAX_ICR      64

//
typedef struct board_kinetis_i2c_info_st {
   unsigned int i2c_base;
   
   int desc_r;
   int desc_w;
   
   unsigned int speed;
   
   unsigned char slave_id;
} board_kinetis_i2c_info_t;

//
unsigned int _kinetis_i2c_x_find_baudrate(unsigned int clock,unsigned int baudrate);
int _kinetis_i2c_x_start_transmission(board_kinetis_i2c_info_t * kinetis_i2c_info, unsigned char mode);

//
#define __i2c_x_start(__kinetis_i2c_x_info__) { \
   volatile unsigned char reg_start; \
   HAL_READ_UINT8(__kinetis_i2c_x_info__->i2c_base + REG_I2Cx_C1, reg_start); \
   reg_start |= REG_I2Cx_C1_TX_MASK; \
   HAL_WRITE_UINT8(__kinetis_i2c_x_info__->i2c_base + REG_I2Cx_C1, reg_start); \
   reg_start |= REG_I2Cx_C1_MST_MASK; \
   HAL_WRITE_UINT8(__kinetis_i2c_x_info__->i2c_base + REG_I2Cx_C1, reg_start); \
}

#define __i2c_x_stop(__kinetis_i2c_x_info__) { \
   volatile unsigned char reg_stop; \
   HAL_READ_UINT8(__kinetis_i2c_x_info__->i2c_base + REG_I2Cx_C1, reg_stop); \
   reg_stop &= ~REG_I2Cx_C1_MST_MASK; \
   HAL_WRITE_UINT8(__kinetis_i2c_x_info__->i2c_base + REG_I2Cx_C1, reg_stop); \
   reg_stop &= ~REG_I2Cx_C1_TX_MASK; \
   HAL_WRITE_UINT8(__kinetis_i2c_x_info__->i2c_base + REG_I2Cx_C1, reg_stop); \
}

#define __i2c_x_wait(__kinetis_i2c_x_info__) { \
   volatile unsigned char reg_wait; \
   volatile unsigned int timeout=1000; \
   do { \
      HAL_READ_UINT8(__kinetis_i2c_x_info__->i2c_base + REG_I2Cx_S, reg_wait); \
      if(!timeout--) { \
         __i2c_x_stop(__kinetis_i2c_x_info__); \
         HAL_WRITE_UINT8(p_i2c_info->i2c_base + REG_I2Cx_C1, 0); \
         return -1; \
      } \
   } while((reg_wait & REG_I2Cx_S_IICIF_MASK)==0); \
   HAL_READ_UINT8(__kinetis_i2c_x_info__->i2c_base + REG_I2Cx_S, reg_wait); \
   reg_wait |= REG_I2Cx_S_IICIF_MASK; \
   HAL_READ_UINT8(__kinetis_i2c_x_info__->i2c_base + REG_I2Cx_S, reg_wait); \
}

#define __i2c_x_pause(__delay__) { \
   unsigned int n; \
   for(n=0; n<__delay__; n++) { \
      __asm__ volatile( "nop" ); \
   } \
}

#define __i2c_x_wait_busy(__kinetis_i2c_x_info__) { \
   volatile unsigned char reg_wait_busy; \
   do { \
      HAL_DELAY_US(10); \
      HAL_READ_UINT8(__kinetis_i2c_x_info__->i2c_base + REG_I2Cx_S, reg_wait_busy); \
   } while(reg_wait_busy & REG_I2Cx_S_BUSY_MASK); \
}

#endif //__DEV_CORTEXM_K60N512_I2CX_H__
