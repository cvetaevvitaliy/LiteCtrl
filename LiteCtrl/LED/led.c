#include "stm32l0xx_hal.h"
#include "led.h"
#include "sz_libc.h"
enum
{
	FLASH_OPEN = 0,
	FLASH_CLOSE = 1,
};
typedef struct _STR_LED
{
	unsigned int start_miliseconds;
	unsigned int state;
	unsigned int old_state;
	unsigned int substate;
	unsigned int open_miliseconds;
	unsigned int close_miliseconds;
	GPIO_TypeDef *pStrGPIO;
	uint16_t GPIO_PPIN;
	int route_level;
}STR_LED;

static unsigned long led_get_tick();
static void led_open(STR_LED *led)
{
	//led->pStrGPIO->BSRR = led->GPIO_PPIN;
	//HAL_GPIO_WritePin(led->pStrGPIO,led->GPIO_PPIN,GPIO_PIN_SET);
	if(led->route_level!=0)
	    led->pStrGPIO->BSRR = led->GPIO_PPIN;
	else
		led->pStrGPIO->BRR = led->GPIO_PPIN;
}

static void led_close(STR_LED *led)
{
	//led->pStrGPIO->BRR = led->GPIO_PPIN;
	//HAL_GPIO_WritePin(led->pStrGPIO,led->GPIO_PPIN,GPIO_PIN_RESET);
	if(led->route_level==0)
	    led->pStrGPIO->BSRR = led->GPIO_PPIN;
	else
		led->pStrGPIO->BRR = led->GPIO_PPIN;
}

static void LED_DO(STR_LED *led)
{
	unsigned long cur_tick;
	switch(led->state)
	{
		case ALLAY_OPEN:
				led_open(led);
				led->state = IDLE;
				led->substate = FLASH_OPEN;
				//sz_printk("open the led\n");
				break;
		case ALLAY_CLOSE:
				led_close(led);
				led->state = IDLE;
				led->substate = FLASH_CLOSE;
				break;
		case ALLAY_FLASH:			
			switch(led->substate)
			{
				case FLASH_OPEN:
					cur_tick = led_get_tick();
					if((cur_tick-led->start_miliseconds >led->open_miliseconds))
					{
						led->start_miliseconds = cur_tick;
						led_open(led);
						led->substate = FLASH_CLOSE;
					}
					break;
				case FLASH_CLOSE:
					cur_tick = led_get_tick();
					if((cur_tick-led->start_miliseconds >led->close_miliseconds))
					{
						led->start_miliseconds = cur_tick;
						led_close(led);
						led->substate = FLASH_OPEN;
					}
				default:
					break;
			}
			break;
		case ONE_FLASH_ON:
			switch(led->substate)
			{
				case FLASH_OPEN:
					cur_tick = led_get_tick();
					if((cur_tick-led->start_miliseconds >150))
					{
						led->start_miliseconds = cur_tick;
						led_open(led);
						led->substate = FLASH_CLOSE;
					}
					break;
				case FLASH_CLOSE:
					cur_tick = led_get_tick();
					if((cur_tick-led->start_miliseconds >5))
					{
						led->start_miliseconds = cur_tick;
						led_close(led);
						led->substate = FLASH_OPEN;
						led->state = led->old_state;
						//sz_printk("led_new_sate:%d\n",	led->state );
					}
					break;
				default:
					break;
			}
		    break;
		default:
			break;
	}
}

static STR_LED led0=
{
	/*.start_miliseconds=*/0,
	/*.state=*/ALLAY_CLOSE,
	/*.old_state=*/ALLAY_CLOSE,
   /* .substate=*/FLASH_OPEN,
   /* .open_miliseconds=*/10,
   /* .close_miliseconds=*/90,
    /*.pStrGPIO=*/GPIOA,
    /*.GPIO_PPIN=*/GPIO_PIN_2,
   /* .route_level=*/1
};

void ShowLeds()
{
	LED_DO(&led0);
}


void SetLedState(int which,int state)
{
	STR_LED *pled;
	switch(which)
	{
		case 0:
			pled = &led0;
			break;
		default:
			break;
	}
	if(pled != 0 && state < LED_STA_END)
	{
		if(pled->state == ONE_FLASH_ON)
			return;
		if(state == ONE_FLASH_ON)
		{
			if(pled->state == IDLE)
		    {
			    if(pled->substate==FLASH_OPEN)
			    {
					pled->old_state = ALLAY_OPEN;
			    }
				else
				{
					pled->old_state = ALLAY_CLOSE;
				}
			}
			else
			{
				pled->old_state = pled->state;
			}
			pled->substate=FLASH_OPEN;
			led_close(pled);
			//sz_printk("led_old_sate:%d\n",	pled->old_state);
		}
		pled->state = state;
		pled->start_miliseconds = led_get_tick();
	}
}

//void SetLedFlashTime(int which,int miliseconds)
//{
//	switch(which)
//	{
//		case 0:
//			led0.open_miliseconds = miliseconds;
//			break;
//
//		default:
//			break;
//	}
//}

static unsigned long led_tick = 0;
static unsigned long max_led_tick = 0x7fffffff;
static void led_time_calculate(unsigned int time)
{
	led_tick -= time;
	led0.start_miliseconds -=time;
}


void led_clk()
{
	led_tick++;
	if(led_tick>max_led_tick)
	{
		led_time_calculate(max_led_tick);
	}
		
}

static unsigned long led_get_tick()
{
	return led_tick;
}


void led_init()
{
    GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
}

