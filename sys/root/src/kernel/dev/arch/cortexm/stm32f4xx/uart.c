/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : uart.c
* Author             : Yoann TREGUIER
* Version            : 1.1.0
* Date               : 2013/01/23
* Description        : UART driver functions
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "kernel/dev/arch/cortexm/stm32f4xx/driverlib/stm32f4xx.h"
#include "gpio.h"
#include "uart.h"

#define _UART_1_SUPPORT

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if !defined (_UART_1_SUPPORT) && !defined (_UART_2_SUPPORT) && !defined (_UART_3_SUPPORT) && !defined (_UART_4_SUPPORT) && !defined (_UART_5_SUPPORT)
  #error "UART support not defined"
#endif

#define UART_IT_ALL   (USART_IT_PE | USART_IT_TXE | USART_IT_TC | USART_IT_RXNE | USART_IT_IDLE | USART_IT_LBD | USART_IT_CTS | USART_IT_ERR)

/* Private macro -------------------------------------------------------------*/
#define UART_IS_RX_HFC_ON(Uart)   gpio_read_output(Uart->Ctrl->Gpio)
#define UART_RX_HFC_ON(Uart)      gpio_set(Uart->Ctrl->Gpio)
#define UART_RX_HFC_OFF(Uart)     gpio_reset(Uart->Ctrl->Gpio)
#define UART_TX_ENABLE(Uart)      gpio_set(Uart->Ctrl->Gpio)
#define UART_TX_DISABLE(Uart)     gpio_reset(Uart->Ctrl->Gpio)

/* Private variables & constants ---------------------------------------------*/
#ifdef _UART_1_SUPPORT
  _Uart_Ctrl Uart1_Ctrl;
  const _Uart_Descriptor Uart1_Descriptor = {USART1, USART1_IRQn, DMA1_Channel5, DMA1_Channel5_IRQn, &Uart1_Ctrl};
#endif

#ifdef _UART_2_SUPPORT
  _Uart_Ctrl Uart2_Ctrl;
  const _Uart_Descriptor Uart2_Descriptor = {USART2, USART2_IRQn, DMA1_Channel6, DMA1_Channel6_IRQn, &Uart2_Ctrl};
#endif

#ifdef _UART_3_SUPPORT
  _Uart_Ctrl Uart3_Ctrl;
  const _Uart_Descriptor Uart3_Descriptor = {USART3, USART3_IRQn, DMA1_Channel3, DMA1_Channel3_IRQn, &Uart3_Ctrl};
#endif

#ifdef _UART_4_SUPPORT
  _Uart_Ctrl Uart4_Ctrl;
  const _Uart_Descriptor Uart4_Descriptor = {UART4, UART4_IRQn, DMA2_Channel3, DMA2_Channel3_IRQn, &Uart4_Ctrl};
#endif

