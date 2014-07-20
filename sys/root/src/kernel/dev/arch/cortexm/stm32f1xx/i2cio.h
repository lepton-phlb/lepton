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
#ifndef __I2CIO_H
#define __I2CIO_H

/* Includes ------------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef __packed struct
{
  u8 I2cAddr;
  u32 MaxFreq;
  u8 ChannelCnt;
} _I2cio_Descriptor;

enum {
  I2CIO_REG_INPUT,
  I2CIO_REG_OUTPUT,
  I2CIO_REG_POLARITY,
  I2CIO_REG_CONFIG,
  I2CIO_REG_NB
  };

enum {
  I2CIO_MODE_OUTPUT,
  I2CIO_MODE_INPUT
  };

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const _I2cio_Descriptor I2cio_Descriptor[];

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void i2cio_set_mode(const _I2cio_Descriptor *I2cio, u16 Pin, u8 Mode, u8 Val);
u16 i2cio_read_register(const _I2cio_Descriptor *I2cio, u8 Reg);
u8 i2cio_read_register_bit(const _I2cio_Descriptor *I2cio, u8 Reg, u16 Pin);
void i2cio_set_output(const _I2cio_Descriptor *I2cio, u16 Pin, u8 Val);
void i2cio_toggle_output(const _I2cio_Descriptor *I2cio, u16 Pin);

#endif /* __I2CIO_H */
