/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : adc.c
* Author             : Arnaud LE GAC / Yoann TREGUIER
* Version            : 1.2.0
* Date               : 2013/06/14
* Description        : Headers of ADC driver functions
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H
#define __ADC_H

/* Includes ------------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
enum {
  ADC_MODE_NORMAL,
  ADC_MODE_CONTINUOUS,
  ADC_MODE_MULTIPLE,
  ADC_MODE_NB
  };

/* Exported types ------------------------------------------------------------*/
typedef __packed struct
{
  ADC_TypeDef *ADCx;
  uint8_t Channel;
  const _Gpio_Descriptor *Gpio;
} _Adc_Descriptor;

/* Exported macro ------------------------------------------------------------*/
#define adc_open_normal(Adc)  adc_open(Adc, ADC_MODE_NORMAL, 0, 0, 0)
#define adc_read_conv(Adc)    ADC_GetConversionValue(Adc->ADCx)

/* Exported constants --------------------------------------------------------*/
extern const _Adc_Descriptor Adc_Descriptor[];

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void adc_startup_init(void);
int adc_open(const _Adc_Descriptor *Adc, u8 Mode, f32 Tsample, void *WrPtr, u16 Nsample);
void adc_close(const _Adc_Descriptor *Adc);
void adc_start(const _Adc_Descriptor *Adc);
int adc_wait(const _Adc_Descriptor *Adc);
u16 adc_get_conv(const _Adc_Descriptor *Adc, u8 Auto);
f32 adc_get_ratio(const _Adc_Descriptor *Adc, u8 Auto);
f32 adc_get_volt(const _Adc_Descriptor *Adc, u8 Auto);

#endif /* __ADC_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
