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

#include "kernel/core/ioctl_eth.h"
//#include "kernel/dev/arch/cortexm/stm32f4xx/driverlib/stm32f4xx.h"
//#include "kernel/dev/arch/cortexm/stm32f4xx/driverlib/stm32f4xx_eth.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/driverlib/stm32f4x7_eth.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/types.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/target.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/gpio.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/eth.h"
#include <string.h>

//#define CHECKSUM_BY_HARDWARE

/* Ethernet Rx & Tx DMA Descriptors */
extern ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB], DMATxDscrTab[ETH_TXBUFNB];

/* Ethernet Receive buffers  */
extern uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE]; 

/* Ethernet Transmit buffers */
extern uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE]; 

/* Global pointers to track current transmit and receive descriptors */
extern ETH_DMADESCTypeDef  *DMATxDescToSet;
extern ETH_DMADESCTypeDef  *DMARxDescToGet;

/* Global pointer for last received frame infos */
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;


eth_stm32f4x7_info_t eth_stm32f4x7_info={0};

/*************************************************************************
 * Function Name: eth_phy_read
 * Parameters:
 * Return:
 *
 * Description: Read data from phy controller
 *
 *************************************************************************/
static uint16_t eth_phy_read (uint8_t PhyAddr, uint8_t Reg)
{
  
  ETH->MACMIIAR = (1<<0) |
                  (0<<1) |
                  (Reg<<6) |
                  (PhyAddr<<11);
  while(ETH->MACMIIAR & (1<<0));
  return(ETH->MACMIIDR & (0xFFFF));
}

/*************************************************************************
 * Function Name: eth_phy_write
 * Parameters:
 * Return:
 *
 * Description: Write data to phy controller
 *
 *************************************************************************/
static void eth_phy_write (uint8_t PhyAddr, uint8_t Reg, uint16_t Data)
{
  ETH->MACMIIDR = Data;
  ETH->MACMIIAR = (1<<0) |
                  (1<<1) |
                  (Reg<<6) |
                  (PhyAddr<<11);
  while(ETH->MACMIIAR & (1<<0));
}

/*************************************************************************
 * Function Name: eth_phy_get_addr
 * Parameters:
 * Return:
 *
 * Description: get phy address
 *
 *************************************************************************/
static unsigned int eth_phy_get_addr(void){
   unsigned int PhyAddr;
   // read the ID for match
   for(PhyAddr = 1; 32 >= PhyAddr; PhyAddr++){
      if((0x0022 == ETH_ReadPHYRegister(PhyAddr,2))
            && (0x1619 == (ETH_ReadPHYRegister(PhyAddr,3)))) break;
   }
   //
   if(32 < PhyAddr){
      //Ethernet Phy Not Found\n\r
      return 0;
   }
   //
   return PhyAddr;
}

/*************************************************************************
 * Function Name: eth_packet_read
 * Parameters:
 * Return:
 *
 * Description: read packet from MAC/DMA Controller
 *
 *************************************************************************/
int eth_packet_read(unsigned char* p_to_user_buffer, int size)
{
	uint16_t len;
	uint32_t l=0,i =0;
	FrameTypeDef frame;
	static int framecnt = 1;
	u8 *buffer;
	ETH_DMADESCTypeDef *DMARxNextDesc;
	
	/* Get received frame */
	frame = ETH_Get_Received_Frame_interrupt();
	
   if(frame.length > size)
     return -1;
   
	if( frame.length > 0 )
	{
		/* check that frame has no error */
		if ((frame.descriptor->Status & ETH_DMARxDesc_ES) == (uint32_t)RESET)
		{
			//rt_kprintf("Get a frame %d buf = 0x%X, len= %d\n", framecnt++, frame.buffer, frame.length);
			/* Obtain the size of the packet and put it into the "len" variable. */
			len = frame.length;
			buffer = (u8 *)frame.buffer;
			memcpy(p_to_user_buffer,buffer,len);
		}
	
		/* Release descriptors to DMA */
		/* Check if received frame with multiple DMA buffer segments */
		if (DMA_RX_FRAME_infos->Seg_Count > 1)
		{
			DMARxNextDesc =  (ETH_DMADESCTypeDef *)DMA_RX_FRAME_infos->FS_Rx_Desc;
		}
		else
		{
			DMARxNextDesc =  (ETH_DMADESCTypeDef *)frame.descriptor;
		}
		
		/* Set Own bit in Rx descriptors: gives the buffers back to DMA */
		for (i=0; i<DMA_RX_FRAME_infos->Seg_Count; i++)
		{  
			DMARxNextDesc->Status = ETH_DMARxDesc_OWN;
			DMARxNextDesc = (ETH_DMADESCTypeDef *)(DMARxNextDesc->Buffer2NextDescAddr);
		}
		
		/* Clear Segment_Count */
		DMA_RX_FRAME_infos->Seg_Count =0;
		
		
		/* When Rx Buffer unavailable flag is set: clear it and resume reception */
		if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)  
		{
			/* Clear RBUS ETHERNET DMA flag */
			ETH->DMASR = ETH_DMASR_RBUS;
			  
			/* Resume DMA reception */
			ETH->DMARPDR = 0;
		}
	}
   //
	return len;
}

