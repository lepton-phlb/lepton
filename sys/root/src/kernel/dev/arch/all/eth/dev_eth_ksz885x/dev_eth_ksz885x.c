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
| Includes
==============================================*/
#include "dev_eth_ksz885x.h"

//tauon definitions
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/ioctl_eth.h"
#include "kernel/core/system.h"

#include "kernel/fs/vfs/vfsdev.h"

#include "kernel/dev/arch/all/eth/common/dev_eth_common.h"

#if defined(USE_ECOS)
   #include "cyg/hal/at91sam9261.h"
   #include <string.h>
   #include <stdlib.h>
#endif
/*============================================
| Global Declaration
==============================================*/
//
typedef struct ksz885x_stat_irq_st {
   unsigned long long tx;
   unsigned long long tx_space;
   unsigned long long tx_stopped;
   unsigned long long tx_errors;
   unsigned long long rx;
   unsigned long long rx_ov;
   unsigned long long rx_stopped;
   unsigned long long rx_err_frame;
   unsigned long long rx_err_size;
   unsigned long long phy;
   unsigned long long wol_frame;
   unsigned long long wol_magic;
   unsigned long long wol_link;
   unsigned long long wol_energy;
   unsigned long long dsr;
   unsigned long long dsr_tx;
   unsigned long long dsr_rx;
   unsigned long long isr;
} ksz885x_stat_irq_t;

//
typedef struct board_ksz885x_net_info_st {
   kernel_pthread_mutex_t     mutex;

   desc_t _eth_ksz885x_desc_rd;
   desc_t _eth_ksz885x_desc_wr;

   unsigned int _input_r;
   unsigned int _input_w;
   unsigned int _output_r;
   unsigned int _output_w;

   //io info
   dev_io_info_t dev_info;

   unsigned long long rx_pkt_cnt;
   unsigned char xmit_flag;

   //for eCos IRQ
   cyg_handle_t  _eth_ksz885x_handle;
   cyg_interrupt _eth_ksz885x_it;

   //for lepton event
   unsigned char _flag_r_irq;
   unsigned char _flag_w_irq;

   //driver info
   ksz885x_hw_t ksz885x_info;

   //stat info
   ksz885x_stat_irq_t * ksz885x_stats;

   //ethernet status
   eth_stat_t  eth_stat;
} board_ksz885x_net_info_t;

//
#define  RCV_BUFFER_SIZE  1500
#define  RCV_POOL_SIZE  16//32//16
typedef struct rcv_pool_st {
   unsigned char buffer[RCV_BUFFER_SIZE];
   unsigned short len;
   unsigned short rx_len;
} rcv_pool_t;

//
const char dev_eth_ksz885x_name[]="eth0\0";
static rcv_pool_t  rx_pool[RCV_POOL_SIZE] __attribute((section(".no_cache")));
static board_ksz885x_net_info_t g_net_ksz885x_inf __attribute((section(".no_cache")));
static unsigned char dev_eth_ksz885x_mac_addr[] = {0x00, 0x10, 0xA1, 0x86, 0x95, 0x11};

//
#if defined(USE_ECOS)
static cyg_uint32 dev_eth_ksz885x_isr(cyg_vector_t vector, cyg_addrword_t data);
static void dev_eth_ksz885x_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
static int dev_eth_ksz885x_isr_send(board_ksz885x_net_info_t * p_net_info);
static int dev_eth_ksz885x_isr_receive(board_ksz885x_net_info_t * p_net_info);
#endif

int dev_eth_ksz885x_load(dev_io_info_t* p_dev_io_info);
int dev_eth_ksz885x_open(desc_t desc, int o_flag);
int dev_eth_ksz885x_close(desc_t desc);
int dev_eth_ksz885x_isset_read(desc_t desc);
int dev_eth_ksz885x_isset_write(desc_t desc);
int dev_eth_ksz885x_read(desc_t desc, char* buf,int size);
int dev_eth_ksz885x_write(desc_t desc, const char* buf,int size);
int dev_eth_ksz885x_seek(desc_t desc,int offset,int origin);
int dev_eth_ksz885x_ioctl(desc_t desc,int request,va_list ap);

#define STAT_KSZ885X

#ifdef STAT_KSZ885X
#define  __inc_tx(__stat_irq__)              __stat_irq__->tx++
#define  __inc_tx_space(__stat_irq__)        __stat_irq__->tx_space++
#define  __inc_tx_stopped(__stat_irq__)      __stat_irq__->tx_stopped++
#define  __inc_tx_errors(__stat_irq__)       __stat_irq__->tx_errors++
#define  __inc_rx(__stat_irq__)              __stat_irq__->rx++
#define  __inc_rx_ov(__stat_irq__)           __stat_irq__->rx_ov++
#define  __inc_rx_stopped(__stat_irq__)      __stat_irq__->rx_stopped++
#define  __inc_rx_err_frame(__stat_irq__)    __stat_irq__->rx_err_frame++
#define  __inc_rx_err_size(__stat_irq__)     __stat_irq__->rx_err_size++
#define  __inc_phy(__stat_irq__)             __stat_irq__->phy++
#define  __inc_wol_frame(__stat_irq__)       __stat_irq__->wol_frame++
#define  __inc_wol_magic(__stat_irq__)       __stat_irq__->wol_magic++
#define  __inc_wol_link(__stat_irq__)        __stat_irq__->wol_link++
#define  __inc_wol_energy(__stat_irq__)      __stat_irq__->wol_energy++
#define  __inc_dsr(__stat_irq__)             __stat_irq__->dsr++
#define  __inc_dsr_tx(__stat_irq__)          __stat_irq__->dsr_tx++
#define  __inc_dsr_rx(__stat_irq__)          __stat_irq__->dsr_rx++
#define  __inc_isr(__stat_irq__)             __stat_irq__->isr++

#else
#define  __inc_tx(__stat_irq__)
#define  __inc_tx_space(__stat_irq__)
#define  __inc_tx_stopped(__stat_irq__)
#define  __inc_tx_errors(__stat_irq__)
#define  __inc_rx(__stat_irq__)
#define  __inc_rx_ov(__stat_irq__)
#define  __inc_rx_stopped(__stat_irq__)
#define  __inc_rx_err_frame(__stat_irq__)
#define  __inc_rx_err_size(__stat_irq__)
#define  __inc_phy(__stat_irq__)
#define  __inc_wol_frame(__stat_irq__)
#define  __inc_wol_magic(__stat_irq__)
#define  __inc_wol_link(__stat_irq__)
#define  __inc_wol_energy(__stat_irq__)
#define  __inc_dsr(__stat_irq__)
#define  __inc_dsr_tx(__stat_irq__)
#define  __inc_dsr_rx(__stat_irq__)
#define  __inc_isr(__stat_irq__)

