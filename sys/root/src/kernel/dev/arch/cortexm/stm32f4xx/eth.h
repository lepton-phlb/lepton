/*************************************************************************
 *
 *    Used with ARM IAR C/C++ Compiler
 *
 *    (c) Copyright IAR Systems 2009
 *
 *    $Revision: #1 $
 **************************************************************************/

//#include "my_includes.h"

#ifndef  __ETHERNET_H
#define  __ETHERNET_H

#include <stdint.h>
#include "kernel/dev/arch/cortexm/stm32f4xx/driverlib/stm32f4xx.h"

#define PHY_ADDR  0x15
#define EMAC_MAX_PACKET_SIZE 1520
// ENET_OperatingMode
#define PHY_OPR_MODE        0x2100      //  Set the full-duplex mode at 100 Mb/s


#define ETH_STM32F4X7_DEFAULT_MAC_ADDR0   0x00
#define ETH_STM32F4X7_DEFAULT_MAC_ADDR1   0xbd
#define ETH_STM32F4X7_DEFAULT_MAC_ADDR2   0x3b
#define ETH_STM32F4X7_DEFAULT_MAC_ADDR3   0x33
#define ETH_STM32F4X7_DEFAULT_MAC_ADDR4   0x05
#define ETH_STM32F4X7_DEFAULT_MAC_ADDR5   0x71

//
typedef struct eth_stm32f4x7_info_st {
   //
   char mac_addr[6];
   //
   u8 link_up;
   //ethernet status
   eth_stat_t eth_stat;
} eth_stm32f4x7_info_t;

//
int eth_packet_read(unsigned char* p_to_user_buffer, int size);
int eth_packet_write(const unsigned char* p_from_user_buffer, int size);
//
void eth_init(void);
int eth_bsp_init(void);

#endif  /* __ETHERNET_H */
