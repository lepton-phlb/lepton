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
#ifndef __DEV_CORTEXM_K60N512_ENET_H__
#define __DEV_CORTEXM_K60N512_ENET_H__


/*============================================
| Includes
==============================================*/

/*============================================
| Declaration
==============================================*/
#define REG_ENET_EIR    (0x4) // ENET Interrupt Event Register
#define REG_ENET_EIMR   (0x8) // ENET Interrupt Mask Register
#define REG_ENET_RDAR   (0x10) // ENET Receive Descriptor Active Register
#define REG_ENET_TDAR   (0x14) // ENET Transmit Descriptor Active Register
#define REG_ENET_ECR    (0x24) // ENET Ethernet Control Register
#define REG_ENET_MMFR   (0x40) // ENET MII Management Frame Register
#define REG_ENET_MSCR   (0x44) // ENET MII Speed Control Register
#define REG_ENET_MIBC   (0x64) // ENET MIB Control Register
#define REG_ENET_RCR    (0x84) // ENET Receive Control Register
#define REG_ENET_TCR    (0xc4) // ENET Transmit Control Register
#define REG_ENET_PALR   (0xe4) // ENET Physical Address Lower Register
#define REG_ENET_PAUR   (0xe8) // ENET Physical Address Upper Register
#define REG_ENET_OPD    (0xec) // ENET Opcode/Pause Duration Register
#define REG_ENET_IAUR   (0x118) // ENET Descriptor Individual Upper Address Register
#define REG_ENET_IALR   (0x11c) // ENET Descriptor Individual Lower Address Register
#define REG_ENET_GAUR   (0x120) // ENET Descriptor Group Upper Address Register
#define REG_ENET_GALR   (0x124) // ENET Descriptor Group Lower Address Register
#define REG_ENET_TFWR   (0x144) // ENET Transmit FIFO Watermark Register
#define REG_ENET_RDSR   (0x180) // ENET Receive Descriptor RingStart Register
#define REG_ENET_TDSR   (0x184) // ENET Transmit Buffer Descriptor Ring Start Register
#define REG_ENET_MRBR   (0x188) // ENET Maximum Receive Buffer Size Register


//EIR bits fields
#define REG_ENET_EIR_BABR                       (0x40000000)
#define REG_ENET_EIR_BABT                       (0x20000000)
#define REG_ENET_EIR_GRA                        (0x10000000)
#define REG_ENET_EIR_TXF                        (0x8000000)
#define REG_ENET_EIR_TXB                        (0x4000000)
#define REG_ENET_EIR_RXF                        (0x2000000)
#define REG_ENET_EIR_RXB                        (0x1000000)
#define REG_ENET_EIR_MII                        (0x800000)
#define REG_ENET_EIR_EBERR                      (0x400000)
#define REG_ENET_EIR_LC                         (0x200000)
#define REG_ENET_EIR_RL                         (0x100000)
#define REG_ENET_EIR_UN                         (0x80000)
#define REG_ENET_EIR_PLR                        (0x40000)
#define REG_ENET_EIR_WAKEUP                     (0x20000)
#define REG_ENET_EIR_TS_AVAIL           (0x10000)
#define REG_ENET_EIR_TS_TIMER           (0x8000)

//EIMR bits fields
#define REG_ENET_EIMR_BABR                      REG_ENET_EIR_BABR
#define REG_ENET_EIMR_BABT                      REG_ENET_EIR_BABT
#define REG_ENET_EIMR_GRA                       REG_ENET_EIR_GRA
#define REG_ENET_EIMR_TXF                       REG_ENET_EIR_TXF
#define REG_ENET_EIMR_TXB                       REG_ENET_EIR_TXB
#define REG_ENET_EIMR_RXF                       REG_ENET_EIR_RXF
#define REG_ENET_EIMR_RXB                       REG_ENET_EIR_RXB
#define REG_ENET_EIMR_MII                       REG_ENET_EIR_MII
#define REG_ENET_EIMR_EBERR                     REG_ENET_EIR_EBERR
#define REG_ENET_EIMR_LC                        REG_ENET_EIR_LC
#define REG_ENET_EIMR_RL                        REG_ENET_EIR_RL
#define REG_ENET_EIMR_UN                        REG_ENET_EIR_UN
#define REG_ENET_EIMR_PLR                       REG_ENET_EIR_PLR
#define REG_ENET_EIMR_WAKEUP            REG_ENET_EIR_WAKEUP
#define REG_ENET_EIMR_TS_AVAIL          REG_ENET_EIR_TS_AVAIL
#define REG_ENET_EIMR_TS_TIMER          REG_ENET_EIR_TS_TIMER

