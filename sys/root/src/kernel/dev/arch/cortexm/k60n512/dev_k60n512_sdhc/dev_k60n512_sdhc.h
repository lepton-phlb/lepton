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
#ifndef __DEV_CORTEXM_SDHC_H__
#define __DEV_CORTEXM_SDHC_H__


/*============================================
| Includes 
==============================================*/

/*============================================
| Declaration  
==============================================*/
#define  REG_SDHC_DSADDR      (0x0) //DMA System Address Register
#define  REG_SDHC_BLKATTR     (0x4) //Block Attributes Register
#define  REG_SDHC_CMDARG      (0x8) //Command Argument Register
#define  REG_SDHC_XFERTYP     (0xc) //Transfer Type Register
#define  REG_SDHC_CMDRSP0     (0x10)//Command Response 0
#define  REG_SDHC_CMDRSP1     (0x14)//Command Response 1
#define  REG_SDHC_CMDRSP2     (0x18)//Command Response 2
#define  REG_SDHC_CMDRSP3     (0x1c)//Command Response 3
#define  REG_SDHC_DATPORT     (0x20)//Buffer Data Port Register
#define  REG_SDHC_PRSSTAT     (0x24)//Present State Register
#define  REG_SDHC_PROCTL      (0x28)//Protocol Control Register
#define  REG_SDHC_SYSCTL      (0x2c)//System Control Register
#define  REG_SDHC_IRQSTAT     (0x30)//Interrupt Status Register
#define  REG_SDHC_IRQSTATEN   (0x34)//Interrupt Status Enable Register
#define  REG_SDHC_IRQSIGEN    (0x38)//Interrupt Signal Enable Register
#define  REG_SDHC_AC12ERR     (0x3c)//Auto CMD12 Error Status Register
#define  REG_SDHC_HTCAPVLT    (0x40)//Host Controller Capabilities
#define  REG_SDHC_WML         (0x44)//Watermark Level Register
#define  REG_SDHC_FEVT        (0x50)//Force Event Register
#define  REG_SDHC_ADMAES      (0x54)//ADMA Error Status Register
#define  REG_SDHC_ADSADDR     (0x58)//ADMA System Address Register
#define  REG_SDHC_VENDOR      (0xc0)//Vendor Specific Register
#define  REG_SDHC_MMCBOOT     (0xc4)//MMC boot Register
#define  REG_SDHC_HOSTVER     (0xfc)//Host Controller Version

//
#define  SIM_SCGC3            (0x40048030)
#define  SIM_SCGC3_SDHC_MASK  (1 << 17)

//
#define PORT_PCR_PS_MASK                         0x1u
#define PORT_PCR_PS_SHIFT                        0
#define PORT_PCR_PE_MASK                         0x2u
#define PORT_PCR_PE_SHIFT                        1
#define PORT_PCR_SRE_MASK                        0x4u
#define PORT_PCR_SRE_SHIFT                       2
#define PORT_PCR_PFE_MASK                        0x10u
#define PORT_PCR_PFE_SHIFT                       4
#define PORT_PCR_ODE_MASK                        0x20u
#define PORT_PCR_ODE_SHIFT                       5
#define PORT_PCR_DSE_MASK                        0x40u
#define PORT_PCR_DSE_SHIFT                       6

//
#define SDHC_ACMD6                           (0x40 + 6)
#define SDHC_ACMD22                          (0x40 + 22)
#define SDHC_ACMD23                          (0x40 + 23)
#define SDHC_ACMD41                          (0x40 + 41)
#define SDHC_ACMD42                          (0x40 + 42)
#define SDHC_ACMD51                          (0x40 + 51)

//
#define REG_SDHC_BLKATTR_BLKSIZE_MASK                0x1FFFu
#define REG_SDHC_BLKATTR_BLKSIZE_SHIFT               0
#define REG_SDHC_BLKATTR_BLKSIZE(x)                  (((uint32_t)(((uint32_t)(x))<<REG_SDHC_BLKATTR_BLKSIZE_SHIFT))&REG_SDHC_BLKATTR_BLKSIZE_MASK)
#define REG_SDHC_BLKATTR_BLKCNT_MASK                 0xFFFF0000u
#define REG_SDHC_BLKATTR_BLKCNT_SHIFT                16
#define REG_SDHC_BLKATTR_BLKCNT(x)                   (((uint32_t)(((uint32_t)(x))<<REG_SDHC_BLKATTR_BLKCNT_SHIFT))&REG_SDHC_BLKATTR_BLKCNT_MASK)

