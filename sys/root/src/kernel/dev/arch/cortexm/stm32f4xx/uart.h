/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : uart.h
* Author             : Yoann TREGUIER
* Version            : 1.1.0
* Date               : 2013/01/23
* Description        : Headers of UART driver functions
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
  IRQn_Type IRQn;
  DMA_Channel_TypeDef *DMAx;
  IRQn_Type DMAx_IRQn;
  _Uart_Ctrl *Ctrl;
} _Uart_Descriptor;

/* Exported define -----------------------------------------------------------*/
#define UART_HW_FLOW_CTRL_RX  0x01
#define UART_HW_FLOW_CTRL_TX  0x02
#define UART_HALF_DUPLEX      0x04
#define UART_TX_IN_PROGRESS   0x80

#define UART_1 (&Uart1_Descriptor)
#define UART_2 (&Uart2_Descriptor)
#define UART_3 (&Uart3_Descriptor)
#define UART_4 (&Uart4_Descriptor)
#define UART_5 (&Uart5_Descriptor)

/* Exported macro ------------------------------------------------------------*/
#define uart_tx_in_progress(Uart)   (Uart->Ctrl->HwCtrl & UART_TX_IN_PROGRESS)

/* Exported constants --------------------------------------------------------*/
extern const _Uart_Descriptor Uart1_Descriptor;
extern const _Uart_Descriptor Uart2_Descriptor;
extern const _Uart_Descriptor Uart3_Descriptor;
extern const _Uart_Descriptor Uart4_Descriptor;
extern const _Uart_Descriptor Uart5_Descriptor;

/* Exported variables --------------------------------------------------------*/
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
