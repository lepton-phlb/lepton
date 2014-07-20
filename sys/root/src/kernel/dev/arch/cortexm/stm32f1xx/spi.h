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
#ifndef __SPI_H
#define __SPI_H

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* Exported types ------------------------------------------------------------*/
typedef __packed struct
{
  SPI_TypeDef *SPIx;
  void (*RCC_APBxPeriphClockCmd)(uint32_t, FunctionalState);
  u32 RCC_APBxPeriph;
  const _Gpio_Descriptor *MISO;
  const _Gpio_Descriptor *MOSI;
  const _Gpio_Descriptor *SCK;
  u8 Mode;
  u32 MaxFreq;
  
  //lepton spi_info
   struct board_stm32f1xx_spi_info_st* board_spi_info;
   
} _Spi_Descriptor;

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const _Spi_Descriptor Spi_Descriptor[];

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void spi_open(const _Spi_Descriptor *Spi);
void spi_close(const _Spi_Descriptor *Spi);
char spi_rw_char(const _Spi_Descriptor *Spi, char Data);
u32 spi_rw_long(const _Spi_Descriptor *Spi, u32 Data);
#define spi_read_char(Spi)          (spi_rw_char(Spi, 0xFF))
#define spi_write_char(Spi, Data)   ((void)(spi_rw_char(Spi, Data)))
#define spi_read_long(Spi)          (spi_rw_long(Spi, 0xFFFFFFFF))
#define spi_write_long(Spi, Data)   ((void)(spi_rw_long(Spi, Data)))

#endif /* __SPI_H */

