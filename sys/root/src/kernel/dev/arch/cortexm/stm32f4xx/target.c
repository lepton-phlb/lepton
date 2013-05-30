/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : target.c
* Author             : Yoann TREGUIER
* Version            : Drivers demo
* Date               : 2013/05/24
* Description        : Target dependant definitions
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "target.h"
#include "gpio.h"
#include "uart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/*******************************************************************************
* GPIO
*******************************************************************************/
const _Gpio_Descriptor Gpio_Descriptor[] = {
  {GPIO_TYPE_STD, GPIOD,  GPIO_Pin_8,   0,  GPIO_FCT_IN,  0},   // GPIO_TXD3
  {GPIO_TYPE_STD, GPIOD,  GPIO_Pin_9,   0,  GPIO_FCT_IN,  0},   // GPIO_RXD3
//   {GPIO_TYPE_STD, GPIOC,  GPIO_Pin_10,   0,  GPIO_FCT_IN,  0},   // GPIO_TXD3
//   {GPIO_TYPE_STD, GPIOC,  GPIO_Pin_11,   0,  GPIO_FCT_IN,  0},   // GPIO_RXD3

  {GPIO_TYPE_STD, GPIOF,  GPIO_Pin_6,   1,  GPIO_FCT_OUT, 1},   // GPIO_LED1
  {GPIO_TYPE_STD, GPIOF,  GPIO_Pin_7,   1,  GPIO_FCT_OUT, 1},   // GPIO_LED2
  {GPIO_TYPE_STD, GPIOF,  GPIO_Pin_8,   1,  GPIO_FCT_OUT, 1},   // GPIO_LED3
  {GPIO_TYPE_STD, GPIOF,  GPIO_Pin_9,   1,  GPIO_FCT_OUT, 1},   // GPIO_LED4

  {GPIO_TYPE_STD, GPIOA,  GPIO_Pin_0,   0,  GPIO_FCT_IN,  0}    // GPIO_WKUP

  };

/*******************************************************************************
* UART
*******************************************************************************/


/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