//
#define REG_SDHC_XFERTYP_DMAEN_MASK                  0x1u
#define REG_SDHC_XFERTYP_DMAEN_SHIFT                 0
#define REG_SDHC_XFERTYP_BCEN_MASK                   0x2u
#define REG_SDHC_XFERTYP_BCEN_SHIFT                  1
#define REG_SDHC_XFERTYP_AC12EN_MASK                 0x4u
#define REG_SDHC_XFERTYP_AC12EN_SHIFT                2
#define REG_SDHC_XFERTYP_DTDSEL_MASK                 0x10u
#define REG_SDHC_XFERTYP_DTDSEL_SHIFT                4
#define REG_SDHC_XFERTYP_MSBSEL_MASK                 0x20u
#define REG_SDHC_XFERTYP_MSBSEL_SHIFT                5
#define REG_SDHC_XFERTYP_RSPTYP_MASK                 0x30000u
#define REG_SDHC_XFERTYP_RSPTYP_SHIFT                16
#define REG_SDHC_XFERTYP_RSPTYP(x)                   (((uint32_t)(((uint32_t)(x))<<REG_SDHC_XFERTYP_RSPTYP_SHIFT))&REG_SDHC_XFERTYP_RSPTYP_MASK)
#define REG_SDHC_XFERTYP_CCCEN_MASK                  0x80000u
#define REG_SDHC_XFERTYP_CCCEN_SHIFT                 19
#define REG_SDHC_XFERTYP_CICEN_MASK                  0x100000u
#define REG_SDHC_XFERTYP_CICEN_SHIFT                 20
#define REG_SDHC_XFERTYP_DPSEL_MASK                  0x200000u
#define REG_SDHC_XFERTYP_DPSEL_SHIFT                 21
#define REG_SDHC_XFERTYP_CMDTYP_MASK                 0xC00000u
#define REG_SDHC_XFERTYP_CMDTYP_SHIFT                22
#define REG_SDHC_XFERTYP_CMDTYP(x)                   (((uint32_t)(((uint32_t)(x))<<REG_SDHC_XFERTYP_CMDTYP_SHIFT))&REG_SDHC_XFERTYP_CMDTYP_MASK)
#define REG_SDHC_XFERTYP_CMDINX_MASK                 0x3F000000u
#define REG_SDHC_XFERTYP_CMDINX_SHIFT                24
#define REG_SDHC_XFERTYP_CMDINX(x)                   (((uint32_t)(((uint32_t)(x))<<REG_SDHC_XFERTYP_CMDINX_SHIFT))&REG_SDHC_XFERTYP_CMDINX_MASK)

#define SDHC_XFERTYP_RSPTYP_NO              (0x00)
#define SDHC_XFERTYP_RSPTYP_136             (0x01)
#define SDHC_XFERTYP_RSPTYP_48              (0x02)
#define SDHC_XFERTYP_RSPTYP_48BUSY          (0x03)

#define SDHC_XFERTYP_CMDTYP_ABORT           (0x03)

//
#define REG_SDHC_PRSSTAT_CIHB_MASK                   0x1u
#define REG_SDHC_PRSSTAT_CIHB_SHIFT                  0
#define REG_SDHC_PRSSTAT_CDIHB_MASK                  0x2u
#define REG_SDHC_PRSSTAT_CDIHB_SHIFT                 1
#define REG_SDHC_PRSSTAT_DLA_MASK                    0x4u
#define REG_SDHC_PRSSTAT_DLA_SHIFT                   2
#define REG_SDHC_PRSSTAT_SDSTB_MASK                  0x8u
#define REG_SDHC_PRSSTAT_SDSTB_SHIFT                 3
#define REG_SDHC_PRSSTAT_IPGOFF_MASK                 0x10u
#define REG_SDHC_PRSSTAT_IPGOFF_SHIFT                4
#define REG_SDHC_PRSSTAT_HCKOFF_MASK                 0x20u
#define REG_SDHC_PRSSTAT_HCKOFF_SHIFT                5
#define REG_SDHC_PRSSTAT_PEROFF_MASK                 0x40u
#define REG_SDHC_PRSSTAT_PEROFF_SHIFT                6
#define REG_SDHC_PRSSTAT_SDOFF_MASK                  0x80u
#define REG_SDHC_PRSSTAT_SDOFF_SHIFT                 7
#define REG_SDHC_PRSSTAT_WTA_MASK                    0x100u
#define REG_SDHC_PRSSTAT_WTA_SHIFT                   8
#define REG_SDHC_PRSSTAT_RTA_MASK                    0x200u
#define REG_SDHC_PRSSTAT_RTA_SHIFT                   9
#define REG_SDHC_PRSSTAT_BWEN_MASK                   0x400u
#define REG_SDHC_PRSSTAT_BWEN_SHIFT                  10
#define REG_SDHC_PRSSTAT_BREN_MASK                   0x800u
#define REG_SDHC_PRSSTAT_BREN_SHIFT                  11
#define REG_SDHC_PRSSTAT_CINS_MASK                   0x10000u
#define REG_SDHC_PRSSTAT_CINS_SHIFT                  16
#define REG_SDHC_PRSSTAT_CLSL_MASK                   0x800000u
#define REG_SDHC_PRSSTAT_CLSL_SHIFT                  23
#define REG_SDHC_PRSSTAT_DLSL_MASK                   0xFF000000u
#define REG_SDHC_PRSSTAT_DLSL_SHIFT                  24
#define REG_SDHC_PRSSTAT_DLSL(x)                     (((uint32_t)(((uint32_t)(x))<<REG_SDHC_PRSSTAT_DLSL_SHIFT))&REG_SDHC_PRSSTAT_DLSL_MASK)

