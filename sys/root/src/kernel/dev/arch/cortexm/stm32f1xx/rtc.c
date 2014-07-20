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
