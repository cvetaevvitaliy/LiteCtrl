#include "stm32l0xx_hal.h"
#include "cc110x_adapter.h"
#include "spi_com.h"


static struct SPI_t cc110x_s;
static struct SPI_pin_t cc110x_pins[4];
static void cc110x_delay(int delay)
{
//	while(delay--)
//		__nop();
}
static void cc100x_pin_init()
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Pin = (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
	HAL_GPIO_WritePin(GPIOA,(GPIO_PIN_4),GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA,(GPIO_PIN_5 | GPIO_PIN_7),GPIO_PIN_RESET);

	
	GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 

    //GDO0-PA3 GDO2-PA2
	//GPIO_InitStruct.Pin = (GPIO_PIN_2|GPIO_PIN_3);
//	GPIO_InitStruct.Pin = (GPIO_PIN_2);
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW  ;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 

    //IRQ
	GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(EXTI2_3_IRQn, 2, 2);
	
}

void cc110x_init()
{
	cc110x_s.data_size   = DATA_SZIE_8;
	cc110x_s.first       = MSB;
	cc110x_s.chip_select = spi_chip_select;//GPIO4   0
	cc110x_s.clk         = spi_clk;//GPIO5    1
	cc110x_s.miso        = spi_miso;//GPIO6   2
	cc110x_s.mosi        = spi_mosi;//GPIO7   3
	cc110x_s.delay       = cc110x_delay;
	cc110x_s.priv        = (void*)&cc110x_pins[0];

    cc110x_s.clk_all     = 3;
	cc110x_s.clk_half    = cc110x_s.clk_all/2;
	cc110x_s.filter_s    = cc110x_s.clk_all/10;
	cc110x_s.clk_s       = cc110x_s.clk_half - cc110x_s.filter_s;
	
	//spi_cs
	cc110x_pins[0].pStrGPIO = GPIOA;
	cc110x_pins[0].GPIO_PIN = GPIO_PIN_4;
	
	//spi_clk
	cc110x_pins[1].pStrGPIO = GPIOA;
	cc110x_pins[1].GPIO_PIN = GPIO_PIN_5;
	
	//MISO
	cc110x_pins[2].pStrGPIO = GPIOA;
	cc110x_pins[2].GPIO_PIN = GPIO_PIN_6;
	
	//MOSI
	cc110x_pins[3].pStrGPIO = GPIOA;
	cc110x_pins[3].GPIO_PIN = GPIO_PIN_7;
	cc100x_pin_init();
	/*input your init code here */
}

int cc110x_read(int regaddr,char *src,int len)
{
	regaddr =regaddr&0x3F;
	regaddr = regaddr|0xC0;
	return ll_spi_read(&cc110x_s,regaddr,src,len,0);
}

int cc110x_write(int regaddr,const char *src,int len)
{
	regaddr =regaddr&0x3F;
	regaddr =regaddr|0x40;
	return ll_spi_write(&cc110x_s,regaddr,src,len,0);
}

int cc110x_read_t(int regaddr)
{
	regaddr = regaddr &0x3F;
	regaddr = regaddr | 0x80;
	return ll_read_t(&cc110x_s,regaddr,0);
}

int cc110x_write_t(int regaddr,int value)
{
	regaddr = regaddr &0x3F;
	return ll_write_t(&cc110x_s,regaddr,value,0);
}

int cc110x_read_status(int regaddr)
{
	regaddr = regaddr & 0x3F;
	regaddr = regaddr | 0xC0;
	return ll_read_t(&cc110x_s,regaddr,0);
}

int cc110x_write_strobe(int regaddr)
{
    return ll_write_strobe(&cc110x_s,regaddr,0);
}


int cc110x_write_sp_cmd(int cmd)
{
	return ll_write_sp_cmd(&cc110x_s,cmd,0);
}

int cc110x_reset(int cmd)
{
	ll_chip_select(&cc110x_s,1);//0 for enable 
	HAL_Delay(1);
	ll_chip_select(&cc110x_s,0);
	HAL_Delay(1);
	ll_chip_select(&cc110x_s,1);
	HAL_Delay(50);
	ll_write_sp_cmd(&cc110x_s,cmd,0);
}

void CC1101_irq_enable()
{
    HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
}

void CC1101_irq_disable()
{
    HAL_NVIC_DisableIRQ(EXTI2_3_IRQn);
}