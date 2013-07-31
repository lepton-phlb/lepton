/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : adc.c
* Author             : Arnaud LE GAC / Yoann TREGUIER
* Version            : 1.2.0
* Date               : 2013/06/14
* Description        : ADC driver functions
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "target.h"
#include "gpio.h"
#include "tmr.h"
#include "adc.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  u8 Mode;
  #ifdef _ADC_OS_SUPPORT
    OS_MUT Mutex;
  #endif
} _Adc_Ctrl;

/* Private define ------------------------------------------------------------*/
#define ADC_VREF  ADC_STD_VREF
#define ADC_TMR   TMR3
#define ADC1_DR_Address    ((u32)0x4001244C)

/* Private macro -------------------------------------------------------------*/
#define delay(x)  sys_delay(x)

/* Private variables ---------------------------------------------------------*/
_Adc_Ctrl Adc_Ctrl;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : adc_startup_init
* Description    : Initialize ADC peripherals
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void adc_startup_init(void)
{
  /* Set ADC clock according to max frequency (14MHz) */
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);

  /* Enable ABP2 ADC clocks */
  #if defined(_ADC_ADC2_SUPPORT) && defined(_ADC_ADC3_SUPPORT)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 | RCC_APB2Periph_ADC3, ENABLE);
  #elif defined(_ADC_ADC2_SUPPORT)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);
  #elif defined(_ADC_ADC3_SUPPORT)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC3, ENABLE);
  #else
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  #endif

  #ifdef _ADC_OS_SUPPORT
    /* Initialize mutex */
    sys_mut_init(Adc_Ctrl.Mutex);
  #endif
}

