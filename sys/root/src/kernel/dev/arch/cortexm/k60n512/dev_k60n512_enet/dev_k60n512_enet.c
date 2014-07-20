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
#include "kernel/core/ioctl_eth.h"

#include "dev_k60n512_enet.h"
#include "kernel/dev/arch/cortexm/k60n512/common/dev_k60n512_sim.h"

#if defined(__KERNEL_UCORE_ECOS)
   #include <cyg/hal/cortexm_regs.h>

   #define __REV        CYGARC_REV
   #define __REV16      CYGARC_REV16
   #define __REVSH      CYGARC_REVSH

#endif
/*===========================================
Global Declaration
=============================================*/
int dev_k60n512_enet_load(void);
int dev_k60n512_enet_open(desc_t desc, int o_flag);
int dev_k60n512_enet_close(desc_t desc);
int dev_k60n512_enet_isset_read(desc_t desc);
int dev_k60n512_enet_isset_write(desc_t desc);
int dev_k60n512_enet_seek(desc_t desc,int offset,int origin);
int dev_k60n512_enet_read(desc_t desc, char* buf,int cb);
int dev_k60n512_enet_write(desc_t desc, const char* buf,int cb);
int dev_k60n512_enet_ioctl(desc_t desc,int request,va_list ap);

//
#define PHY_MII_TIMEOUT       (0x1FFFF)
#define REG_PHY_ADDRESS       1

#if 0
   #define PHY_LINK_DELAY        500
#else
   #define PHY_LINK_DELAY        5000
#endif

#define PHY_LINK_RETRY        5000

//
#define KINETIS_ENET_VECTOR_NO                          3
#define KINETIS_ENET_VECTOR_PRIORITY            3
#define KINETIS_ENET_TX_IRQ            77 //76
//#define KINETIS_ENET_TX_VECTOR       92
#define KINETIS_ENET_RX_IRQ            78 //77
//#define KINETIS_ENET_RX_VECTOR       93
#define KINETIS_ENET_ERR_IRQ           79 //78
//#define KINETIS_ENET_ERR_VECTOR      94

//
#define KINETIS_ENET_RX_BUFFER_SIZE       1520
#define KINETIS_ENET_RX_BUFFER_ALIGNMENT  16
#define KINETIS_ENET_RX_BUFS              8 //1//1//4
#define KINETIS_ENET_TX_BUFS              1

//

//just simple descriptor now
typedef struct enet_buf_desc_st {
   unsigned short status;       //control and status
   unsigned short length;       //transfer length
   unsigned char  *data;        //buffer address
} enet_buf_desc_t;

typedef struct board_kinetis_enet_info_st {
   unsigned int enet_base;

   desc_t _desc_rd;
   desc_t _desc_wr;

   unsigned int _input_r;
   unsigned int _input_w;
   unsigned int _output_r;
   unsigned int _output_w;

   unsigned char mac_addr[6];

   cyg_vector_t interrupts[KINETIS_ENET_VECTOR_NO];     // interrupt vector used by controller
   cyg_priority_t priorities[KINETIS_ENET_VECTOR_NO];

   enet_buf_desc_t *p_tx_desc;
   enet_buf_desc_t *p_rx_desc;

   unsigned char *rx_buf;

   //for lepton event
   unsigned char _flag_w_irq;
   unsigned char _flag_r_irq;

   //ethernet status
   eth_stat_t eth_stat;

   cyg_handle_t interrupt_handles[KINETIS_ENET_VECTOR_NO];
   cyg_interrupt interrupt_objects[KINETIS_ENET_VECTOR_NO];

} board_kinetis_enet_info_t;

//
static unsigned char kinetis_enet_tx_desc[KINETIS_ENET_TX_BUFS*
                                          sizeof(enet_buf_desc_t)] __attribute((aligned(
                                                                                   KINETIS_ENET_RX_BUFFER_ALIGNMENT)));
static unsigned char kinetis_enet_rx_desc[KINETIS_ENET_RX_BUFS*
                                          sizeof(enet_buf_desc_t)] __attribute((aligned(
                                                                                   KINETIS_ENET_RX_BUFFER_ALIGNMENT)));
static unsigned char kinetis_enet_rx_buffers[KINETIS_ENET_RX_BUFS*
                                             KINETIS_ENET_RX_BUFFER_SIZE] __attribute((aligned(
                                                                                          KINETIS_ENET_RX_BUFFER_ALIGNMENT)));