#endif //STAT_KSZ885X


#define  ALLOWED_IRQs   (INT_TX | INT_RX)
#define __ksz885x_enable_irq(__pksz885xHW__)     __ksz885x_write_reg(__pksz885xHW__, REG_INT_MASK, (__pksz885xHW__)->irq_mask)
#define __ksz885x_disable_irq(__pksz885xHW__)    __ksz885x_write_reg(__pksz885xHW__, REG_INT_MASK, 0x0000)
#define __ksz885x_ack_irq(__pksz885xHW__, __irq_mask__) __ksz885x_write_reg(__pksz885xHW__, REG_INT_STATUS, (unsigned short)__irq_mask__)

/*============================================
| Implementation
==============================================*/
/*--------------------------------------------
| Name:        _ksz885x_read_word
| Description: This routine read a 16 bit data from ks8851 MLL chip
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_read_word(void * phw, unsigned short addr, unsigned short * data) {
   ksz885x_hw_t * pksz885xHW=(ksz885x_hw_t *)phw;
   __ksz885x_mio_word((pksz885xHW)->io_virt_addr + (pksz885xHW)->io_virt_data) =
         (unsigned short)(addr) | ((BE1 | BE0) << (addr & 0x02));  
   *(data) = __ksz885x_mio_word((pksz885xHW)->io_virt_addr + CMD_LOW);
}


/*--------------------------------------------
| Name:        _ksz885x_write_word
| Description: This routine write a 16 bit data to ks8851 MLL chip
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_write_word(void * phw, unsigned short addr,  unsigned short data) {
   ksz885x_hw_t * pksz885xHW=(ksz885x_hw_t *)phw;
   __ksz885x_mio_word((pksz885xHW)->io_virt_addr + (pksz885xHW)->io_virt_data ) =
         (unsigned short)(addr) | ((BE1 | BE0) << (addr & 0x02));  
   __ksz885x_mio_word((pksz885xHW)->io_virt_addr + CMD_LOW) = (unsigned short)(data);
}


/*--------------------------------------------
| Name:        _ksz885x_read_data16
| Description: This routine read data from RX queue
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_read_data16(void * phw, unsigned short * data) {
   ksz885x_hw_t * pksz885xHW=(ksz885x_hw_t *)phw;
   *(data) = __ksz885x_mio_word(((p_ksz885x_hw_t)(pksz885xHW))->io_virt_addr + CMD_LOW);
}


/*--------------------------------------------
| Name:        _ksz885x_write_data16
| Description: This routine write data to TX queue
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_write_data16(void * phw, unsigned short data) {
	ksz885x_hw_t * pksz885xHW=(ksz885x_hw_t *)phw;
	__ksz885x_mio_word(((p_ksz885x_hw_t)(pksz885xHW))->io_virt_addr + CMD_LOW) = (unsigned short)(data);
}


/*--------------------------------------------
| Name:        _ksz885x_write_data_header
| Description: KS8851 chip data QMU header, and do a 32 data aligment.
| Parameters:  unsigned short uOrgLength  IN
|                 original data length.
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_write_data_header(void *pksz885xHW,unsigned short uOrgLength) {
   _ksz885x_write_data16(pksz885xHW, TX_CTRL_INTERRUPT_ON);
   //the data length in the head should be original data length
   _ksz885x_write_data16(pksz885xHW, uOrgLength);
}

#define __ksz885x_write_data(pHw, pIn, p, len, sentLen) { \
   unsigned short * pw; \
   pw=(unsigned short *)pIn; \
   while(len--) { \
      sentLen+=DATA_ALIGNMENT; \
      _ksz885x_write_data16(pHw,*pw++); \
   }\
   p=(unsigned char *)pw;\
}

/*--------------------------------------------
| Name:        _ksz885x_write_data_buffer
| Description: This function write the data to the hardware.
| Parameters:  unsigned short uOrgLength  IN
|                 original data length.
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_write_data_buffer(void * pksz885xHW, unsigned char *  pData, unsigned short uOrgPktLen) {
   unsigned short tx_send = ((uOrgPktLen+3) & ~0x03)>>1;
   unsigned short tx_send_count=0;
   unsigned char * data_ptr=0;
   if(tx_send) {
      __ksz885x_write_data(pksz885xHW, pData, data_ptr, tx_send, tx_send_count);
   }
}


/*--------------------------------------------
| Name:        _ksz885x_restart_auto_nego
| Description: This routine restarts PHY auto-negotiation.
| Parameters:
| Return Type: autoNegoStatus : 1-> auto-nego completed; 0-> auto-nego not completed.
| Comments:
| See:
----------------------------------------------*/
int _ksz885x_restart_auto_nego(p_ksz885x_hw_t pksz885xHW) {
   unsigned short  RegData;
   int    cnTimeOut;

   /* Set restart auto-negitiation to Port Control Register (P1CR) */
   __ksz885x_read_reg(pksz885xHW, REG_PORT_CTRL, &RegData );
   RegData &= ~(PORT_FORCE_100_MBIT | PORT_FORCE_FULL_DUPLEX);
   RegData |= (PORT_AUTO_NEG_ENABLE | PORT_AUTO_NEG_RESTART);
   __ksz885x_write_reg(pksz885xHW, REG_PORT_CTRL, RegData );

   /* Wait for auto-negotiation completed (P1SR) */
   cnTimeOut = PHY_RESET_TIMEOUT;
   do {
      __udelay(100);
      __ksz885x_read_reg(pksz885xHW, REG_PORT_STATUS, &RegData );
   } while (--cnTimeOut && !(RegData & PORT_AUTO_NEG_COMPLETE));

   return ((RegData & PORT_AUTO_NEG_COMPLETE) >> 6);
}


/*--------------------------------------------
| Name:        _ksz885x_enable_hw
| Description: This routine enables the hardware.
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_enable_hw(p_ksz885x_hw_t pksz885xHW) {
   unsigned short   txCntl;
   unsigned short   rxCntl;

   /* Enables QMU Transmit (TXCR). */
   __ksz885x_read_reg(pksz885xHW,REG_TX_CTRL,&txCntl);
   txCntl |= TX_CTRL_ENABLE ;
   __ksz885x_write_reg(pksz885xHW, REG_TX_CTRL, txCntl);

   /* Enables QMU Receive (RXCR1). */
   __ksz885x_read_reg(pksz885xHW,REG_RX_CTRL1,&rxCntl);
   rxCntl |= RX_CTRL_ENABLE ;
   __ksz885x_write_reg(pksz885xHW, REG_RX_CTRL1, rxCntl);
   pksz885xHW->enabled = TRUE;
}


