/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : gpio.h
* Author             : Yoann TREGUIER
* Version            : 0.1.0
* Date               : 2013/05/24
* Description        : Headers of GPIO driver functions for STM32F4xx devices
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H
#define __GPIO_H

/* Includes ------------------------------------------------------------------*/
#ifdef _GPIO_I2CIO_SUPPORT
  #include "i2cio.h"
#endif

/* Exported define -----------------------------------------------------------*/
#define GPIO_FCT_IN     0x00000000
#define GPIO_FCT_OUT    0x00000001
#define GPIO_FCT_AF     0x00000002
#define GPIO_FCT_AN     0x00000003

#define GPIO_TYPE_PP    0x00000000
#define GPIO_TYPE_OD    0x00000100

#define GPIO_SPD_LOW    0x00800000
#define GPIO_SPD_MED    0x00810000
#define GPIO_SPD_FAST   0x00820000
#define GPIO_SPD_HIGH   0x00830000

#define GPIO_PUPD_NONE  0x00000000
#define GPIO_PUPD_UP    0x01000000
#define GPIO_PUPD_DOWN  0x02000000

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
  u8 Init;
  u32 DefMode;
  u8 DefVal;
} _Gpio_Descriptor;

// typedef __packed struct
// {
//   u8 GPIO_Port;
//   u8 GPIO_Pin;
//   IRQn_Type IRQn;
//   u32 EXTI_Line;
//   EXTIMode_TypeDef EXTI_Mode;
//   EXTITrigger_TypeDef EXTI_Trigger;
// } _Gpio_ExtInt_Descriptor;

/* Exported constants --------------------------------------------------------*/
extern const _Gpio_Descriptor Gpio_Descriptor[];
// extern const _Gpio_ExtInt_Descriptor Gpio_ExtInt_Descriptor[];

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void gpio_startup_init(void);
void gpio_set_mode(const _Gpio_Descriptor *Gpio, u32 Mode, u8 Val);
void gpio_init(const _Gpio_Descriptor *Gpio);
void gpio_set(const _Gpio_Descriptor *Gpio);
void gpio_reset(const _Gpio_Descriptor *Gpio);
void gpio_toggle(const _Gpio_Descriptor *Gpio);
u8 gpio_read(const _Gpio_Descriptor *Gpio);
u8 gpio_read_output(const _Gpio_Descriptor *Gpio);
void gpio_set_function(const _Gpio_Descriptor *Gpio, u8 Function);
// void gpio_set_port_mode(const _Gpio_Descriptor *Gpio, GPIOMode_TypeDef Mode);
// void gpio_set_port_mode(const _Gpio_Descriptor *Gpio, GPIOMode_TypeDef Mode);
// u16 gpio_read_port(const _Gpio_Descriptor *Gpio);
// void gpio_enable_ext_int(const _Gpio_ExtInt_Descriptor *ExtInt);
// void gpio_disable_ext_int(const _Gpio_ExtInt_Descriptor *ExtInt);

#endif /* __GPIO_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
