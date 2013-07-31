/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : i2cio.h
* Author             : Yoann TREGUIER
* Version            : 1.1.2
* Date               : 2013/04/12
* Description        : Headers of EEPROM driver functions
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2CIO_H
#define __I2CIO_H

/* Includes ------------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef __packed struct
{
  u8 I2cAddr;
  u32 MaxFreq;
  u8 ChannelCnt;
} _I2cio_Descriptor;

enum {
  I2CIO_REG_INPUT,
  I2CIO_REG_OUTPUT,
  I2CIO_REG_POLARITY,
  I2CIO_REG_CONFIG,
  I2CIO_REG_NB
  };

enum {
  I2CIO_MODE_OUTPUT,
  I2CIO_MODE_INPUT
  };

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const _I2cio_Descriptor I2cio_Descriptor[];

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void i2cio_set_mode(const _I2cio_Descriptor *I2cio, u16 Pin, u8 Mode, u8 Val);
u16 i2cio_read_register(const _I2cio_Descriptor *I2cio, u8 Reg);
u8 i2cio_read_register_bit(const _I2cio_Descriptor *I2cio, u8 Reg, u16 Pin);
void i2cio_set_output(const _I2cio_Descriptor *I2cio, u16 Pin, u8 Val);
void i2cio_toggle_output(const _I2cio_Descriptor *I2cio, u16 Pin);

#endif /* __I2CIO_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
