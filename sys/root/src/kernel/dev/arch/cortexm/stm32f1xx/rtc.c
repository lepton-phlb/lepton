/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : rtc.c
* Author             : Yoann TREGUIER
* Version            : 1.2.0
* Date               : 2013/06/14
* Description        : RTC interface functions
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "target.h"
#include "rtc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RTC_DEFAULT_TIME  946684800   // Timestamp for 2000-01-01 00:00:00

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : rtc_startup_init
* Description    : Initialize the RTC configuration
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void rtc_startup_init(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  if (RTC_GetCounter() < RTC_DEFAULT_TIME)
  {
    PWR_BackupAccessCmd(ENABLE);
    RCC_LSEConfig(RCC_LSE_ON);
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForSynchro();
    RTC_WaitForLastTask();
    RTC_SetPrescaler(32767);
    RTC_WaitForLastTask();
    RTC_SetCounter(RTC_DEFAULT_TIME);
    RTC_WaitForLastTask();
    RTC_SetAlarm(RTC_DEFAULT_TIME);
    RTC_WaitForLastTask();
    PWR_BackupAccessCmd(DISABLE);
  }

  /* Enable alarm interrupt */
  RTC_WaitForLastTask();
  RTC_ITConfig(RTC_IT_ALR, ENABLE);
  RTC_WaitForLastTask();
  NVIC_EnableIRQ(RTC_IRQn);
}

/*******************************************************************************
* Function Name  : rtc_get_time
* Description    : Get time from the RTC
* Input          : None
* Output         : None
* Return         : Number of seconds past since 1-Jan-70
*******************************************************************************/
time_t rtc_get_time(void)
{
  return(RTC_GetCounter());
}

/*******************************************************************************
* Function Name  : rtc_set_time
* Description    : Set RTC counter value
* Input          : Time
* Output         : None
* Return         : -1 in case of error
*******************************************************************************/
void rtc_set_time(time_t Tme)
{
  PWR_BackupAccessCmd(ENABLE);
  RTC_WaitForLastTask();
  RTC_SetCounter(Tme);
  RTC_WaitForLastTask();
  PWR_BackupAccessCmd(DISABLE);
}

/*******************************************************************************
* Function Name  : rtc_set_alarm
* Description    : Set the RTC alarm value
* Input          : Tme: Number of seconds past since 1-Jan-70
* Output         : None
* Return         : None
*******************************************************************************/
void rtc_set_alarm(time_t Tme)
{
  PWR_BackupAccessCmd(ENABLE);
  RTC_WaitForLastTask();
  RTC_SetAlarm(Tme);
  RTC_WaitForLastTask();
  PWR_BackupAccessCmd(DISABLE);
}

/*******************************************************************************
* Function Name  : rtc_get_alarm
* Description    : Get the RTC alarm value
* Input          : None
* Output         : None
* Return         : RTC alarm value
*******************************************************************************/
time_t rtc_get_alarm(void)
{
  u16 tmp = 0;

  tmp = RTC->ALRL;
  return(((time_t)RTC->ALRH << 16 ) | tmp);
}

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
