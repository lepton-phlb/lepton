/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : 073a_target.h
* Author             : Yoann TREGUIER
* Version            : CMsys_1-18.5
* Date               : 2013/04/12
* Description        : Target dependant definitions
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __073A_TARGET_H
#define __073A_TARGET_H

/*##############################################################################
#
# Device configuration
#
##############################################################################*/
/* Includes ------------------------------------------------------------------*/
#include "kernel/core/ucore/cmsis/Device/st/stm32f1xx/stm32f10x.h"

/* Exported define -----------------------------------------------------------*/
#define FLASH_BASE_ADDR   0x08000000
#define FLASH_SIZE        0x00080000
#define FLASH_VECT_SIZE   0x00000130
#define FLASH_PAGE_SIZE   0x00000800
#define FLASH_BOOT_SIZE   0x0000C000
#define FLASH_DATA_SIZE   0x00004000

#define FLASH_VER_ADDR  ((u32)(FLASH_BASE_ADDR + FLASH_BOOT_SIZE + FLASH_VECT_SIZE))
#define FLASH_UID_ADDR  ((u32)(FLASH_BASE_ADDR + FLASH_VECT_SIZE + sizeof(_Ver)))
#define FLASH_ID_ADDR   ((u32)(FLASH_UID_ADDR + sizeof(_Uid)))

#define FLASH_FIRM_DESC_ADDR    ((u32)(FLASH_BASE_ADDR + FLASH_SIZE - FLASH_PAGE_SIZE))
#define FLASH_CONF_ADDR         ((u32)(FLASH_FIRM_DESC_ADDR - FLASH_PAGE_SIZE))
#define FLASH_RF_CONF_ADDR      ((u32)(FLASH_CONF_ADDR))
#define FLASH_PERIPH_CONF_ADDR  ((u32)(FLASH_CONF_ADDR + (FLASH_PAGE_SIZE / 2)))

#define Boot_Ver  (*(_Ver *)(FLASH_BASE_ADDR + FLASH_VECT_SIZE))
#define Sys_Ver   (*(_Ver *)FLASH_VER_ADDR)
#define Sys_Uid   (*(_Uid *)FLASH_UID_ADDR)
#define Sys_Id    (*(_Id *)FLASH_ID_ADDR)

#define FlashFirmDesc     (*(_Firm_Descriptor *)FLASH_FIRM_DESC_ADDR)
#define FlashRfConf       (*(_Rf_Conf *)FLASH_RF_CONF_ADDR)
#define FlashPeriphConf   (*(_Periph_Conf *)FLASH_PERIPH_CONF_ADDR)

/* Exported macro ------------------------------------------------------------*/
#define NVIC_SetVectorTable(Base, Offset) (SCB->VTOR = ((u32)Base) | (Offset & (u32)0x1FFFFF80))

/* Exported constants --------------------------------------------------------*/
extern const u16 BKP_ADDR[42];


/*##############################################################################
#
# System configuration
#
##############################################################################*/
/* Exported define -----------------------------------------------------------*/
#define _TASK_MAX_CNT   24


/*##############################################################################
#
# Drivers configuration
#
##############################################################################*/
/*******************************************************************************
* GPIO
*******************************************************************************/
#define _GPIO_I2CIO_SUPPORT
#define _GPIO_DEFAULT_SPEED   GPIO_Speed_50MHz

