/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : gpio.c
* Author             : Yoann TREGUIER
* Version            : 1.1.0
* Date               : 2013/01/23
* Description        : GPIO initialisation functions
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "kernel/dev/arch/cortexm/stm32f4xx/driverlib/stm32f4xx.h"
#include "gpio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define gpio_is_std(Gpio)   (Gpio->Type == GPIO_TYPE_STD)
#define gpio_is_i2c(Gpio)   (Gpio->Type == GPIO_TYPE_I2C)

/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void GPIO_ToggleOutputDataBit(GPIO_TypeDef *GPIOx, u16 GPIO_Pin);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : gpio_startup_init
* Description    : Initialize all defined GPIOs
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void gpio_startup_init(void)
{
  u8 i;

  /* Enable ABP2 GPIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
  
  /* Initialize GPIO */
  for (i = 0 ; i < GPIO_NB ; i++) if (Gpio_Descriptor[i].Init) gpio_init((&Gpio_Descriptor[i]));
}

/*******************************************************************************
* Function Name  : gpio_set_mode
* Description    : Set the mode of a specified GPIO
* Input          : - Gpio: Select the GPIO function to be configured
*                  - Mode: Config mode (in, out, open-drain, ...)
*                  - Val: Value to set if pin is configured as an output
* Output         : None
* Return         : None
*******************************************************************************/
void gpio_set_mode(const _Gpio_Descriptor *Gpio, GPIOMode_TypeDef Mode, u8 Val)
{
  GPIO_InitTypeDef gpio_init_structure;

  if (!Gpio) return;
  #ifdef _GPIO_I2CIO_SUPPORT
    if (gpio_is_i2c(Gpio))
    {
      if ((Mode == GPIO_Mode_Out_PP) || (Mode == GPIO_Mode_Out_OD)) i2cio_set_mode(Gpio->Port, Gpio->GPIO_Pin, I2CIO_MODE_OUTPUT, Val);
      else i2cio_set_mode(Gpio->Port, Gpio->GPIO_Pin, I2CIO_MODE_INPUT, 0);
      return;
    }
  #endif
  gpio_init_structure.GPIO_Pin = Gpio->GPIO_Pin;
  gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init_structure.GPIO_Mode = Mode;
  GPIO_Init((GPIO_TypeDef *)Gpio->Port, &gpio_init_structure);
  if ((Mode == GPIO_Mode_Out_PP) || (Mode == GPIO_Mode_Out_OD)) GPIO_WriteBit((GPIO_TypeDef *)Gpio->Port, Gpio->GPIO_Pin, (BitAction)(Val));
}

/*******************************************************************************
* Function Name  : gpio_init
* Description    : Initialize a specified GPIO to its default mode and value
* Input          : - Gpio: Select the GPIO to be initialized
* Output         : None
* Return         : None
*******************************************************************************/
void gpio_init(const _Gpio_Descriptor *Gpio)
{
  if (!Gpio) return;
  gpio_set_mode(Gpio, Gpio->DefMode, Gpio->DefVal);
}

/*******************************************************************************
* Function Name  : gpio_set
* Description    : Set the value of a specified GPIO
* Input          : - Gpio: Select the GPIO to be set
* Output         : None
* Return         : None
*******************************************************************************/
void gpio_set(const _Gpio_Descriptor *Gpio)
{
  if (!Gpio) return;
  #ifdef _GPIO_I2CIO_SUPPORT
    if (gpio_is_i2c(Gpio)) i2cio_set_output(Gpio->Port, Gpio->GPIO_Pin, 1);
    else GPIO_SetBits((GPIO_TypeDef *)Gpio->Port, Gpio->GPIO_Pin);
  #else
    GPIO_SetBits((GPIO_TypeDef *)Gpio->Port, Gpio->GPIO_Pin);
  #endif
}

/*******************************************************************************
* Function Name  : gpio_reset
* Description    : Reset the value of a specified GPIO
* Input          : - Gpio: Select the GPIO to be reset
* Output         : None
* Return         : None
*******************************************************************************/
void gpio_reset(const _Gpio_Descriptor *Gpio)
{
  if (!Gpio) return;
  #ifdef _GPIO_I2CIO_SUPPORT
    if (gpio_is_i2c(Gpio)) i2cio_set_output(Gpio->Port, Gpio->GPIO_Pin, 0);
    else GPIO_ResetBits((GPIO_TypeDef *)Gpio->Port, Gpio->GPIO_Pin);
  #else
    GPIO_ResetBits((GPIO_TypeDef *)Gpio->Port, Gpio->GPIO_Pin);
  #endif
}

/*******************************************************************************
* Function Name  : gpio_toggle
* Description    : Toggle the value of a specified GPIO
* Input          : - Gpio: Select the GPIO to be toggled
* Output         : None
* Return         : None
*******************************************************************************/
void gpio_toggle(const _Gpio_Descriptor *Gpio)
{
  if (!Gpio) return;
  #ifdef _GPIO_I2CIO_SUPPORT
    if (gpio_is_i2c(Gpio)) i2cio_toggle_output(Gpio->Port, Gpio->GPIO_Pin);
    else GPIO_ToggleOutputDataBit((GPIO_TypeDef *)Gpio->Port, Gpio->GPIO_Pin);
  #else
    GPIO_ToggleOutputDataBit((GPIO_TypeDef *)Gpio->Port, Gpio->GPIO_Pin);
  #endif
}

