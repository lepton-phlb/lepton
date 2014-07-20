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
#include "kernel/dev/arch/cortexm/stm32f4xx/driverlib/stm32f4xx.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/types.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/target.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/spi.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : spi_open
* Description    : Open the SPI port communication
* Input          : - Spi: Select the SPI peripheral.
* Output         : None
* Return         : None
*******************************************************************************/
void spi_open(const _Spi_Descriptor *Spi)
{
  RCC_ClocksTypeDef rcc_clocks;
  SPI_InitTypeDef spi_init_structure;
  u32 pclk;

  /* Enable peripheral clock */
  (*Spi->RCC_APBxPeriphClockCmd)(Spi->RCC_APBxPeriph, ENABLE);

  /* Init GPIO */
  gpio_set_function(Spi->MISO, Spi->GPIO_AF);
  gpio_set_function(Spi->MOSI, Spi->GPIO_AF);
  gpio_set_function(Spi->SCK, Spi->GPIO_AF);
  gpio_set_mode(Spi->MISO, GPIO_MODE_AF, 0);
  gpio_set_mode(Spi->MOSI, GPIO_MODE_AF, 0);
  gpio_set_mode(Spi->SCK, GPIO_MODE_AF, 0);

  /* Init SPI peripheral */
  SPI_I2S_DeInit(Spi->SPIx);
  SPI_StructInit(&spi_init_structure);
  spi_init_structure.SPI_Mode = SPI_Mode_Master;
  spi_init_structure.SPI_NSS = SPI_NSS_Soft;
    /* Set SPI mode */
  switch (Spi->Mode)
  {
    case 0:
      spi_init_structure.SPI_CPOL = SPI_CPOL_Low;
      spi_init_structure.SPI_CPHA = SPI_CPHA_1Edge;
      break;

    case 1:
      spi_init_structure.SPI_CPOL = SPI_CPOL_Low;
      spi_init_structure.SPI_CPHA = SPI_CPHA_2Edge;
      break;

    case 2:
      spi_init_structure.SPI_CPOL = SPI_CPOL_High;
      spi_init_structure.SPI_CPHA = SPI_CPHA_1Edge;
      break;

    case 3:
      spi_init_structure.SPI_CPOL = SPI_CPOL_High;
      spi_init_structure.SPI_CPHA = SPI_CPHA_2Edge;
      break;

    default:
      return;
  }
    /* Set clock speed */
  RCC_GetClocksFreq(&rcc_clocks);
  if (Spi->RCC_APBxPeriphClockCmd == RCC_APB2PeriphClockCmd) pclk = rcc_clocks.PCLK2_Frequency / 2;
  else pclk = rcc_clocks.PCLK1_Frequency / 2;
  while ((pclk > Spi->MaxFreq) && (spi_init_structure.SPI_BaudRatePrescaler < SPI_BaudRatePrescaler_256))
  {
    pclk /= 2;
    spi_init_structure.SPI_BaudRatePrescaler += SPI_BaudRatePrescaler_4;
  }
  SPI_Init(Spi->SPIx, &spi_init_structure);

  SPI_Cmd(Spi->SPIx, ENABLE);
}

/*******************************************************************************
* Function Name  : spi_close
* Description    : Close the SPI port communication
* Input          : - Spi: Select the SPI peripheral.
* Output         : None
* Return         : None
*******************************************************************************/
void spi_close(const _Spi_Descriptor *Spi)
{
  SPI_I2S_DeInit(Spi->SPIx);

  /* Disable peripheral clock */
  (*Spi->RCC_APBxPeriphClockCmd)(Spi->RCC_APBxPeriph, DISABLE);

  /* Configure GPIO to default state */
  gpio_init(Spi->MISO);
  gpio_init(Spi->MOSI);
  gpio_init(Spi->SCK);
}

/*******************************************************************************
* Function Name  : spi_rw_char
* Description    : Transmit a single byte to the SPI port and return the
*                  received byte
* Input          : - Spi: Select the SPI peripheral.
*                  - Data: The data to transmit.
* Output         : None
* Return         : Data received
*******************************************************************************/
char spi_rw_char(const _Spi_Descriptor *Spi, char Data)
{
  while (SPI_I2S_GetFlagStatus(Spi->SPIx, SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(Spi->SPIx, Data);
  while (SPI_I2S_GetFlagStatus(Spi->SPIx, SPI_I2S_FLAG_RXNE) == RESET);
  return((char)SPI_I2S_ReceiveData(Spi->SPIx));
}

/*******************************************************************************
* Function Name  : spi_rw_long
* Description    : Transmit 4 bytes to the SPI port and return the 4
*                  received bytes
* Input          : - Spi: Select the SPI peripheral.
*                  - Data: The data to transmit.
* Output         : None
* Return         : Data received
*******************************************************************************/
u32 spi_rw_long(const _Spi_Descriptor *Spi, u32 Data)
{
  union
  {
    char tab[4];
    u32 val;
  } data;

  data.tab[3] = spi_rw_char(Spi, (Data >> 24) & 0xFF);
  data.tab[2] = spi_rw_char(Spi, (Data >> 16) & 0xFF);
  data.tab[1] = spi_rw_char(Spi, (Data >> 8) & 0xFF);
  data.tab[0] = spi_rw_char(Spi, Data & 0xFF);
  return(data.val);
}

