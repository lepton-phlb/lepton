/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : ow.c
* Author             : Yoann TREGUIER
* Version            : 1.2.0
* Date               : 2013/06/14
* Description        : One-Wire driver functions
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "target.h"
#include "misc.h"
#include "gpio.h"
#include "crc.h"
#include "ow.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define INT_DISABLE()   __disable_irq()
#define INT_ENABLE()    __enable_irq()

#define DQ_HI(Gpio)   gpio_set(Gpio)
#define DQ_LO(Gpio)   gpio_reset(Gpio)
#define DQ_VAL(Gpio)  gpio_read(Gpio)

#define OW_DELAY_6()    sys_delay(6)
#define OW_DELAY_9()    sys_delay(9)
#define OW_DELAY_10()   sys_delay(10)
#define OW_DELAY_55()   sys_delay(55)
#define OW_DELAY_60()   sys_delay(60)
#define OW_DELAY_64()   sys_delay(64)
#define OW_DELAY_70()   sys_delay(70)
#define OW_DELAY_410()  sys_delay(410)
#define OW_DELAY_480()  sys_delay(480)

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ow_reset
*******************************************************************************/
u8 ow_reset(const _Gpio_Descriptor *Gpio)
{
  u8 val;

  INT_DISABLE();
  DQ_LO(Gpio);
  OW_DELAY_480();
  DQ_HI(Gpio);
  OW_DELAY_70();
  val = !DQ_VAL(Gpio);
  OW_DELAY_410();
  INT_ENABLE();
  return(val);    //1 = Dectected / 0 = Not detected
}

/*******************************************************************************
* ow_read_bit
*******************************************************************************/
u8 ow_read_bit(const _Gpio_Descriptor *Gpio)
{
  u8 val;

  DQ_LO(Gpio);
  OW_DELAY_6();
  DQ_HI(Gpio);
  OW_DELAY_9();
  val = DQ_VAL(Gpio);
  OW_DELAY_55();
  return(val);
}

/*******************************************************************************
* ow_write_bit
*******************************************************************************/
void ow_write_bit(const _Gpio_Descriptor *Gpio, u8 Val)
{
  DQ_LO(Gpio);
  if (Val) OW_DELAY_6();
  else OW_DELAY_60();
  DQ_HI(Gpio);
  if (Val) OW_DELAY_64();
  else OW_DELAY_10();
}

/*******************************************************************************
* ow_read_byte
*******************************************************************************/
u8 ow_read_byte(const _Gpio_Descriptor *Gpio)
{
  u8 i;
  u8 val = 0;

  INT_DISABLE();
  for (i = 0 ; i < 8 ; i++) if (ow_read_bit(Gpio)) val |= 1 << i;
  INT_ENABLE();
  return(val);
}

/*******************************************************************************
* ow_write_byte
*******************************************************************************/
void ow_write_byte(const _Gpio_Descriptor *Gpio, u8 Val)
{
  u8 i;

  INT_DISABLE();
  for (i = 0 ; i < 8 ; i++) ow_write_bit(Gpio, (Val >> i) & 0x01);
  INT_ENABLE();
}

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
