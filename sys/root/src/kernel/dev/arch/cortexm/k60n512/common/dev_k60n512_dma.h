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
#ifndef __DEV_CORTEXM_K60N512_DMA_H__
#define __DEV_CORTEXM_K60N512_DMA_H__


/*============================================
| Includes 
==============================================*/

/*============================================
| Declaration  
==============================================*/
#define REG_DMA_CR               (0x0) //Control Register
#define REG_DMA_ES               (0x4) //Error Status Register
#define REG_DMA_ERQ              (0xc) //Enable Request Register
#define REG_DMA_EEI              (0x14)//Enable Error Interrupt
#define REG_DMA_CEEI             (0x18)//Clear Enable Error Interrupt
#define REG_DMA_SEEI             (0x19)//Set Enable Error Interrupt
#define REG_DMA_CERQ             (0x1a)//Clear Enable Request Interrupt
#define REG_DMA_SERQ             (0x1b)//Set Enable Request Interrupt
#define REG_DMA_CDNE             (0x1c)//Clear DONE Status Bit
#define REG_DMA_SSRT             (0x1d)//Set START Bit Register
#define REG_DMA_CERR             (0x1e)//Clear Error Register
#define REG_DMA_CINT             (0x1f)//Clear Interrupt Request
#define REG_DMA_INT              (0x24)//Interrupt Request Register
#define REG_DMA_ERR              (0x2c)//Error Register
#define REG_DMA_HRS              (0x34)//Hardware Request Status
#define REG_DMA_DCHPRI3          (0x100)//Channel n Priority
#define REG_DMA_DCHPRI2          (0x101)//Channel n Priority
#define REG_DMA_DCHPRI1          (0x102)//Channel n Priority
#define REG_DMA_DCHPRI0          (0x103)//Channel n Priority
#define REG_DMA_DCHPRI7          (0x104)//Channel n Priority
#define REG_DMA_DCHPRI6          (0x105)//Channel n Priority
#define REG_DMA_DCHPRI5          (0x106)//Channel n Priority
#define REG_DMA_DCHPRI4          (0x107)//Channel n Priority
#define REG_DMA_DCHPRI11         (0x108)//Channel n Priority
#define REG_DMA_DCHPRI10         (0x109)//Channel n Priority
#define REG_DMA_DCHPRI9          (0x10a)//Channel n Priority
#define REG_DMA_DCHPRI8          (0x10b)//Channel n Priority
#define REG_DMA_DCHPRI15         (0x10c)//Channel n Priority
#define REG_DMA_DCHPRI14         (0x10d)//Channel n Priority
#define REG_DMA_DCHPRI13         (0x10e)//Channel n Priority
#define REG_DMA_DCHPRI12         (0x10f)//Channel n Priority

#define REG_DMA_TCDx_SADDR       (0x1000)//TCD Source Address
#define REG_DMA_TCDx_SOFF        (0x1004)//TCD Signed Source Address Offset
#define REG_DMA_TCDx_ATTR        (0x1006)//TCD Transfer Attributes
#define REG_DMA_TCDx_NBYTES_MLNO (0x1008)//TCD Minor Byte Count
#define REG_DMA_TCDx_NBYTES_MLON (0x1008)//TCD Signed Minor Loop Offset
#define REG_DMA_TCDx_NBYTES_MLOY (0x1008)//TCD Signed Minor Loop Offset
#define REG_DMA_TCDx_SLAST       (0x100c)//TCD Last Source Address Adjustment
#define REG_DMA_TCDx_DADDR       (0x1010)//TCD Destination Address
#define REG_DMA_TCDx_DOFF        (0x1014)//TCD Signed Destination Address Offset
#define REG_DMA_TCDx_CITER_ELKY  (0x1016)//TCD Current Minor Loop Link, Major Loop Count
#define REG_DMA_TCDx_CITER_ELKN  (0x1016)//
#define REG_DMA_TCDx_DLASTSGA    (0x1018)//TCD Last Destination Address Adjustment
#define REG_DMA_TCDx_CSR         (0x101c)//TCD Control and Status
#define REG_DMA_TCDx_BITER_ELKY  (0x101e)//TCD Beginning Minor Loop Link, Major Loop Count
#define REG_DMA_TCDx_BITER_ELKN  (0x101e)//

