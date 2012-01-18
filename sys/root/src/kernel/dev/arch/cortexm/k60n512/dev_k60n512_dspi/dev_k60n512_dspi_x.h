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
#ifndef __DEV_CORTEXM_DSPI_H__
#define __DEV_CORTEXM_DSPI_H__


/*============================================
| Includes 
==============================================*/

/*============================================
| Declaration  
==============================================*/
#define REG_SPI_MCR              (0x0) //Module Configuration Register
#define REG_SPI_TCR              (0x8) //Transfer Count Register
#define REG_SPI_CTAR0            (0xc) //Clock and Transfer Attributes (Master)
#define REG_SPI_CTAR_SLAVE       (0xc) //Clock and Transfer Attributes (Slave)
#define REG_SPI_CTAR1            (0x10)//Clock and Transfer Attributes (Master)
#define REG_SPI_SR               (0x2c)//Status Register
#define REG_SPI_RSER             (0x30)//DMA/IRQ Enable register
#define REG_SPI_PUSHR            (0x34)//PUSH TX FIFO Register (Master)
#define REG_SPI_PUSHR_SLAVE      (0x34)//PUSH TX FIFO Register (Master)
#define REG_SPI_POPR             (0x38)//POP RX FIFO Register
#define REG_SPI_TXFR0            (0x3c)//Transmit FIFO Register
#define REG_SPI_TXFR1            (0x40)//Transmit FIFO Register
#define REG_SPI_TXFR2            (0x44)//Transmit FIFO Register
#define REG_SPI_TXFR3            (0x48)//Transmit FIFO Register
#define REG_SPI_RXFR0            (0x7c)//Receive FIFO Register
#define REG_SPI_RXFR1            (0x80)//Receive FIFO Register
#define REG_SPI_RXFR2            (0x84)//Receive FIFO Register
#define REG_SPI_RXFR3            (0x88)//Receive FIFO Register

//
#define REG_SPI_MCR_HALT_MASK                        0x1u
#define REG_SPI_MCR_HALT_SHIFT                       0
#define REG_SPI_MCR_SMPL_PT_MASK                     0x300u
#define REG_SPI_MCR_SMPL_PT_SHIFT                    8
#define REG_SPI_MCR_SMPL_PT(x)                       (((uint32_t)(((uint32_t)(x))<<REG_SPI_MCR_SMPL_PT_SHIFT))&REG_SPI_MCR_SMPL_PT_MASK)
#define REG_SPI_MCR_CLR_RXF_MASK                     0x400u
#define REG_SPI_MCR_CLR_RXF_SHIFT                    10
#define REG_SPI_MCR_CLR_TXF_MASK                     0x800u
#define REG_SPI_MCR_CLR_TXF_SHIFT                    11
#define REG_SPI_MCR_DIS_RXF_MASK                     0x1000u
#define REG_SPI_MCR_DIS_RXF_SHIFT                    12
#define REG_SPI_MCR_DIS_TXF_MASK                     0x2000u
#define REG_SPI_MCR_DIS_TXF_SHIFT                    13
#define REG_SPI_MCR_MDIS_MASK                        0x4000u
#define REG_SPI_MCR_MDIS_SHIFT                       14
#define REG_SPI_MCR_DOZE_MASK                        0x8000u
#define REG_SPI_MCR_DOZE_SHIFT                       15
#define REG_SPI_MCR_PCSIS_MASK                       0x3F0000u
#define REG_SPI_MCR_PCSIS_SHIFT                      16
#define REG_SPI_MCR_PCSIS(x)                         (((uint32_t)(((uint32_t)(x))<<REG_SPI_MCR_PCSIS_SHIFT))&REG_SPI_MCR_PCSIS_MASK)
#define REG_SPI_MCR_ROOE_MASK                        0x1000000u
#define REG_SPI_MCR_ROOE_SHIFT                       24
#define REG_SPI_MCR_PCSSE_MASK                       0x2000000u
#define REG_SPI_MCR_PCSSE_SHIFT                      25
#define REG_SPI_MCR_MTFE_MASK                        0x4000000u
#define REG_SPI_MCR_MTFE_SHIFT                       26
#define REG_SPI_MCR_FRZ_MASK                         0x8000000u
#define REG_SPI_MCR_FRZ_SHIFT                        27
#define REG_SPI_MCR_DCONF_MASK                       0x30000000u
#define REG_SPI_MCR_DCONF_SHIFT                      28
#define REG_SPI_MCR_DCONF(x)                         (((uint32_t)(((uint32_t)(x))<<REG_SPI_MCR_DCONF_SHIFT))&REG_SPI_MCR_DCONF_MASK)
#define REG_SPI_MCR_CONT_SCKE_MASK                   0x40000000u
#define REG_SPI_MCR_CONT_SCKE_SHIFT                  30
#define REG_SPI_MCR_MSTR_MASK                        0x80000000u
#define REG_SPI_MCR_MSTR_SHIFT                       31