/*******************************************************************************
* Function Name  : adc_open
* Description    : Open and initialize ADC peripheral
* Input          : - Adc: Select the ADC peripheral
*                : - Mode: Adc run mode
*                : - Tsample: Adc sample period
*                : - WrPtr: DMA write address (ADC_MODE_CONTINUOUS & MODE_MULTIPLE only)
*                : - Nsample: Number of samples to acquire (MODE_MULTIPLE only)
* Output         : None
* Return         : -1 in case of error
*******************************************************************************/
int adc_open(const _Adc_Descriptor *Adc, u8 Mode, f32 Tsample, void *WrPtr, u16 Nsample)
{
  ADC_InitTypeDef adc_init_structure;
  RCC_ClocksTypeDef rcc_clocks;
  u8 sample_time;

  if ((Mode != ADC_MODE_NORMAL) && (Adc->ADCx != ADC1)) return(-1);

  #ifdef _ADC_OS_SUPPORT
    /* Wait for the ADC mutex to become available */
    sys_mut_wait(Adc_Ctrl.Mutex, T_NO_TIMEOUT);
  #endif
  Adc_Ctrl.Mode = Mode;

  /* ADC initialization */
  ADC_DeInit(Adc->ADCx);

  /* DMA configuration */
  if ((Mode == ADC_MODE_CONTINUOUS) || (Mode == ADC_MODE_MULTIPLE))
  {
    DMA_InitTypeDef dma_init_structure;

    DMA_DeInit(DMA1_Channel1);
    DMA_StructInit(&dma_init_structure);
    dma_init_structure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    dma_init_structure.DMA_MemoryBaseAddr = (u32)WrPtr;
    if (Mode == ADC_MODE_CONTINUOUS)
    {
      dma_init_structure.DMA_BufferSize = 1;
      dma_init_structure.DMA_Mode = DMA_Mode_Circular;
    }
    else
    {
      dma_init_structure.DMA_BufferSize = Nsample;
      dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    }
    dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma_init_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma_init_structure.DMA_Priority = DMA_Priority_High;
    DMA_Init(DMA1_Channel1, &dma_init_structure);
    DMA_Cmd(DMA1_Channel1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
  }

  /* TMR configuration */
  if (Mode == ADC_MODE_MULTIPLE)
  {
    tmr_open_trigger(ADC_TMR, Tsample);
    tmr_unwind(ADC_TMR);
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
  }

  /* ADC configuration */
  ADC_StructInit(&adc_init_structure);
  if (Mode == ADC_MODE_CONTINUOUS) adc_init_structure.ADC_ContinuousConvMode = ENABLE;
  if (Mode == ADC_MODE_MULTIPLE) adc_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
  else adc_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_Init(Adc->ADCx, &adc_init_structure);

  /* ADC channel configuration */
  RCC_GetClocksFreq(&rcc_clocks);
  if (!Tsample || (Tsample > (252 / (f32)rcc_clocks.ADCCLK_Frequency))) sample_time = ADC_SampleTime_239Cycles5;
  else if (Tsample > (84 / (f32)rcc_clocks.ADCCLK_Frequency)) sample_time = ADC_SampleTime_71Cycles5;
  else if (Tsample > (68 / (f32)rcc_clocks.ADCCLK_Frequency)) sample_time = ADC_SampleTime_55Cycles5;
  else if (Tsample > (54 / (f32)rcc_clocks.ADCCLK_Frequency)) sample_time = ADC_SampleTime_41Cycles5;
  else if (Tsample > (41 / (f32)rcc_clocks.ADCCLK_Frequency)) sample_time = ADC_SampleTime_28Cycles5;
  else if (Tsample > (26 / (f32)rcc_clocks.ADCCLK_Frequency)) sample_time = ADC_SampleTime_13Cycles5;
  else if (Tsample > (20 / (f32)rcc_clocks.ADCCLK_Frequency)) sample_time = ADC_SampleTime_7Cycles5;
  else sample_time = ADC_SampleTime_1Cycles5;
  if (Adc->Gpio) gpio_set_mode(Adc->Gpio, GPIO_MODE_AN, 0);
  else if (Adc->Channel > ADC_Channel_15) ADC_TempSensorVrefintCmd(ENABLE);
  ADC_RegularChannelConfig(Adc->ADCx, Adc->Channel, 1, sample_time);

  /* Enable ADC */
  ADC_Cmd(Adc->ADCx, ENABLE);

  /* ADC calibration */
  ADC_ResetCalibration(Adc->ADCx);
  while(ADC_GetResetCalibrationStatus(Adc->ADCx));
  ADC_StartCalibration(Adc->ADCx);
  while(ADC_GetCalibrationStatus(Adc->ADCx));

  return(0);
}

/*******************************************************************************
* Function Name  : adc_close
* Description    : Close and deactivate ADC peripheral
* Input          : - Adc: Select the ADC peripheral
* Output         : None
* Return         : None
*******************************************************************************/
void adc_close(const _Adc_Descriptor *Adc)
{
  /* ADC deactivation */
  ADC_DeInit(Adc->ADCx);

  /* TMR deactivation */
  if (Adc_Ctrl.Mode == ADC_MODE_MULTIPLE) tmr_close(ADC_TMR);

  /* DMA deactivation */
  if ((Adc_Ctrl.Mode == ADC_MODE_CONTINUOUS) || (Adc_Ctrl.Mode == ADC_MODE_MULTIPLE))
  {
    DMA_DeInit(DMA1_Channel1);
    DMA_Cmd(DMA1_Channel1, DISABLE);
  }

  /* Deactivate measurement channel */
  if (Adc->Gpio) gpio_init(Adc->Gpio);
  else if (Adc->Channel > ADC_Channel_15) ADC_TempSensorVrefintCmd(DISABLE);

  #ifdef _ADC_OS_SUPPORT
    /* Release the ADC mutex */
    sys_mut_release(Adc_Ctrl.Mutex);
  #endif
}

/*******************************************************************************
* Function Name  : adc_start
* Description    : Start AD conversion
* Input          : - Adc: Select the ADC peripheral
* Output         : None
* Return         : None
*******************************************************************************/
void adc_start(const _Adc_Descriptor *Adc)
{
  if (Adc_Ctrl.Mode == ADC_MODE_MULTIPLE) tmr_start(ADC_TMR);
  else ADC_SoftwareStartConvCmd(Adc->ADCx, ENABLE);
}

/*******************************************************************************
* Function Name  : adc_wait
* Description    : Wait for multiple AD conversion to finish
* Input          : - Adc: Select the ADC peripheral
* Output         : None
* Return         : Result of conversion
*******************************************************************************/
int adc_wait(const _Adc_Descriptor *Adc)
{
  if (Adc_Ctrl.Mode == ADC_MODE_CONTINUOUS) return(-1);
  if (Adc_Ctrl.Mode == ADC_MODE_MULTIPLE)
  {
    while (!DMA_GetFlagStatus(DMA1_FLAG_TC1))
    {
      #ifdef _ADC_OS_SUPPORT
        sys_wait(1);
      #endif
    }
    tmr_stop(ADC_TMR);
    DMA_ClearFlag(DMA1_FLAG_TC1);
  }
  else
  {
    while (ADC_GetFlagStatus(Adc->ADCx, ADC_FLAG_EOC) == RESET)
    {
      #ifdef _ADC_OS_SUPPORT
        sys_wait(1);
      #endif
    }
  }
  return(0);
}

/*******************************************************************************
* Function Name  : adc_get_conv
* Description    : Make an AD conversion
* Input          : - Adc: Select the ADC peripheral
*                : - Auto: Automated call to open and close functions
* Output         : None
* Return         : Result of conversion
*******************************************************************************/
u16 adc_get_conv(const _Adc_Descriptor *Adc, u8 Auto)
{
  u16 conv;

  if (Auto) {adc_open(Adc, ADC_MODE_NORMAL, 0, 0, 0); delay(1000);}
  else if (Adc_Ctrl.Mode != ADC_MODE_NORMAL) return(0xFFFF);
  ADC_SoftwareStartConvCmd(Adc->ADCx, ENABLE);
  while (ADC_GetFlagStatus(Adc->ADCx, ADC_FLAG_EOC) == RESET);
  conv = ADC_GetConversionValue(Adc->ADCx);
  if (Auto) adc_close(Adc);
  return(conv);
}

/*******************************************************************************
* Function Name  : adc_get_ratio
* Description    : Make an AD conversion
* Input          : - Adc: Select the ADC peripheral
*                : - Auto: Automated call to open and close functions
* Output         : None
* Return         : Ratio converted result of conversion
*******************************************************************************/
f32 adc_get_ratio(const _Adc_Descriptor *Adc, u8 Auto)
{
  return(((f32)adc_get_conv(Adc, Auto)) / 0xFFF);
}

/*******************************************************************************
* Function Name  : adc_get_volt
* Description    : Make an AD conversion
* Input          : - Adc: Select the ADC peripheral
*                : - Auto: Automated call to open and close functions
* Output         : None
* Return         : Voltage converted result of conversion
*******************************************************************************/
f32 adc_get_volt(const _Adc_Descriptor *Adc, u8 Auto)
{
  return(ADC_VREF * (((f32)adc_get_conv(Adc, Auto)) / 0xFFF));
}

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
