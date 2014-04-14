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