//RDAR bits fields
#define REG_ENET_RDAR_RDAR                      (0x1000000)

//TDAR bits fields
#define REG_ENET_TDAR_TDAR                      (0x1000000)

//ECR bits fields
#define REG_ENET_ECR_STOPEN                     (0x80)
#define REG_ENET_ECR_DBGEN                      (0x40)
#define REG_ENET_ECR_EN1588                     (0x10)
#define REG_ENET_ECR_SLEEP                      (0x8)
#define REG_ENET_ECR_MAGICEN            (0x4)
#define REG_ENET_ECR_ETHEREN            (0x2)
#define REG_ENET_ECR_RESET                      (0x1)

//MMFR bits fields
#define REG_ENET_MMFR_ST_MASK           (0xC0000000)
#define REG_ENET_MMFR_ST_SHIFT          30
#define REG_ENET_MMFR_ST(x)                     (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_MMFR_ST_SHIFT))& \
                                                 REG_ENET_MMFR_ST_MASK)
#define REG_ENET_MMFR_OP_MASK           (0x30000000)
#define REG_ENET_MMFR_OP_SHIFT          28
#define REG_ENET_MMFR_OP(x)                     (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_MMFR_OP_SHIFT))& \
                                                 REG_ENET_MMFR_OP_MASK)
#define REG_ENET_MMFR_PA_MASK           (0xF800000)
#define REG_ENET_MMFR_PA_SHIFT          23
#define REG_ENET_MMFR_PA(x)                     (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_MMFR_PA_SHIFT))& \
                                                 REG_ENET_MMFR_PA_MASK)
#define REG_ENET_MMFR_RA_MASK           (0x7C0000)
#define REG_ENET_MMFR_RA_SHIFT          18
#define REG_ENET_MMFR_RA(x)                     (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_MMFR_RA_SHIFT))& \
                                                 REG_ENET_MMFR_RA_MASK)
#define REG_ENET_MMFR_TA_MASK           (0x30000)
#define REG_ENET_MMFR_TA_SHIFT          16
#define REG_ENET_MMFR_TA(x)                     (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_MMFR_TA_SHIFT))& \
                                                 REG_ENET_MMFR_TA_MASK)
#define REG_ENET_MMFR_DATA_MASK         (0xFFFF)
#define REG_ENET_MMFR_DATA_SHIFT        0
#define REG_ENET_MMFR_DATA(x)           (((unsigned int)(((unsigned int)(x))<< \
                                                         REG_ENET_MMFR_DATA_SHIFT))& \
                                         REG_ENET_MMFR_DATA_MASK)

//MSCR bits fields
#define REG_ENET_MSCR_HOLDTIME_MASK                     (0x700)
#define REG_ENET_MSCR_HOLDTIME_SHIFT            8
#define REG_ENET_MSCR_HOLDTIME(x)                       (((unsigned int)(((unsigned int)(x))<< \
                                                                         REG_ENET_MSCR_HOLDTIME_SHIFT)) \
                                                         &REG_ENET_MSCR_HOLDTIME_MASK)
#define REG_ENET_MSCR_DIS_PRE_MASK              (0x80)
#define REG_ENET_MSCR_MII_SPEED_MASK            (0x7E)
#define REG_ENET_MSCR_MII_SPEED_SHIFT           1
#define REG_ENET_MSCR_MII_SPEED(x)                      (((unsigned int)(((unsigned int)(x))<< \
                                                                         REG_ENET_MSCR_MII_SPEED_SHIFT)) \
                                                         &REG_ENET_MSCR_MII_SPEED_MASK)

//MIBC bits fields
#define REG_ENET_MIBC_MIB_DIS           (0x80000000)
#define REG_ENET_MIBC_MIB_IDLE          (0x40000000)
#define REG_ENET_MIBC_MIB_CLEAR         (0x20000000)

