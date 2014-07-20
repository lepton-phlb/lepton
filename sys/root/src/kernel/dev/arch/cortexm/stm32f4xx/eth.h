/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Philippe Le Boulanger.
Portions created by Philippe Le Boulanger are Copyright (C) 2014 <lepton.phlb@gmail.com>.
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
