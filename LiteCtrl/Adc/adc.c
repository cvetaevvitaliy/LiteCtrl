#include "stm32l0xx_hal.h"
#include "delay_work.h"
#include "sz_libc.h"

//over write the HAL_ADC_MspInit function 1.
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef                 GPIO_InitStruct;
  RCC_OscInitTypeDef        RCC_OscInitStructure;

  HAL_RCC_GetOscConfig(&RCC_OscInitStructure);
  RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStructure.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStructure.HSIState = RCC_HSI_ON;
  HAL_RCC_OscConfig(&RCC_OscInitStructure);

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* ADCx Channel GPIO pin configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /*##-3- Configure the DMA ##################################################*/

  /* ADC does not use DMA in this ADC example */

  /*##-4- Configure the NVIC #################################################*/

  /* NVIC configuration for ADC interrupt */
  /* Priority: high-priority */
  //HAL_NVIC_SetPriority(ADC1_COMP_IRQn, 2, 1);
  //HAL_NVIC_EnableIRQ(ADC1_COMP_IRQn);
}

//over write the HAL_ADC_MspDeInit function 2.
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  /*##-1- Reset peripherals ##################################################*/
 __HAL_RCC_ADC1_FORCE_RESET();
  __HAL_RCC_ADC1_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* De-initialize the ADC Channel GPIO pin */
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);

  /*##-3- Disable the DMA ####################################################*/

  /* ADC does not use DMA in this ADC example */

  /*##-4- Disable the NVIC ###################################################*/  
  /* Disable the NVIC configuration for ADC interrupt */
  HAL_NVIC_DisableIRQ(ADC1_COMP_IRQn);
}

//over write the HAL_TIM_Base_MspInit function 3.
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  /* TIM peripheral clock enable */
  __HAL_RCC_TIM2_CLK_ENABLE();
}

//over write the HAL_TIM_Base_MspDeInit function
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
  /*##-1- Reset peripherals ##################################################*/
  __HAL_RCC_TIM2_FORCE_RESET();
  __HAL_RCC_TIM2_RELEASE_RESET();
}




#define TIMER_FREQUENCY_HZ 10
#define VDD_APPLI                      ((uint32_t)3000)    /* Value of analog voltage supply Vdda (unit: mV) */
#define RANGE_12BITS                   ((uint32_t)4095)    /* Max value with a full range of 12 bits */
#define COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(ADC_DATA)                        \
  ( (ADC_DATA) * VDD_APPLI / RANGE_12BITS)
struct delay_work adc_delay;
TIM_HandleTypeDef    TimHandle;
ADC_HandleTypeDef AdcHandle;

unsigned short get_adc_value()
{
	__IO uint16_t value;
	value = HAL_ADC_GetValue(&AdcHandle);
	return COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(value);
}

//#define ADC_LOWPOWER
static void init_ADC1_channel2()
{
  ADC_ChannelConfTypeDef   sConfig;
  
  /* Configuration of ADCx init structure: ADC parameters and regular group */
	
	__HAL_RCC_ADC1_CLK_ENABLE();
  AdcHandle.Instance = ADC1;

  AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV4;
  AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;    /* Sequencer will convert the number of channels configured below, successively from the lowest to the highest channel number */
  AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
#if defined(ADC_LOWPOWER)
  AdcHandle.Init.LowPowerAutoWait      = ENABLE;                        /* Enable the dynamic low power Auto Delay: new conversion start only when the previous conversion (for regular group) or previous sequence (for injected group) has been treated by user software. */
  AdcHandle.Init.LowPowerAutoPowerOff  = ENABLE;                        /* Enable the auto-off mode: the ADC automatically powers-off after a conversion and automatically wakes-up when a new conversion is triggered (with startup time between trigger and start of sampling). */
#else
  AdcHandle.Init.LowPowerAutoWait      = DISABLE;
  AdcHandle.Init.LowPowerAutoPowerOff  = DISABLE;
#endif
  AdcHandle.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
  AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T2_TRGO;  /* Trig of conversion start done by external event */
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;
  AdcHandle.Init.DMAContinuousRequests = ENABLE;
  AdcHandle.Init.Overrun               = ADC_OVR_DATA_PRESERVED;            /* Overrun set to data preserved to trig overrun event as an error in this exemple */
  AdcHandle.Init.SamplingTime          = ADC_SAMPLETIME_41CYCLES_5;

  HAL_ADC_DeInit(&AdcHandle); //HAL_ADC_MspDeInit
  HAL_ADC_Init(&AdcHandle) ;//it will be call HAL_ADC_MspInit  
 
  /* Configuration of channel on ADCx regular group on sequencer rank 1 */
  /* Note: Considering IT occurring after each ADC conversion (ADC IT         */
  /*       enabled), select sampling time and ADC clock with sufficient       */
  /*       duration to not create an overhead situation in IRQHandler.        */
  sConfig.Channel      = ADC_CHANNEL_2;
  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
  HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
}


