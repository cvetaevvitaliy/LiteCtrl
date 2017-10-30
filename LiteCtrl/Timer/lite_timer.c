#include "lite_timer.h"
#include "stm32l0xx_hal.h"
#include "sz_libc.h"
#include "lite_timer.h"
#include "trf796xx_module.h"

static TIM_HandleTypeDef    TimHandle;
static TIM_MasterConfigTypeDef sMasterConfig;
void lite_timer_init()
{
	unsigned int uwPrescalerValue = 0;
		__HAL_RCC_TIM6_CLK_ENABLE();
    uwPrescalerValue = (unsigned int) ((SystemCoreClock / 100000) - 1);//10000for10k
    TimHandle.Instance = TIM6;
    TimHandle.Init.Period = 10000;
    TimHandle.Init.Prescaler = uwPrescalerValue;
    TimHandle.Init.ClockDivision = 0;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	if(HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    {
        sz_printk("Timer init fail\n");
    }	
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&TimHandle, &sMasterConfig);	
	
}


void lite_enable_timer()
{
	HAL_TIM_Base_Start_IT(&TimHandle);
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 3, 3);
  HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
void lite_disable_timer()
{
	HAL_TIM_Base_Stop_IT(&TimHandle);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
}

static int cnt=0;
void TIM6_DAC_IRQHandler()
{
//	int ss=HAL_GetTick();
//	sz_printk("timer interrup:%dms\n",ss-cnt);
//	cnt = ss;
    send_read_uid(0);
	HAL_TIM_IRQHandler(&TimHandle);
	lite_disable_timer();
}