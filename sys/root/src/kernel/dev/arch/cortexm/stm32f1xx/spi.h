/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : spi.h
* Author             : Yoann TREGUIER
* Version            : 1.2.0
* Date               : 2013/06/14
* Description        : Headers of SPI driver functions for STM32F1xx devices
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H
#define __SPI_H

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* Exported types ------------------------------------------------------------*/
typedef __packed struct
{
  SPI_TypeDef *SPIx;
  void (*RCC_APBxPeriphClockCmd)(uint32_t, FunctionalState);
  u32 RCC_APBxPeriph;
  const _Gpio_Descriptor *MISO;
  const _Gpio_Descriptor *MOSI;
  const _Gpio_Descriptor *SCK;
  u8 Mode;
  u32 MaxFreq;
  
  //lepton spi_info
   struct board_stm32f1xx_spi_info_st* board_spi_info;
   
} _Spi_Descriptor;

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const _Spi_Descriptor Spi_Descriptor[];

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void spi_open(const _Spi_Descriptor *Spi);
void spi_close(const _Spi_Descriptor *Spi);
char spi_rw_char(const _Spi_Descriptor *Spi, char Data);
u32 spi_rw_long(const _Spi_Descriptor *Spi, u32 Data);
#define spi_read_char(Spi)          (spi_rw_char(Spi, 0xFF))
#define spi_write_char(Spi, Data)   ((void)(spi_rw_char(Spi, Data)))
#define spi_read_long(Spi)          (spi_rw_long(Spi, 0xFFFFFFFF))
#define spi_write_long(Spi, Data)   ((void)(spi_rw_long(Spi, Data)))

#endif /* __SPI_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