static void init_timer2()
{
  TIM_MasterConfigTypeDef sMasterConfig;
  
  /* Time Base configuration */
  TimHandle.Instance = TIM2;
  
  /* Configure timer frequency */
  /* Note: Setting of timer prescaler to 30 to increase the maximum range     */
  /*       of the timer, to fit within timer range of 0xFFFF.                 */
  /*       Setting of reload period to SysClk/30 to maintain a base           */
  /*       frequency of 1us.                                                  */
  /*       With SysClk set to 2MHz, timer frequency (defined by label         */
  /*       TIMER_FREQUENCY_HZ range) is min=1Hz, max=33.3kHz.                 */
  /* Note: Timer clock source frequency is retrieved with function            */
  /*       HAL_RCC_GetPCLK1Freq().                                            */
  /*       Alternate possibility, depending on prescaler settings:            */
  /*       use variable "SystemCoreClock" holding HCLK frequency, updated by  */
  /*       function HAL_RCC_ClockConfig().                                    */
  TimHandle.Init.Period = ((HAL_RCC_GetPCLK1Freq() / (30 * ((uint32_t)TIMER_FREQUENCY_HZ))) - 1);
  TimHandle.Init.Prescaler = (30-1);
  TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  HAL_TIM_Base_DeInit(&TimHandle);
  HAL_TIM_Base_Init(&TimHandle);//HAL_TIM_Base_MspInit
  /* Timer TRGO selection */
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&TimHandle, &sMasterConfig);
}


void init_adc_timer()
{
	init_ADC1_channel2();
	HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED);
	
	init_timer2();
	HAL_TIM_Base_Start(&TimHandle);
	
	//__HAL_ADC_ENABLE_IT(&AdcHandle, (ADC_IT_OVR));
	HAL_ADC_Start(&AdcHandle);
	HAL_ADC_PollForConversion(&AdcHandle, (1000/TIMER_FREQUENCY_HZ));
	//__HAL_RCC_TIM2_CLK_ENABLE();
	//__HAL_RCC_ADC1_CLK_ENABLE();
}

void ADC1_COMP_IRQHandler(void)
{
  HAL_ADC_IRQHandler(&AdcHandle);
}

void adc_start()
{
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_ADC1_CLK_ENABLE();
	HAL_NVIC_EnableIRQ(ADC1_COMP_IRQn);
}

void adc_stop()
{
	//__HAL_RCC_ADC1_FORCE_RESET();
  //__HAL_RCC_ADC1_RELEASE_RESET();
	__HAL_RCC_TIM2_CLK_DISABLE();
	__HAL_RCC_ADC1_CLK_DISABLE();
	HAL_NVIC_DisableIRQ(ADC1_COMP_IRQn);
}

int get_channel()

{
	int value = get_adc_value();
	if(220<value && value<322)
	{
		value = 0;
	}
	else if(450<value && value<550)
	{
		value =  1;
	}
	else if(642<value && value<732)
	{
		value =  2;
	}
	else if(909<value && value<1009)
	{
		value =  3;
	}
	else if(1164<value && value<1264)
	{
		value =  4;
	}
	else
	{
		value = 5;
	}
    sz_printk("select channel:%d\n",value);
}