//RCR bits fields
#define REG_ENET_RCR_GRS                                (0x80000000)
#define REG_ENET_RCR_NLC                                (0x40000000)
#define REG_ENET_RCR_MAX_FL_MASK                (0x3FFF0000)
#define REG_ENET_RCR_MAX_FL_SHIFT               16
#define REG_ENET_RCR_MAX_FL(x)                  (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_RCR_MAX_FL_SHIFT))& \
                                                 REG_ENET_RCR_MAX_FL_MASK)
#define REG_ENET_RCR_CFEN                               (0x8000)
#define REG_ENET_RCR_CRCFWD                             (0x4000)
#define REG_ENET_RCR_PAUFWD                             (0x2000)
#define REG_ENET_RCR_PADEN                              (0x1000)
#define REG_ENET_RCR_RMII_10T                   (0x200)
#define REG_ENET_RCR_RMII_MODE                  (0x100)
#define REG_ENET_RCR_FCE                                (0x20)
#define REG_ENET_RCR_BC_REJ                             (0x10)
#define REG_ENET_RCR_PROM                               (0x8)
#define REG_ENET_RCR_MII_MODE                   (0x4)
#define REG_ENET_RCR_DRT                                (0x2)
#define REG_ENET_RCR_LOOP                               (0x1)

//TCR bits fields
#define REG_ENET_TCR_CRCFWD                             (0x200)
#define REG_ENET_TCR_ADDINS                             (0x100)
#define REG_ENET_TCR_ADDSEL_MASK                (0xE0)
#define REG_ENET_TCR_ADDSEL_SHIFT               5
#define REG_ENET_TCR_ADDSEL(x)                  (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_TCR_ADDSEL_SHIFT))& \
                                                 REG_ENET_TCR_ADDSEL_MASK)
#define REG_ENET_TCR_RFC_PAUSE                  (0x10)
#define REG_ENET_TCR_TFC_PAUSE                  (0x8)
#define REG_ENET_TCR_FDEN                               (0x4)
#define REG_ENET_TCR_GTS                                (0x1)

//PALR bits fields
#define REG_ENET_PALR_PADDR1_MASK               (0xFFFFFFFF)
#define REG_ENET_PALR_PADDR1_SHIFT              0
#define REG_ENET_PALR_PADDR1(x)                 (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_PALR_PADDR1_SHIFT))& \
                                                 REG_ENET_PALR_PADDR1_MASK)

//PAUR bits fields
#define REG_ENET_PAUR_PADDR2_MASK               (0xFFFF0000)
#define REG_ENET_PAUR_PADDR2_SHIFT              16
#define REG_ENET_PAUR_PADDR2(x)                 (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_PAUR_PADDR2_SHIFT))& \
                                                 REG_ENET_PAUR_PADDR2_MASK)
#define REG_ENET_PAUR_TYPE_MASK                 (0xFFFF)
#define REG_ENET_PAUR_TYPE_SHIFT                0
#define REG_ENET_PAUR_TYPE(x)                   (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_PAUR_TYPE_SHIFT))& \
                                                 REG_ENET_PAUR_TYPE_MASK)

//OPD bits fields
#define REG_ENET_OPD_OPCODE_MASK                (0xFFFF0000)
#define REG_ENET_OPD_OPCODE_SHIFT               16
#define REG_ENET_OPD_OPCODE(x)                  (((unsigned int)(((unsigned int)(x))<< \
                                                                 ENET_OPD_OPCODE_SHIFT))& \
                                                 ENET_OPD_OPCODE_MASK)
#define REG_ENET_OPD_PAUSE_DUR_MASK             (0xFFFF)
#define REG_ENET_OPD_PAUSE_DUR_SHIFT    0
#define REG_ENET_OPD_PAUSE_DUR(x)               (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_OPD_PAUSE_DUR_SHIFT))& \
                                                 REG_ENET_OPD_PAUSE_DUR_MASK)

//IAUR Bit Fields
#define REG_ENET_IAUR_IADDR1_MASK               (0xFFFFFFFF)
#define REG_ENET_IAUR_IADDR1_SHIFT      0
#define REG_ENET_IAUR_IADDR1(x)                 (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_IAUR_IADDR1_SHIFT))& \
                                                 REG_ENET_IAUR_IADDR1_MASK)

