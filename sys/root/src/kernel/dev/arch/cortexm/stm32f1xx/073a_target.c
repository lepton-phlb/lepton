/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : 073a_target.c
* Author             : Yoann TREGUIER
* Version            : CMsys_1-18.5
* Date               : 2013/04/12
* Description        : Target dependant definitions
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/

#include "kernel/core/ucore/cmsis/Device/st/stm32f1xx/stm32f10x.h"
#include "sys_types.h"
#include "target.h"
#include "gpio.h"
#include "uart.h"
#include "spi.h"
#include "adc.h"
#include "tmr.h"
#include "eeprom.h"
#include "i2cio.h"

/*##############################################################################
#
# Device configuration
#
##############################################################################*/
/*******************************************************************************
* Version
*******************************************************************************/
#define VER_PCB_ID  73
#define VER_BOM_ID  1

//const _Ver_1 Firm_Ver __attribute__((at(FLASH_VER_ADDR))) = {FIRM_MIN, FIRM_MAJ, FIRM_TYP, VER_BOM_ID, VER_PCB_ID, 1};
//const char Sys_Date[] = FIRM_DATE;

/*******************************************************************************
* BKP REGISTERS
*******************************************************************************/
const u16 BKP_ADDR[42] = {
  BKP_DR1, BKP_DR2, BKP_DR3, BKP_DR4, BKP_DR5, BKP_DR6, BKP_DR7,
  BKP_DR8, BKP_DR9, BKP_DR10, BKP_DR11, BKP_DR12, BKP_DR13, BKP_DR14,
  BKP_DR15, BKP_DR16, BKP_DR17, BKP_DR18, BKP_DR19, BKP_DR20, BKP_DR21,
  BKP_DR22, BKP_DR23, BKP_DR24, BKP_DR25, BKP_DR26, BKP_DR27, BKP_DR28,
  BKP_DR29, BKP_DR30, BKP_DR31, BKP_DR32, BKP_DR33, BKP_DR34, BKP_DR35,
  BKP_DR36, BKP_DR37, BKP_DR38, BKP_DR39, BKP_DR40, BKP_DR41, BKP_DR42
  };


