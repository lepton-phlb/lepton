/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Philippe Le Boulanger.
Portions created by Philippe Le Boulanger are Copyright (C) 2014 <lepton.phlb@gmail.com>.
All Rights Reserved.

Contributor(s): Jean-Jacques Pitrolle <lepton.jjp@gmail.com>.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting  the provisions above and replace
them with the notice and other provisions required by the [eCos GPL] License.
If you do not delete the provisions above, a recipient may use your version of this file under
either the MPL or the [eCos GPL] License."
*/


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

#define GPIO_PUPD_NONE  0x00000000
#define GPIO_PUPD_UP    0x00010000
#define GPIO_PUPD_DOWN  0x00020000

#define GPIO_SPD_LOW    0x80000000
#define GPIO_SPD_MED    0x81000000
#define GPIO_SPD_FAST   0x82000000
#define GPIO_SPD_HIGH   0x83000000

#define GPIO_MODE_IN      0x00000000
#define GPIO_MODE_IN_PU   0x00010000
#define GPIO_MODE_IN_PD   0x00020000
#define GPIO_MODE_OUT     0x00000001
#define GPIO_MODE_OUT_PP  0x00000001
#define GPIO_MODE_OUT_OD  0x00000101
#define GPIO_MODE_AF      0x00000002
#define GPIO_MODE_AF_PP   0x00000002
#define GPIO_MODE_AF_OD   0x00000102
#define GPIO_MODE_AN      0x00000003

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