//IALR Bit Fields
#define REG_ENET_IALR_IADDR2_MASK               (0xFFFFFFFF)
#define REG_ENET_IALR_IADDR2_SHIFT              0
#define REG_ENET_IALR_IADDR2(x)                 (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_IALR_IADDR2_SHIFT))& \
                                                 REG_ENET_IALR_IADDR2_MASK)

//GAUR Bit Fields
#define REG_ENET_GAUR_GADDR1_MASK               (0xFFFFFFFF)
#define REG_ENET_GAUR_GADDR1_SHIFT              0
#define REG_ENET_GAUR_GADDR1(x)                 (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_GAUR_GADDR1_SHIFT))& \
                                                 REG_ENET_GAUR_GADDR1_MASK)

//GALR Bit Fields
#define REG_ENET_GALR_GADDR2_MASK               (0xFFFFFFFF)
#define REG_ENET_GALR_GADDR2_SHIFT              0
#define REG_ENET_GALR_GADDR2(x)                 (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_GALR_GADDR2_SHIFT))& \
                                                 REG_ENET_GALR_GADDR2_MASK)

//TFWR Bit Fields
#define REG_ENET_TFWR_STRFWD_MASK               (0x100)
#define REG_ENET_TFWR_TFWR_MASK                 (0x3F)
#define REG_ENET_TFWR_TFWR_SHIFT                0
#define REG_ENET_TFWR_TFWR(x)                   (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_TFWR_TFWR_SHIFT))& \
                                                 REG_ENET_TFWR_TFWR_MASK)

//RDSR Bit Fields
#define REG_ENET_RDSR_R_DES_START_MASK          (0xFFFFFFF8)
#define REG_ENET_RDSR_R_DES_START_SHIFT         3
#define REG_ENET_RDSR_R_DES_START(x)            (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_RDSR_R_DES_START_SHIFT))& \
                                                 REG_ENET_RDSR_R_DES_START_MASK)

//TDSR Bit Fields
#define REG_ENET_TDSR_X_DES_START_MASK          (0xFFFFFFF8)
#define REG_ENET_TDSR_X_DES_START_SHIFT 3
#define REG_ENET_TDSR_X_DES_START(x)            (((unsigned int)(((unsigned int)(x))<< \
                                                                 REG_ENET_TDSR_X_DES_START_SHIFT))& \
                                                 REG_ENET_TDSR_X_DES_START_MASK)

//MRBR Bit Fields
#define REG_ENET_MRBR_R_BUF_SIZE_MASK           (0x3FF0)
#define REG_ENET_MRBR_R_BUF_SIZE_SHIFT          4
#define REG_ENET_MRBR_R_BUF_SIZE(x)                     (((unsigned int)(((unsigned int)(x))<< \
                                                                         REG_ENET_MRBR_R_BUF_SIZE_SHIFT)) \
                                                         &REG_ENET_MRBR_R_BUF_SIZE_MASK)

//In little endian mode
// TX Buffer Descriptor Bit Definitions
#define REG_ENET_TX_BD_R                                (0x0080)
#define REG_ENET_TX_BD_TO1                              (0x0040)
#define REG_ENET_TX_BD_W                                (0x0020)
#define REG_ENET_TX_BD_TO2                              (0x0010)
#define REG_ENET_TX_BD_L                                (0x0008)
#define REG_ENET_TX_BD_TC                               (0x0004)
#define REG_ENET_TX_BD_ABC                              (0x0002)

// RX Buffer Descriptor Bit Definitions
#define REG_ENET_RX_BD_E                                (0x0080)
#define REG_ENET_RX_BD_R01                              (0x0040)
#define REG_ENET_RX_BD_W                                (0x0020)
#define REG_ENET_RX_BD_R02                              (0x0010)
#define REG_ENET_RX_BD_L                                (0x0008)
#define REG_ENET_RX_BD_M                                (0x0001)
#define REG_ENET_RX_BD_BC                               (0x8000)
#define REG_ENET_RX_BD_MC                               (0x4000)
#define REG_ENET_RX_BD_LG                               (0x2000)
#define REG_ENET_RX_BD_NO                               (0x1000)
#define REG_ENET_RX_BD_CR                               (0x0400)
#define REG_ENET_RX_BD_OV                               (0x0200)
#define REG_ENET_RX_BD_TR                               (0x0100)