typedef enum
{
  GPIO_ID_RXD1,
  GPIO_ID_TXD1,
  GPIO_ID_RTS1,
  GPIO_ID_CTS1,

  GPIO_ID_RXD2,
  GPIO_ID_TXD2,
  GPIO_ID_RTS2,
  GPIO_ID_CTS2,

  GPIO_ID_RXD3,
  GPIO_ID_TXD3,

  GPIO_ID_SCK1,
  GPIO_ID_MISO1,
  GPIO_ID_MOSI1,

  GPIO_ID_SCK2,
  GPIO_ID_MISO2,
  GPIO_ID_MOSI2,

  GPIO_ID_SCL,
  GPIO_ID_SDA,

  GPIO_ID_CANRX,
  GPIO_ID_CANTX,

  GPIO_ID_LED_V,
  GPIO_ID_LED_R,

  GPIO_ID_VBAT_MES_DIS,
  GPIO_ID_VBAT_MES_ADC,

  GPIO_ID_RF_CS,
  GPIO_ID_RF_GD0,
  GPIO_ID_RF_GD2,

  GPIO_ID_FLASH_CS,
  GPIO_ID_VP_DIS,

  GPIO_ID_WKUP,

  GPIO_ID_INT01,
  GPIO_ID_INT02,
  GPIO_ID_INT03,
  GPIO_ID_INT04,
  GPIO_ID_INT11,
  GPIO_ID_INT12,
  GPIO_ID_INT21,
  GPIO_ID_INT22,

  GPIO_ID_EXT11,
  GPIO_ID_EXT12,
  GPIO_ID_EXT13,
  GPIO_ID_EXT14,
  GPIO_ID_EXT15,
  GPIO_ID_EXT16,
  GPIO_ID_EXT17,

  GPIO_ID_EXT21,
  GPIO_ID_EXT22,
  GPIO_ID_EXT23,
    
  GPIO_ID_DB01,
  GPIO_ID_DB02,
  GPIO_ID_DB03,
  GPIO_ID_DB04,
  GPIO_ID_DB05,
  GPIO_ID_DB06,
  GPIO_ID_DB07,
  GPIO_ID_DB08,
  
  GPIO_NB
} _GPIO_LIST;

#define GPIO_RXD1   (&Gpio_Descriptor[GPIO_ID_RXD1])
#define GPIO_TXD1   (&Gpio_Descriptor[GPIO_ID_TXD1])
#define GPIO_RTS1   (&Gpio_Descriptor[GPIO_ID_RTS1])

#define GPIO_RXD2   (&Gpio_Descriptor[GPIO_ID_RXD2])
#define GPIO_TXD2   (&Gpio_Descriptor[GPIO_ID_TXD2])
#define GPIO_RTS2   (&Gpio_Descriptor[GPIO_ID_RTS2])

#define GPIO_RXD3   (&Gpio_Descriptor[GPIO_ID_RXD3])
#define GPIO_TXD3   (&Gpio_Descriptor[GPIO_ID_TXD3])

#define GPIO_SCK1   (&Gpio_Descriptor[GPIO_ID_SCK1])
#define GPIO_MISO1  (&Gpio_Descriptor[GPIO_ID_MISO1])
#define GPIO_MOSI1  (&Gpio_Descriptor[GPIO_ID_MOSI1])

#define GPIO_SCK2   (&Gpio_Descriptor[GPIO_ID_SCK2])
#define GPIO_MISO2  (&Gpio_Descriptor[GPIO_ID_MISO2])
#define GPIO_MOSI2  (&Gpio_Descriptor[GPIO_ID_MOSI2])

#define GPIO_SCL  (&Gpio_Descriptor[GPIO_ID_SCL])
#define GPIO_SDA  (&Gpio_Descriptor[GPIO_ID_SDA])

#define GPIO_CANRX  (&Gpio_Descriptor[GPIO_ID_CANRX])
#define GPIO_CANTX  (&Gpio_Descriptor[GPIO_ID_CANTX])

#define GPIO_LED_V  (&Gpio_Descriptor[GPIO_ID_LED_V])
#define GPIO_LED_R  (&Gpio_Descriptor[GPIO_ID_LED_R])

#define GPIO_VBAT_MES_DIS   (&Gpio_Descriptor[GPIO_ID_VBAT_MES_DIS])
#define GPIO_VBAT_MES_ADC   (&Gpio_Descriptor[GPIO_ID_VBAT_MES_ADC])

#define GPIO_RF_CS    (&Gpio_Descriptor[GPIO_ID_RF_CS])
#define GPIO_RF_GD0   (&Gpio_Descriptor[GPIO_ID_RF_GD0])
#define GPIO_RF_GD1   (&Gpio_Descriptor[GPIO_ID_MISO2])
#define GPIO_RF_GD2   (&Gpio_Descriptor[GPIO_ID_RF_GD2])

#define GPIO_FLASH_CS   (&Gpio_Descriptor[GPIO_ID_FLASH_CS])
#define GPIO_VP_DIS     (&Gpio_Descriptor[GPIO_ID_VP_DIS])

#define GPIO_WKUP     (&Gpio_Descriptor[GPIO_ID_WKUP])