#ifdef _UART_5_SUPPORT
  _Uart_Ctrl Uart5_Ctrl;
  const _Uart_Descriptor Uart5_Descriptor = {UART5, UART5_IRQn, (DMA_Channel_TypeDef *)0, (IRQn_Type)0, &Uart5_Ctrl};
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : uart_open
* Description    : Open the UART port communication
* Input          : - Uart: Select the USART or the UART peripheral
*                : - BaudRate: Baud rate configuration
*                : - DmaBufSize: DMA buffer size
*                : - RxBufSize: Receive buffer size
*                : - TxBufSize: Transmit buffer size
*                : - HwCtrl: Hardware control options
*                : - Gpio: GPIO used for hardware control
* Output         : None
* Return         : 0 if OK, -1 in case of error
*******************************************************************************/
int uart_open(const _Uart_Descriptor *Uart, u32 BaudRate, u8 DmaBufSize, u16 RxBufSize, u16 TxBufSize, u8 HwCtrl, const _Gpio_Descriptor *Gpio)
{
  GPIO_InitTypeDef gpio_init_structure;
  USART_InitTypeDef usart_init_structure;
  DMA_InitTypeDef dma_init_structure;
  GPIO_TypeDef *txd_gpio;
  u16 txd_pin;

  if (Uart->Ctrl->DmaBufPtr) _free(Uart->Ctrl->DmaBufPtr);
  memset(Uart->Ctrl, 0, sizeof(_Uart_Ctrl));
  if ((Uart->Ctrl->DmaBufPtr = _malloc(DmaBufSize + RxBufSize + TxBufSize)) == 0) return(-1);
  Uart->Ctrl->DmaBufSize = DmaBufSize;
  Uart->Ctrl->RxBufSize = RxBufSize;
  Uart->Ctrl->RxBufPtr = Uart->Ctrl->DmaBufPtr + Uart->Ctrl->DmaBufSize;
  Uart->Ctrl->TxBufSize = TxBufSize;
  Uart->Ctrl->TxBufPtr = Uart->Ctrl->RxBufPtr + Uart->Ctrl->RxBufSize;
  Uart->Ctrl->HwCtrl = HwCtrl;
  Uart->Ctrl->Gpio = Gpio;
  #ifdef _UART_OS_SUPPORT
    Uart->Ctrl->Event = SYS_EVT_INCOMING_DATA;
    Uart->Ctrl->Task = sys_task_self();
  #endif

  switch (*(u32 *)&Uart->UARTx)
  {
    #ifdef _UART_1_SUPPORT
    case USART1_BASE:
      /* TxD */
      txd_gpio = GPIOA;
      txd_pin = GPIO_Pin_9;
      gpio_init_structure.GPIO_Pin = GPIO_Pin_9;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_PP;
      GPIO_Init(GPIOA, &gpio_init_structure);
      /* RxD */
      gpio_init_structure.GPIO_Pin = GPIO_Pin_10;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init(GPIOA, &gpio_init_structure);
      /* AHB peripheral clocks */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
      /* DMA */
      if (Uart->Ctrl->DmaBufSize)
      {
        DMA_StructInit(&dma_init_structure);
        dma_init_structure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;
        dma_init_structure.DMA_MemoryBaseAddr = (u32)Uart->Ctrl->DmaBufPtr;
        dma_init_structure.DMA_BufferSize = Uart->Ctrl->DmaBufSize;
        dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        dma_init_structure.DMA_Mode = DMA_Mode_Circular;
        dma_init_structure.DMA_Priority = DMA_Priority_High;
      }
      break;
    #endif

    #ifdef _UART_2_SUPPORT
    case USART2_BASE:
      /* TxD */
      txd_gpio = GPIOA;
      txd_pin = GPIO_Pin_2;
      gpio_init_structure.GPIO_Pin = GPIO_Pin_2;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_PP;
      GPIO_Init(GPIOA, &gpio_init_structure);
      /* RxD */
      gpio_init_structure.GPIO_Pin = GPIO_Pin_3;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init(GPIOB, &gpio_init_structure);
      /* AHB peripheral clocks */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
      /* DMA */
      if (Uart->Ctrl->DmaBufSize)
      {
        DMA_StructInit(&dma_init_structure);
        dma_init_structure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;
        dma_init_structure.DMA_MemoryBaseAddr = (u32)Uart->Ctrl->DmaBufPtr;
        dma_init_structure.DMA_BufferSize = Uart->Ctrl->DmaBufSize;
        dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        dma_init_structure.DMA_Mode = DMA_Mode_Circular;
        dma_init_structure.DMA_Priority = DMA_Priority_High;
      }
      break;
    #endif

    #ifdef _UART_3_SUPPORT
    case USART3_BASE:
      /* TxD */
      txd_gpio = GPIOB;
      txd_pin = GPIO_Pin_10;
      gpio_init_structure.GPIO_Pin = GPIO_Pin_10;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_PP;
      GPIO_Init(GPIOB, &gpio_init_structure);
      /* RxD */
      gpio_init_structure.GPIO_Pin = GPIO_Pin_11;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init(GPIOB, &gpio_init_structure);
      /* AHB peripheral clocks */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
      /* DMA */
      if (Uart->Ctrl->DmaBufSize)
      {
        DMA_StructInit(&dma_init_structure);
        dma_init_structure.DMA_PeripheralBaseAddr = (u32)&USART3->DR;
        dma_init_structure.DMA_MemoryBaseAddr = (u32)Uart->Ctrl->DmaBufPtr;
        dma_init_structure.DMA_BufferSize = Uart->Ctrl->DmaBufSize;
        dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        dma_init_structure.DMA_Mode = DMA_Mode_Circular;
        dma_init_structure.DMA_Priority = DMA_Priority_High;
      }
      break;
    #endif

    #ifdef _UART_4_SUPPORT
    case UART4_BASE:
      /* TxD */
      txd_gpio = GPIOC;
      txd_pin = GPIO_Pin_10;
      gpio_init_structure.GPIO_Pin = GPIO_Pin_10;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_PP;
      GPIO_Init(GPIOC, &gpio_init_structure);
      /* RxD */
      gpio_init_structure.GPIO_Pin = GPIO_Pin_11;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init(GPIOC, &gpio_init_structure);
      /* AHB peripheral clocks */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
      /* DMA */
      if (Uart->Ctrl->DmaBufSize)
      {
        DMA_StructInit(&dma_init_structure);
        dma_init_structure.DMA_PeripheralBaseAddr = (u32)&UART4->DR;
        dma_init_structure.DMA_MemoryBaseAddr = (u32)Uart->Ctrl->DmaBufPtr;
        dma_init_structure.DMA_BufferSize = Uart->Ctrl->DmaBufSize;
        dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        dma_init_structure.DMA_Mode = DMA_Mode_Circular;
        dma_init_structure.DMA_Priority = DMA_Priority_High;
      }
      break;
    #endif

    #ifdef _UART_5_SUPPORT
    case UART5_BASE:
      /* TxD */
      txd_gpio = GPIOC;
      txd_pin = GPIO_Pin_12;
      gpio_init_structure.GPIO_Pin = GPIO_Pin_12;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_PP;
      GPIO_Init(GPIOC, &gpio_init_structure);
      /* RxD */
      gpio_init_structure.GPIO_Pin = GPIO_Pin_2;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init(GPIOD, &gpio_init_structure);
      /* AHB peripheral clocks */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
      /* DMA */
      if (Uart->Ctrl->DmaBufSize) while (1);
      break;
    #endif

    default:
      while (1);
  }

  USART_DeInit(Uart->UARTx);
  USART_StructInit(&usart_init_structure);
  usart_init_structure.USART_BaudRate = BaudRate;
  if (Uart->Ctrl->HwCtrl & UART_HW_FLOW_CTRL_TX) usart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_CTS;
  else usart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(Uart->UARTx, &usart_init_structure);
  USART_ITConfig(Uart->UARTx, UART_IT_ALL, DISABLE);
  if (Uart->Ctrl->DmaBufSize)
  {
    DMA_Init(Uart->DMAx, &dma_init_structure);
    Uart->Ctrl->iDma = Uart->Ctrl->DmaBufSize;
    DMA_ITConfig(Uart->DMAx, DMA_IT_TC | DMA_IT_HT, ENABLE);
    DMA_Cmd(Uart->DMAx, ENABLE);
    NVIC_EnableIRQ(Uart->DMAx_IRQn);
    USART_DMACmd(Uart->UARTx, USART_DMAReq_Rx, ENABLE);
    USART_ITConfig(Uart->UARTx, USART_IT_IDLE, ENABLE);
  }
  else USART_ITConfig(Uart->UARTx, USART_IT_RXNE, ENABLE);
  if (Uart->Ctrl->HwCtrl & (UART_HW_FLOW_CTRL_RX | UART_HALF_DUPLEX)) gpio_set_mode(Uart->Ctrl->Gpio, GPIO_Mode_Out_PP, 0);
  USART_ITConfig(Uart->UARTx, USART_IT_TC, ENABLE);
  NVIC_EnableIRQ(Uart->IRQn);
  USART_Cmd(Uart->UARTx, ENABLE);
  while (!GPIO_ReadInputDataBit(txd_gpio, txd_pin));
  return(0);
}