//
#define REG_SDHC_PROCTL_LCTL_MASK                    0x1u
#define REG_SDHC_PROCTL_LCTL_SHIFT                   0
#define REG_SDHC_PROCTL_DTW_MASK                     0x6u
#define REG_SDHC_PROCTL_DTW_SHIFT                    1
#define REG_SDHC_PROCTL_DTW(x)                       (((uint32_t)(((uint32_t)(x))<<REG_SDHC_PROCTL_DTW_SHIFT))&REG_SDHC_PROCTL_DTW_MASK)
#define REG_SDHC_PROCTL_D3CD_MASK                    0x8u
#define REG_SDHC_PROCTL_D3CD_SHIFT                   3
#define REG_SDHC_PROCTL_EMODE_MASK                   0x30u
#define REG_SDHC_PROCTL_EMODE_SHIFT                  4
#define REG_SDHC_PROCTL_EMODE(x)                     (((uint32_t)(((uint32_t)(x))<<REG_SDHC_PROCTL_EMODE_SHIFT))&REG_SDHC_PROCTL_EMODE_MASK)
#define REG_SDHC_PROCTL_CDTL_MASK                    0x40u
#define REG_SDHC_PROCTL_CDTL_SHIFT                   6
#define REG_SDHC_PROCTL_CDSS_MASK                    0x80u
#define REG_SDHC_PROCTL_CDSS_SHIFT                   7
#define REG_SDHC_PROCTL_DMAS_MASK                    0x300u
#define REG_SDHC_PROCTL_DMAS_SHIFT                   8
#define REG_SDHC_PROCTL_DMAS(x)                      (((uint32_t)(((uint32_t)(x))<<REG_SDHC_PROCTL_DMAS_SHIFT))&REG_SDHC_PROCTL_DMAS_MASK)
#define REG_SDHC_PROCTL_SABGREQ_MASK                 0x10000u
#define REG_SDHC_PROCTL_SABGREQ_SHIFT                16
#define REG_SDHC_PROCTL_CREQ_MASK                    0x20000u
#define REG_SDHC_PROCTL_CREQ_SHIFT                   17
#define REG_SDHC_PROCTL_RWCTL_MASK                   0x40000u
#define REG_SDHC_PROCTL_RWCTL_SHIFT                  18
#define REG_SDHC_PROCTL_IABG_MASK                    0x80000u
#define REG_SDHC_PROCTL_IABG_SHIFT                   19
#define REG_SDHC_PROCTL_WECINT_MASK                  0x1000000u
#define REG_SDHC_PROCTL_WECINT_SHIFT                 24
#define REG_SDHC_PROCTL_WECINS_MASK                  0x2000000u
#define REG_SDHC_PROCTL_WECINS_SHIFT                 25
#define REG_SDHC_PROCTL_WECRM_MASK                   0x4000000u
#define REG_SDHC_PROCTL_WECRM_SHIFT                  26

#define SDHC_PROCTL_EMODE_INVARIANT       (0x02)
#define SDHC_PROCTL_DTW_1BIT              (0x00)
#define SDHC_PROCTL_DTW_4BIT              (0x01)
#define SDHC_PROCTL_DTW_8BIT              (0x10)

