/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : i2cio.c
* Author             : Yoann TREGUIER
* Version            : 1.1.2
* Date               : 2013/04/12
* Description        : EEPROM driver functions
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "target.h"
#include "i2c.h"
#include "i2cio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* i2cio_set_mode
*******************************************************************************/
void i2cio_set_mode(const _I2cio_Descriptor *I2cio, u16 Pin, u8 Mode, u8 Val)
{
  __packed union
  {
    char tab[3];
    __packed struct
    {
      char reg;
      u16 mode;
    };
  } buf;

  if (i2c_open(I2cio->MaxFreq) == -1) return;
  buf.reg = I2CIO_REG_CONFIG * I2cio->ChannelCnt;
  if (i2c_transmit(I2cio->I2cAddr, buf.tab, 1, 0) != -1)
  {
    i2c_receive(I2cio->I2cAddr, (char *)&buf.mode, I2cio->ChannelCnt);
    if (Mode == I2CIO_MODE_INPUT) buf.mode |= Pin;
    else buf.mode &= ~Pin;
    i2c_transmit(I2cio->I2cAddr, buf.tab, I2cio->ChannelCnt + 1, 1);
    if (Mode == I2CIO_MODE_OUTPUT)
    {
      buf.reg = I2CIO_REG_OUTPUT * I2cio->ChannelCnt;
      i2c_transmit(I2cio->I2cAddr, buf.tab, 1, 0);
      i2c_receive(I2cio->I2cAddr, (char *)&buf.mode, I2cio->ChannelCnt);
      if (Val) buf.mode |= Pin;
      else buf.mode &= ~Pin;
      i2c_transmit(I2cio->I2cAddr, buf.tab, I2cio->ChannelCnt + 1, 1);
    }
  }
  i2c_close();
}

/*******************************************************************************
* i2cio_read_register
*******************************************************************************/
u16 i2cio_read_register(const _I2cio_Descriptor *I2cio, u8 Reg)
{
  u16 reg_val = 0;
  char reg = Reg * I2cio->ChannelCnt;

  if (i2c_open(I2cio->MaxFreq) == -1) return(0);
  if (i2c_transmit(I2cio->I2cAddr, &reg, 1, 0) != -1) i2c_receive(I2cio->I2cAddr, (char *)&reg_val, I2cio->ChannelCnt);
  i2c_close();
  return(reg_val);
}

/*******************************************************************************
* i2cio_read_register_bit
*******************************************************************************/
u8 i2cio_read_register_bit(const _I2cio_Descriptor *I2cio, u8 Reg, u16 Pin)
{
  u16 reg_val;

  reg_val = i2cio_read_register(I2cio, Reg);
  if ((reg_val & Pin) != 0) return(1);
  return(0);
}

/*******************************************************************************
* i2cio_set_output
*******************************************************************************/
void i2cio_set_output(const _I2cio_Descriptor *I2cio, u16 Pin, u8 Val)
{
  __packed union
  {
    char tab[3];
    __packed struct
    {
      char reg;
      u16 output;
    };
  } buf;

  if (i2c_open(I2cio->MaxFreq) == -1) return;
  buf.reg = I2CIO_REG_OUTPUT * I2cio->ChannelCnt;
  if (i2c_transmit(I2cio->I2cAddr, buf.tab, 1, 0) != -1)
  {
    i2c_receive(I2cio->I2cAddr, (char *)&buf.output, I2cio->ChannelCnt);
    if (Val) buf.output |= Pin;
    else buf.output &= ~Pin;
    i2c_transmit(I2cio->I2cAddr, buf.tab, I2cio->ChannelCnt + 1, 1);
  }
  i2c_close();
}

/*******************************************************************************
* i2cio_toggle_output
*******************************************************************************/
void i2cio_toggle_output(const _I2cio_Descriptor *I2cio, u16 Pin)
{
  __packed union
  {
    char tab[3];
    __packed struct
    {
      char reg;
      u16 output;
    };
  } buf;

  if (i2c_open(I2cio->MaxFreq) == -1) return;
  buf.reg = I2CIO_REG_OUTPUT * I2cio->ChannelCnt;
  if (i2c_transmit(I2cio->I2cAddr, buf.tab, 1, 0) != -1)
  {
    i2c_receive(I2cio->I2cAddr, (char *)&buf.output, I2cio->ChannelCnt);
    buf.output ^= Pin;
    i2c_transmit(I2cio->I2cAddr, buf.tab, I2cio->ChannelCnt + 1, 1);
  }
  i2c_close();
}

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