//
#define REG_SPI_CTAR_BR_MASK                         0xFu
#define REG_SPI_CTAR_BR_SHIFT                        0
#define REG_SPI_CTAR_DT_MASK                         0xF0u
#define REG_SPI_CTAR_DT_SHIFT                        4
#define REG_SPI_CTAR_ASC_MASK                        0xF00u
#define REG_SPI_CTAR_ASC_SHIFT                       8
#define REG_SPI_CTAR_CSSCK_MASK                      0xF000u
#define REG_SPI_CTAR_CSSCK_SHIFT                     12
#define REG_SPI_CTAR_PBR_MASK                        0x30000u
#define REG_SPI_CTAR_PBR_SHIFT                       16
#define REG_SPI_CTAR_PDT_MASK                        0xC0000u
#define REG_SPI_CTAR_PDT_SHIFT                       18
#define REG_SPI_CTAR_PASC_MASK                       0x300000u
#define REG_SPI_CTAR_PASC_SHIFT                      20
#define REG_SPI_CTAR_PCSSCK_MASK                     0xC00000u
#define REG_SPI_CTAR_PCSSCK_SHIFT                    22
#define REG_SPI_CTAR_LSBFE_MASK                      0x1000000u
#define REG_SPI_CTAR_CPHA_MASK                       0x2000000u
#define REG_SPI_CTAR_CPOL_MASK                       0x4000000u
#define REG_SPI_CTAR_FMSZ_MASK                       0x78000000u
#define REG_SPI_CTAR_FMSZ_SHIFT                      27
#define REG_SPI_CTAR_DBR_MASK                        0x80000000u
#define REG_SPI_CTAR_DBR_SHIFT                       31
#define REG_SPI_CTAR_SLAVE_FMSZ_MASK                 0xF8000000u
#define REG_SPI_CTAR_SLAVE_FMSZ_SHIFT                27

#define REG_SPI_CTAR_BR(x)          (((x) << REG_SPI_CTAR_BR_SHIFT) & REG_SPI_CTAR_BR_MASK)
#define REG_SPI_CTAR_DT(x)          (((x) << REG_SPI_CTAR_DT_SHIFT) & REG_SPI_CTAR_DT_MASK)
#define REG_SPI_CTAR_ASC(x)         (((x) << REG_SPI_CTAR_ASC_SHIFT) & REG_SPI_CTAR_ASC_MASK)
#define REG_SPI_CTAR_CSSCK(x)       (((x) << REG_SPI_CTAR_CSSCK_SHIFT) & REG_SPI_CTAR_CSSCK_MASK)
#define REG_SPI_CTAR_PBR(x)         (((x) << REG_SPI_CTAR_PBR_SHIFT) & REG_SPI_CTAR_PBR_MASK)
#define REG_SPI_CTAR_PDT(x)         (((x) << REG_SPI_CTAR_PDT_SHIFT) & REG_SPI_CTAR_PDT_MASK)
#define REG_SPI_CTAR_PASC(x)        (((x) << REG_SPI_CTAR_PASC_SHIFT) & REG_SPI_CTAR_PASC_MASK)
#define REG_SPI_CTAR_PCSSCK(x)      (((x) << REG_SPI_CTAR_PCSSCK_SHIFT) & REG_SPI_CTAR_PCSSCK_MASK)
#define REG_SPI_CTAR_FMSZ(x)        (((x) << REG_SPI_CTAR_FMSZ_SHIFT) & REG_SPI_CTAR_FMSZ_MASK)
#define REG_SPI_CTAR_FMSZ_GET(x)    (((x) & REG_SPI_CTAR_FMSZ_MASK) >> REG_SPI_CTAR_FMSZ_SHIFT)
#define REG_SPI_CTAR_PBR_GET(x)     (((x) & REG_SPI_CTAR_PBR_MASK) >> REG_SPI_CTAR_PBR_SHIFT)
#define REG_SPI_CTAR_BR_GET(x)      (((x) & REG_SPI_CTAR_BR_MASK) >> REG_SPI_CTAR_BR_SHIFT)