#define KINETIS_ENET_MAC_ADDR_1 0x12
#define KINETIS_ENET_MAC_ADDR_2 0x34
#define KINETIS_ENET_MAC_ADDR_3 0x56
#define KINETIS_ENET_MAC_ADDR_4 0x78
#define KINETIS_ENET_MAC_ADDR_5 0x9a
#define KINETIS_ENET_MAC_ADDR_6 0xbc

static board_kinetis_enet_info_t kinetis_enet_info = {
   enet_base : 0x400C0000,
   _desc_rd : -1,
   _desc_wr : -1,
   _input_r : 0,
   _output_r : -1,
   _output_w : 0,
   mac_addr : {KINETIS_ENET_MAC_ADDR_1,KINETIS_ENET_MAC_ADDR_2,KINETIS_ENET_MAC_ADDR_3,
               KINETIS_ENET_MAC_ADDR_4,KINETIS_ENET_MAC_ADDR_5,KINETIS_ENET_MAC_ADDR_6},
   interrupts : {KINETIS_ENET_TX_IRQ, KINETIS_ENET_RX_IRQ, KINETIS_ENET_ERR_IRQ},
   priorities : {KINETIS_ENET_VECTOR_PRIORITY, KINETIS_ENET_VECTOR_PRIORITY,
                 KINETIS_ENET_VECTOR_PRIORITY},
   p_tx_desc : (enet_buf_desc_t *)&(kinetis_enet_tx_desc[0]),
   p_rx_desc : (enet_buf_desc_t *)&(kinetis_enet_rx_desc[0]),
   rx_buf : &(kinetis_enet_rx_buffers[0]),
   _flag_r_irq : 0,
   eth_stat : ETH_STAT_LINK_DOWN,
   interrupt_handles : {0, 0, 0}
};
//
static void _kinetis_enet_mii_init(board_kinetis_enet_info_t *penet, unsigned int sys_clk_mhz);
static int _kinetis_enet_mii_write(board_kinetis_enet_info_t *penet, unsigned int phy_addr,
                                   unsigned int reg_addr,
                                   unsigned int data);
static int _kinetis_enet_mii_read(board_kinetis_enet_info_t *penet, unsigned int phy_addr,
                                  unsigned int reg_addr,
                                  unsigned int *data);
static void _kinetis_enet_config_buffers(board_kinetis_enet_info_t *penet);
static unsigned char _kinetis_enet_hash_addr(const unsigned char* addr);
static void _kinetis_enet_set_mac_addr(board_kinetis_enet_info_t *penet, const unsigned char *pa);

static cyg_uint32 _kinetis_enet_isr(cyg_vector_t vector, cyg_addrword_t data);
static void _kinetis_enet_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

typedef struct board_kinetis_enet_stat_st {
   unsigned int isr_tx;
   unsigned int isr_rx;
   unsigned int isr_err;
   unsigned int dsr_tx;
   unsigned int dsr_rx;

   unsigned int tx_ok;
   unsigned int tx_ko;

   unsigned int rx_ok;
   unsigned int rx_ko;
}board_kinetis_enet_stat_t;

#define DEBUG_ETH

#ifdef DEBUG_ETH
static board_kinetis_enet_stat_t kinetis_enet_stat;

   #define  __init_eth_stat() memset((void*)&kinetis_enet_stat, 0, sizeof(board_kinetis_enet_stat_t))
   #define  __inc_isr_tx()    kinetis_enet_stat.isr_tx++
   #define  __inc_isr_rx()    kinetis_enet_stat.isr_rx++
   #define  __inc_isr_err()   kinetis_enet_stat.isr_err++
   #define  __inc_dsr_tx()    kinetis_enet_stat.dsr_tx++
   #define  __inc_dsr_rx()    kinetis_enet_stat.dsr_rx++
   #define  __inc_tx_ok()     kinetis_enet_stat.tx_ok++
   #define  __inc_tx_ko()     kinetis_enet_stat.tx_ko++
   #define  __inc_rx_ok()     kinetis_enet_stat.rx_ok++
   #define  __inc_rx_ko()     kinetis_enet_stat.rx_ko++

#else //DEBUG_ETH

   #define  __init_eth_stat()
   #define  __inc_isr_tx()
   #define  __inc_isr_rx()
   #define  __inc_isr_err()
   #define  __inc_dsr_tx()
   #define  __inc_dsr_rx()
   #define  __inc_tx_ok()
   #define  __inc_tx_ko()
   #define  __inc_rx_ok()
   #define  __inc_rx_ko()