///// PHY REGISTER aka ksz8041_nl /////
// MII Register Addresses
#define REG_PHY_BMCR                    (0x00)
#define REG_PHY_BMSR                    (0x01)
#define REG_PHY_PHYIDR1                 (0x02)
#define REG_PHY_PHYIDR2                 (0x03)
#define REG_PHY_ANAR                    (0x04)
#define REG_PHY_ANLPAR                  (0x05)
#define REG_PHY_ANLPARNP                (0x05)
#define REG_PHY_ANER                    (0x06)
#define REG_PHY_ANNPTR                  (0x07)
#define REG_PHY_PHYSTS                  (0x10)
#define REG_PHY_MICR                    (0x11)
#define REG_PHY_MISR                    (0x12)
#define REG_PHY_PAGESEL                 (0x13)
#define REG_PHY_PHYCR2                  (0x1C)
#define REG_PHY_PHYCTRL1                (0x1E)
#define REG_PHY_PHYCTRL2                (0x1F)

// Bit definitions and macros for PHY_BMCR
#define REG_PHY_BMCR_RESET              (0x8000)
#define REG_PHY_BMCR_LOOP               (0x4000)
#define REG_PHY_BMCR_SPEED              (0x2000)
#define REG_PHY_BMCR_AN_ENABLE          (0x1000)
#define REG_PHY_BMCR_POWERDOWN          (0x0800)
#define REG_PHY_BMCR_ISOLATE            (0x0400)
#define REG_PHY_BMCR_AN_RESTART         (0x0200)
#define REG_PHY_BMCR_FDX                (0x0100)
#define REG_PHY_BMCR_COL_TEST           (0x0080)

// Bit definitions and macros for PHY_BMSR
#define REG_PHY_BMSR_100BT4             (0x8000)
#define REG_PHY_BMSR_100BTX_FDX         (0x4000)
#define REG_PHY_BMSR_100BTX             (0x2000)
#define REG_PHY_BMSR_10BT_FDX           (0x1000)
#define REG_PHY_BMSR_10BT               (0x0800)
#define REG_PHY_BMSR_NO_PREAMBLE        (0x0040)
#define REG_PHY_BMSR_AN_COMPLETE        (0x0020)
#define REG_PHY_BMSR_REMOTE_FAULT       (0x0010)
#define REG_PHY_BMSR_AN_ABILITY         (0x0008)
#define REG_PHY_BMSR_LINK               (0x0004)
#define REG_PHY_BMSR_JABBER             (0x0002)
#define REG_PHY_BMSR_EXTENDED           (0x0001)

// Bit definitions and macros for PHY_ANAR
#define REG_PHY_ANAR_NEXT_PAGE          (0x8001)
#define REG_PHY_ANAR_REM_FAULT          (0x2001)
#define REG_PHY_ANAR_PAUSE              (0x0401)
#define REG_PHY_ANAR_100BT4             (0x0201)
#define REG_PHY_ANAR_100BTX_FDX         (0x0101)
#define REG_PHY_ANAR_100BTX             (0x0081)
#define REG_PHY_ANAR_10BT_FDX           (0x0041)
#define REG_PHY_ANAR_10BT               (0x0021)
#define REG_PHY_ANAR_802_3              (0x0001)

// Bit definitions and macros for PHY_ANLPAR
#define REG_PHY_ANLPAR_NEXT_PAGE        (0x8000)
#define REG_PHY_ANLPAR_ACK              (0x4000)
#define REG_PHY_ANLPAR_REM_FAULT        (0x2000)
#define REG_PHY_ANLPAR_PAUSE            (0x0400)
#define REG_PHY_ANLPAR_100BT4           (0x0200)
#define REG_PHY_ANLPAR_100BTX_FDX       (0x0100)
#define REG_PHY_ANLPAR_100BTX           (0x0080)
#define REG_PHY_ANLPAR_10BTX_FDX        (0x0040)
#define REG_PHY_ANLPAR_10BT             (0x0020)