//
#define REG_SDHC_SYSCTL_IPGEN_MASK                   0x1u
#define REG_SDHC_SYSCTL_IPGEN_SHIFT                  0
#define REG_SDHC_SYSCTL_HCKEN_MASK                   0x2u
#define REG_SDHC_SYSCTL_HCKEN_SHIFT                  1
#define REG_SDHC_SYSCTL_PEREN_MASK                   0x4u
#define REG_SDHC_SYSCTL_PEREN_SHIFT                  2
#define REG_SDHC_SYSCTL_SDCLKEN_MASK                 0x8u
#define REG_SDHC_SYSCTL_SDCLKEN_SHIFT                3
#define REG_SDHC_SYSCTL_DVS_MASK                     0xF0u
#define REG_SDHC_SYSCTL_DVS_SHIFT                    4
#define REG_SDHC_SYSCTL_DVS(x)                       (((uint32_t)(((uint32_t)(x))<<REG_SDHC_SYSCTL_DVS_SHIFT))&REG_SDHC_SYSCTL_DVS_MASK)
#define REG_SDHC_SYSCTL_SDCLKFS_MASK                 0xFF00u
#define REG_SDHC_SYSCTL_SDCLKFS_SHIFT                8
#define REG_SDHC_SYSCTL_SDCLKFS(x)                   (((uint32_t)(((uint32_t)(x))<<REG_SDHC_SYSCTL_SDCLKFS_SHIFT))&REG_SDHC_SYSCTL_SDCLKFS_MASK)
#define REG_SDHC_SYSCTL_DTOCV_MASK                   0xF0000u
#define REG_SDHC_SYSCTL_DTOCV_SHIFT                  16
#define REG_SDHC_SYSCTL_DTOCV(x)                     (((uint32_t)(((uint32_t)(x))<<REG_SDHC_SYSCTL_DTOCV_SHIFT))&REG_SDHC_SYSCTL_DTOCV_MASK)
#define REG_SDHC_SYSCTL_RSTA_MASK                    0x1000000u
#define REG_SDHC_SYSCTL_RSTA_SHIFT                   24
#define REG_SDHC_SYSCTL_RSTC_MASK                    0x2000000u
#define REG_SDHC_SYSCTL_RSTC_SHIFT                   25
#define REG_SDHC_SYSCTL_RSTD_MASK                    0x4000000u
#define REG_SDHC_SYSCTL_RSTD_SHIFT                   26
#define REG_SDHC_SYSCTL_INITA_MASK                   0x8000000u
#define REG_SDHC_SYSCTL_INITA_SHIFT                  27

//
#define REG_SDHC_IRQSTAT_CC_MASK                     0x1u
#define REG_SDHC_IRQSTAT_CC_SHIFT                    0
#define REG_SDHC_IRQSTAT_TC_MASK                     0x2u
#define REG_SDHC_IRQSTAT_TC_SHIFT                    1
#define REG_SDHC_IRQSTAT_BGE_MASK                    0x4u
#define REG_SDHC_IRQSTAT_BGE_SHIFT                   2
#define REG_SDHC_IRQSTAT_DINT_MASK                   0x8u
#define REG_SDHC_IRQSTAT_DINT_SHIFT                  3
#define REG_SDHC_IRQSTAT_BWR_MASK                    0x10u
#define REG_SDHC_IRQSTAT_BWR_SHIFT                   4
#define REG_SDHC_IRQSTAT_BRR_MASK                    0x20u
#define REG_SDHC_IRQSTAT_BRR_SHIFT                   5
#define REG_SDHC_IRQSTAT_CINS_MASK                   0x40u
#define REG_SDHC_IRQSTAT_CINS_SHIFT                  6
#define REG_SDHC_IRQSTAT_CRM_MASK                    0x80u
#define REG_SDHC_IRQSTAT_CRM_SHIFT                   7
#define REG_SDHC_IRQSTAT_CINT_MASK                   0x100u
#define REG_SDHC_IRQSTAT_CINT_SHIFT                  8
#define REG_SDHC_IRQSTAT_CTOE_MASK                   0x10000u
#define REG_SDHC_IRQSTAT_CTOE_SHIFT                  16
#define REG_SDHC_IRQSTAT_CCE_MASK                    0x20000u
#define REG_SDHC_IRQSTAT_CCE_SHIFT                   17
#define REG_SDHC_IRQSTAT_CEBE_MASK                   0x40000u
#define REG_SDHC_IRQSTAT_CEBE_SHIFT                  18
#define REG_SDHC_IRQSTAT_CIE_MASK                    0x80000u
#define REG_SDHC_IRQSTAT_CIE_SHIFT                   19
#define REG_SDHC_IRQSTAT_DTOE_MASK                   0x100000u
#define REG_SDHC_IRQSTAT_DTOE_SHIFT                  20
#define REG_SDHC_IRQSTAT_DCE_MASK                    0x200000u
#define REG_SDHC_IRQSTAT_DCE_SHIFT                   21
#define REG_SDHC_IRQSTAT_DEBE_MASK                   0x400000u
#define REG_SDHC_IRQSTAT_DEBE_SHIFT                  22
#define REG_SDHC_IRQSTAT_AC12E_MASK                  0x1000000u
#define REG_SDHC_IRQSTAT_AC12E_SHIFT                 24
#define REG_SDHC_IRQSTAT_DMAE_MASK                   0x10000000u
#define REG_SDHC_IRQSTAT_DMAE_SHIFT                  28