/*******************************************************************************
* Function Name  : uart_close
* Description    : Close the USART port communication
* Input          : - Uart: Select the USART or the UART peripheral.
* Output         : None
* Return         : 0
*******************************************************************************/
int uart_close(const _Uart_Descriptor *Uart)
{
  GPIO_InitTypeDef gpio_init_structure;

  USART_DeInit(Uart->UARTx);
  NVIC_DisableIRQ(Uart->IRQn);
  if (Uart->Ctrl->DmaBufSize)
  {
    DMA_Cmd(Uart->DMAx, DISABLE);
    NVIC_DisableIRQ(Uart->DMAx_IRQn);
    USART_DMACmd(Uart->UARTx, USART_DMAReq_Rx, DISABLE);
  }
  if ((Uart->Ctrl->HwCtrl & UART_HW_FLOW_CTRL_RX) || (Uart->Ctrl->HwCtrl & UART_HALF_DUPLEX)) gpio_init(Uart->Ctrl->Gpio);
  _free(Uart->Ctrl->DmaBufPtr);
  memset(Uart->Ctrl, 0, sizeof(_Uart_Ctrl));

  switch (*(u32 *)&Uart->UARTx)
  {
    #ifdef _UART_1_SUPPORT
    case USART1_BASE:
      /* TxD - RxD */
      gpio_init_structure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init(GPIOA, &gpio_init_structure);
      /* AHB peripheral clocks */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
      break;
    #endif

    #ifdef _UART_2_SUPPORT
    case USART2_BASE:
      /* TxD - RxD */
      gpio_init_structure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init(GPIOA, &gpio_init_structure);
      /* AHB peripheral clocks */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
      break;
    #endif

    #ifdef _UART_3_SUPPORT
    case USART3_BASE:
      /* TxD - RxD */
      gpio_init_structure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init(GPIOB, &gpio_init_structure);
      /* AHB peripheral clocks */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, DISABLE);
      break;
    #endif

    #ifdef _UART_4_SUPPORT
    case UART4_BASE:
      /* TxD - RxD */
      gpio_init_structure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init(GPIOB, &gpio_init_structure);
      /* AHB peripheral clocks */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, DISABLE);
      break;
    #endif

    #ifdef _UART_5_SUPPORT
    case UART5_BASE:
      /* TxD */
      gpio_init_structure.GPIO_Pin = GPIO_Pin_12;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init(GPIOC, &gpio_init_structure);
      /* RxD */
      gpio_init_structure.GPIO_Pin = GPIO_Pin_2;
      gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
      gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_Init(GPIOD, &gpio_init_structure);
      /* AHB peripheral clocks */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, DISABLE);
      break;
    #endif

    default:
      while (1);
  }
  return(0);
}

