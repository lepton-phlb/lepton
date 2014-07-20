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
//#include "misc.h"
#include "gpio.h"
#include "i2c.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define I2C_MAX_CLOCK_SPEED(s)  ((s > 400000) ? (400000) : (s))

#define delay(x)  sys_delay(x)

/* Private variables ---------------------------------------------------------*/
#ifdef _I2C_OS_SUPPORT
  OS_MUT I2c_Mut;
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : i2c_check_status
* Description    : Check for the I2C port pins
* Input          : None
* Output         : None
* Return         : -1 if faulty
*******************************************************************************/
int i2c_check_status(void)
{
  int ret = 0;

  #ifdef _I2C_OS_SUPPORT
    /* Wait for the I2C mutex to become available */
    sys_mut_wait(I2c_Mut, T_NO_TIMEOUT);
  #endif

  /* Check for I2C bus status */
  gpio_set_mode(GPIO_SCL, GPIO_MODE_OUT_OD, 0);
  gpio_set_mode(GPIO_SDA, GPIO_MODE_OUT_OD, 0);
  delay(1);
  if (gpio_read(GPIO_SCL) || gpio_read(GPIO_SDA)) ret = -1;
  gpio_set(GPIO_SDA);
  gpio_set(GPIO_SCL);
  delay(5);
  if (!gpio_read(GPIO_SCL) || !gpio_read(GPIO_SDA)) ret = -1;
  gpio_init(GPIO_SCL);
  gpio_init(GPIO_SDA);

  #ifdef _I2C_OS_SUPPORT
    /* Release the I2C mutex */
    sys_mut_release(I2c_Mut);
  #endif

  return(ret);
}

/*******************************************************************************
* Function Name  : i2c_open
* Description    : Open the I2C port communication
* Input          : - ClockSpeed: I2C clock frequency (<= 400000)
* Output         : None
* Return         : -1 if an error occurred
*******************************************************************************/
int i2c_open(u32 ClockSpeed)
{
  GPIO_InitTypeDef gpio_init_structure;
  I2C_InitTypeDef i2c_init_structure;

  #ifdef _I2C_AUTO_CHECK_STATUS
    /* Check for I2C bus status */
    if (i2c_check_status() == -1) return(-1);
  #endif

  #ifdef _I2C_OS_SUPPORT
    /* Wait for the I2C mutex to become available */
    sys_mut_wait(I2c_Mut, T_NO_TIMEOUT);
  #endif

  /* AHB peripheral clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  /* SCL-SDA GPIO */
  gpio_init_structure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &gpio_init_structure);

  /* I2C configuration */
  I2C_DeInit(I2C1);
  I2C_StructInit(&i2c_init_structure);
  i2c_init_structure.I2C_Ack = I2C_Ack_Enable;
  i2c_init_structure.I2C_ClockSpeed = I2C_MAX_CLOCK_SPEED(ClockSpeed);
  I2C_Init(I2C1, &i2c_init_structure);

  /* Enable I2C */
  I2C_Cmd(I2C1, ENABLE);

  #ifdef _I2C_BUS_ERROR_RESET
    /* Reset system if bus is busy (happens if SDL or SDA is stuck low) */
    if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) NVIC_SystemReset();
  #endif

  return(0);
}

/*******************************************************************************
* Function Name  : i2c_close
* Description    : Close the I2C port communication
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void i2c_close(void)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Disable I2C */
  I2C_DeInit(I2C1);

  /* SCL-SDA GPIO */
  gpio_init_structure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &gpio_init_structure);

  /* AHB peripheral clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);

  #ifdef _I2C_OS_SUPPORT
    /* Release the I2C mutex */
    sys_mut_release(I2c_Mut);
  #endif
}