//
#define REG_SDHC_IRQSTATEN_CCSEN_MASK                0x1u
#define REG_SDHC_IRQSTATEN_CCSEN_SHIFT               0
#define REG_SDHC_IRQSTATEN_TCSEN_MASK                0x2u
#define REG_SDHC_IRQSTATEN_TCSEN_SHIFT               1
#define REG_SDHC_IRQSTATEN_BGESEN_MASK               0x4u
#define REG_SDHC_IRQSTATEN_BGESEN_SHIFT              2
#define REG_SDHC_IRQSTATEN_DINTSEN_MASK              0x8u
#define REG_SDHC_IRQSTATEN_DINTSEN_SHIFT             3
#define REG_SDHC_IRQSTATEN_BWRSEN_MASK               0x10u
#define REG_SDHC_IRQSTATEN_BWRSEN_SHIFT              4
#define REG_SDHC_IRQSTATEN_BRRSEN_MASK               0x20u
#define REG_SDHC_IRQSTATEN_BRRSEN_SHIFT              5
#define REG_SDHC_IRQSTATEN_CINSEN_MASK               0x40u
#define REG_SDHC_IRQSTATEN_CINSEN_SHIFT              6
#define REG_SDHC_IRQSTATEN_CRMSEN_MASK               0x80u
#define REG_SDHC_IRQSTATEN_CRMSEN_SHIFT              7
#define REG_SDHC_IRQSTATEN_CINTSEN_MASK              0x100u
#define REG_SDHC_IRQSTATEN_CINTSEN_SHIFT             8
#define REG_SDHC_IRQSTATEN_CTOESEN_MASK              0x10000u
#define REG_SDHC_IRQSTATEN_CTOESEN_SHIFT             16
#define REG_SDHC_IRQSTATEN_CCESEN_MASK               0x20000u
#define REG_SDHC_IRQSTATEN_CCESEN_SHIFT              17
#define REG_SDHC_IRQSTATEN_CEBESEN_MASK              0x40000u
#define REG_SDHC_IRQSTATEN_CEBESEN_SHIFT             18
#define REG_SDHC_IRQSTATEN_CIESEN_MASK               0x80000u
#define REG_SDHC_IRQSTATEN_CIESEN_SHIFT              19
#define REG_SDHC_IRQSTATEN_DTOESEN_MASK              0x100000u
#define REG_SDHC_IRQSTATEN_DTOESEN_SHIFT             20
#define REG_SDHC_IRQSTATEN_DCESEN_MASK               0x200000u
#define REG_SDHC_IRQSTATEN_DCESEN_SHIFT              21
#define REG_SDHC_IRQSTATEN_DEBESEN_MASK              0x400000u
#define REG_SDHC_IRQSTATEN_DEBESEN_SHIFT             22
#define REG_SDHC_IRQSTATEN_AC12ESEN_MASK             0x1000000u
#define REG_SDHC_IRQSTATEN_AC12ESEN_SHIFT            24
#define REG_SDHC_IRQSTATEN_DMAESEN_MASK              0x10000000u
#define REG_SDHC_IRQSTATEN_DMAESEN_SHIFT             28

//
typedef struct mci_cmd_st {
   /// Command status.
   volatile char status;
   /// Command code.
   unsigned int cmd;
   /// Command argument.
   unsigned int arg;
   /// Data buffer.
   unsigned char *p_data;
   /// Size of data buffer in bytes.
   unsigned short block_size;
   /// Number of blocks to be transfered
   unsigned short block_count;
   /// Indicates if the command is a read operation.
   unsigned char is_read;
   /// Response buffer.
   unsigned int  *p_resp;
   /// SD card response type.
   unsigned char  resp_type;
} mci_cmd_t;

#endif //__DEV_CORTEXM_SDHC_H__