/*--------------------------------------------
| Name:        _ksz885x_disable_hw
| Description: This routine disables the hardware.
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_disable_hw(p_ksz885x_hw_t pksz885xHW ) {
   unsigned short   txCntl;
   unsigned short   rxCntl;

   /* Disable QMU Transmit (TXCR). */
   __ksz885x_read_reg(pksz885xHW,REG_TX_CTRL,&txCntl);
   txCntl &= ~TX_CTRL_ENABLE ;
   __ksz885x_write_reg(pksz885xHW, REG_TX_CTRL, txCntl );

   /* Disable QMU Receive (RXCR1). */
   __ksz885x_read_reg(pksz885xHW,REG_RX_CTRL1,&rxCntl);
   rxCntl &= ~RX_CTRL_ENABLE ;
   __ksz885x_write_reg(pksz885xHW, REG_RX_CTRL1, rxCntl );

   pksz885xHW->enabled = FALSE;
}


/*--------------------------------------------
| Name:        _ksz885x_setup_hw
| Description: This routine setup KS8851 hardware.
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_setup_hw(p_ksz885x_hw_t pksz885xHW) {
   unsigned short txCntl, rxCntl,w;

   /* Setup Transmit Frame Data Pointer Auto-Increment (TXFDPR) */
   __ksz885x_write_reg(pksz885xHW, REG_TX_ADDR_PTR, ADDR_PTR_AUTO_INC );

   /* Setup Receive Frame Data Pointer Auto-Increment */
   __ksz885x_write_reg(pksz885xHW,REG_RX_ADDR_PTR, ADDR_PTR_AUTO_INC);

   /* Setup Receive Frame Threshold - 1 frame (RXFCTFC) */
   __ksz885x_write_reg(pksz885xHW, REG_RX_FRAME_CNT_THRES, 1 & RX_FRAME_THRESHOLD_MASK);

   /* Setup RxQ Command Control (RXQCR) */
   //__ksz885x_write_reg( pksz885xHW, REG_RXQ_CMD, RXQ_CMD_CNTL | RXQ_TWOBYTE_OFFSET);
   __ksz885x_write_reg(pksz885xHW, REG_RXQ_CMD, RXQ_CMD_CNTL );

   // set the force mode to half duplex, default is full duplex.
   //because if the auto-negotiation is fail, most switch will use half-duplex.
   __ksz885x_read_reg(pksz885xHW,REG_PHY_CNTL,&w);
   w&=~PHY_FULL_DUPLEX;
   __ksz885x_write_reg(pksz885xHW,REG_PHY_CNTL,w);

   txCntl = (
         TX_CTRL_FLOW_ENABLE |
         TX_CTRL_PAD_ENABLE |
         TX_CTRL_CRC_ENABLE |
         TX_CTRL_IP_CHECKSUM
   );

   __ksz885x_write_reg(pksz885xHW, REG_TX_CTRL, txCntl );

   //use with promiscious
   rxCntl = (
         RX_CTRL_IPV6_UDP_NOCHECKSUM |
         RX_CTRL_UDP_LITE_CHECKSUM |
         RX_CTRL_ICMP_CHECKSUM
   );
   __ksz885x_write_reg(pksz885xHW, REG_RX_CTRL2, rxCntl);
   rxCntl = (
         RX_CTRL_UDP_CHECKSUM |
         RX_CTRL_TCP_CHECKSUM |
         RX_CTRL_IP_CHECKSUM|
         RX_CTRL_FLOW_ENABLE |
         RX_CTRL_BROADCAST |
         RX_CTRL_UNICAST |
         RX_CTRL_MAC_FILTER |
         RX_CTRL_PROMISCUOUS
   );
   __ksz885x_write_reg(pksz885xHW, REG_RX_CTRL1, rxCntl);
}


/*--------------------------------------------
| Name:        _ksz885x_check_link
| Description: This routine reads PHY registers to determine the current link status.
|              If link is up, it also return link speed in 'ulSpeed'
|              and duplex mode in 'bDuplex'.
|
| Parameters:  unsigned long    *pSpeed
|                 current link speed (100000: 10BASE-T; 1000000: 100BASE-T)
|              unsigned long   *pDuplex;
|                 current link duplex mode (1: half duplex; 2: full duplex)
|
| Return Type:
| Comments:
| See:
----------------------------------------------*/
int _ksz885x_check_link (p_ksz885x_hw_t pksz885xHW, unsigned long  *pSpeed, unsigned long *pDuplex) {
   unsigned short  RegData=0;

   /* Read Port Status Register (P1SR) */
   __ksz885x_read_reg(pksz885xHW, REG_PORT_STATUS, &RegData );

   if((RegData & PORT_STATUS_LINK_GOOD) == PORT_STATUS_LINK_GOOD) {

      if((RegData & PORT_STAT_SPEED_100MBIT) == PORT_STAT_SPEED_100MBIT) {
         *pSpeed = 1000000;
         pksz885xHW->speed = ETH_STAT_LINK_100;
      }
      else {
         *pSpeed = 100000;
         pksz885xHW->speed = ETH_STAT_LINK_10;
      }

      if((RegData & PORT_STAT_FULL_DUPLEX) == PORT_STAT_FULL_DUPLEX) {
         *pDuplex = 2;
         pksz885xHW->duplex = 2;
      }
      else {
         *pDuplex = 1;
         pksz885xHW->duplex = 1;
      }

      return 1;  	/* Link is up */
   }
   else {
      pksz885xHW->speed = ETH_STAT_LINK_DOWN;
      pksz885xHW->duplex = 0;
      return 0;  	/* Link is down */
   }
} 


/*--------------------------------------------
| Name:        _ksz885x_setup_irq
| Description: This routine setup the interrupt mask for proper operation.
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_setup_irq(p_ksz885x_hw_t pksz885xHW) {
   unsigned short intMask;
   // in the OALIntrInit function, all interrupt mode has been
   // set to normal IRQ, so we do not need to do it again.
   pksz885xHW->irq_mask=0x00;
   /* Clear the interrupts status of the hardware. */
   __ksz885x_ack_irq(pksz885xHW,0xFFFF);

   /* Enables the interrupts of the hardware. */
   intMask = (ALLOWED_IRQs);

   pksz885xHW->irq_mask=intMask;

   __ksz885x_write_reg(pksz885xHW,REG_INT_MASK, intMask);
}