#define SPI_CTAR_CLK_POL_PHA_MODE0              (0x00)   /* Inactive SPICLK low & sample rising */
#define SPI_CTAR_CLK_POL_PHA_MODE1              (0x01)   /* Inactive SPICLK low & sample falling */
#define SPI_CTAR_CLK_POL_PHA_MODE2              (0x02)   /* Inactive SPICLK high & sample falling */
#define SPI_CTAR_CLK_POL_PHA_MODE3              (0x03)   /* Inactive SPICLK high & sample rising */

//
#define REG_SPI_SR_POPNXTPTR_MASK                    0xFu
#define REG_SPI_SR_POPNXTPTR_SHIFT                   0
#define REG_SPI_SR_POPNXTPTR(x)                      (((uint32_t)(((uint32_t)(x))<<REG_SPI_SR_POPNXTPTR_SHIFT))&REG_SPI_SR_POPNXTPTR_MASK)
#define REG_SPI_SR_RXCTR_MASK                        0xF0u
#define REG_SPI_SR_RXCTR_SHIFT                       4
#define REG_SPI_SR_RXCTR(x)                          (((uint32_t)(((uint32_t)(x))<<REG_SPI_SR_RXCTR_SHIFT))&REG_SPI_SR_RXCTR_MASK)
#define REG_SPI_SR_TXNXTPTR_MASK                     0xF00u
#define REG_SPI_SR_TXNXTPTR_SHIFT                    8
#define REG_SPI_SR_TXNXTPTR(x)                       (((uint32_t)(((uint32_t)(x))<<REG_SPI_SR_TXNXTPTR_SHIFT))&REG_SPI_SR_TXNXTPTR_MASK)
#define REG_SPI_SR_TXCTR_MASK                        0xF000u
#define REG_SPI_SR_TXCTR_SHIFT                       12
#define REG_SPI_SR_TXCTR(x)                          (((uint32_t)(((uint32_t)(x))<<REG_SPI_SR_TXCTR_SHIFT))&REG_SPI_SR_TXCTR_MASK)
#define REG_SPI_SR_RFDF_MASK                         0x20000u
#define REG_SPI_SR_RFDF_SHIFT                        17
#define REG_SPI_SR_RFOF_MASK                         0x80000u
#define REG_SPI_SR_RFOF_SHIFT                        19
#define REG_SPI_SR_TFFF_MASK                         0x2000000u
#define REG_SPI_SR_TFFF_SHIFT                        25
#define REG_SPI_SR_TFUF_MASK                         0x8000000u
#define REG_SPI_SR_TFUF_SHIFT                        27
#define REG_SPI_SR_EOQF_MASK                         0x10000000u
#define REG_SPI_SR_EOQF_SHIFT                        28
#define REG_SPI_SR_TXRXS_MASK                        0x40000000u
#define REG_SPI_SR_TXRXS_SHIFT                       30
#define REG_SPI_SR_TCF_MASK                          0x80000000u
#define REG_SPI_SR_TCF_SHIFT                         31

