/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : gpio.c
* Author             : Yoann TREGUIER
* Version            : 0.1.0
* Date               : 2013/05/24
* Description        : GPIO driver functions for STM32F4xx devices
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "kernel/dev/arch/cortexm/stm32f4xx/target.h"
#include "kernel/dev/arch/cortexm/stm32f4xx/gpio.h"

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef __packed union
{
  u32 Val;
  __packed struct
  {
    u32 Function  :8;
    u32 Type      :8;
    u32 Speed     :7;
    u32 SetSpeed  :1;
    u32 PuPd      :8;
  };
} _Gpio_Mode;

/* Private macro -------------------------------------------------------------*/
#define gpio_is_std(Gpio)   (Gpio->Type == GPIO_TYPE_STD)
#define gpio_is_i2c(Gpio)   (Gpio->Type == GPIO_TYPE_I2C)

/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
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

  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | \
                          RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI, ENABLE);

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
void gpio_set_mode(const _Gpio_Descriptor *Gpio, u32 Mode, u8 Val)
{
  GPIO_InitTypeDef gpio_init_structure;
  _Gpio_Mode mode;

  if (!Gpio) return;
  mode.Val = Mode;
  #ifdef _GPIO_I2CIO_SUPPORT
    if (gpio_is_i2c(Gpio))
    {
      if (mode.Function == GPIO_FCT_OUT) i2cio_set_mode(Gpio->Port, Gpio->GPIO_Pin, I2CIO_MODE_OUTPUT, Val);
      else i2cio_set_mode(Gpio->Port, Gpio->GPIO_Pin, I2CIO_MODE_INPUT, 0);
      return;
    }
  #endif
  gpio_init_structure.GPIO_Pin  = Gpio->GPIO_Pin;
  gpio_init_structure.GPIO_Mode = (GPIOMode_TypeDef)mode.Function;
  if (mode.SetSpeed) gpio_init_structure.GPIO_Speed = (GPIOSpeed_TypeDef)mode.Speed;
  else gpio_init_structure.GPIO_Speed = _GPIO_DEFAULT_SPEED;
  gpio_init_structure.GPIO_OType = (GPIOOType_TypeDef)mode.Type;
  gpio_init_structure.GPIO_PuPd = (GPIOPuPd_TypeDef)mode.PuPd;
  GPIO_Init((GPIO_TypeDef *)Gpio->Port, &gpio_init_structure);
  if (mode.Function == GPIO_FCT_OUT) GPIO_WriteBit((GPIO_TypeDef *)Gpio->Port, Gpio->GPIO_Pin, (BitAction)Val);
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
    else GPIO_ToggleBits((GPIO_TypeDef *)Gpio->Port, Gpio->GPIO_Pin);
  #else
    GPIO_ToggleBits((GPIO_TypeDef *)Gpio->Port, Gpio->GPIO_Pin);
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
* Function Name  : gpio_set_function
* Description    : Changes the mapping of a specified GPIO
* Input          : - Gpio: Select the GPIO function to be configured
*                  - Function: Select a function to associate with the GPIO
* Output         : None
* Return         : None
*******************************************************************************/
void gpio_set_function(const _Gpio_Descriptor *Gpio, u8 Function)
{
  u8 i = 0;

  while ((1 << i) < Gpio->GPIO_Pin) i++;
  GPIO_PinAFConfig((GPIO_TypeDef *)Gpio->Port, i, Function);
}

// /*******************************************************************************
// * Function Name  : gpio_set_port_mode
// * Description    : Set the mode for a GPIO port
// * Input          : - Gpio: Select a GPIO of the port to be configured
// *                  - Mode: Config mode (in, out, open-drain, ...)
// * Output         : None
// * Return         : None
// *******************************************************************************/
// void gpio_set_port_mode(const _Gpio_Descriptor *Gpio, GPIOMode_TypeDef Mode)
// {
//   GPIO_InitTypeDef gpio_init_structure;

//   if (!Gpio) return;
//   #ifdef _GPIO_I2CIO_SUPPORT
//     if (gpio_is_i2c(Gpio))
//     {
//       if ((Mode == GPIO_Mode_Out_PP) || (Mode == GPIO_Mode_Out_OD)) i2cio_set_mode(Gpio->Port, 0xFFFF, I2CIO_MODE_OUTPUT, 0);
//       else i2cio_set_mode(Gpio->Port, 0xFFFF, I2CIO_MODE_INPUT, 0);
//       return;
//     }
//   #endif
//   gpio_init_structure.GPIO_Pin = 0xFFFF;
//   gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
//   gpio_init_structure.GPIO_Mode = Mode;
//   GPIO_Init((GPIO_TypeDef *)Gpio->Port, &gpio_init_structure);
//   if ((Mode == GPIO_Mode_Out_PP) || (Mode == GPIO_Mode_Out_OD)) GPIO_Write((GPIO_TypeDef *)Gpio->Port, 0);
// }

// /*******************************************************************************
// * Function Name  : gpio_read_port
// * Description    : Read the input value of a specified GPIO port
// * Input          : - Gpio: Select the GPIO port to be read
// * Output         : None
// * Return         : None
// *******************************************************************************/
// u16 gpio_read_port(const _Gpio_Descriptor *Gpio)
// {
//   if (!Gpio) return(0);
//   #ifdef _GPIO_I2CIO_SUPPORT
//     if (gpio_is_i2c(Gpio)) return(i2cio_read_register(Gpio->Port, I2CIO_REG_INPUT));
//   #endif
//   return(GPIO_ReadInputData((GPIO_TypeDef *)Gpio->Port));
// }

// /*******************************************************************************
// * Function Name  : gpio_enable_ext_int
// * Description    : Configure and enable an external interrupt line
// * Input          : - ExtInt: Select the EXTI line to be configured
// * Output         : None
// * Return         : None
// *******************************************************************************/
// void gpio_enable_ext_int(const _Gpio_ExtInt_Descriptor *ExtInt)
// {
//   EXTI_InitTypeDef exti_init_structure;

//   GPIO_EXTILineConfig(ExtInt->GPIO_Port, ExtInt->GPIO_Pin);
//   EXTI_ClearITPendingBit(ExtInt->EXTI_Line);
//   exti_init_structure.EXTI_Line = ExtInt->EXTI_Line;
//   exti_init_structure.EXTI_Mode = ExtInt->EXTI_Mode;
//   exti_init_structure.EXTI_Trigger = ExtInt->EXTI_Trigger;
//   exti_init_structure.EXTI_LineCmd = ENABLE;
//   EXTI_Init(&exti_init_structure);
//   NVIC_EnableIRQ(ExtInt->IRQn);
// }

// /*******************************************************************************
// * Function Name  : gpio_disable_ext_int
// * Description    : Disable an external interrupt line
// * Input          : - ExtInt: Select the EXTI line to be configured
// * Output         : None
// * Return         : None
// *******************************************************************************/
// void gpio_disable_ext_int(const _Gpio_ExtInt_Descriptor *ExtInt)
// {
//   EXTI_InitTypeDef exti_init_structure;

//   NVIC_DisableIRQ(ExtInt->IRQn);
//   exti_init_structure.EXTI_Line = ExtInt->EXTI_Line;
//   exti_init_structure.EXTI_LineCmd = DISABLE;
//   EXTI_Init(&exti_init_structure);
//   EXTI_ClearITPendingBit(ExtInt->EXTI_Line);
// }

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