/*--------------------------------------------
| Name:        _ksz885x_get_mac_addr
| Description: This function retrieves the MAC address of the hardware.
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_get_mac_addr(p_ksz885x_hw_t pksz885xHW) {
   unsigned short u, w, *pDest;
   pDest=(unsigned short *)&pksz885xHW->mac_addr[0];

   __ksz885x_read_reg(pksz885xHW, REG_MAC_ADDR_4, &u);
   w=(unsigned short)(((u&0xFF)<<8) | ((u>>8)&0xFF));
   *pDest++=w;

   __ksz885x_read_reg(pksz885xHW, REG_MAC_ADDR_2, &u);
   w=(unsigned short)(((u&0xFF)<<8) | ((u>>8)&0xFF));
   *pDest++=w;

   __ksz885x_read_reg(pksz885xHW, REG_MAC_ADDR_0, &u);
   w=(unsigned short)(((u&0xFF)<<8) | ((u>>8)&0xFF));
   *pDest++=w;
}


/*--------------------------------------------
| Name:        _ksz885x_set_mac_addr
| Description: Set KS8851 MAC address.
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_set_mac_addr(p_ksz885x_hw_t pksz885xHW,unsigned char * pByte) {
   unsigned short * pw;
   unsigned short i,w,u;
   pw=(unsigned short *)pByte;
   u=*pw;
   w=(unsigned short)(((u&0xFF)<<8) | ((u>>8)&0xFF));
   __ksz885x_write_reg(pksz885xHW, REG_MAC_ADDR_4  , w );
   
   pw++;
   u=*pw;
   w=(unsigned short)(((u&0xFF)<<8) | ((u>>8)&0xFF));
   __ksz885x_write_reg(pksz885xHW, REG_MAC_ADDR_2  , w);
   
   pw++;
   u=*pw;
   w=(unsigned short)(((u&0xFF)<<8) | ((u>>8)&0xFF));
   __ksz885x_write_reg(pksz885xHW, REG_MAC_ADDR_0, w);
   
   for(i=0;i<6;i++)
    pksz885xHW->mac_addr[i]=*(pByte+i);
}


/*--------------------------------------------
| Name:        _ksz885x_reset_hw
| Description: Reset KS8851 chip.
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_reset_hw(p_ksz885x_hw_t pksz885xHW) {
   if(!pksz885xHW->io_virt_addr) {
      return;
   }
   /* Must disable interrupt first */
   __ksz885x_write_reg(pksz885xHW, REG_INT_MASK, 0x0000);
   __ksz885x_write_reg(pksz885xHW,REG_RESET_CTRL, GLOBAL_SOFTWARE_RESET );

   /* Wait for device to reset */
   __udelay(100);

   /* Write 0 to clear device reset */
   __ksz885x_write_reg(pksz885xHW, REG_RESET_CTRL, 0 );
   __udelay(300);
}


/*--------------------------------------------
| Name:        _ksz885x_init_hw
| Description: KS8851 chip initialization.
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
ksz885x_err_t  _ksz885x_init_hw(p_ksz885x_hw_t pksz885xHW) {
   unsigned short uRegData;

   //check chip ID
   do {
      __ksz885x_read_reg(pksz885xHW, REG_CHIP_ID, &uRegData);
   } while((uRegData & 0xFFF0)!= 0x8870);

   if((uRegData & 0xFFF0)!= 0x8870)
      return KS885XERR_ERROR;

   _ksz885x_set_mac_addr(pksz885xHW, pksz885xHW->mac_addr);

   return KS885XERR_NOERROR;
}


/*--------------------------------------------
| Name:        _ksz885x_init_phy
| Description: KS8851 PHY initialization.
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
int _ksz885x_init_phy(p_ksz885x_hw_t pksz885xHW) {
   unsigned long    dwSpeed;
   unsigned long    bDuplex;
   int      linkStatus;
   int      timeOut=100;


   /* Get device link status */
   linkStatus = _ksz885x_check_link (pksz885xHW,&dwSpeed, &bDuplex);

   /* Restart auto-negotiation if link is down */
   if(!linkStatus) {
      // auto-negotiation not complete ?
      if(_ksz885x_restart_auto_nego(pksz885xHW) == 0) {
      }

      do {
         __udelay(10);
         linkStatus = _ksz885x_check_link(pksz885xHW, &dwSpeed, &bDuplex);
      } while(--timeOut && !linkStatus);

      if(!linkStatus)
         return -1;
   }

   return 0;
}


/*--------------------------------------------
| Name:        _ksz885x_clear_wol_event
| Description: Clear Wake-On-Lan Event.
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
int _ksz885x_clear_wol_event(p_ksz885x_hw_t pksz885xHW, unsigned short wolEvent) {
   unsigned short reg_val=0;

   /* Clear PME_Status to deassert PMEN pin */
   __ksz885x_read_reg(pksz885xHW, REG_POWER_CNTL, &reg_val);
   reg_val &= ~WAKEUP_EVENT_MASK;
   reg_val |= wolEvent;
   __ksz885x_write_reg(pksz885xHW, REG_POWER_CNTL, reg_val);
   return 0;
}

/*--------------------------------------------
| Name:        _ksz885x_clean_tx_queue
| Description: Clean TX queue.
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_clean_tx_queue(p_ksz885x_hw_t pksz885xHW) {
   unsigned short reg_val=0;

   /* Clean TX queue and restore it*/
   __ksz885x_read_reg(pksz885xHW,REG_TX_CTRL,&reg_val);
   reg_val |= TX_CTRL_FLUSH_QUEUE;
   __ksz885x_write_reg(pksz885xHW, REG_TX_CTRL, reg_val );

   __ksz885x_read_reg(pksz885xHW,REG_TX_CTRL,&reg_val);
   reg_val &= ~TX_CTRL_FLUSH_QUEUE;
   __ksz885x_write_reg(pksz885xHW, REG_TX_CTRL, reg_val );
}


/*--------------------------------------------
| Name:        _ksz885x_clean_rx_queue
| Description: Clean RX queue.
| Parameters:
| Return Type:
| Comments:
| See:
----------------------------------------------*/
void _ksz885x_clean_rx_queue(p_ksz885x_hw_t pksz885xHW) {
   unsigned short reg_val=0;

   /* Clean RX queue and restore it*/
   __ksz885x_read_reg(pksz885xHW,REG_RX_CTRL1,&reg_val);
   reg_val |= RX_CTRL_FLUSH_QUEUE;
   __ksz885x_write_reg(pksz885xHW, REG_RX_CTRL1, reg_val );

   __ksz885x_read_reg(pksz885xHW,REG_RX_CTRL1,&reg_val);
   reg_val &= ~RX_CTRL_FLUSH_QUEUE;
   __ksz885x_write_reg(pksz885xHW, REG_RX_CTRL1, reg_val );
}


