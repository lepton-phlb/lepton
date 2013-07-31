/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : ow.h
* Author             : Yoann TREGUIER
* Version            : 1.2.0
* Date               : 2013/06/14
* Description        : Headers of One-Wire driver functions
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OW_H
#define __OW_H

/* Includes ------------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
u8 ow_reset(const _Gpio_Descriptor *Gpio);
u8 ow_read_bit(const _Gpio_Descriptor *Gpio);
void ow_write_bit(const _Gpio_Descriptor *Gpio, u8 Val);
u8 ow_read_byte(const _Gpio_Descriptor *Gpio);
void ow_write_byte(const _Gpio_Descriptor *Gpio, u8 Val);

#endif /* __OW_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