//
#define REG_DMA_CR_EDBG_MASK                         0x2u
#define REG_DMA_CR_EDBG_SHIFT                        1
#define REG_DMA_CR_ERCA_MASK                         0x4u
#define REG_DMA_CR_ERCA_SHIFT                        2
#define REG_DMA_CR_HOE_MASK                          0x10u
#define REG_DMA_CR_HOE_SHIFT                         4
#define REG_DMA_CR_HALT_MASK                         0x20u
#define REG_DMA_CR_HALT_SHIFT                        5
#define REG_DMA_CR_CLM_MASK                          0x40u
#define REG_DMA_CR_CLM_SHIFT                         6
#define REG_DMA_CR_EMLM_MASK                         0x80u
#define REG_DMA_CR_EMLM_SHIFT                        7
#define REG_DMA_CR_ECX_MASK                          0x10000u
#define REG_DMA_CR_ECX_SHIFT                         16
#define REG_DMA_CR_CX_MASK                           0x20000u
#define REG_DMA_CR_CX_SHIFT                          17

//
#define REG_DMA_SADDR_SADDR_MASK                     0xFFFFFFFFu
#define REG_DMA_SADDR_SADDR_SHIFT                    0
#define REG_DMA_SADDR_SADDR(x)                       (((uint32_t)(((uint32_t)(x))<<REG_DMA_SADDR_SADDR_SHIFT))&REG_DMA_SADDR_SADDR_MASK)

//
#define REG_DMA_SOFF_SOFF_MASK                       0xFFFFu
#define REG_DMA_SOFF_SOFF_SHIFT                      0
#define REG_DMA_SOFF_SOFF(x)                         (((uint16_t)(((uint16_t)(x))<<REG_DMA_SOFF_SOFF_SHIFT))&REG_DMA_SOFF_SOFF_MASK)

//
#define REG_DMA_ATTR_DSIZE_MASK                      0x7u
#define REG_DMA_ATTR_DSIZE_SHIFT                     0
#define REG_DMA_ATTR_DSIZE(x)                        (((uint16_t)(((uint16_t)(x))<<REG_DMA_ATTR_DSIZE_SHIFT))&REG_DMA_ATTR_DSIZE_MASK)
#define REG_DMA_ATTR_DMOD_MASK                       0xF8u
#define REG_DMA_ATTR_DMOD_SHIFT                      3
#define REG_DMA_ATTR_DMOD(x)                         (((uint16_t)(((uint16_t)(x))<<REG_DMA_ATTR_DMOD_SHIFT))&REG_DMA_ATTR_DMOD_MASK)
#define REG_DMA_ATTR_SSIZE_MASK                      0x700u
#define REG_DMA_ATTR_SSIZE_SHIFT                     8
#define REG_DMA_ATTR_SSIZE(x)                        (((uint16_t)(((uint16_t)(x))<<REG_DMA_ATTR_SSIZE_SHIFT))&REG_DMA_ATTR_SSIZE_MASK)
#define REG_DMA_ATTR_SMOD_MASK                       0xF800u
#define REG_DMA_ATTR_SMOD_SHIFT                      11
#define REG_DMA_ATTR_SMOD(x)                         (((uint16_t)(((uint16_t)(x))<<REG_DMA_ATTR_SMOD_SHIFT))&REG_DMA_ATTR_SMOD_MASK)

//
#define REG_DMA_NBYTES_MLNO_NBYTES_MASK              0xFFFFFFFFu
#define REG_DMA_NBYTES_MLNO_NBYTES_SHIFT             0
#define REG_DMA_NBYTES_MLNO_NBYTES(x)                (((uint32_t)(((uint32_t)(x))<<REG_DMA_NBYTES_MLNO_NBYTES_SHIFT))&REG_DMA_NBYTES_MLNO_NBYTES_MASK)

//
#define REG_DMA_NBYTES_MLOFFYES_NBYTES_MASK          0x3FFu
#define REG_DMA_NBYTES_MLOFFYES_NBYTES_SHIFT         0
#define REG_DMA_NBYTES_MLOFFYES_NBYTES(x)            (((uint32_t)(((uint32_t)(x))<<REG_DMA_NBYTES_MLOFFYES_NBYTES_SHIFT))&REG_DMA_NBYTES_MLOFFYES_NBYTES_MASK)
#define REG_DMA_NBYTES_MLOFFYES_MLOFF_MASK           0x3FFFFC00u
#define REG_DMA_NBYTES_MLOFFYES_MLOFF_SHIFT          10
#define REG_DMA_NBYTES_MLOFFYES_MLOFF(x)             (((uint32_t)(((uint32_t)(x))<<REG_DMA_NBYTES_MLOFFYES_MLOFF_SHIFT))&REG_DMA_NBYTES_MLOFFYES_MLOFF_MASK)
#define REG_DMA_NBYTES_MLOFFYES_DMLOE_MASK           0x40000000u
#define REG_DMA_NBYTES_MLOFFYES_DMLOE_SHIFT          30
#define REG_DMA_NBYTES_MLOFFYES_SMLOE_MASK           0x80000000u
#define REG_DMA_NBYTES_MLOFFYES_SMLOE_SHIFT          31