//
#define REG_SPI_RSER_RFDF_DIRS_MASK                  0x10000u
#define REG_SPI_RSER_RFDF_DIRS_SHIFT                 16
#define REG_SPI_RSER_RFDF_RE_MASK                    0x20000u
#define REG_SPI_RSER_RFDF_RE_SHIFT                   17
#define REG_SPI_RSER_RFOF_RE_MASK                    0x80000u
#define REG_SPI_RSER_RFOF_RE_SHIFT                   19
#define REG_SPI_RSER_TFFF_DIRS_MASK                  0x1000000u
#define REG_SPI_RSER_TFFF_DIRS_SHIFT                 24
#define REG_SPI_RSER_TFFF_RE_MASK                    0x2000000u
#define REG_SPI_RSER_TFFF_RE_SHIFT                   25
#define REG_SPI_RSER_TFUF_RE_MASK                    0x8000000u
#define REG_SPI_RSER_TFUF_RE_SHIFT                   27
#define REG_SPI_RSER_EOQF_RE_MASK                    0x10000000u
#define REG_SPI_RSER_EOQF_RE_SHIFT                   28
#define REG_SPI_RSER_TCF_RE_MASK                     0x80000000u
#define REG_SPI_RSER_TCF_RE_SHIFT                    31

//
#define REG_SPI_PUSHR_TXDATA_MASK                    0xFFFFu
#define REG_SPI_PUSHR_TXDATA_SHIFT                   0
#define REG_SPI_PUSHR_TXDATA(x)                      (((uint32_t)(((uint32_t)(x))<<REG_SPI_PUSHR_TXDATA_SHIFT))&REG_SPI_PUSHR_TXDATA_MASK)
#define REG_SPI_PUSHR_PCS_MASK                       0x3F0000u
#define REG_SPI_PUSHR_PCS_SHIFT                      16
#define REG_SPI_PUSHR_PCS(x)                         (((uint32_t)(((uint32_t)(x))<<REG_SPI_PUSHR_PCS_SHIFT))&REG_SPI_PUSHR_PCS_MASK)
#define REG_SPI_PUSHR_CTCNT_MASK                     0x4000000u
#define REG_SPI_PUSHR_CTCNT_SHIFT                    26
#define REG_SPI_PUSHR_EOQ_MASK                       0x8000000u
#define REG_SPI_PUSHR_EOQ_SHIFT                      27
#define REG_SPI_PUSHR_CTAS_MASK                      0x70000000u
#define REG_SPI_PUSHR_CTAS_SHIFT                     28
#define REG_SPI_PUSHR_CTAS(x)                        (((uint32_t)(((uint32_t)(x))<<REG_SPI_PUSHR_CTAS_SHIFT))&REG_SPI_PUSHR_CTAS_MASK)
#define REG_SPI_PUSHR_CONT_MASK                      0x80000000u
#define REG_SPI_PUSHR_CONT_SHIFT                     31

//
#define REG_SPI_POPR_RXDATA_MASK                     0xFFFFFFFFu
#define REG_SPI_POPR_RXDATA_SHIFT                    0
#define REG_SPI_POPR_RXDATA(x)                       (((uint32_t)(((uint32_t)(x))<<REG_SPI_POPR_RXDATA_SHIFT))&REG_SPI_POPR_RXDATA_MASK)


//
#define SPI_RX_BUFFER_SIZE     (32)
#define SPI_TX_BUFFER_SIZE     (32)

//
#define SPI_MODE_SLAVE           0
#define SPI_MODE_MASTER          1


//
typedef struct board_kinetis_dspi_info_st {
   unsigned int dspi_base;
   
   int desc_r;
   int desc_w;
   
   unsigned int irq_no;
   unsigned int irq_prio;
   
   unsigned char * input_buffer;
   unsigned int input_r;
   unsigned int input_w;

   
   unsigned char * output_buffer;
   unsigned int output_r;
   unsigned int output_w;
   
   unsigned int clk;
   unsigned char clk_phase;
   unsigned char mode;
   
   unsigned char cs;
   
   cyg_handle_t irq_handle;
	cyg_interrupt irq_it;
   
} board_kinetis_dspi_info_t;

#endif //__DEV_CORTEXM_DSPI_H__