//tauon device driver functions
/*-------------------------------------------
| Name: dev_eth_ksz885x_load
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_ksz885x_load(dev_io_info_t* p_dev_io_info) {
   pthread_mutexattr_t  mutex_attr=0;
   unsigned short reg_val=0;

   //reset all values
   memset((void *)&g_net_ksz885x_inf, 0, sizeof(g_net_ksz885x_inf));
   memset((void *)&rx_pool, 0, sizeof(rx_pool));

   //get io info
   g_net_ksz885x_inf.dev_info.io_addr = p_dev_io_info->io_addr;
   g_net_ksz885x_inf.dev_info.io_data  = p_dev_io_info->io_data;
   g_net_ksz885x_inf.dev_info.irq_no   = p_dev_io_info->irq_no;
   g_net_ksz885x_inf.dev_info.irq_prio = p_dev_io_info->irq_prio;

   //init mutex
   kernel_pthread_mutex_init(&g_net_ksz885x_inf.mutex,&mutex_attr);

   //desc no
   g_net_ksz885x_inf._eth_ksz885x_desc_rd = -1;
   g_net_ksz885x_inf._eth_ksz885x_desc_wr = -1;

   //copy MAC address and io addr
   memcpy((void *)&g_net_ksz885x_inf.ksz885x_info.mac_addr, dev_eth_ksz885x_mac_addr, 6);
   g_net_ksz885x_inf.ksz885x_info.io_virt_addr = g_net_ksz885x_inf.dev_info.io_addr;
   g_net_ksz885x_inf.ksz885x_info.io_virt_data = g_net_ksz885x_inf.dev_info.io_data;

   //call inits functions
   _ksz885x_reset_hw(&g_net_ksz885x_inf.ksz885x_info);
   _ksz885x_init_hw(&g_net_ksz885x_inf.ksz885x_info);
   _ksz885x_disable_hw(&g_net_ksz885x_inf.ksz885x_info);
   _ksz885x_setup_hw(&g_net_ksz885x_inf.ksz885x_info);
   _ksz885x_init_phy(&g_net_ksz885x_inf.ksz885x_info);
   g_net_ksz885x_inf.eth_stat = g_net_ksz885x_inf.ksz885x_info.speed;
   _ksz885x_clear_wol_event(&g_net_ksz885x_inf.ksz885x_info, WAKEUP_EVENT_MASK);
   _ksz885x_setup_irq(&g_net_ksz885x_inf.ksz885x_info);

#if defined(USE_ECOS)
   cyg_interrupt_create((cyg_vector_t)g_net_ksz885x_inf.dev_info.irq_no, (cyg_priority_t)g_net_ksz885x_inf.dev_info.irq_prio,
            (cyg_addrword_t)&g_net_ksz885x_inf,
            &dev_eth_ksz885x_isr,
            &dev_eth_ksz885x_dsr,
            &g_net_ksz885x_inf._eth_ksz885x_handle, &g_net_ksz885x_inf._eth_ksz885x_it);
   //
   cyg_interrupt_configure(g_net_ksz885x_inf.dev_info.irq_no, 0/*edge*/, 0 /*falling*/ );
   cyg_interrupt_attach(g_net_ksz885x_inf._eth_ksz885x_handle);

#endif

#ifdef STAT_KSZ885X
   if(!(g_net_ksz885x_inf.ksz885x_stats = (ksz885x_stat_irq_t *)malloc(sizeof(ksz885x_stat_irq_t)))) {
      return -1;
   }
   memset((void*)g_net_ksz885x_inf.ksz885x_stats, 0, sizeof(ksz885x_stat_irq_t));
#endif

   return 0;
}