/*##############################################################################
#
# Drivers configuration
#
##############################################################################*/
/*******************************************************************************
* GPIO
*******************************************************************************/
const _Gpio_Descriptor Gpio_Descriptor[] = {
  {GPIO_TYPE_STD, GPIOA,      GPIO_Pin_10,  0,  GPIO_MODE_IN,       0},   // GPIO_RXD1
  {GPIO_TYPE_STD, GPIOA,      GPIO_Pin_9,   0,  GPIO_MODE_IN,       0},   // GPIO_TXD1
  {GPIO_TYPE_STD, GPIOA,      GPIO_Pin_12,  0,  GPIO_MODE_IN,       0},   // GPIO_RTS1
  {GPIO_TYPE_STD, GPIOA,      GPIO_Pin_11,  0,  GPIO_MODE_IN,       0},   // GPIO_CTS1

  {GPIO_TYPE_STD, GPIOA,      GPIO_Pin_3,   0,  GPIO_MODE_IN,       0},   // GPIO_RXD2
  {GPIO_TYPE_STD, GPIOA,      GPIO_Pin_2,   0,  GPIO_MODE_IN,       0},   // GPIO_TXD2
  {GPIO_TYPE_STD, GPIOA,      GPIO_Pin_1,   0,  GPIO_MODE_IN,       0},   // GPIO_RTS2
  {GPIO_TYPE_STD, GPIOA,      GPIO_Pin_8,   0,  GPIO_MODE_IN,       0},   // GPIO_CTS2

  {GPIO_TYPE_STD, GPIOB,      GPIO_Pin_11,  0,  GPIO_MODE_IN,       0},   // GPIO_RXD3
  {GPIO_TYPE_STD, GPIOB,      GPIO_Pin_10,  0,  GPIO_MODE_IN,       0},   // GPIO_TXD3

  {GPIO_TYPE_STD, GPIOA,      GPIO_Pin_5,   0,  GPIO_MODE_IN,       0},   // GPIO_SCK1
  {GPIO_TYPE_STD, GPIOA,      GPIO_Pin_6,   0,  GPIO_MODE_IN,       0},   // GPIO_MISO1
  {GPIO_TYPE_STD, GPIOA,      GPIO_Pin_7,   0,  GPIO_MODE_IN,       0},   // GPIO_MOSI1

  {GPIO_TYPE_STD, GPIOB,      GPIO_Pin_13,  0,  GPIO_MODE_IN,       0},   // GPIO_SCK2
  {GPIO_TYPE_STD, GPIOB,      GPIO_Pin_14,  0,  GPIO_MODE_IN,       0},   // GPIO_MISO2
  {GPIO_TYPE_STD, GPIOB,      GPIO_Pin_15,  0,  GPIO_MODE_IN,       0},   // GPIO_MOSI2

  {GPIO_TYPE_STD, GPIOB,      GPIO_Pin_6,   0,  GPIO_MODE_IN,       0},   // GPIO_SCL
  {GPIO_TYPE_STD, GPIOB,      GPIO_Pin_7,   0,  GPIO_MODE_IN,       0},   // GPIO_SDA

  {GPIO_TYPE_STD, GPIOB,      GPIO_Pin_8,   0,  GPIO_MODE_IN,       0},   // GPIO_CANRX
  {GPIO_TYPE_STD, GPIOB,      GPIO_Pin_9,   0,  GPIO_MODE_IN,       0},   // GPIO_CANTX

  {GPIO_TYPE_STD, GPIOD,      GPIO_Pin_2,   1,  GPIO_MODE_OUT,      1},   // GPIO_LED_V
  {GPIO_TYPE_STD, GPIOC,      GPIO_Pin_12,  1,  GPIO_MODE_OUT,      0},   // GPIO_LED_R

  {GPIO_TYPE_STD, GPIOB,      GPIO_Pin_2,   1,  GPIO_MODE_OUT_OD,   1},   // GPIO_VBAT_MES_DIS
  {GPIO_TYPE_STD, GPIOB,      GPIO_Pin_0,   1,  GPIO_MODE_AN,                  0},   // GPIO_VBAT_MES_ADC

  {GPIO_TYPE_STD, GPIOB,      GPIO_Pin_12,  1,  GPIO_MODE_OUT,      1},   // GPIO_RF_CS
  {GPIO_TYPE_STD, GPIOB,      GPIO_Pin_1,   0,  GPIO_MODE_IN,       0},   // GPIO_RF_GD0
  {GPIO_TYPE_STD, GPIOC,      GPIO_Pin_10,  0,  GPIO_MODE_IN,       0},   // GPIO_RF_GD2

  {GPIO_TYPE_STD, GPIOA,      GPIO_Pin_4,   1,  GPIO_MODE_OUT,      1},   // GPIO_FLASH_CS
  {GPIO_TYPE_STD, GPIOC,      GPIO_Pin_11,  1,  GPIO_MODE_OUT,      0},   // GPIO_VP_DIS

  {GPIO_TYPE_STD, GPIOA,      GPIO_Pin_0,   0,  GPIO_MODE_IN,       0},   // GPIO_WKUP

  {GPIO_TYPE_I2C, I2CIO_MB,   GPIO_Pin_0,   0,  GPIO_MODE_IN,       0},   // GPIO_INT01
  {GPIO_TYPE_I2C, I2CIO_MB,   GPIO_Pin_1,   0,  GPIO_MODE_IN,       0},   // GPIO_INT02
  {GPIO_TYPE_I2C, I2CIO_MB,   GPIO_Pin_2,   0,  GPIO_MODE_IN,       0},   // GPIO_INT03
  {GPIO_TYPE_I2C, I2CIO_MB,   GPIO_Pin_3,   0,  GPIO_MODE_IN,       0},   // GPIO_INT04
  {GPIO_TYPE_I2C, I2CIO_MB,   GPIO_Pin_4,   0,  GPIO_MODE_IN,       0},   // GPIO_INT11
  {GPIO_TYPE_I2C, I2CIO_MB,   GPIO_Pin_5,   0,  GPIO_MODE_IN,       0},   // GPIO_INT12
  {GPIO_TYPE_I2C, I2CIO_MB,   GPIO_Pin_6,   0,  GPIO_MODE_IN,       0},   // GPIO_INT21
  {GPIO_TYPE_I2C, I2CIO_MB,   GPIO_Pin_7,   0,  GPIO_MODE_IN,       0},   // GPIO_INT22

  {GPIO_TYPE_STD, GPIOC,      GPIO_Pin_0,   0,  GPIO_MODE_IN,       0},   // GPIO_EXT11
  {GPIO_TYPE_STD, GPIOC,      GPIO_Pin_1,   0,  GPIO_MODE_IN,       0},   // GPIO_EXT12
  {GPIO_TYPE_STD, GPIOC,      GPIO_Pin_2,   0,  GPIO_MODE_IN,       0},   // GPIO_EXT13
  {GPIO_TYPE_STD, GPIOC,      GPIO_Pin_3,   0,  GPIO_MODE_IN,       0},   // GPIO_EXT14
  {GPIO_TYPE_STD, GPIOC,      GPIO_Pin_6,   0,  GPIO_MODE_IN,       0},   // GPIO_EXT15
  {GPIO_TYPE_STD, GPIOC,      GPIO_Pin_7,   0,  GPIO_MODE_IN,       0},   // GPIO_EXT16
  {GPIO_TYPE_STD, GPIOC,      GPIO_Pin_8,   0,  GPIO_MODE_IN,       0},   // GPIO_EXT17

  {GPIO_TYPE_STD, GPIOC,      GPIO_Pin_4,   0,  GPIO_MODE_IN,       0},   // GPIO_EXT21
  {GPIO_TYPE_STD, GPIOC,      GPIO_Pin_5,   0,  GPIO_MODE_IN,       0},   // GPIO_EXT22
  {GPIO_TYPE_STD, GPIOC,      GPIO_Pin_9,   0,  GPIO_MODE_IN,       0},   // GPIO_EXT23
  
  {GPIO_TYPE_I2C, I2CIO_DB0,   GPIO_Pin_0,   0, GPIO_MODE_IN,       0},   // GPIO_DB01
  {GPIO_TYPE_I2C, I2CIO_DB0,   GPIO_Pin_1,   0, GPIO_MODE_IN,       0},   // GPIO_DB02
  {GPIO_TYPE_I2C, I2CIO_DB0,   GPIO_Pin_2,   0, GPIO_MODE_IN,       0},   // GPIO_DB03
  {GPIO_TYPE_I2C, I2CIO_DB0,   GPIO_Pin_3,   0, GPIO_MODE_IN,       0},   // GPIO_DB04
  {GPIO_TYPE_I2C, I2CIO_DB0,   GPIO_Pin_4,   0, GPIO_MODE_IN,       0},   // GPIO_DB05
  {GPIO_TYPE_I2C, I2CIO_DB0,   GPIO_Pin_5,   0, GPIO_MODE_IN,       0},   // GPIO_DB06
  {GPIO_TYPE_I2C, I2CIO_DB0,   GPIO_Pin_6,   0, GPIO_MODE_IN,       0},   // GPIO_DB07
  {GPIO_TYPE_I2C, I2CIO_DB0,   GPIO_Pin_7,   0, GPIO_MODE_IN,       0}    // GPIO_DB08
  
  };

