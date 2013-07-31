/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : tmr.h
* Author             : Yoann TREGUIER
* Version            : 1.2.0
* Date               : 2013/06/14
* Description        : Headers of timers interface functions
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TMR_H
#define __TMR_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef __packed struct
{
  u8 Status;
  u16 Counter;
  const void *Desc;
  u16 Event;
  //OS_TID Target;
} _Tmr_Ctrl;

typedef __packed struct
{
  TIM_TypeDef* TIMx;
  u8 APBx;
  IRQn_Type IRQn;
  void (*RCC_APBxPeriphClockCmd)(uint32_t RCC_APB2Periph, FunctionalState NewState);
  u32 RCC_APBxPeriph;
  _Tmr_Ctrl *Ctrl;
} _Tmr_Descriptor;

typedef __packed struct
{
  const _Gpio_Descriptor *Gpio;
  const _Tmr_Descriptor *Tmr;
  u16 Channel;
} _Pwm_Descriptor;

/* Exported define -----------------------------------------------------------*/
#define TMR_STAT_UNUSED   0x00
#define TMR_STAT_IN_USE   0x01
#define TMR_OPT_EVENT     0x02
#define TMR_OPT_TRIGGER   0x04
#define TMR_OPT_PWM       0x08

#define TMR_INIT_OPT_NONE     0x00
#define TMR_INIT_OPT_START    0x01
#define TMR_INIT_OPT_UNWIND   0x02

/* Exported macro ------------------------------------------------------------*/
#define TIM_GetAutoReload(TIMx)   (TIMx->PSC)

#define tmr_startup_init()  memset(Tmr_Ctrl, 0, TMR_NB * sizeof(_Tmr_Ctrl))

#define tmr_start(Tmr)          TIM_Cmd(Tmr->TIMx, ENABLE)
#define tmr_stop(Tmr)           TIM_Cmd(Tmr->TIMx, DISABLE)
#define tmr_set_cnt(Tmr, Cnt)   TIM_SetCounter(Tmr->TIMx, Cnt)
#define tmr_rewind(Tmr)         tmr_set_cnt(Tmr, 0)
#define tmr_unwind(Tmr)         tmr_set_cnt(Tmr, TIM_GetAutoReload(Tmr->TIMx))
#define tmr_wait(Tmr)           while (Tmr->Ctrl->Counter)

#define tmr_open_event(Tmr, Period)       tmr_open(Tmr, TMR_OPT_EVENT, Period, 0, 0)

#define tmr_open_trigger(Tmr, Period)     tmr_open(Tmr, TMR_OPT_TRIGGER, Period, 0, 0)

#define tmr_open_pwm(Pwm, Period, Toggle) tmr_open(((_Pwm_Descriptor *)Pwm)->Tmr, TMR_OPT_PWM, Period, Toggle, Pwm)
#define tmr_close_pwm(Pwm)                tmr_close(((_Pwm_Descriptor *)Pwm)->Tmr)
#define tmr_start_pwm(Pwm)                TIM_Cmd(((_Pwm_Descriptor *)Pwm)->Tmr->TIMx, ENABLE)
#define tmr_stop_pwm(Pwm)                 TIM_Cmd(((_Pwm_Descriptor *)Pwm)->Tmr->TIMx, DISABLE)
#define tmr_init_pwm(Pwm, Count, Opt)     tmr_init(((_Pwm_Descriptor *)Pwm)->Tmr, Count, Opt)
#define tmr_wait_pwm(Pwm)                 while (((_Pwm_Descriptor *)Pwm)->Tmr->Ctrl->Counter)

/* Exported constants --------------------------------------------------------*/
extern const _Tmr_Descriptor Tmr_Descriptor[];
extern const _Pwm_Descriptor Pwm_Descriptor[];

/* Exported variables --------------------------------------------------------*/
extern _Tmr_Ctrl Tmr_Ctrl[TMR_NB];

/* Exported functions --------------------------------------------------------*/
u32 tmr_get_clock(const _Tmr_Descriptor *Tmr);
int tmr_open(const _Tmr_Descriptor *Tmr, u8 Opt, f32 Period, f32 Toggle, const void *Desc);
void tmr_close(const _Tmr_Descriptor *Tmr);
void tmr_irq_handler(const _Tmr_Descriptor *Tmr);
void tmr_init(const _Tmr_Descriptor *Tmr, u16 Count, u8 Opt);

#endif /* __TMR_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