/*************************************************************************
 * Function Name: eth_packet_write
 * Parameters:
 * Return:
 *
 * Description: write packet to MAC/DMA Controller
 *
 *************************************************************************/
int eth_packet_write( const unsigned char* p_from_user_buffer, int size)
{
   uint32_t l = 0;
	u8 *buffer ;
	
	//
   buffer =  (u8 *)(DMATxDescToSet->Buffer1Addr);
	memcpy((u8*)&buffer[l], p_from_user_buffer, size);
   //
   l=size;
	//
	if( ETH_Prepare_Transmit_Descriptors(l) == ETH_ERROR ){
	   return -1;//error
	}
	
	/* Return SUCCESS */
	return size;
}

/*************************************************************************
 * Function Name: eth_nvic_configuration
 * Parameters:
 * Return:
 *
 * Description: configuration of MAC/DMA Controller
 *
 *************************************************************************/
static void eth_nvic_configuration(void)
{
        NVIC_InitTypeDef NVIC_InitStructure;
        
        /* 2 bit for pre-emption priority, 2 bits for subpriority */
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
        /* Enable the Ethernet global Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
}

/*************************************************************************
 * Function Name: eth_macdma_it_config
 * Parameters:
 * Return:
 *
 * Description: configuration of MAC/DMA Controller
 *
 *************************************************************************/
static int eth_macdma_it_config(void){
   //
   int i=0;
   /* Initialize Tx Descriptors list: Chain Mode */
   ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
   /* Initialize Rx Descriptors list: Chain Mode  */
   ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
   
   /* Enable Ethernet Rx/Tx interrrupt */
   { 
      for(i=0; i<ETH_RXBUFNB; i++)
      {
         ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
      }
      
      for(i=0; i<ETH_TXBUFNB; i++)
      {
        ETH_DMATxDescTransmitITConfig(&DMATxDscrTab[i], ENABLE);
      }
   }
   #ifdef CHECKSUM_BY_HARDWARE
	/* Enable the checksum insertion for the Tx frames */
	{
		for(i=0; i<ETH_TXBUFNB; i++)
		{
		  ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
		}
	} 
	#endif
   //
   eth_nvic_configuration();
   /* Enable the Ethernet Rx Interrupt */
   ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R | ETH_DMA_IT_T, ENABLE);
   return 0;
}

/*************************************************************************
 * Function Name: eth_macdma_config
 * Parameters:
 * Return:
 *
 * Description: configuration of MAC/DMA Controller
 *
 *************************************************************************/
static int eth_macdma_config(void)
{
   uint16_t PhyAddr=0;
   uint32_t  EthInitStatus = 0;
   ETH_InitTypeDef ETH_InitStructure;
   
   /* Enable ETHERNET clock  */
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
                     RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);
                     
   /* Reset ETHERNET on AHB Bus */
   ETH_DeInit();
   
   /* Software reset */
   ETH_SoftwareReset();
   
   /* Wait for software reset */
   while (ETH_GetSoftwareResetStatus() == SET);
   
   /* ETHERNET Configuration --------------------------------------------------*/
   /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
   ETH_StructInit(&ETH_InitStructure);
   
   /* Fill ETH_InitStructure parametrs */
   /*------------------------   MAC   -----------------------------------*/
   ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
   //ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
   //  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
   //  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   
   
   ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
   ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
   ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
   ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
   ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
   ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
   ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
   ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
   #ifdef CHECKSUM_BY_HARDWARE
   ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
   #endif
   
   /*------------------------   DMA   -----------------------------------*/  
   
   /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
   the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
   if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
   ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; 
   ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;         
   ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;     
   
   ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;       
   ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;   
   ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
   ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;      
   ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                
   ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;          
   ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
   ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
   //
   if((PhyAddr=eth_phy_get_addr())==0){
      return -1;
   }
   /* Configure Ethernet */
   if(EthInitStatus = ETH_Init(&ETH_InitStructure, PhyAddr)==0){
      //Ethernet Initialization Failed
      return -1;
   }
   
   return 0;
}