#endif //DEBUG_ETH

/*===========================================
Implementation
=============================================*/
/*-------------------------------------------
| Name:_kinetis_enet_mii_init
| Description:
| Parameters:
| Return Type:
| Comments: Initialize the MII interface controller
| See:
---------------------------------------------*/
void _kinetis_enet_mii_init(board_kinetis_enet_info_t *penet, unsigned int sys_clk_mhz) {
   unsigned mii_val = 0 | REG_ENET_MSCR_MII_SPEED((2*sys_clk_mhz/5)+1);
   HAL_WRITE_UINT32(penet->enet_base + REG_ENET_MSCR, mii_val);
}

/*-------------------------------------------
| Name:_kinetis_enet_mii_write
| Description:
| Parameters:
| Return Type:
| Comments: Write a value to a PHY's MII register
| See:
---------------------------------------------*/
int _kinetis_enet_mii_write(board_kinetis_enet_info_t *penet, unsigned int phy_addr,
                            unsigned int reg_addr,
                            unsigned int data) {
   unsigned int timeout, mii_val;

   //Clear the MII interrupt bit
   mii_val = REG_ENET_EIR_MII;
   HAL_WRITE_UINT32(penet->enet_base + REG_ENET_EIR, mii_val);

   //Initiatate the MII Management write
   mii_val = 0
             | REG_ENET_MMFR_ST(0x01)
             | REG_ENET_MMFR_OP(0x01)
             | REG_ENET_MMFR_PA(phy_addr)
             | REG_ENET_MMFR_RA(reg_addr)
             | REG_ENET_MMFR_TA(0x02)
             | REG_ENET_MMFR_DATA(data);
   HAL_WRITE_UINT32(penet->enet_base + REG_ENET_MMFR,      mii_val);

   // Poll for the MII interrupt
   for(timeout = 0; timeout < PHY_MII_TIMEOUT; timeout++) {
      HAL_READ_UINT32(penet->enet_base + REG_ENET_EIR, mii_val);
      if(mii_val & REG_ENET_EIR_MII)
         break;
   }

   if(timeout == PHY_MII_TIMEOUT)
      return 1;

   // Clear the MII interrupt bit
   mii_val = REG_ENET_EIR_MII;
   HAL_WRITE_UINT32(penet->enet_base + REG_ENET_EIR, mii_val);

   return 0;
}

/*-------------------------------------------
| Name:_kinetis_enet_mii_read
| Description:
| Parameters:
| Return Type:
| Comments: Read a value from a PHY's MII register
| See:
---------------------------------------------*/
int _kinetis_enet_mii_read(board_kinetis_enet_info_t *penet, unsigned int phy_addr,
                           unsigned int reg_addr,
                           unsigned int *data) {
   unsigned int timeout, mii_val;

   //
   mii_val = REG_ENET_EIR_MII;
   HAL_WRITE_UINT32(penet->enet_base + REG_ENET_EIR, mii_val);

   // Initiatate the MII Management read
   mii_val = 0
             | REG_ENET_MMFR_ST(0x01)
             | REG_ENET_MMFR_OP(0x2)
             | REG_ENET_MMFR_PA(phy_addr)
             | REG_ENET_MMFR_RA(reg_addr)
             | REG_ENET_MMFR_TA(0x02);
   HAL_WRITE_UINT32(penet->enet_base + REG_ENET_MMFR,      mii_val);

   // Poll for the MII interrupt (interrupt should be masked)
   for(timeout = 0; timeout < PHY_MII_TIMEOUT; timeout++) {
      HAL_READ_UINT32(penet->enet_base + REG_ENET_EIR, mii_val);
      if(mii_val & REG_ENET_EIR_MII)
         break;
   }

   if(timeout == PHY_MII_TIMEOUT)
      return 1;

   // Clear the MII interrupt bit
   mii_val = REG_ENET_EIR_MII;
   HAL_WRITE_UINT32(penet->enet_base + REG_ENET_EIR, mii_val);

   HAL_READ_UINT32(penet->enet_base + REG_ENET_MMFR, mii_val);
   *data = mii_val & 0x0000FFFF;

   return 0;
}

