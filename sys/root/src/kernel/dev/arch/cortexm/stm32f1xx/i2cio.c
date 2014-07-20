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
#include "kernel/core/ucore/cmsis/Device/st/stm32f1xx/stm32f10x.h"
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
