
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H
#define __UART_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef __packed struct
{
  u8 HwCtrl;
  const _Gpio_Descriptor *Gpio;
  u8 DmaBufSize;
  char *DmaBufPtr;
  u8 iDma;
  u16 RxBufSize;
  char *RxBufPtr;
  u16 RxCnt;
  u16 RxiPut;
  u16 RxiGet;
  u16 TxBufSize;
  char *TxBufPtr;
  u16 TxCnt;
  u16 TxiPut;
  u16 TxiGet;
  #ifdef _UART_OS_SUPPORT
    u16 Event;
    OS_TID Task;
  #endif
} _Uart_Ctrl;

typedef __packed struct
{
  USART_TypeDef *UARTx;
  void (*RCC_APBxPeriphClockCmd)(uint32_t, FunctionalState);
  u32 RCC_APBxPeriph;
  IRQn_Type IRQn;
  DMA_Channel_TypeDef *DMAx;
  IRQn_Type DMAx_IRQn;
  const _Gpio_Descriptor *TxGpio;
  const _Gpio_Descriptor *RxGpio;
  _Uart_Ctrl **Ctrl;
  
  //lepton uart_info
   struct board_stm32f1xx_uart_info_st* board_uart_info;
  
} _Uart_Descriptor;

/* Exported define -----------------------------------------------------------*/
#define UART_HW_FLOW_CTRL_NONE   0x00
#define UART_HW_FLOW_CTRL_RX     0x01
#define UART_HW_FLOW_CTRL_TX     0x02
#define UART_HALF_DUPLEX         0x04
#define UART_TX_IN_PROGRESS      0x80

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const _Uart_Descriptor Uart_Descriptor[];

/* Exported variables --------------------------------------------------------*/
extern _Uart_Ctrl *Uart_Ctrl[UART_NB];

/* Exported functions --------------------------------------------------------*/
int uart_open(const _Uart_Descriptor *Uart, u32 BaudRate, u8 DmaBufSize, u16 RxBufSize, u16 TxBufSize, u8 HwCtrl, const _Gpio_Descriptor *Gpio);
int uart_close(const _Uart_Descriptor *Uart);
void uart_irq_handler(const _Uart_Descriptor *Uart);
void uart_dma_irq_handler(const _Uart_Descriptor *Uart);
int uart_get_char(const _Uart_Descriptor *Uart);
int uart_put_char(char Data, const _Uart_Descriptor *Uart);
int uart_read(void *WrPtr, u16 Size, const _Uart_Descriptor *Uart);
int uart_write(const void *RdPtr, u16 Size, const _Uart_Descriptor *Uart);

#endif /* __UART_H */