/*-------------------------------------------
| Name: dev_eth_ksz885x_open
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_ksz885x_open(desc_t desc, int o_flag) {
   //
   if(o_flag & O_RDONLY){
      if(g_net_ksz885x_inf._eth_ksz885x_desc_rd<0)
         g_net_ksz885x_inf._eth_ksz885x_desc_rd = desc;
      else
         return -1; //already open

      if(!ofile_lst[desc].p)
         ofile_lst[desc].p=&g_net_ksz885x_inf;
   }

   if(o_flag & O_WRONLY){
      if(g_net_ksz885x_inf._eth_ksz885x_desc_wr<0) {
         g_net_ksz885x_inf._eth_ksz885x_desc_wr = desc;
         g_net_ksz885x_inf._output_r = -1;
      }
      else
         return -1; //already open

      if(!ofile_lst[desc].p)
         ofile_lst[desc].p=&g_net_ksz885x_inf;
   }

   //unmask IRQ and enable ksz
   if(g_net_ksz885x_inf._eth_ksz885x_desc_rd>0 && g_net_ksz885x_inf._eth_ksz885x_desc_wr>0){
#if defined(USE_ECOS)
      cyg_interrupt_acknowledge((cyg_vector_t)g_net_ksz885x_inf.dev_info.irq_no);
      cyg_interrupt_unmask((cyg_vector_t)g_net_ksz885x_inf.dev_info.irq_no);
#endif
      _ksz885x_enable_hw(&g_net_ksz885x_inf.ksz885x_info);
   }
   return 0;
}

/*-------------------------------------------
| Name: dev_eth_ksz885x_close
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_ksz885x_close(desc_t desc) {
   if(ofile_lst[desc].oflag & O_RDONLY){
      if(!ofile_lst[desc].nb_reader){
         g_net_ksz885x_inf._eth_ksz885x_desc_rd = -1;
      }
   }

   if(ofile_lst[desc].oflag & O_WRONLY){
      if(!ofile_lst[desc].nb_writer){
         g_net_ksz885x_inf._eth_ksz885x_desc_wr = -1;
      }
   }

   if(!ofile_lst[desc].nb_writer
      &&!ofile_lst[desc].nb_reader) {
      ofile_lst[desc].p = NULL;
      cyg_interrupt_mask((cyg_vector_t)g_net_ksz885x_inf.dev_info.irq_no);
      _ksz885x_disable_hw(&g_net_ksz885x_inf.ksz885x_info);
   }
   return 0;
}

/*-------------------------------------------
| Name: dev_eth_ksz885x_isset_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_ksz885x_isset_read(desc_t desc) {
   board_ksz885x_net_info_t * p_net_info = (board_ksz885x_net_info_t *)ofile_lst[desc].p;
   if(!p_net_info)
      return -1;

   if(p_net_info->_input_r != p_net_info->_input_w)
      return 0;

   return -1;
}

/*-------------------------------------------
| Name: dev_eth_ksz885x_isset_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_ksz885x_isset_write(desc_t desc) {
   board_ksz885x_net_info_t * p_net_info = (board_ksz885x_net_info_t *)ofile_lst[desc].p;
   if(!p_net_info)
      return -1;

   if(p_net_info->_output_r==p_net_info->_output_w) {
      p_net_info->_output_r = -1;
      return 0;
   }

   return -1;
}

/*-------------------------------------------
| Name: dev_eth_ksz885x_read
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_ksz885x_read(desc_t desc, char* buf,int size) {
   board_ksz885x_net_info_t * p_net_info = (board_ksz885x_net_info_t *)ofile_lst[desc].p;
   unsigned cb=0;
   if(!p_net_info)
      return -1;

   //
   if(p_net_info->_input_r != p_net_info->_input_w) {
      cyg_interrupt_mask((cyg_vector_t)p_net_info->dev_info.irq_no);
      if(size > rx_pool[p_net_info->_input_r].len)
         cb = rx_pool[p_net_info->_input_r].len;

      //copy data
      memcpy((void*)buf, (void *)rx_pool[p_net_info->_input_r].buffer,cb);

      //
      if(++p_net_info->_input_r>=RCV_POOL_SIZE) {
         p_net_info->_input_r = 0;
      }

      cyg_interrupt_unmask((cyg_vector_t)p_net_info->dev_info.irq_no);
   }
   //
   return cb;
}

/*-------------------------------------------
| Name: dev_eth_ksz885x_write
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_ksz885x_write(desc_t desc, const char* buf,int size) {
   volatile unsigned short reg_val=0;
   board_ksz885x_net_info_t * p_net_info = (board_ksz885x_net_info_t *)ofile_lst[desc].p;
   unsigned int tx_length = ((size+3) & ~0x03)>>1;
   unsigned short tx_enq_status=0;

   if(!p_net_info || (p_net_info->eth_stat == ETH_STAT_LINK_DOWN))
      return -1;

   while(!p_net_info->xmit_flag) {
      __udelay(100000);
   }
   kernel_pthread_mutex_lock(&p_net_info->mutex);

   //check free space in TXQ
   __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_TX_MEM_INFO, (unsigned short *)&reg_val);
   if((reg_val & TX_MEM_AVAILABLE_MASK) < (size+4)) {
      //no available space in TXQ
      kernel_pthread_mutex_unlock(&p_net_info->mutex);
      return -1;
   }

   //disable all device interrupt
   cyg_interrupt_mask((cyg_vector_t)p_net_info->dev_info.irq_no);
   __ksz885x_disable_irq(&p_net_info->ksz885x_info);

   //start QMU DMA to enable TXQ transfer
   reg_val = 0;
   __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_RXQ_CMD, (unsigned short *)&reg_val);
   reg_val |= RXQ_START;
   __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_RXQ_CMD, (unsigned short)(reg_val&0xff));

   //write packet
   _ksz885x_write_data_header(&p_net_info->ksz885x_info, (unsigned short)size);
   _ksz885x_write_data_buffer(&p_net_info->ksz885x_info, (unsigned char *)buf, (unsigned short)size);

   //stop QMU DMA
   reg_val = 0;
   __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_RXQ_CMD, (unsigned short *)&reg_val);
   reg_val &= ~RXQ_START;
   __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_RXQ_CMD, (unsigned short)(reg_val&0xff));

   //manual TXQ
   reg_val = TXQ_ENQUEUE;
   __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_TXQ_CMD, (unsigned short)reg_val);

   //
   p_net_info->_output_w++;

   //enable device interrupt
   __ksz885x_enable_irq(&p_net_info->ksz885x_info);
   cyg_interrupt_unmask((cyg_vector_t)p_net_info->dev_info.irq_no);

   kernel_pthread_mutex_unlock(&p_net_info->mutex);

   return size;
}

/*-------------------------------------------
| Name: dev_eth_ksz885x_seek
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_ksz885x_seek(desc_t desc,int offset,int origin) {
   return -1;
}

/*-------------------------------------------
| Name: dev_eth_ksz885x_ioctl
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_ksz885x_ioctl(desc_t desc,int request,va_list ap) {
   switch(request){

     //reset interface
     case ETHRESET:{
        board_ksz885x_net_info_t * p_net_info = (board_ksz885x_net_info_t *)ofile_lst[desc].p;
        if(!p_net_info)
           return -1;

        cyg_interrupt_mask((cyg_vector_t)p_net_info->dev_info.irq_no);
        _ksz885x_disable_hw(&p_net_info->ksz885x_info);

        _ksz885x_clean_tx_queue(&p_net_info->ksz885x_info);
        _ksz885x_clean_rx_queue(&p_net_info->ksz885x_info);
        p_net_info->_output_r = -1;
        p_net_info->_input_r = p_net_info->_input_w;

        _ksz885x_enable_hw(&p_net_info->ksz885x_info);
        cyg_interrupt_unmask((cyg_vector_t)p_net_info->dev_info.irq_no);
     }
     break;

     //status interface
     case ETHSTAT:{
        eth_stat_t* p_eth_stat = va_arg( ap, eth_stat_t*);
        board_ksz885x_net_info_t * p_net_info = (board_ksz885x_net_info_t *)ofile_lst[desc].p;
        unsigned short reg_val=0;

        if(!p_net_info || !p_eth_stat)
           return -1;

        //read port status register
        __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_PORT_STATUS, &reg_val);
        if(reg_val & PORT_STATUS_LINK_GOOD) {
           if(reg_val & PORT_STAT_SPEED_100MBIT) {
              p_net_info->eth_stat = ETH_STAT_LINK_100;
           }
           else {
              p_net_info->eth_stat = ETH_STAT_LINK_10;
           }
        }
        else {
           p_net_info->eth_stat = ETH_STAT_LINK_DOWN;
        }
        *p_eth_stat = p_net_info->eth_stat;
     }
     break;

     case ETHSETHWADDRESS:{
        board_ksz885x_net_info_t* p_net_info=(board_ksz885x_net_info_t *)ofile_lst[desc].p;
        unsigned char* p_eth_hwaddr = va_arg( ap, unsigned char*);

        if(!p_net_info || !p_eth_hwaddr)
           return -1;

        //stop ehternet interface
        cyg_interrupt_mask((cyg_vector_t)p_net_info->dev_info.irq_no);
        _ksz885x_disable_hw(&p_net_info->ksz885x_info);

        //set mac address
        p_net_info->ksz885x_info.mac_addr[0] = p_eth_hwaddr[0];
        p_net_info->ksz885x_info.mac_addr[1] = p_eth_hwaddr[1];
        p_net_info->ksz885x_info.mac_addr[2] = p_eth_hwaddr[2];
        p_net_info->ksz885x_info.mac_addr[3] = p_eth_hwaddr[3];
        p_net_info->ksz885x_info.mac_addr[4] = p_eth_hwaddr[4];
        p_net_info->ksz885x_info.mac_addr[5] = p_eth_hwaddr[5];
        _ksz885x_set_mac_addr(&p_net_info->ksz885x_info, p_net_info->ksz885x_info.mac_addr);

        //renable ethernet interface
        _ksz885x_enable_hw(&p_net_info->ksz885x_info);
        cyg_interrupt_unmask((cyg_vector_t)p_net_info->dev_info.irq_no);
     }
     break;

     case ETHGETHWADDRESS:{
        board_ksz885x_net_info_t * p_net_info = (board_ksz885x_net_info_t *)ofile_lst[desc].p;
        unsigned char* p_eth_hwaddr = va_arg( ap, unsigned char*);
        if(!p_net_info)
           return -1;

        p_eth_hwaddr[0] = p_net_info->ksz885x_info.mac_addr[0];
        p_eth_hwaddr[1] = p_net_info->ksz885x_info.mac_addr[1];
        p_eth_hwaddr[2] = p_net_info->ksz885x_info.mac_addr[2];
        p_eth_hwaddr[3] = p_net_info->ksz885x_info.mac_addr[3];
        p_eth_hwaddr[4] = p_net_info->ksz885x_info.mac_addr[4];
        p_eth_hwaddr[5] = p_net_info->ksz885x_info.mac_addr[5];
     }
     break;

     //
     default:
     return -1;
   }
   return 0;
}


/*-------------------------------------------
| Name: dev_eth_ksz885x_interrupt_isr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
cyg_uint32 dev_eth_ksz885x_isr(cyg_vector_t vector, cyg_addrword_t data) {
   volatile unsigned short reg_val = 0;
   board_ksz885x_net_info_t * p_net_info = (board_ksz885x_net_info_t *)data;
   volatile unsigned int ret=CYG_ISR_HANDLED;
   volatile unsigned short isr_status = 0;
   volatile unsigned short ier_status = 0;

   if(!p_net_info)
      return ret;

   //disable all device interrupt
   cyg_interrupt_mask(vector);
   __ksz885x_disable_irq(&p_net_info->ksz885x_info);

   //read interrupt status register
   __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_INT_STATUS, (unsigned short *)&isr_status);
   __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_INT_MASK, (unsigned short *)&ier_status);

   if(isr_status & INT_TX) {
      if(!dev_eth_ksz885x_isr_send(p_net_info)) {
         __inc_tx(p_net_info->ksz885x_stats);
         ret |= CYG_ISR_CALL_DSR;
      }

      reg_val = INT_TX;
      __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_INT_STATUS, (unsigned short)reg_val);
   }

   /*  Transmit space available */
   if(isr_status & INT_TX_SPACE) {
      __inc_tx_space(p_net_info->ksz885x_stats);
   }

   /*  Transmit error */
   if(isr_status & INT_TX_STOPPED) {
      __inc_tx_stopped(p_net_info->ksz885x_stats);
      reg_val = INT_TX_STOPPED;
      __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_INT_STATUS, (unsigned short)reg_val);
   }

   if(isr_status & INT_PHY) {
      __inc_phy(p_net_info->ksz885x_stats);
      reg_val = INT_PHY;
      __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_INT_STATUS, (unsigned short)reg_val);
      p_net_info->ksz885x_info.auto_nego_req = 1;
      p_net_info->xmit_flag=1;
   }

   if(isr_status & INT_RX_WOL_FRAME) {
      __inc_wol_frame(p_net_info->ksz885x_stats);
      __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_POWER_CNTL, (unsigned short *)&reg_val);
      reg_val &= ~PMECR_WKEVT_MASK;
      reg_val |= PMECR_WKEVT_WAKEUP;
      __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_POWER_CNTL, (unsigned short)reg_val);
   }

   if(isr_status & INT_RX_WOL_MAGIC) {
      __inc_wol_magic(p_net_info->ksz885x_stats);
      __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_POWER_CNTL, (unsigned short *)&reg_val);
      reg_val &= ~PMECR_WKEVT_MASK;
      reg_val |= PMECR_WKEVT_MAGIC;
      __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_POWER_CNTL, (unsigned short)reg_val);
   }

   if(isr_status & INT_RX_WOL_LINKUP) {
      __inc_wol_link(p_net_info->ksz885x_stats);
      __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_POWER_CNTL, (unsigned short *)&reg_val);
      reg_val &= ~PMECR_WKEVT_MASK;
      reg_val |= PMECR_WKEVT_LINK;
      __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_POWER_CNTL, (unsigned short)reg_val);
   }

   if(isr_status & INT_RX_WOL_ENERGY) {
      __inc_wol_energy(p_net_info->ksz885x_stats);
      __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_POWER_CNTL, (unsigned short *)&reg_val);
      reg_val &= ~PMECR_WKEVT_MASK;
      reg_val |= PMECR_WKEVT_ENERGY;
      __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_POWER_CNTL, (unsigned short)reg_val);
   }

   if(isr_status & INT_RX_OVERRUN) {
      __inc_rx_ov(p_net_info->ksz885x_stats);
      reg_val = INT_RX_OVERRUN;
      __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_INT_STATUS, (unsigned short)reg_val);
   }

   if(isr_status & INT_RX_STOPPED) {
      __inc_rx_stopped(p_net_info->ksz885x_stats);
      reg_val = INT_RX_STOPPED;
      __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_INT_STATUS, (unsigned short)reg_val);
   }

   if(isr_status & INT_RX) {
      reg_val = INT_RX;
      __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_INT_STATUS, (unsigned short)reg_val);

      if(!dev_eth_ksz885x_isr_receive(p_net_info)) {
         __inc_rx(p_net_info->ksz885x_stats);
         ret |= CYG_ISR_CALL_DSR;
      }
   }

   cyg_interrupt_acknowledge(vector);

   if(!(ret & CYG_ISR_CALL_DSR)) {
      //enable IRQ again
      __ksz885x_enable_irq(&p_net_info->ksz885x_info);
      cyg_interrupt_unmask(vector);
   }

   __inc_isr(p_net_info->ksz885x_stats);

   //need to execute DSR
   return ret;
}