/*******************************************************************************
* EXTI
*******************************************************************************/
const _Gpio_ExtInt_Descriptor Gpio_ExtInt_Descriptor[] = {
  {GPIO_PortSourceGPIOB, GPIO_PinSource1, EXTI1_IRQn, EXTI_Line1, EXTI_Mode_Interrupt, EXTI_Trigger_Falling},         // EXTI_RF_RX
  {GPIO_PortSourceGPIOA, GPIO_PinSource0, EXTI0_IRQn, EXTI_Line0, EXTI_Mode_Interrupt, EXTI_Trigger_Rising}           // EXTI_WKUP
  };

/*******************************************************************************
* UART
*******************************************************************************/
const _Uart_Descriptor Uart_Descriptor[] = {
  {USART1, RCC_APB2PeriphClockCmd, RCC_APB2Periph_USART1, USART1_IRQn, DMA1_Channel5, DMA1_Channel5_IRQn, GPIO_TXD1, GPIO_RXD1, &Uart_Ctrl[UART_ID_1]},   // UART_1
  {USART2, RCC_APB1PeriphClockCmd, RCC_APB1Periph_USART2, USART2_IRQn, DMA1_Channel6, DMA1_Channel6_IRQn, GPIO_TXD2, GPIO_RXD2, &Uart_Ctrl[UART_ID_2]},   // UART_2
  {USART3, RCC_APB1PeriphClockCmd, RCC_APB1Periph_USART3, USART3_IRQn, DMA1_Channel3, DMA1_Channel3_IRQn, GPIO_TXD3, GPIO_RXD3, &Uart_Ctrl[UART_ID_3]}    // UART_3
  };

