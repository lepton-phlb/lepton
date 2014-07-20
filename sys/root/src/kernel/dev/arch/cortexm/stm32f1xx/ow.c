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