/*-------------------------------------------
| Name:_kinetis_enet_hash_addr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
unsigned char _kinetis_enet_hash_addr(const unsigned char* addr) {
   unsigned int crc;
   unsigned char byte;
   int i, j;

   crc = 0xFFFFFFFF;
   for(i=0; i<6; ++i) {
      byte = addr[i];
      for(j=0; j<8; ++j) {
         if((byte & 0x01)^(crc & 0x01)) {
            crc >>= 1;
            crc = crc ^ 0xEDB88320;
         }
         else {
            crc >>= 1;
            byte >>= 1;
         }
      }
   }
   return (unsigned char)(crc >> 26);
}

/*-------------------------------------------
| Name:_kinetis_enet_set_mac_addr
| Description:
| Parameters:
| Return Type:
| Comments: Set board MAC address
| See:
---------------------------------------------*/
void _kinetis_enet_set_mac_addr(board_kinetis_enet_info_t *penet, const unsigned char *pa) {
   unsigned char crc;
   unsigned int mii_val;

   // Set the Physical Address
   mii_val = (unsigned int)((pa[0]<<24) | (pa[1]<<16) | (pa[2]<<8) | pa[3]);
   HAL_WRITE_UINT32(penet->enet_base + REG_ENET_PALR, mii_val);
   mii_val = (unsigned int)((pa[4]<<24) | (pa[5]<<16));
   HAL_WRITE_UINT32(penet->enet_base + REG_ENET_PAUR, mii_val);

   // Calculate and set the hash for given Physical Address in the  Individual Address Hash registers
   crc = _kinetis_enet_hash_addr(pa);
   if(crc >= 32) {
      HAL_READ_UINT32(penet->enet_base + REG_ENET_IAUR, mii_val);
      mii_val |= (unsigned int)(1 << (crc - 32));
      HAL_WRITE_UINT32(penet->enet_base + REG_ENET_IAUR, mii_val);
   }
   else {
      HAL_READ_UINT32(penet->enet_base + REG_ENET_IALR, mii_val);
      mii_val |= (unsigned int)(1 << crc);
      HAL_WRITE_UINT32(penet->enet_base + REG_ENET_IALR, mii_val);
   }
}

/*-------------------------------------------
| Name:_kinetis_enet_config_buffers
| Description:
| Parameters:
| Return Type:
| Comments: Configure transmit and receive buffer descriptors
| See:
---------------------------------------------*/
void _kinetis_enet_config_buffers(board_kinetis_enet_info_t *penet) {
   unsigned int i;
   unsigned char *p;

   // There is no Tx buffer as the Rx buffer is reused.
   penet->p_tx_desc->length = 0;
   penet->p_tx_desc->status = 0;

   // Align the Rx buffers and fill in the Rx descriptors.
   p = penet->rx_buf;
   for( i = 0; i < KINETIS_ENET_RX_BUFS; i++ ) {
      penet->p_rx_desc[i].status = REG_ENET_RX_BD_E;
      penet->p_rx_desc[i].length = 0;
      __REV((unsigned char *)penet->p_rx_desc[i].data, (unsigned int)p);

      p += KINETIS_ENET_RX_BUFFER_SIZE;
   }

   // Set the wrap bit in the last descriptors to form a ring.
   penet->p_rx_desc[KINETIS_ENET_RX_BUFS-1].status |= REG_ENET_RX_BD_W;
}