/*******************************************************************************
* SPI
*******************************************************************************/
const _Spi_Descriptor Spi_Descriptor[] = {
  {SPI1, RCC_APB2PeriphClockCmd, RCC_APB2Periph_SPI1, GPIO_MISO1, GPIO_MOSI1, GPIO_SCK1, 3, 20000000},   // SPI_ID_FLASH (Flash memories)
  {SPI2, RCC_APB1PeriphClockCmd, RCC_APB1Periph_SPI2, GPIO_MISO2, GPIO_MOSI2, GPIO_SCK2, 0, 6500000}     // SPI_ID_RADIO (CC1101)
  };

/*******************************************************************************
* ADC
*******************************************************************************/
const _Adc_Descriptor Adc_Descriptor[] = {
  {ADC1, ADC_Channel_8, GPIO_VBAT_MES_ADC},   //ADC_ID_VBAT
  {ADC1, ADC_Channel_16, 0},                  //ADC_ID_TEMP
  {ADC1, ADC_Channel_10, GPIO_EXT11},         //ADC_ID_DB01
  {ADC1, ADC_Channel_11, GPIO_EXT12},         //ADC_ID_DB02
  {ADC1, ADC_Channel_12, GPIO_EXT13},         //ADC_ID_DB03
  {ADC1, ADC_Channel_13, GPIO_EXT14},         //ADC_ID_DB04
  {ADC1, ADC_Channel_14, GPIO_EXT21},         //ADC_ID_DB11
  {ADC1, ADC_Channel_15, GPIO_EXT22}          //ADC_ID_DB12
  };

/*******************************************************************************
* I2CIO
*******************************************************************************/
const _I2cio_Descriptor I2cio_Descriptor[] = {
  {0xE6, 400000, 1},   // I2CIO_ID_MB  (8-bit expander)
  {0xE4, 400000, 1}    // I2CIO_ID_DB0 (8-bit expander)
  };

/*******************************************************************************
* TMR
*******************************************************************************/
const _Tmr_Descriptor Tmr_Descriptor[] = {
  {TIM1, 2, TIM1_UP_TIM10_IRQn, (void (*)(uint32_t, FunctionalState))RCC_APB2PeriphClockCmd, RCC_APB2Periph_TIM1, &Tmr_Ctrl[0]},
  {TIM2, 1, TIM2_IRQn, (void (*)(uint32_t, FunctionalState))RCC_APB1PeriphClockCmd, RCC_APB1Periph_TIM2, &Tmr_Ctrl[1]},
  {TIM3, 1, TIM3_IRQn, (void (*)(uint32_t, FunctionalState))RCC_APB1PeriphClockCmd, RCC_APB1Periph_TIM3, &Tmr_Ctrl[2]},
  {TIM4, 1, TIM4_IRQn, (void (*)(uint32_t, FunctionalState))RCC_APB1PeriphClockCmd, RCC_APB1Periph_TIM4, &Tmr_Ctrl[3]},
  {TIM5, 1, TIM5_IRQn, (void (*)(uint32_t, FunctionalState))RCC_APB1PeriphClockCmd, RCC_APB1Periph_TIM5, &Tmr_Ctrl[4]},
  {TIM6, 1, TIM6_IRQn, (void (*)(uint32_t, FunctionalState))RCC_APB1PeriphClockCmd, RCC_APB1Periph_TIM6, &Tmr_Ctrl[5]},
  {TIM7, 1, TIM7_IRQn, (void (*)(uint32_t, FunctionalState))RCC_APB1PeriphClockCmd, RCC_APB1Periph_TIM7, &Tmr_Ctrl[6]},
  {TIM8, 2, TIM8_UP_TIM13_IRQn, (void (*)(uint32_t, FunctionalState))RCC_APB2PeriphClockCmd, RCC_APB2Periph_TIM8, &Tmr_Ctrl[7]}
  };

/*******************************************************************************
* PWM
*******************************************************************************/
const _Pwm_Descriptor Pwm_Descriptor[] = {
  {GPIO_EXT15, TMR8, TIM_Channel_1},  // PWM_ID_DB05
  {GPIO_EXT16, TMR8, TIM_Channel_2},  // PWM_ID_DB06
  {GPIO_EXT17, TMR8, TIM_Channel_3},  // PWM_ID_DB07
  {GPIO_EXT23, TMR8, TIM_Channel_4}   // PWM_ID_DB13
  };

/*******************************************************************************
* EEPROM
*******************************************************************************/
const _Eeprom_Descriptor Eeprom_Descriptor[] = {
  {0xAE, 400000, 128},  // EEPROM_ID_DB0
  {0xA6, 400000, 128}   // EEPROM_ID_DB1
  };

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