#define GPIO_VEXT     (&Gpio_Descriptor[GPIO_ID_INT01])
#define GPIO_SWITCH   (&Gpio_Descriptor[GPIO_ID_INT02])
#define GPIO_INT03    (&Gpio_Descriptor[GPIO_ID_INT03])
#define GPIO_INT04    (&Gpio_Descriptor[GPIO_ID_INT04])
#define GPIO_INT11    (&Gpio_Descriptor[GPIO_ID_INT11])
#define GPIO_INT12    (&Gpio_Descriptor[GPIO_ID_INT12])
#define GPIO_INT21    (&Gpio_Descriptor[GPIO_ID_INT21])
#define GPIO_INT22    (&Gpio_Descriptor[GPIO_ID_INT22])

#define GPIO_EXT11  (&Gpio_Descriptor[GPIO_ID_EXT11])
#define GPIO_EXT12  (&Gpio_Descriptor[GPIO_ID_EXT12])
#define GPIO_EXT13  (&Gpio_Descriptor[GPIO_ID_EXT13])
#define GPIO_EXT14  (&Gpio_Descriptor[GPIO_ID_EXT14])
#define GPIO_EXT15  (&Gpio_Descriptor[GPIO_ID_EXT15])
#define GPIO_EXT16  (&Gpio_Descriptor[GPIO_ID_EXT16])
#define GPIO_EXT17  (&Gpio_Descriptor[GPIO_ID_EXT17])

#define GPIO_EXT21  (&Gpio_Descriptor[GPIO_ID_EXT21])
#define GPIO_EXT22  (&Gpio_Descriptor[GPIO_ID_EXT22])
#define GPIO_EXT23  (&Gpio_Descriptor[GPIO_ID_EXT23])

#define GPIO_DB01  (&Gpio_Descriptor[GPIO_ID_DB01])
#define GPIO_DB02  (&Gpio_Descriptor[GPIO_ID_DB02])
#define GPIO_DB03  (&Gpio_Descriptor[GPIO_ID_DB03])
#define GPIO_DB04  (&Gpio_Descriptor[GPIO_ID_DB04])
#define GPIO_DB05  (&Gpio_Descriptor[GPIO_ID_DB05])
#define GPIO_DB06  (&Gpio_Descriptor[GPIO_ID_DB06])
#define GPIO_DB07  (&Gpio_Descriptor[GPIO_ID_DB07])
#define GPIO_DB08  (&Gpio_Descriptor[GPIO_ID_DB08])

/*******************************************************************************
* EXTI
*******************************************************************************/
#define _EXTI_GPIO_PORT_SUPPORT

typedef enum
{
  EXTI_ID_RF_RX,
  EXTI_ID_WKUP,
  EXTI_NB
} _EXTI_LIST;

#define EXTI_RF_RX  (&Gpio_ExtInt_Descriptor[EXTI_ID_RF_RX])
#define EXTI_WKUP   (&Gpio_ExtInt_Descriptor[EXTI_ID_WKUP])

#define EXTI_RF_RX_IRQHandler EXTI1_IRQHandler
#define EXTI_WKUP_IRQHandler  EXTI0_IRQHandler

// EXTI GPIO port definition (if any)
#define EXTI_GPIO_PORT  (&Gpio_Descriptor[GPIO_ID_INT01])
#define EXTI_GPIO_CNT   8

/*******************************************************************************
* UART
*******************************************************************************/
//#define _UART_OS_SUPPORT

typedef enum
{
  UART_ID_1,
  UART_ID_2,
  UART_ID_3,
  UART_NB
} _UART_LIST;

#define UART_1  (&Uart_Descriptor[UART_ID_1])
#define UART_2  (&Uart_Descriptor[UART_ID_2])
#define UART_3  (&Uart_Descriptor[UART_ID_3])

/*******************************************************************************
* SPI
*******************************************************************************/
typedef enum
{
  SPI_ID_FLASH,
  SPI_ID_RADIO,
  SPI_NB
} _SPI_LIST;

#define SPI_FLASH   (&Spi_Descriptor[SPI_ID_FLASH])
#define SPI_RADIO   (&Spi_Descriptor[SPI_ID_RADIO])

/*******************************************************************************
* SPIFLASH
*******************************************************************************/
#define SPIFLASH_AT_FIRM_FROM_SECTOR   57
#define SPIFLASH_AT_FIRM_SECTOR_CNT    7
#define SPIFLASH_ST_FIRM_FROM_SECTOR   25
#define SPIFLASH_ST_FIRM_SECTOR_CNT    7