/*************************************************************************
 * Function Name: eth_init
 * Parameters:
 * Return:
 *
 * Description: init Ethernet peripheral
 *
 *************************************************************************/
void eth_init(void){
   //
   ETH_MACAddressConfig(ETH_MAC_Address0, (u8*)&eth_stm32f4x7_info.mac_addr[0]);
   //
   eth_macdma_it_config();
   //
   ETH_Start();
}

/*************************************************************************
 * Function Name: eth_bsp_init
 * Parameters:
 * Return:
 *
 * Description:
 *
 *************************************************************************/
int eth_bsp_init(void)
{
   //
   unsigned int PhyAddr;
   //
   GPIO_InitTypeDef GPIO_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
   ETH_InitTypeDef ETH_InitStructure;
   
   /*Set default MAC Address*/
   eth_stm32f4x7_info.mac_addr[0]=ETH_STM32F4X7_DEFAULT_MAC_ADDR0;
   eth_stm32f4x7_info.mac_addr[1]=ETH_STM32F4X7_DEFAULT_MAC_ADDR1;
   eth_stm32f4x7_info.mac_addr[2]=ETH_STM32F4X7_DEFAULT_MAC_ADDR2;
   eth_stm32f4x7_info.mac_addr[3]=ETH_STM32F4X7_DEFAULT_MAC_ADDR3;
   eth_stm32f4x7_info.mac_addr[4]=ETH_STM32F4X7_DEFAULT_MAC_ADDR4;
   eth_stm32f4x7_info.mac_addr[5]=ETH_STM32F4X7_DEFAULT_MAC_ADDR5;
   
   /* Enable ETHERNET clocks  */
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
   RCC_AHB1Periph_ETH_MAC_Rx | RCC_AHB1Periph_ETH_MAC_PTP, ENABLE);
   
   
   /* Enable GPIOs clocks */
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |	RCC_AHB1Periph_GPIOB |
   RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOG, ENABLE);
   
   /* Enable SYSCFG clock */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
   /*Select RMII Interface*/
   SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
   
   /* ETHERNET pins configuration */
   /* PA
   ETH_RMII_REF_CLK: PA1
   ETH_RMII_MDIO: PA2
   ETH_RMII_MDINT: PA3
   ETH_RMII_CRS_DV: PA7
   */
   
   /* Configure PA1, PA2, PA3 and PA7*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_7;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
   /* Connect PA1, PA2, PA3 and PA7 to ethernet module*/
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);
   
   /* PB
   ETH_RMII_TX_EN: PB11
   */
   
   /* Configure PB11*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
   
   /* Connect PB11 to ethernet module*/
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);
   
   /* PC
   ETH_RMII_MDC: PC1
   ETH_RMII_RXD0: PC4
   ETH_RMII_RXD1: PC5
   */
   
   /* Configure PC1, PC4 and PC5*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
   
   /* Connect PC1, PC4 and PC5 to ethernet module*/
   GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);
   
   /* PG
   ETH_RMII_TXD0: PG13
   ETH_RMII_TXD1: PG14
   */
   
   /* Configure PG14 and PG15*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOG, &GPIO_InitStructure);
   
   /* Connect PG13 and PG15 to ethernet module*/
   GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
   GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);
 
#if 0
   /* Reset ETHERNET on AHB Bus */
   ETH_DeInit();
   
   /* Software reset */
   ETH_SoftwareReset();
   
   /* Wait for software reset */
   while(ETH_GetSoftwareResetStatus()==SET);
   
   /* ETHERNET Configuration ------------------------------------------------------*/
   /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
   ETH_StructInit(&ETH_InitStructure);
   
   /* Fill ETH_InitStructure parametrs */
   /*------------------------   MAC   -----------------------------------*/
   //ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable  ;
   ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
   //ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
   ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
   //ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
   ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
   ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
   ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;
   ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Disable;
   ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
   ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
   ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
   ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
   ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
   
   // read the ID for match
   for(PhyAddr = 1; 32 >= PhyAddr; PhyAddr++)
   {
      if((0x0022 == ETH_ReadPHYRegister(PhyAddr,2))
            && (0x1619 == (ETH_ReadPHYRegister(PhyAddr,3)))) break;
   }
   //
   if(32 < PhyAddr)
   {
      //printf("Ethernet Phy Not Found\n\r");
      return -1;
   }
   /* Configure Ethernet */
   if(0 == ETH_Init(&ETH_InitStructure, PhyAddr))
   {
      //printf("Ethernet Initialization Failed\n\r");
      return -1;
   }
#endif
   //
   if(eth_macdma_config()<0)
     return -1;
   //
   return 0;//no error
}