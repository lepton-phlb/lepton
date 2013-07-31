/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : rtc.h
* Author             : Yoann TREGUIER
* Version            : 1.2.0
* Date               : 2013/06/14
* Description        : Headers of RTC interface functions
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H
#define __RTC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void rtc_startup_init(void);
time_t rtc_get_time(void);
void rtc_set_time(time_t Tme);
void rtc_set_alarm(time_t Tme);
time_t rtc_get_alarm(void);

#endif /* __RTC_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