//
#define REG_DMA_SLAST_SLAST_MASK                     0xFFFFFFFFu
#define REG_DMA_SLAST_SLAST_SHIFT                    0
#define REG_DMA_SLAST_SLAST(x)                       (((uint32_t)(((uint32_t)(x))<<REG_DMA_SLAST_SLAST_SHIFT))&REG_DMA_SLAST_SLAST_MASK)

//
#define REG_DMA_DADDR_DADDR_MASK                     0xFFFFFFFFu
#define REG_DMA_DADDR_DADDR_SHIFT                    0
#define REG_DMA_DADDR_DADDR(x)                       (((uint32_t)(((uint32_t)(x))<<REG_DMA_DADDR_DADDR_SHIFT))&REG_DMA_DADDR_DADDR_MASK)

//
#define REG_DMA_DOFF_DOFF_MASK                       0xFFFFu
#define REG_DMA_DOFF_DOFF_SHIFT                      0
#define REG_DMA_DOFF_DOFF(x)                         (((uint16_t)(((uint16_t)(x))<<REG_DMA_DOFF_DOFF_SHIFT))&REG_DMA_DOFF_DOFF_MASK)

//
#define REG_DMA_CITER_ELINKNO_CITER_MASK             0x7FFFu
#define REG_DMA_CITER_ELINKNO_CITER_SHIFT            0
#define REG_DMA_CITER_ELINKNO_CITER(x)               (((uint16_t)(((uint16_t)(x))<<REG_DMA_CITER_ELINKNO_CITER_SHIFT))&REG_DMA_CITER_ELINKNO_CITER_MASK)
#define REG_DMA_CITER_ELINKNO_ELINK_MASK             0x8000u
#define REG_DMA_CITER_ELINKNO_ELINK_SHIFT            15

//
#define REG_DMA_DLAST_SGA_DLASTSGA_MASK              0xFFFFFFFFu
#define REG_DMA_DLAST_SGA_DLASTSGA_SHIFT             0
#define REG_DMA_DLAST_SGA_DLASTSGA(x)                (((uint32_t)(((uint32_t)(x))<<REG_DMA_DLAST_SGA_DLASTSGA_SHIFT))&REG_DMA_DLAST_SGA_DLASTSGA_MASK)

//
#define REG_DMA_CSR_START_MASK                       0x1u
#define REG_DMA_CSR_START_SHIFT                      0
#define REG_DMA_CSR_INTMAJOR_MASK                    0x2u
#define REG_DMA_CSR_INTMAJOR_SHIFT                   1
#define REG_DMA_CSR_INTHALF_MASK                     0x4u
#define REG_DMA_CSR_INTHALF_SHIFT                    2
#define REG_DMA_CSR_DREQ_MASK                        0x8u
#define REG_DMA_CSR_DREQ_SHIFT                       3
#define REG_DMA_CSR_ESG_MASK                         0x10u
#define REG_DMA_CSR_ESG_SHIFT                        4
#define REG_DMA_CSR_MAJORELINK_MASK                  0x20u
#define REG_DMA_CSR_MAJORELINK_SHIFT                 5
#define REG_DMA_CSR_ACTIVE_MASK                      0x40u
#define REG_DMA_CSR_ACTIVE_SHIFT                     6
#define REG_DMA_CSR_DONE_MASK                        0x80u
#define REG_DMA_CSR_DONE_SHIFT                       7
#define REG_DMA_CSR_MAJORLINKCH_MASK                 0xF00u
#define REG_DMA_CSR_MAJORLINKCH_SHIFT                8
#define REG_DMA_CSR_MAJORLINKCH(x)                   (((uint16_t)(((uint16_t)(x))<<REG_DMA_CSR_MAJORLINKCH_SHIFT))&REG_DMA_CSR_MAJORLINKCH_MASK)
#define REG_DMA_CSR_BWC_MASK                         0xC000u
#define REG_DMA_CSR_BWC_SHIFT                        14
#define REG_DMA_CSR_BWC(x)                           (((uint16_t)(((uint16_t)(x))<<REG_DMA_CSR_BWC_SHIFT))&REG_DMA_CSR_BWC_MASK)

//
#define REG_DMA_BITER_ELINKNO_BITER_MASK             0x7FFFu
#define REG_DMA_BITER_ELINKNO_BITER_SHIFT            0
#define REG_DMA_BITER_ELINKNO_BITER(x)               (((uint16_t)(((uint16_t)(x))<<REG_DMA_BITER_ELINKNO_BITER_SHIFT))&REG_DMA_BITER_ELINKNO_BITER_MASK)
#define REG_DMA_BITER_ELINKNO_ELINK_MASK             0x8000u
#define REG_DMA_BITER_ELINKNO_ELINK_SHIFT            15

#endif //__DEV_CORTEXM_K60N512_DMA_H__
