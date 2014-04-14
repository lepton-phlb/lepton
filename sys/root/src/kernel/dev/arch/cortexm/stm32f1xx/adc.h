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
