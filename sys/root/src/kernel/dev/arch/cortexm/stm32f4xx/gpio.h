/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : gpio.h
* Author             : Yoann TREGUIER
* Version            : 1.1.0
* Date               : 2013/01/23
* Description        : GPIO definitions and GPIO manipulation macros
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H
#define __GPIO_H

/* Includes ------------------------------------------------------------------*/
#ifdef _GPIO_I2CIO_SUPPORT
  #include "i2cio.h"
#endif

/* Exported define -----------------------------------------------------------*/
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
  GPIOMode_TypeDef DefMode;
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
void gpio_set_mode(const _Gpio_Descriptor *Gpio, GPIOMode_TypeDef Mode, u8 Val);
void gpio_init(const _Gpio_Descriptor *Gpio);
void gpio_set(const _Gpio_Descriptor *Gpio);
void gpio_reset(const _Gpio_Descriptor *Gpio);
void gpio_toggle(const _Gpio_Descriptor *Gpio);
u8 gpio_read(const _Gpio_Descriptor *Gpio);
u8 gpio_read_output(const _Gpio_Descriptor *Gpio);
void gpio_set_port_mode(const _Gpio_Descriptor *Gpio, GPIOMode_TypeDef Mode);
void gpio_set_port_mode(const _Gpio_Descriptor *Gpio, GPIOMode_TypeDef Mode);
u16 gpio_read_port(const _Gpio_Descriptor *Gpio);
void gpio_enable_ext_int(const _Gpio_ExtInt_Descriptor *ExtInt);
void gpio_disable_ext_int(const _Gpio_ExtInt_Descriptor *ExtInt);

#endif /* __GPIO_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