/*******************************************************************************
* Function Name  : uart_irq_handler
* Description    : Interrupt handle function
* Input          : - Uart: Select the USART or the UART peripheral.
* Output         : None
* Return         : None
*******************************************************************************/
void uart_irq_handler(const _Uart_Descriptor *Uart)
{
  if (Uart->Ctrl->DmaBufSize)
  {
    if (USART_GetITStatus(Uart->UARTx, USART_IT_IDLE) != RESET)                          // Idle line
    {
      NVIC_SetPendingIRQ(Uart->DMAx_IRQn);
      USART_ReceiveData(Uart->UARTx);
    }
  }
  else
  {
    if (USART_GetITStatus(Uart->UARTx, USART_IT_RXNE) != RESET)                          // Received Data Ready to be Read
    {
      if (Uart->Ctrl->RxCnt < Uart->Ctrl->RxBufSize)
      {
        Uart->Ctrl->RxBufPtr[Uart->Ctrl->RxiPut++] = (char)USART_ReceiveData(Uart->UARTx);
        Uart->Ctrl->RxCnt++;
        if (Uart->Ctrl->RxiPut >= Uart->Ctrl->RxBufSize) Uart->Ctrl->RxiPut = 0;
        if ((Uart->Ctrl->HwCtrl & UART_HW_FLOW_CTRL_RX) && (Uart->Ctrl->RxCnt > (Uart->Ctrl->RxBufSize - Uart->Ctrl->DmaBufSize)) && (!UART_IS_RX_HFC_ON(Uart))) UART_RX_HFC_ON(Uart);
      }
      else USART_ClearITPendingBit(Uart->UARTx, USART_IT_RXNE);
      #ifdef _UART_OS_SUPPORT
        isr_evt_set(Uart->Ctrl->Event, Uart->Ctrl->Task);
      #endif
    }
  }

  if (USART_GetITStatus(Uart->UARTx, USART_IT_TXE) != RESET)                             // Transmit Data Register Empty
  {
    if (Uart->Ctrl->TxCnt)
    {
      USART_SendData(Uart->UARTx, Uart->Ctrl->TxBufPtr[Uart->Ctrl->TxiGet++]);
      Uart->Ctrl->TxCnt--;
      if (Uart->Ctrl->TxiGet >= Uart->Ctrl->TxBufSize) Uart->Ctrl->TxiGet = 0;
    }
    else USART_ITConfig(Uart->UARTx, USART_IT_TXE, DISABLE);
  }

  if (USART_GetITStatus(Uart->UARTx, USART_IT_TC) != RESET)                              // Transmission complete
  {
    if (Uart->Ctrl->HwCtrl & UART_HALF_DUPLEX) UART_TX_DISABLE(Uart);
    Uart->Ctrl->HwCtrl &= ~UART_TX_IN_PROGRESS;
    USART_ClearITPendingBit(Uart->UARTx, USART_IT_TC);
  }
}