/*-------------------------------------------
| Name:dev_k60n512_enet_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_enet_load(void){
   unsigned int i=0;
   unsigned int mii_val=0;
   unsigned int reg_val=0;
   unsigned int *tmp;
   volatile unsigned int phy_link_retry = PHY_LINK_RETRY;

   //enable clock gating (SIM_SCGC2 |= SIM_SCGC2_ENET_MASK)
   HAL_READ_UINT32(REG_SIM_SCGC2_ADDR, reg_val);
   reg_val |= REG_SIM_SCGC2_ENET_MASK;
   HAL_WRITE_UINT32(REG_SIM_SCGC2_ADDR, reg_val);

   //FSL: allow concurrent access to MPU controller. Example: ENET uDMA to SRAM, otherwise bus error
   reg_val = 0;
   HAL_WRITE_UINT32(0x4000d000, reg_val);     //MPU_CESR = 0;

   //
   _kinetis_enet_config_buffers(&kinetis_enet_info);

   //Set the Reset bit and clear the Enable bit
   reg_val = REG_ENET_ECR_RESET;
   HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_ECR, reg_val);

   //Wait at least 8 clock cycles
   HAL_DELAY_US(1);

   //FSL: start MII interface
   _kinetis_enet_mii_init(&kinetis_enet_info, CYGHWR_HAL_CORTEXM_KINETIS_CLK_PER_BUS/1000000);

   // Can we talk to the PHY? read phy ID
   do {
      HAL_DELAY_US(PHY_LINK_DELAY);
      mii_val = 0xffff;
      _kinetis_enet_mii_read(&kinetis_enet_info, REG_PHY_ADDRESS, REG_PHY_PHYIDR1, &mii_val);
   } while(mii_val == 0xffff);

   // Start auto negotiate.
   _kinetis_enet_mii_write(&kinetis_enet_info, REG_PHY_ADDRESS, REG_PHY_BMCR,
                           (REG_PHY_BMCR_AN_RESTART | REG_PHY_BMCR_AN_ENABLE));

   // Wait for auto negotiate to complete.
   do {
#if 0
      if(!(phy_link_retry--)) {
         return -1;
      }
#endif

      HAL_DELAY_US(PHY_LINK_DELAY);
      _kinetis_enet_mii_read(&kinetis_enet_info, REG_PHY_ADDRESS, REG_PHY_BMSR, &mii_val);
   } while(!(mii_val & REG_PHY_BMSR_AN_COMPLETE));

   // When we get here we have a link - find out what has been negotiated
   mii_val = 0;
   _kinetis_enet_mii_read(&kinetis_enet_info, REG_PHY_ADDRESS, REG_PHY_PHYCTRL2, &mii_val);

   // Clear the Individual and Group Address Hash registers
   reg_val = 0;
   HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_IALR, reg_val);
   HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_IAUR, reg_val);
   HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_GALR, reg_val);
   HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_GAUR, reg_val);

   // Set the Physical Address for the selected ENET
   _kinetis_enet_set_mac_addr(&kinetis_enet_info, kinetis_enet_info.mac_addr);

   // Use only RMII now
   reg_val = REG_ENET_RCR_MAX_FL(KINETIS_ENET_RX_BUFFER_SIZE) | REG_ENET_RCR_MII_MODE |
             REG_ENET_RCR_CRCFWD | REG_ENET_RCR_RMII_MODE;
   HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_RCR, reg_val);

   // FSL: clear rx/tx control registers
   reg_val = 0;
   HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_TCR, reg_val);

   // Setup half or full duplex.
   if(mii_val & REG_PHY_PHYCTRL2_DUPLEX_STATUS) {
      // Full duplex
      HAL_READ_UINT32(kinetis_enet_info.enet_base + REG_ENET_RCR, reg_val);
      reg_val &= ~REG_ENET_RCR_DRT;
      HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_RCR, reg_val);

      HAL_READ_UINT32(kinetis_enet_info.enet_base + REG_ENET_TCR, reg_val);
      reg_val |= REG_ENET_TCR_FDEN;
      HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_TCR, reg_val);
   }
   else {
      // Half duplex
      HAL_READ_UINT32(kinetis_enet_info.enet_base + REG_ENET_RCR, reg_val);
      reg_val |= REG_ENET_RCR_DRT;
      HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_RCR, reg_val);

      HAL_READ_UINT32(kinetis_enet_info.enet_base + REG_ENET_TCR, reg_val);
      reg_val &= ~REG_ENET_TCR_FDEN;
      HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_TCR, reg_val);
   }

   // Setup speed
   if(mii_val & REG_PHY_PHYCTRL2_SPEED_STATUS)     {
      // 10Mbps
      HAL_READ_UINT32(kinetis_enet_info.enet_base + REG_ENET_RCR, reg_val);
      reg_val |= REG_ENET_RCR_RMII_10T;
      HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_RCR, reg_val);
      //
      kinetis_enet_info.eth_stat = ETH_STAT_LINK_10;
   }
   else {
      kinetis_enet_info.eth_stat = ETH_STAT_LINK_100;
   }

   //set normal buffer descriptor
   reg_val = 0;
   HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_ECR, reg_val);

   //Set Rx Buffer Size
   reg_val = KINETIS_ENET_RX_BUFFER_SIZE;
   HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_MRBR, reg_val);


   //Point to the start of the circular Rx buffer descriptor queue
   reg_val = (unsigned int)kinetis_enet_info.p_rx_desc;
   HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_RDSR, reg_val);

   //Point to the start of the circular Tx buffer descriptor queue
   reg_val = (unsigned int)kinetis_enet_info.p_tx_desc;
   HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_TDSR, reg_val);

   ///
   __init_eth_stat();
   ///
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_enet_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_enet_open(desc_t desc, int o_flag) {
   if(o_flag & O_RDONLY) {
      if(kinetis_enet_info._desc_rd<0) {
         kinetis_enet_info._desc_rd = desc;
      }
      else
         return -1;                //already open
   }

   if(o_flag & O_WRONLY) {
      if(kinetis_enet_info._desc_wr<0) {
         kinetis_enet_info._desc_wr = desc;
      }
      else
         return -1;                //already open
   }

   if(!ofile_lst[desc].p)
      ofile_lst[desc].p=&kinetis_enet_info;

   //unmask IRQ and enable ksz
   if(kinetis_enet_info._desc_rd>=0 && kinetis_enet_info._desc_wr>=0) {
      unsigned int reg_val = 0;
      cyg_interrupt_create(kinetis_enet_info.interrupts[0],
                           kinetis_enet_info.priorities[0],
                           (cyg_addrword_t)&kinetis_enet_info,
                           _kinetis_enet_isr,
                           _kinetis_enet_dsr,
                           &kinetis_enet_info.interrupt_handles[0],
                           &kinetis_enet_info.interrupt_objects[0]);
      cyg_interrupt_attach(kinetis_enet_info.interrupt_handles[0]);


      cyg_interrupt_create(kinetis_enet_info.interrupts[1],
                           kinetis_enet_info.priorities[1],
                           (cyg_addrword_t)&kinetis_enet_info,
                           _kinetis_enet_isr,
                           _kinetis_enet_dsr,
                           &kinetis_enet_info.interrupt_handles[1],
                           &kinetis_enet_info.interrupt_objects[1]);
      cyg_interrupt_attach(kinetis_enet_info.interrupt_handles[1]);

      cyg_interrupt_create(kinetis_enet_info.interrupts[2],
                           kinetis_enet_info.priorities[2],
                           (cyg_addrword_t)&kinetis_enet_info,
                           _kinetis_enet_isr,
                           _kinetis_enet_dsr,
                           &kinetis_enet_info.interrupt_handles[2],
                           &kinetis_enet_info.interrupt_objects[2]);
      cyg_interrupt_attach(kinetis_enet_info.interrupt_handles[2]);

      // Clear all ENET interrupt events
      reg_val = 0xffffffff;
      HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_EIR, reg_val);

      //enable IRQ
      reg_val = REG_ENET_EIMR_TXF | REG_ENET_EIMR_RXF | REG_ENET_EIMR_RXB |
                REG_ENET_EIMR_UN | REG_ENET_EIMR_RL | REG_ENET_EIMR_LC |
                REG_ENET_EIMR_BABT | REG_ENET_EIMR_BABR | REG_ENET_EIR_EBERR;
      HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_EIMR, reg_val);

      //enable MAC and empty receive buffer
      HAL_READ_UINT32(kinetis_enet_info.enet_base + REG_ENET_ECR, reg_val);
      reg_val |= REG_ENET_ECR_ETHEREN;
      HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_ECR, reg_val);

      // Indicate that there have been empty receive buffers produced
      reg_val = REG_ENET_RDAR_RDAR;
      HAL_WRITE_UINT32(kinetis_enet_info.enet_base + REG_ENET_RDAR, reg_val);

      //unmask IRQ
      cyg_interrupt_unmask(kinetis_enet_info.interrupts[0]);
      cyg_interrupt_unmask(kinetis_enet_info.interrupts[1]);
      cyg_interrupt_unmask(kinetis_enet_info.interrupts[2]);
   }
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_enet_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_enet_close(desc_t desc){
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_enet_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_enet_isset_read(desc_t desc) {
   board_kinetis_enet_info_t * p_net_info = (board_kinetis_enet_info_t*)ofile_lst[desc].p;

   if(!p_net_info)
      return -1;

   cyg_interrupt_mask(KINETIS_ENET_RX_IRQ);
   if((p_net_info->p_rx_desc[p_net_info->_input_r].status & REG_ENET_RX_BD_E) == 0) {
      cyg_interrupt_unmask(KINETIS_ENET_RX_IRQ);
      __inc_rx_ok();
      return 0;
   }

   cyg_interrupt_unmask(KINETIS_ENET_RX_IRQ);
   __inc_rx_ko();
   return -1;
}

/*-------------------------------------------
| Name:dev_k60n512_enet_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_enet_isset_write(desc_t desc) {
   board_kinetis_enet_info_t * p_net_info = (board_kinetis_enet_info_t*)ofile_lst[desc].p;

   if(!p_net_info)
      return -1;

   cyg_interrupt_mask(KINETIS_ENET_TX_IRQ);
   if(p_net_info->_output_r==p_net_info->_output_w) {
      p_net_info->_output_r = -1;
      cyg_interrupt_unmask(KINETIS_ENET_TX_IRQ);
      __inc_tx_ok();
      return 0;
   }

   cyg_interrupt_unmask(KINETIS_ENET_TX_IRQ);
   __inc_tx_ko();
   return -1;
}

/*-------------------------------------------
| Name:dev_k60n512_enet_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_enet_read(desc_t desc, char* buf,int size){
   board_kinetis_enet_info_t * p_net_info = (board_kinetis_enet_info_t*)ofile_lst[desc].p;
   int cb = 0;
   int status = 0;
   unsigned char *rcv_buf_addr = NULL;

   // is a new buffer received
   if((p_net_info->p_rx_desc[p_net_info->_input_r].status & REG_ENET_RX_BD_E) == 0) {
      //get status, packet len and copy data to buffer
      __REVSH(status, p_net_info->p_rx_desc[p_net_info->_input_r].status);
      __REVSH(cb, p_net_info->p_rx_desc[p_net_info->_input_r].length);
      __REV((unsigned char *)rcv_buf_addr,
            (unsigned int)p_net_info->p_rx_desc[p_net_info->_input_r].data);

      //copy data
      memcpy((void *)buf, (void *)rcv_buf_addr, cb);
      //

      cyg_interrupt_mask(KINETIS_ENET_RX_IRQ);
      /* Doing this here could cause corruption! */
      p_net_info->p_rx_desc[p_net_info->_input_r].status |= REG_ENET_RX_BD_E;

      //inc receive buffer
      p_net_info->_input_r = (p_net_info->_input_r + 1) & (KINETIS_ENET_RX_BUFS - 1);

      cyg_interrupt_unmask(KINETIS_ENET_RX_IRQ);

      //buffer is ready to receive again
      HAL_WRITE_UINT32(p_net_info->enet_base + REG_ENET_RDAR, REG_ENET_RDAR_RDAR);
   }

   return cb;
}