/*-------------------------------------------
| Name: dev_eth_ksz885x_interrupt_dsr
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
void dev_eth_ksz885x_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
   board_ksz885x_net_info_t * p_net_info = (board_ksz885x_net_info_t *)data;
   unsigned short reg_val=0;

   if(!p_net_info)
      return;

   if(p_net_info->ksz885x_info.auto_nego_req) {
      _ksz885x_init_phy(&p_net_info->ksz885x_info);
      p_net_info->eth_stat = p_net_info->ksz885x_info.speed;
      _ksz885x_clear_wol_event(&p_net_info->ksz885x_info, WAKEUP_EVENT_MASK);
      p_net_info->ksz885x_info.auto_nego_req = 0;
   }
   
   if(p_net_info->_eth_ksz885x_desc_wr>0 && p_net_info->_flag_w_irq) {
      __inc_dsr_tx(p_net_info->ksz885x_stats);
      p_net_info->_flag_w_irq = 0;
      __fire_io_int(ofile_lst[p_net_info->_eth_ksz885x_desc_wr].owner_pthread_ptr_write);
   }

   if(p_net_info->_eth_ksz885x_desc_rd>0 && p_net_info->_flag_r_irq) {
      __inc_dsr_rx(p_net_info->ksz885x_stats);
      p_net_info->_flag_r_irq = 0;
      __fire_io_int(ofile_lst[p_net_info->_eth_ksz885x_desc_rd].owner_pthread_ptr_read);
   }

   __inc_dsr(p_net_info->ksz885x_stats);
   //enable IRQ again
   __ksz885x_enable_irq(&p_net_info->ksz885x_info);
   cyg_interrupt_unmask(vector);
}


/*-------------------------------------------
| Name: dev_eth_ksz885x_isr_send
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_ksz885x_isr_send(board_ksz885x_net_info_t * p_net_info) {
   volatile unsigned short tx_status=0;
   volatile unsigned short reg_val=0;
   volatile unsigned short reg_val_tmp=0;

   if(!p_net_info)
      return -1;

   /* Read transmit status register (TXSR) */
   __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_TX_STATUS, (unsigned short *)&tx_status );

   if(tx_status & TX_STAT_ERRORS) {
      __inc_tx_errors(p_net_info->ksz885x_stats);
      /* Disable QMU Transmit and Flush Transmit Queue (TXCR) */
      __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_TX_CTRL, (unsigned short *)&reg_val );
      reg_val_tmp = reg_val ;

      reg_val &= ~TX_CTRL_ENABLE;
      reg_val |= TX_CTRL_FLUSH_QUEUE;
      __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_TX_CTRL, reg_val);

      /* Enable QMU Transmit again (TXCR) */
      __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_TX_CTRL, reg_val_tmp);

      return -1;
   }

   p_net_info->_output_r = p_net_info->_output_w;
   p_net_info->_flag_w_irq = 1;
   return 0;
}


