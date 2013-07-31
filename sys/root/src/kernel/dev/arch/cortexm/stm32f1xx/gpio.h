/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : gpio.h
* Author             : Yoann TREGUIER
* Version            : 1.2.0
* Date               : 2013/06/14
* Description        : Headers of GPIO driver functions for STM32F1xx devices
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H
#define __GPIO_H

/* Includes ------------------------------------------------------------------*/
#ifdef _GPIO_I2CIO_SUPPORT
  #include "i2cio.h"
#endif

/* Exported define -----------------------------------------------------------*/
#define GPIO_MODE_AN      0x0000
#define GPIO_MODE_IN      0x0004
#define GPIO_MODE_IN_PD   0x0028
#define GPIO_MODE_IN_PU   0x0048
#define GPIO_MODE_OUT_OD  0x0014
#define GPIO_MODE_OUT     0x0010
#define GPIO_MODE_OUT_PP  0x0010
#define GPIO_MODE_AF      0x0018
#define GPIO_MODE_AF_OD   0x001C
#define GPIO_MODE_AF_PP   0x0018

#define GPIO_SPD_LOW      0x8200
#define GPIO_SPD_MED      0x8100
#define GPIO_SPD_FAST     0x8300
#define GPIO_SPD_HIGH     0x8300

enum {
  GPIO_TYPE_STD,
  GPIO_TYPE_I2C
  };

/* Exported types ------------------------------------------------------------*/
typedef __packed struct
{
  u8 Type;
  const void *Port;
  u16 GPIO_Pin;
  char Init;
  u32 DefMode;
  u8 DefVal;
} _Gpio_Descriptor;

typedef __packed struct
{
  u8 GPIO_Port;
  u8 GPIO_Pin;
  IRQn_Type IRQn;
  u32 EXTI_Line;
  EXTIMode_TypeDef EXTI_Mode;
  EXTITrigger_TypeDef EXTI_Trigger;
} _Gpio_ExtInt_Descriptor;

/* Exported constants --------------------------------------------------------*/
extern const _Gpio_Descriptor Gpio_Descriptor[];
extern const _Gpio_ExtInt_Descriptor Gpio_ExtInt_Descriptor[];

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void gpio_startup_init(void);
void gpio_set_mode(const _Gpio_Descriptor *Gpio, u16 Mode, u8 Val);
void gpio_init(const _Gpio_Descriptor *Gpio);
void gpio_set(const _Gpio_Descriptor *Gpio);
void gpio_reset(const _Gpio_Descriptor *Gpio);
void gpio_toggle(const _Gpio_Descriptor *Gpio);
u8 gpio_read(const _Gpio_Descriptor *Gpio);
u8 gpio_read_output(const _Gpio_Descriptor *Gpio);
void gpio_set_port_mode(const _Gpio_Descriptor *Gpio, u16 Mode);
u16 gpio_read_port(const _Gpio_Descriptor *Gpio);
void gpio_enable_ext_int(const _Gpio_ExtInt_Descriptor *ExtInt);
void gpio_disable_ext_int(const _Gpio_ExtInt_Descriptor *ExtInt);

#endif /* __GPIO_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