/*******************************************************************************
* Function Name  : uart_dma_irq_handler
* Description    : DMA interrupt handle function
* Input          : - Uart: Select the USART or the UART peripheral.
* Output         : None
* Return         : None
*******************************************************************************/
void uart_dma_irq_handler(const _Uart_Descriptor *Uart)
{
  while ((DMA_GetCurrDataCounter(Uart->DMAx) != Uart->Ctrl->iDma) && (Uart->Ctrl->RxCnt < Uart->Ctrl->RxBufSize))
  {
    Uart->Ctrl->RxBufPtr[Uart->Ctrl->RxiPut++] = Uart->Ctrl->DmaBufPtr[Uart->Ctrl->DmaBufSize - Uart->Ctrl->iDma--];
    if (!Uart->Ctrl->iDma) Uart->Ctrl->iDma = Uart->Ctrl->DmaBufSize;
    Uart->Ctrl->RxCnt++;
    if (Uart->Ctrl->RxiPut >= Uart->Ctrl->RxBufSize) Uart->Ctrl->RxiPut = 0;
    if ((Uart->Ctrl->HwCtrl & UART_HW_FLOW_CTRL_RX) && (Uart->Ctrl->RxCnt > (Uart->Ctrl->RxBufSize - Uart->Ctrl->DmaBufSize)) && (!UART_IS_RX_HFC_ON(Uart))) UART_RX_HFC_ON(Uart);
  }
  #ifdef _UART_OS_SUPPORT
    isr_evt_set(Uart->Ctrl->Event, Uart->Ctrl->Task);
  #endif
}