/*-------------------------------------------
| Name: dev_eth_ksz885x_isr_receive
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int dev_eth_ksz885x_isr_receive(board_ksz885x_net_info_t * p_net_info) {
   volatile unsigned short reg_val=0;
   volatile unsigned short rx_frame_count=0;
   volatile unsigned short rx_status=0;
   unsigned int rx_length_word=0;
   unsigned int rx_length=0;
   unsigned short * rcv_ptr = NULL;
   unsigned int i=0;
   int ret=-1;

   if(!p_net_info)
      return -1;

   //read frame count
   __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_RX_FRAME_CNT_THRES, (unsigned short *)&rx_frame_count);
   rx_frame_count &= RX_FRAME_CNT_MASK;
   rx_frame_count = rx_frame_count >> 8;

   //no received frame
   if(!rx_frame_count) {
      return -1;
   }

   while(rx_frame_count-->0) {
      //read status register
      __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_RX_FHR_STATUS, (unsigned short *)&rx_status);
      if(!(rx_status & RX_VALID)
            || (rx_status & RX_ERRORS)) {
         //error on received packet
         //release RX frame error
         reg_val = RXQ_CMD_CNTL|RXQ_CMD_FREE_PACKET;
         __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_RXQ_CMD, (unsigned short)reg_val);
         __inc_rx_err_frame(p_net_info->ksz885x_stats);
      }
      else {
         //how many bytes to read
         __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_RX_FHR_BYTE_CNT, (unsigned short *)&rx_length);
         rx_length &= RX_BYTE_CNT_MASK;

         if(rx_length <= 0) {
            reg_val = RXQ_CMD_CNTL|RXQ_CMD_FREE_PACKET;
            __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_RXQ_CMD, (unsigned short)reg_val);
            __inc_rx_err_size(p_net_info->ksz885x_stats);
         }
         else {
            rx_pool[p_net_info->_input_w].len = rx_length;

            //reset QMU RXQ frame pointer
            reg_val = 0x4000;
            __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_RX_ADDR_PTR, (unsigned short)reg_val);

            //start QMU DMA
            reg_val = 0;
            __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_RXQ_CMD, (unsigned short *)&reg_val);
            reg_val |= RXQ_START;
            __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_RXQ_CMD, (unsigned short)reg_val);

            //dummy read from frame header QMU RXQ
            _ksz885x_read_data16((void *)&p_net_info->ksz885x_info, (unsigned short *)&reg_val);
            //read status from frame header QMU RXQ
            _ksz885x_read_data16((void *)&p_net_info->ksz885x_info, (unsigned short *)&rx_status);
            //read count bytes from frame header QMU RXQ
            _ksz885x_read_data16((void *)&p_net_info->ksz885x_info, (unsigned short *)&reg_val);

            rx_pool[p_net_info->_input_w].rx_len = ((rx_pool[p_net_info->_input_w].len+3) & ~0x03)>>1;

            //prepare receive buffer and read data
            rcv_ptr = (unsigned short *)rx_pool[p_net_info->_input_w].buffer;
            while(rx_pool[p_net_info->_input_w].rx_len-- > 2) {
               _ksz885x_read_data16((void *)&p_net_info->ksz885x_info, rcv_ptr++);
            }
            //4 last bytes is CRC
            _ksz885x_read_data16((void *)&p_net_info->ksz885x_info, (unsigned short *)&reg_val);
            _ksz885x_read_data16((void *)&p_net_info->ksz885x_info, (unsigned short *)&reg_val);

            //stop QMU DMA
            reg_val = 0;
            __ksz885x_read_reg((void *)&p_net_info->ksz885x_info, REG_RXQ_CMD, (unsigned short *)&reg_val);
            reg_val &= ~RXQ_START;
            __ksz885x_write_reg((void *)&p_net_info->ksz885x_info, REG_RXQ_CMD, (unsigned short)reg_val);

            //
            p_net_info->_flag_r_irq = 1;
            p_net_info->rx_pkt_cnt++;
            ret = 0;
            if(++p_net_info->_input_w>=RCV_POOL_SIZE) {
               p_net_info->_input_w=0;
            }
         }
      }
   }

   return ret;
}

/*============================================
| End of Source  : dev_eth_ksz885x.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/

