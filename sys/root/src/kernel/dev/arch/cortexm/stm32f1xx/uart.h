/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : uart.h
* Author             : Yoann TREGUIER
* Version            : 1.2.0
* Date               : 2013/06/14
* Description        : Headers of UART driver functions for STM32F1xx devices
*******************************************************************************/
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

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
