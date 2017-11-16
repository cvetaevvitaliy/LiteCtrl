#include "stm32l0xx_hal.h"
#include "sz_libc.h"
#include "sysmode.h"

void init_sysmde_pin()
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Pin = (GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW  ;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

int read_sysmode()
{
	int res = 0;
	if(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15))
	{
		res |=0x01;
	}
	if(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14))
	{
		res |=(0x01<<1);
	}
	if(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13))
	{
		res |=(0x01<<2);
	}
	printk_red("sysmod=%#x\n",res);
	return res;
}

