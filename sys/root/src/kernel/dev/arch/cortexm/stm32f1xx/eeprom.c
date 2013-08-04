/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : eeprom.c
* Author             : Yoann TREGUIER
* Version            : 1.1.2
* Date               : 2013/04/12
* Description        : EEPROM driver functions
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "kernel/core/ucore/cmsis/Device/st/stm32f1xx/stm32f10x.h"
#include "target.h"
#include "i2c.h"
#include "eeprom.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* eeprom_read
*******************************************************************************/
int eeprom_read(const _Eeprom_Descriptor *Eeprom, u8 Addr, char *Ptr, u8 Size)
{
  int ret = -1;

  if ((Addr + Size) > Eeprom->MemSize) return(-1);
  if (i2c_open(Eeprom->MaxFreq) == -1) return(-1);
  if (i2c_transmit(Eeprom->I2cAddr, (const char *)&Addr, 1, 0) != -1)
  {
    i2c_receive(Eeprom->I2cAddr, Ptr, Size);
    ret = Size;
  }
  i2c_close();
  return(ret);
}

/*******************************************************************************
* eeprom_write
*******************************************************************************/
int eeprom_write(const _Eeprom_Descriptor *Eeprom, u8 Addr, const char *Ptr, u8 Size)
{
  int ret = -1;
  char tab[2];

  if ((Addr + Size) > Eeprom->MemSize) return(-1);
  if (i2c_open(Eeprom->MaxFreq) == -1) return(-1);
  tab[0] = Addr;
  tab[1] = *Ptr++;
  Size--;
  if (i2c_transmit(Eeprom->I2cAddr, tab, 2, 1) != -1)
  {
    while (Size--)
    {
      tab[0]++;
      tab[1] = *Ptr++;
      while (i2c_transmit(Eeprom->I2cAddr, tab, 2, 1) == -1);
    }
    while (i2c_transmit(Eeprom->I2cAddr, 0, 0, 1) == -1);
    ret = 0;
  }
  i2c_close();
  return(ret);
}

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
