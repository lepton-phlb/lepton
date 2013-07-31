/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : eeprom.h
* Author             : Yoann TREGUIER
* Version            : 1.1.2
* Date               : 2013/04/12
* Description        : Headers of EEPROM driver functions
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_H
#define __EEPROM_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef __packed struct
{
  u8 I2cAddr;
  u32 MaxFreq;
  u32 MemSize;
} _Eeprom_Descriptor;

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const _Eeprom_Descriptor Eeprom_Descriptor[];

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int eeprom_read(const _Eeprom_Descriptor *Eeprom, u8 Addr, char *Ptr, u8 Size);
int eeprom_write(const _Eeprom_Descriptor *Eeprom, u8 Addr, const char *Ptr, u8 Size);

#endif /* __EEPROM_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