// Bit definitions of PHY_PHYSTS: National
#define REG_PHY_PHYSTS_MDIXMODE         (0x4000)
#define REG_PHY_PHYSTS_RX_ERR_LATCH     (0x2000)
#define REG_PHY_PHYSTS_POL_STATUS       (0x1000)
#define REG_PHY_PHYSTS_FALSECARRSENSLAT (0x0800)
#define REG_PHY_PHYSTS_SIGNALDETECT     (0x0400)
#define REG_PHY_PHYSTS_PAGERECEIVED     (0x0100)
#define REG_PHY_PHYSTS_MIIINTERRUPT     (0x0080)
#define REG_PHY_PHYSTS_REMOTEFAULT      (0x0040)
#define REG_PHY_PHYSTS_JABBERDETECT     (0x0020)
#define REG_PHY_PHYSTS_AUTONEGCOMPLETE  (0x0010)
#define REG_PHY_PHYSTS_LOOPBACKSTATUS   (0x0008)
#define REG_PHY_PHYSTS_DUPLEXSTATUS     (0x0004)
#define REG_PHY_PHYSTS_SPEEDSTATUS      (0x0002)
#define REG_PHY_PHYSTS_LINKSTATUS       (0x0001)


// Bit definitions of PHY_PHYCR2
#define REG_PHY_PHYCR2_SYNC_ENET_EN     (0x2000)
#define REG_PHY_PHYCR2_CLK_OUT_RXCLK    (0x1000)
#define REG_PHY_PHYCR2_BC_WRITE         (0x0800)
#define REG_PHY_PHYCR2_PHYTER_COMP      (0x0400)
#define REG_PHY_PHYCR2_SOFT_RESET       (0x0200)
#define REG_PHY_PHYCR2_CLK_OUT_DIS      (0x0001)

// Bit definition and macros for PHY_PHYCTRL1
#define REG_PHY_PHYCTRL1_LED_MASK       (0xC000)
#define REG_PHY_PHYCTRL1_POLARITY       (0x2000)
#define REG_PHY_PHYCTRL1_MDX_STATE      (0x0800)
#define REG_PHY_PHYCTRL1_REMOTE_LOOP    (0x0080)

// Bit definition and macros for PHY_PHYCTRL2
#define REG_PHY_PHYCTRL2_HP_MDIX        (0x8000)
#define REG_PHY_PHYCTRL2_MDIX_SELECT    (0x4000)
#define REG_PHY_PHYCTRL2_PAIRSWAP_DIS   (0x2000)
#define REG_PHY_PHYCTRL2_ENERGY_DET     (0x1000)
#define REG_PHY_PHYCTRL2_FORCE_LINK     (0x0800)
#define REG_PHY_PHYCTRL2_POWER_SAVING   (0x0400)
#define REG_PHY_PHYCTRL2_INT_LEVEL      (0x0200)
#define REG_PHY_PHYCTRL2_EN_JABBER      (0x0100)
#define REG_PHY_PHYCTRL2_AUTONEG_CMPLT  (0x0080)
#define REG_PHY_PHYCTRL2_ENABLE_PAUSE   (0x0040)
#define REG_PHY_PHYCTRL2_PHY_ISOLATE    (0x0020)
#define REG_PHY_PHYCTRL2_OP_MOD_MASK    (0x001C)
#define REG_PHY_PHYCTRL2_EN_SQE_TEST    (0x0002)
#define REG_PHY_PHYCTRL2_DATA_SCRAM_DIS (0x0001)

// Bit definitions of PHY_PHYCTRL2_OP_MOD_MASK */
#define REG_PHY_PHYCTRL2_OP_MOD_SHIFT                   2
#define REG_PHY_PHYCTRL2_MODE_OP_MOD_STILL_NEG          0
#define REG_PHY_PHYCTRL2_MODE_OP_MOD_10MBPS_HD          1
#define REG_PHY_PHYCTRL2_MODE_OP_MOD_100MBPS_HD         2
#define REG_PHY_PHYCTRL2_MODE_OP_MOD_10MBPS_FD          5
#define REG_PHY_PHYCTRL2_MODE_OP_MOD_100MBPS_FD         6
#define REG_PHY_PHYCTRL2_DUPLEX_STATUS                          (4<<2)
#define REG_PHY_PHYCTRL2_SPEED_STATUS                           (1<<2)

#endif //__DEV_CORTEXM_K60N512_ENET_H__