/*-------------------------------------------
| Name:dev_k60n512_enet_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_enet_write(desc_t desc, const char* buf,int size){
   board_kinetis_enet_info_t * p_net_info = (board_kinetis_enet_info_t*)ofile_lst[desc].p;

   // When we get here the Tx descriptor should show as having completed.
   while(p_net_info->p_tx_desc->status & REG_ENET_TX_BD_R) {
      HAL_DELAY_US(10);
   }

   //disable irq
   cyg_interrupt_mask(KINETIS_ENET_TX_IRQ);

   // To maintain the zero copy implementation, point the Tx descriptor
   //to the data from the Rx buffer.
   __REV((unsigned char *)p_net_info->p_tx_desc->data, (uint32_t)buf);
   // Setup the buffer descriptor for transmission
   __REVSH(p_net_info->p_tx_desc->length, size);

   // NB this assumes only one Tx descriptor!
   p_net_info->p_tx_desc->status = (REG_ENET_TX_BD_R | REG_ENET_TX_BD_W |
                                    REG_ENET_TX_BD_L |      REG_ENET_TX_BD_TC);

   p_net_info->_output_w++;
   p_net_info->_output_r = 0;
   //enable irq
   cyg_interrupt_unmask(KINETIS_ENET_TX_IRQ);

   // Continue the Tx DMA task (in case it was waiting for a new TxBD)
   HAL_WRITE_UINT32(p_net_info->enet_base + REG_ENET_TDAR, REG_ENET_TDAR_TDAR);
   return size;
}

/*-------------------------------------------
| Name:dev_k60n512_enet_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_enet_seek(desc_t desc,int offset,int origin){
   return 0;
}

/*-------------------------------------------
| Name:dev_k60n512_enet_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_k60n512_enet_ioctl(desc_t desc,int request,va_list ap) {
   switch(request) {

   //reset interface
   case ETHRESET: {
      //!TODO
   }
   break;

   //status interface
   case ETHSTAT: {
      eth_stat_t* p_eth_stat = va_arg( ap, eth_stat_t*);
      board_kinetis_enet_info_t * p_net_info = (board_kinetis_enet_info_t*)ofile_lst[desc].p;
      unsigned short reg_val=0;

      if(!p_net_info || !p_eth_stat)
         return -1;

      *p_eth_stat = p_net_info->eth_stat;
   }
   break;

   case ETHSETHWADDRESS: {
      //!TODO
   }
   break;

   case ETHGETHWADDRESS: {
      board_kinetis_enet_info_t * p_net_info = (board_kinetis_enet_info_t*)ofile_lst[desc].p;
      unsigned char* p_eth_hwaddr = va_arg( ap, unsigned char*);
      if(!p_net_info)
         return -1;

      p_eth_hwaddr[0] = p_net_info->mac_addr[0];
      p_eth_hwaddr[1] = p_net_info->mac_addr[1];
      p_eth_hwaddr[2] = p_net_info->mac_addr[2];
      p_eth_hwaddr[3] = p_net_info->mac_addr[3];
      p_eth_hwaddr[4] = p_net_info->mac_addr[4];
      p_eth_hwaddr[5] = p_net_info->mac_addr[5];
   }
   break;

   //
   default:
      return -1;
   }
   return 0;
}

/*-------------------------------------------
| Name:_kinetis_uart_isr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
cyg_uint32 _kinetis_enet_isr(cyg_vector_t vector, cyg_addrword_t data) {
   board_kinetis_enet_info_t * p_net_info = (board_kinetis_enet_info_t*)data;
   volatile unsigned int eimr = 0;
   volatile unsigned int reg_val = 0;

   cyg_interrupt_mask(vector);

   HAL_READ_UINT32(p_net_info->enet_base + REG_ENET_EIMR, eimr);
   HAL_READ_UINT32(p_net_info->enet_base + REG_ENET_EIR, reg_val);
   eimr &= reg_val;

   //write
   if(eimr & REG_ENET_EIR_TXF) {
      reg_val = REG_ENET_EIR_TXF;
      HAL_WRITE_UINT32(p_net_info->enet_base + REG_ENET_EIR, reg_val);
      //
      p_net_info->p_tx_desc->data = NULL;
      p_net_info->_output_r = p_net_info->_output_w;

      //
      __inc_isr_tx();

   }
   //read
   if(eimr & REG_ENET_EIR_RXF) {
      reg_val = REG_ENET_EIR_RXF;
      HAL_WRITE_UINT32(p_net_info->enet_base + REG_ENET_EIR, reg_val);
      p_net_info->_flag_r_irq = 1;

      //
      __inc_isr_rx();
   }

   //error
   if (eimr & (REG_ENET_EIR_UN | REG_ENET_EIR_RL | REG_ENET_EIR_LC |
               REG_ENET_EIR_EBERR | REG_ENET_EIR_BABT | REG_ENET_EIR_BABR)) {
      reg_val = REG_ENET_RDAR_RDAR;
      HAL_WRITE_UINT32(p_net_info->enet_base + REG_ENET_RDAR, reg_val);

      reg_val = REG_ENET_EIR_UN | REG_ENET_EIR_RL | REG_ENET_EIR_LC |
                REG_ENET_EIR_EBERR | REG_ENET_EIR_BABT | REG_ENET_EIR_BABR;

      HAL_WRITE_UINT32(p_net_info->enet_base + REG_ENET_EIR, reg_val);

      //
      __inc_isr_err();
   }


   cyg_interrupt_acknowledge(vector);

   return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
}

/*-------------------------------------------
| Name:_kinetis_uart_dsr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void _kinetis_enet_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
   board_kinetis_enet_info_t * p_net_info = (board_kinetis_enet_info_t*)data;

   //write
   if(p_net_info->_output_r == p_net_info->_output_w) {
      __inc_dsr_tx();
      //
      __fire_io_int(ofile_lst[p_net_info->_desc_wr].owner_pthread_ptr_write);
   }
   //read
   if(p_net_info->_flag_r_irq) {     ////if(p_net_info->_input_r != p_net_info->_input_w) {
      p_net_info->_flag_r_irq = 0;
      __inc_dsr_rx();
      //
      __fire_io_int(ofile_lst[p_net_info->_desc_rd].owner_pthread_ptr_read);
   }

   cyg_interrupt_unmask(vector);
}

/*============================================
| End of Source  : dev_k60n512_enet.c
==============================================*/