/*******************************************************************************
* Function Name  : i2c_transmit
* Description    : Transmit byte(s)
* Input          : - Addr: Slave address
*                : - Ptr: Pointer to the first byte to be transmitted
*                : - Size: Number of bytes to transmit
*                : - Stop: Indicate if a STOP condition is to be generated
* Output         : None
* Return         : -1 if an error occurred
*******************************************************************************/
int i2c_transmit(u8 Addr, const char *Ptr, u16 Size, u8 Stop)
{
  int ret = 0;

  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress(I2C1,Addr, I2C_Direction_Transmitter);
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) && !I2C_GetFlagStatus(I2C1, I2C_FLAG_AF));
  if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF))
  {
    I2C_ClearFlag(I2C1, I2C_FLAG_AF);
    ret = -1;
  }
  else if (Size)
  {
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    while (Size--)
    {
      while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE));
      I2C_SendData(I2C1, *Ptr++);
    }
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  }
  if (Stop || (ret == -1))
  {
    I2C_GenerateSTOP(I2C1, ENABLE);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
  }
  return(ret);
}

/*******************************************************************************
* Function Name  : i2c_receive
* Description    : Receive byte(s)
* Input          : - Addr: Slave address
*                : - Ptr: Pointer to the first byte to be receiver
*                : - Size: Number of bytes to receive
* Output         : None
* Return         : -1 if an error occurred
*******************************************************************************/
int i2c_receive(u8 Addr, char *Ptr, u16 Size)
{
  int ret = 0;
  __IO uint32_t temp;

  if (!Size) return(-1);
  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress(I2C1, Addr, I2C_Direction_Receiver);
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) && !I2C_GetFlagStatus(I2C1, I2C_FLAG_AF));
  if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF))
  {
    I2C_ClearFlag(I2C1, I2C_FLAG_AF);
    I2C_GenerateSTOP(I2C1, ENABLE);
    ret = -1;
  }
  else if (Size == 1)
  {
    /* Clear ACK */
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    __disable_irq();
    /* Clear ADDR flag */
    temp = I2C1->SR2;
    /* Program the STOP */
    I2C_GenerateSTOP(I2C1, ENABLE);
    __enable_irq();
    while ((I2C_GetLastEvent(I2C1) & 0x0040) != 0x000040); /* Poll on RxNE */
    /* Read the data */
    *Ptr = I2C_ReceiveData(I2C1);
    /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
    while ((I2C1->CR1&0x200) == 0x200);
    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2C1, ENABLE);
  }
  else if (Size == 2)
  {
    I2C1->CR1 = 0xC01; /* ACK=1; POS =1 */
    __disable_irq();
    /* Clear ADDR */
    temp = I2C1->SR2;
    /* Disable ACK */
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    __enable_irq();
    while ((I2C_GetLastEvent(I2C1) & 0x0004) != 0x00004); /* Poll on BTF */
     __disable_irq();
    /* Program the STOP */
    I2C_GenerateSTOP(I2C1, ENABLE);
    /* Read first data */
    *Ptr++ = I2C1->DR;
    /* Read second data */
    *Ptr = I2C1->DR;
    __enable_irq();
    I2C1->CR1 = 0x0401; /* POS = 0, ACK = 1, PE = 1 */
  }
  else
  {
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    /* While there is data to be read; here the safe procedure is implemented */
    while (Size > 3) /* Receive bytes from first byte until byte N-3 */
    {
      while ((I2C_GetLastEvent(I2C1) & 0x00004) != 0x000004); /* Poll on BTF */
      /* Read data */
      *Ptr++ = I2C_ReceiveData(I2C1);
      Size--;
    }
    /* it remains to read three data: data N-2, data N-1, Data N */
    /* Data N-2 in DR and data N -1 in shift register */
    while ((I2C_GetLastEvent(I2C1) & 0x000004) != 0x0000004); /* Poll on BTF */
    /* Clear ACK */
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    __disable_irq();
    /* Read Data N-2 */
    *Ptr++ = I2C_ReceiveData(I2C1);
    /* Program the STOP */
    I2C_GenerateSTOP(I2C1, ENABLE);
    /* Read DataN-1 */
    *Ptr++ = I2C_ReceiveData(I2C1);
    __enable_irq();
    while ((I2C_GetLastEvent(I2C1) & 0x00000040) != 0x0000040); /* Poll on RxNE */
    /* Read DataN */
    *Ptr = I2C1->DR;
    /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
    while ((I2C1->CR1&0x200) == 0x200);
    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2C1, ENABLE);
  }
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
  return(ret);
}