/*******************************************************************************
* FFS
*******************************************************************************/
#define F_ATMEL_RESERVE_FROM_SECTOR   58u
#define ST_RES_PAGES_END              1792

/*******************************************************************************
* I2C
*******************************************************************************/
//#define _I2C_OS_SUPPORT
//#define _I2C_AUTO_CHECK_STATUS
//#define _I2C_BUS_ERROR_RESET

/*******************************************************************************
* ADC
*******************************************************************************/
#define _ADC_OS_SUPPORT
//#define _ADC_ADC2_SUPPORT
//#define _ADC_ADC3_SUPPORT
#define ADC_STD_VREF    2.7

typedef enum
{
  ADC_ID_VBAT,
  ADC_ID_TEMP,
  ADC_ID_DB01,
  ADC_ID_DB02,
  ADC_ID_DB03,
  ADC_ID_DB04,
  ADC_ID_DB11,
  ADC_ID_DB12,
  ADC_NB
} _ADC_LIST;

#define ADC_VBAT  (&Adc_Descriptor[ADC_ID_VBAT])
#define ADC_TEMP  (&Adc_Descriptor[ADC_ID_TEMP])
#define ADC_DB01  (&Adc_Descriptor[ADC_ID_DB01])
#define ADC_DB02  (&Adc_Descriptor[ADC_ID_DB02])
#define ADC_DB03  (&Adc_Descriptor[ADC_ID_DB03])
#define ADC_DB04  (&Adc_Descriptor[ADC_ID_DB04])
#define ADC_DB11  (&Adc_Descriptor[ADC_ID_DB11])
#define ADC_DB12  (&Adc_Descriptor[ADC_ID_DB12])

/*******************************************************************************
* I2CIO
*******************************************************************************/
typedef enum
{
  I2CIO_ID_MB,
  I2CIO_ID_DB0,
  I2CIO_NB
} _I2CIO_LIST;

#define I2CIO_MB  (&I2cio_Descriptor[I2CIO_ID_MB])
#define I2CIO_DB0 (&I2cio_Descriptor[I2CIO_ID_DB0])

/*******************************************************************************
* TMR
*******************************************************************************/
typedef enum
{
  TMR_ID_1,
  TMR_ID_2,
  TMR_ID_3,
  TMR_ID_4,
  TMR_ID_5,
  TMR_ID_6,
  TMR_ID_7,
  TMR_ID_8,
  TMR_NB
} _TMR_LIST;

#define TMR1  (&Tmr_Descriptor[TMR_ID_1])
#define TMR2  (&Tmr_Descriptor[TMR_ID_2])
#define TMR3  (&Tmr_Descriptor[TMR_ID_3])   // Used by ADC driver
#define TMR4  (&Tmr_Descriptor[TMR_ID_4])
#define TMR5  (&Tmr_Descriptor[TMR_ID_5])
#define TMR6  (&Tmr_Descriptor[TMR_ID_6])
#define TMR7  (&Tmr_Descriptor[TMR_ID_7])
#define TMR8  (&Tmr_Descriptor[TMR_ID_8])

/*******************************************************************************
* PWM
*******************************************************************************/
typedef enum
{
  PWM_ID_DB05,
  PWM_ID_DB06,
  PWM_ID_DB07,
  PWM_ID_DB13,
  PWM_NB
} _PWM_LIST;

#define PWM_DB05  (&Pwm_Descriptor[PWM_ID_DB05])
#define PWM_DB06  (&Pwm_Descriptor[PWM_ID_DB06])
#define PWM_DB07  (&Pwm_Descriptor[PWM_ID_DB07])
#define PWM_DB13  (&Pwm_Descriptor[PWM_ID_DB13])

/*******************************************************************************
* EEPROM
*******************************************************************************/
typedef enum
{
  EEPROM_ID_DB0,
  EEPROM_ID_DB1,
  EEPROM_NB
} _EEPROM_LIST;

#define EEPROM_DB0  (&Eeprom_Descriptor[EEPROM_ID_DB0])
#define EEPROM_DB1  (&Eeprom_Descriptor[EEPROM_ID_DB1])

/*******************************************************************************
* BOARDS
*******************************************************************************/
#define BOARD_DEFAULT_MB_PERIPH_MASK  0x0003

typedef enum
{
  BOARD_MB,
  BOARD_DB0,
  BOARD_DB1,
  BOARD_NB
} _BOARD_LIST;

#endif /* __073A_TARGET_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
