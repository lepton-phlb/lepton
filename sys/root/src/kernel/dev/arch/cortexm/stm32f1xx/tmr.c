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
#include "gpio.h"
#include "tmr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM_IT_ALL  (TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4 | TIM_IT_COM | TIM_IT_Trigger | TIM_IT_Break)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
_Tmr_Ctrl Tmr_Ctrl[TMR_NB];

/* Private constants ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : tmr_get_clock
* Description    : Get the clock frequency that feeds a timer
* Input          : - Tmr: Select the hardware timer
* Output         : None
* Return         : Clock frequency
*******************************************************************************/
u32 tmr_get_clock(const _Tmr_Descriptor *Tmr)
{
  RCC_ClocksTypeDef rcc_clocks;

  RCC_GetClocksFreq(&rcc_clocks);
  if (Tmr->APBx == 1)
  {
    if (rcc_clocks.PCLK1_Frequency == rcc_clocks.HCLK_Frequency) return(rcc_clocks.PCLK1_Frequency);
    else return(rcc_clocks.PCLK1_Frequency * 2);
  }
  if (Tmr->APBx == 2)
  {
    if (rcc_clocks.PCLK2_Frequency == rcc_clocks.HCLK_Frequency) return(rcc_clocks.PCLK2_Frequency);
    else return(rcc_clocks.PCLK2_Frequency * 2);
  }
  return(0);
}

/*******************************************************************************
* Function Name  : tmr_open
* Description    : Initialize a hardware timer
* Input          : - Desc: Select the peripheral to open
*                : - Opt: Timer options (bit field)
*                : - Period: Timer period for update event (in Sec)
*                : - Toggle: PWM transition (in Sec)
*                : - Desc: Optional peripheral descriptor (ex: PWM)
* Output         : None
* Return         : -1 if faulty
*******************************************************************************/
int tmr_open(const _Tmr_Descriptor *Tmr, u8 Opt, f32 Period, f32 Toggle, const void *Desc)
{
  TIM_TimeBaseInitTypeDef tim_time_base_structure;
  u32 clock;
  u32 counter;

  clock = tmr_get_clock(Tmr);
  counter = Period * clock;
  if (!counter) return(-1);
  while (Tmr->Ctrl->Status != TMR_STAT_UNUSED) sys_wait(1);
  Tmr->Ctrl->Status = TMR_STAT_IN_USE;
  Tmr->Ctrl->Status |= Opt;
  Tmr->Ctrl->Desc = Desc;

  // Timer configuration
  Tmr->RCC_APBxPeriphClockCmd(Tmr->RCC_APBxPeriph, ENABLE);
  TIM_DeInit(Tmr->TIMx);
  TIM_TimeBaseStructInit(&tim_time_base_structure);
  tim_time_base_structure.TIM_Prescaler = (counter >> 16) & 0xFFFF;
  tim_time_base_structure.TIM_Period = counter / (tim_time_base_structure.TIM_Prescaler + 1);
  TIM_TimeBaseInit(Tmr->TIMx, &tim_time_base_structure);

  // Interrupt channel configuration
  TIM_ITConfig(Tmr->TIMx, TIM_IT_ALL, DISABLE);
  TIM_ClearITPendingBit(Tmr->TIMx, TIM_IT_ALL);
  NVIC_EnableIRQ(Tmr->IRQn);

  // Event signal configuration
  if (Opt & TMR_OPT_EVENT)
  {
    Tmr->Ctrl->Event = SYS_EVT_TIMER;
    Tmr->Ctrl->Target = sys_task_self();
  }

  // Trigger configuration
  if (Opt & TMR_OPT_TRIGGER) TIM_SelectOutputTrigger(Tmr->TIMx, TIM_TRGOSource_Update);

  // PWM configuration
  if (Opt & TMR_OPT_PWM)
  {
    TIM_OCInitTypeDef tim_oc_init_structure;
    _Pwm_Descriptor *pwm = (_Pwm_Descriptor *)Desc;

    // PWM channel configuration
    gpio_set_mode(pwm->Gpio, GPIO_MODE_AF, 0);
    counter = Toggle * clock;
    TIM_OCStructInit(&tim_oc_init_structure);
    tim_oc_init_structure.TIM_OCMode = TIM_OCMode_PWM2;
    tim_oc_init_structure.TIM_OutputState = TIM_OutputState_Enable;
    tim_oc_init_structure.TIM_Pulse = counter / (tim_time_base_structure.TIM_Prescaler + 1);
    if (pwm->Channel == TIM_Channel_4) TIM_OC4Init(Tmr->TIMx, &tim_oc_init_structure);
    else if (pwm->Channel == TIM_Channel_3) TIM_OC3Init(Tmr->TIMx, &tim_oc_init_structure);
    else if (pwm->Channel == TIM_Channel_2) TIM_OC2Init(Tmr->TIMx, &tim_oc_init_structure);
    else TIM_OC1Init(Tmr->TIMx, &tim_oc_init_structure);
    TIM_CtrlPWMOutputs(Tmr->TIMx, ENABLE);
  }

  return(0);
}

/*******************************************************************************
* Function Name  : tmr_close
* Description    : Release timer
* Input          : - Tmr: Select the timer to close
* Output         : None
* Return         : None
*******************************************************************************/
void tmr_close(const _Tmr_Descriptor *Tmr)
{
  TIM_DeInit(Tmr->TIMx);
  NVIC_DisableIRQ(Tmr->IRQn);
  Tmr->RCC_APBxPeriphClockCmd(Tmr->RCC_APBxPeriph, DISABLE);
  if (Tmr->Ctrl->Status & TMR_OPT_PWM) gpio_init(((_Pwm_Descriptor *)Tmr->Ctrl->Desc)->Gpio);
  memset(Tmr->Ctrl, 0, sizeof(_Tmr_Ctrl));
}

/*******************************************************************************
* Function Name  : tmp_irq_handler
* Description    : Interrupt handle function
* Input          : - Tmr: Select the hardware timer.
* Output         : None
* Return         : None
*******************************************************************************/
void tmr_irq_handler(const _Tmr_Descriptor *Tmr)
{
  TIM_ClearITPendingBit(Tmr->TIMx, TIM_IT_Update);
  if (Tmr->Ctrl->Event) isr_evt_set(Tmr->Ctrl->Event, Tmr->Ctrl->Target);
  if (Tmr->Ctrl->Counter && !--Tmr->Ctrl->Counter) TIM_Cmd(Tmr->TIMx, DISABLE);
}

/*******************************************************************************
* Function Name  : tmr_init
* Description    : Start timer
* Input          : - Tmr: Select the hardware timer.
*                : - Count: Set the number of period to wait before stopping the timer
*                : - Opt: Init options (bit field)
* Output         : None
* Return         : None
*******************************************************************************/
void tmr_init(const _Tmr_Descriptor *Tmr, u16 Count, u8 Opt)
{
  TIM_Cmd(Tmr->TIMx, DISABLE);
  Tmr->Ctrl->Counter = Count;
  if (Tmr->Ctrl->Event) sys_evt_clr(Tmr->Ctrl->Event, Tmr->Ctrl->Target);
  if (Count || Tmr->Ctrl->Event)
  {
    TIM_ClearITPendingBit(Tmr->TIMx, TIM_IT_Update);
    TIM_ITConfig(Tmr->TIMx, TIM_IT_Update, ENABLE);
  }
  if (Opt & TMR_INIT_OPT_UNWIND) tmr_unwind(Tmr);
  else tmr_rewind(Tmr);
  if (Opt & TMR_INIT_OPT_START) TIM_Cmd(Tmr->TIMx, ENABLE);
}

