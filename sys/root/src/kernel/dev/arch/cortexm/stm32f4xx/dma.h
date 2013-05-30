/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : dma.h
* Author             : Yoann TREGUIER
* Version            : 0.1.0
* Date               : 2013/05/24
* Description        : Headers of DMA driver functions for STM32F4xx devices
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DMA_H
#define __DMA_H

/* Includes ------------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define DMA_IT_ALL_0  (DMA_IT_FEIF0 | DMA_IT_DMEIF0 | DMA_IT_TEIF0 | DMA_IT_HTIF0 | DMA_IT_TCIF0)
#define DMA_IT_ALL_1  (DMA_IT_FEIF1 | DMA_IT_DMEIF1 | DMA_IT_TEIF1 | DMA_IT_HTIF1 | DMA_IT_TCIF1)
#define DMA_IT_ALL_2  (DMA_IT_FEIF2 | DMA_IT_DMEIF2 | DMA_IT_TEIF2 | DMA_IT_HTIF2 | DMA_IT_TCIF2)
#define DMA_IT_ALL_3  (DMA_IT_FEIF3 | DMA_IT_DMEIF3 | DMA_IT_TEIF3 | DMA_IT_HTIF3 | DMA_IT_TCIF3)
#define DMA_IT_ALL_4  (DMA_IT_FEIF4 | DMA_IT_DMEIF4 | DMA_IT_TEIF4 | DMA_IT_HTIF4 | DMA_IT_TCIF4)
#define DMA_IT_ALL_5  (DMA_IT_FEIF5 | DMA_IT_DMEIF5 | DMA_IT_TEIF5 | DMA_IT_HTIF5 | DMA_IT_TCIF5)
#define DMA_IT_ALL_6  (DMA_IT_FEIF6 | DMA_IT_DMEIF6 | DMA_IT_TEIF6 | DMA_IT_HTIF6 | DMA_IT_TCIF6)
#define DMA_IT_ALL_7  (DMA_IT_FEIF7 | DMA_IT_DMEIF7 | DMA_IT_TEIF7 | DMA_IT_HTIF7 | DMA_IT_TCIF7)

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define dma_startup_init()  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 | RCC_AHB1Periph_DMA2, ENABLE)

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#endif /* __DMA_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