/*******************************************************************************
* Function Name  : uart_get_char
* Description    : Get a single data byte from the USARTx RxBuffer.
* Input          : - Uart: Select the USART or the UART peripheral.
* Output         : None
* Return         : The data read from the buffer (-1 if the RxBuffer is empty)
*******************************************************************************/
int uart_get_char(const _Uart_Descriptor *Uart)
{
  char data;

  #ifdef _UART_OS_SUPPORT
	  //phlb modif
    //if ((sys_task_self() != Uart->Ctrl->Task) || !Uart->Ctrl->RxCnt) return(-1);
		if (!Uart->Ctrl->RxCnt) return(-1);
  #else
    if (!Uart->Ctrl->RxCnt) return(-1);
  #endif
  if (Uart->Ctrl->DmaBufSize) NVIC_DisableIRQ(Uart->DMAx_IRQn);
  else USART_ITConfig(Uart->UARTx, USART_IT_RXNE, DISABLE);
  data = Uart->Ctrl->RxBufPtr[Uart->Ctrl->RxiGet++];
  Uart->Ctrl->RxCnt--;
  if (Uart->Ctrl->RxiGet >= Uart->Ctrl->RxBufSize) Uart->Ctrl->RxiGet = 0;
  if ((Uart->Ctrl->HwCtrl & UART_HW_FLOW_CTRL_RX) && (UART_IS_RX_HFC_ON(Uart)) && (Uart->Ctrl->RxCnt < (Uart->Ctrl->RxBufSize / 2))) UART_RX_HFC_OFF(Uart);
  if (Uart->Ctrl->DmaBufSize) NVIC_EnableIRQ(Uart->DMAx_IRQn);
  else USART_ITConfig(Uart->UARTx, USART_IT_RXNE, ENABLE);
  return((int)data);
}

/*******************************************************************************
* Function Name  : uart_put_char
* Description    : Put a single data byte to the USARTx TxBuffer.
* Input          : - Data: the data to transmit.
*                  - Uart: Select the USART or the UART peripheral.
* Output         : None
* Return         : 0 if OK, -1 if ERROR
*******************************************************************************/
int uart_put_char(char Data, const _Uart_Descriptor *Uart)
{
  if (!Uart->Ctrl->TxBufPtr) return(-1);
  while (Uart->Ctrl->TxCnt == Uart->Ctrl->TxBufSize)
  {
    #ifdef _UART_OS_SUPPORT
      sys_wait(1);
    #endif
  }
  USART_ITConfig(Uart->UARTx, USART_IT_TXE, DISABLE);
  Uart->Ctrl->TxBufPtr[Uart->Ctrl->TxiPut++] = Data;
  Uart->Ctrl->TxCnt++;
  if (Uart->Ctrl->TxiPut >= Uart->Ctrl->TxBufSize) Uart->Ctrl->TxiPut = 0;
  if (Uart->Ctrl->HwCtrl & UART_HALF_DUPLEX) UART_TX_ENABLE(Uart);
  Uart->Ctrl->HwCtrl |= UART_TX_IN_PROGRESS;
  USART_ITConfig(Uart->UARTx, USART_IT_TXE, ENABLE);
  return(0);
}

/*******************************************************************************
* Function Name  : uart_read
* Description    : Read data bytes from the USARTx RxBuffer.
* Input          : - WrPtr: Location where the first byte is to be stored
*                : - Size: Number of bytes to be read
*                : - Uart: Select the USART or the UART peripheral.
* Output         : None
* Return         : Number of bytes read from the UARTx RxBuffer (-1 if the number
*                  of bytes to be read superior to the number of bytes available)
*******************************************************************************/
int uart_read(void *WrPtr, u16 Size, const _Uart_Descriptor *Uart)
{
  u16 i;
  char *ptr;
  int ch;

  if (Size > Uart->Ctrl->RxCnt) return(-1);
  ptr = (char *)WrPtr;
  for (i = 0 ; i < Size ; i++)
  {
    if ((ch = uart_get_char(Uart)) == -1) break;
    *ptr++ = (char)ch;
  }
  return((int)(Size - i));
}

/*******************************************************************************
* Function Name  : uart_write
* Description    : Write data bytes to the USARTx TxBuffer.
* Input          : - RdPtr: Location where the first byte is to be read
*                : - Size: Number of bytes to be written
*                : - Uart: Select the USART or the UART peripheral.
* Output         : None
* Return         : -1 if ERROR, Number of char not written (0 if OK)
*******************************************************************************/
int uart_write(const void *RdPtr, u16 Size, const _Uart_Descriptor *Uart)
{
  u16 i;
  char *ptr = (char *)RdPtr;

  for (i = 0 ; i < Size ; i++) if (uart_put_char(*ptr++, Uart) == -1) break;
  return((int)(Size - i));
}

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