/*******************************************************************************
* Function Name  : gpio_read
* Description    : Read the input value of a specified GPIO
* Input          : - Gpio: Select the GPIO to be read
* Output         : None
* Return         : None
*******************************************************************************/
u8 gpio_read(const _Gpio_Descriptor *Gpio)
{
  if (!Gpio) return(0);
  #ifdef _GPIO_I2CIO_SUPPORT
    if (gpio_is_i2c(Gpio)) return(i2cio_read_register_bit(Gpio->Port, I2CIO_REG_INPUT, Gpio->GPIO_Pin));
  #endif
  return(GPIO_ReadInputDataBit((GPIO_TypeDef *)Gpio->Port, Gpio->GPIO_Pin));
}

/*******************************************************************************
* Function Name  : gpio_read_output
* Description    : Read the output value of a specified GPIO
* Input          : - Gpio: Select the GPIO to be read
* Output         : None
* Return         : None
*******************************************************************************/
u8 gpio_read_output(const _Gpio_Descriptor *Gpio)
{
  if (!Gpio) return(0);
  #ifdef _GPIO_I2CIO_SUPPORT
    if (gpio_is_i2c(Gpio)) return(i2cio_read_register_bit(Gpio->Port, I2CIO_REG_OUTPUT, Gpio->GPIO_Pin));
  #endif
  return(GPIO_ReadOutputDataBit((GPIO_TypeDef *)Gpio->Port, Gpio->GPIO_Pin));
}

/*******************************************************************************
* Function Name  : gpio_set_port_mode
* Description    : Set the mode for a GPIO port
* Input          : - Gpio: Select a GPIO of the port to be configured
*                  - Mode: Config mode (in, out, open-drain, ...)
* Output         : None
* Return         : None
*******************************************************************************/
void gpio_set_port_mode(const _Gpio_Descriptor *Gpio, GPIOMode_TypeDef Mode)
{
  GPIO_InitTypeDef gpio_init_structure;

  if (!Gpio) return;
  #ifdef _GPIO_I2CIO_SUPPORT
    if (gpio_is_i2c(Gpio))
    {
      if ((Mode == GPIO_Mode_Out_PP) || (Mode == GPIO_Mode_Out_OD)) i2cio_set_mode(Gpio->Port, 0xFFFF, I2CIO_MODE_OUTPUT, 0);
      else i2cio_set_mode(Gpio->Port, 0xFFFF, I2CIO_MODE_INPUT, 0);
      return;
    }
  #endif
  gpio_init_structure.GPIO_Pin = 0xFFFF;
  gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init_structure.GPIO_Mode = Mode;
  GPIO_Init((GPIO_TypeDef *)Gpio->Port, &gpio_init_structure);
  if ((Mode == GPIO_OType_PP) || (Mode == GPIO_OType_PP)) GPIO_Write((GPIO_TypeDef *)Gpio->Port, 0);
}

/*******************************************************************************
* Function Name  : gpio_read_port
* Description    : Read the input value of a specified GPIO port
* Input          : - Gpio: Select the GPIO port to be read
* Output         : None
* Return         : None
*******************************************************************************/
u16 gpio_read_port(const _Gpio_Descriptor *Gpio)
{
  if (!Gpio) return(0);
  #ifdef _GPIO_I2CIO_SUPPORT
    if (gpio_is_i2c(Gpio)) return(i2cio_read_register(Gpio->Port, I2CIO_REG_INPUT));
  #endif
  return(GPIO_ReadInputData((GPIO_TypeDef *)Gpio->Port));
}

/*******************************************************************************
* Function Name  : gpio_enable_ext_int
* Description    : Configure and enable an external interrupt line
* Input          : - ExtInt: Select the EXTI line to be configured
* Output         : None
* Return         : None
*******************************************************************************/
void gpio_enable_ext_int(const _Gpio_ExtInt_Descriptor *ExtInt)
{
  EXTI_InitTypeDef exti_init_structure;

  GPIO_EXTILineConfig(ExtInt->GPIO_Port, ExtInt->GPIO_Pin);
  EXTI_ClearITPendingBit(ExtInt->EXTI_Line);
  exti_init_structure.EXTI_Line = ExtInt->EXTI_Line;
  exti_init_structure.EXTI_Mode = ExtInt->EXTI_Mode;
  exti_init_structure.EXTI_Trigger = ExtInt->EXTI_Trigger;
  exti_init_structure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&exti_init_structure);
  NVIC_EnableIRQ(ExtInt->IRQn);
}

/*******************************************************************************
* Function Name  : gpio_disable_ext_int
* Description    : Disable an external interrupt line
* Input          : - ExtInt: Select the EXTI line to be configured
* Output         : None
* Return         : None
*******************************************************************************/
void gpio_disable_ext_int(const _Gpio_ExtInt_Descriptor *ExtInt)
{
  EXTI_InitTypeDef exti_init_structure;

  NVIC_DisableIRQ(ExtInt->IRQn);
  exti_init_structure.EXTI_Line = ExtInt->EXTI_Line;
  exti_init_structure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&exti_init_structure);
  EXTI_ClearITPendingBit(ExtInt->EXTI_Line);
}

/*******************************************************************************
* Function Name  : GPIO_ToggleOutputDataBit (missing in ST PeriphLib)
* Description    : Toggles the specified output data port bit.
* Input          : - GPIOx: where x can be (A..G) to select the GPIO peripheral.
*                : - GPIO_Pin:  specifies the port bit to read.
*                    This parameter can be GPIO_Pin_x where x can be (0..15).
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_ToggleOutputDataBit(GPIO_TypeDef *GPIOx, u16 GPIO_Pin)
{
#if 0
  if ((GPIOx->ODR & GPIO_Pin) != (u32)Bit_RESET) 
     GPIOx->BRR = GPIO_Pin;
  else 
     GPIOx->BSRR = GPIO_Pin;
#endif
}

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
