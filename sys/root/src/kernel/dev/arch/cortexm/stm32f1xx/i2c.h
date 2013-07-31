/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : i2c.c
* Author             : Yoann TREGUIER
* Version            : 1.2.0
* Date               : 2013/06/14
* Description        : Headers of I2C driver functions
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_H
#define __I2C_H

/* Includes ------------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#ifdef _I2C_OS_SUPPORT
  #define i2c_startup_init()  sys_mut_init(I2c_Mut)
#else
  #define i2c_startup_init()  ((void)0)
#endif

/* Exported variables --------------------------------------------------------*/
#ifdef _I2C_OS_SUPPORT
  extern OS_MUT I2c_Mut;
#endif

/* Exported functions --------------------------------------------------------*/
int i2c_check_status(void);
int i2c_open(u32 ClockSpeed);
void i2c_close(void);
int i2c_transmit(u8 Addr, const char *Ptr, u16 Size, u8 Stop);
int i2c_receive(u8 Addr, char *Ptr, u16 Size);

#endif /* __I2C_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
