#ifndef _SPI_COM_H_
#define _SPI_COM_H_
#include "stm32l0xx_hal.h"
enum 
{
	LSB = 0,
	MSB = 1
};
enum
{
	DATA_SZIE_8  = 8,
	DATA_SZIE_16 = 16,	
};
struct SPI_pin_t
{
	GPIO_TypeDef *pStrGPIO;
	uint16_t GPIO_PIN;
};
struct SPI_t
{
	char data_size;	
	char first;
	void (*chip_select)(void *,int);
	void (*clk)(void *,int);
	void (*mosi)(void *,int);
	int  (*miso)(void *);
	void (*delay)(int ten_us);
	int  clk_all;
	int  clk_half;//clk_half=clk_all/2
	int  clk_s;//clk_s=clk_half - filter_s
	int  filter_s;//filter_s = clk_half/10
	void *priv;
};


static void spi_chip_select(void *priv,int level)
{
	struct SPI_pin_t *p_pin = (struct SPI_pin_t *)priv;
	if(0==level)
	{
		HAL_GPIO_WritePin(p_pin[0].pStrGPIO,p_pin[0].GPIO_PIN,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(p_pin[0].pStrGPIO,p_pin[0].GPIO_PIN,GPIO_PIN_SET);
	}
}

static void spi_clk(void *priv,int level)
{
	struct SPI_pin_t *p_pin = (struct SPI_pin_t *)priv;
	if(1==level)
	{
		HAL_GPIO_WritePin(p_pin[1].pStrGPIO,p_pin[1].GPIO_PIN,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(p_pin[1].pStrGPIO,p_pin[1].GPIO_PIN,GPIO_PIN_RESET);
	}
}

static int  spi_miso(void *priv)
{
	struct SPI_pin_t *p_pin = (struct SPI_pin_t *)priv;
	return (int)HAL_GPIO_ReadPin(p_pin[2].pStrGPIO,p_pin[2].GPIO_PIN);
}

static void spi_mosi(void *priv,int level)
{
	struct SPI_pin_t *p_pin = (struct SPI_pin_t *)priv;
	if(1==level)
	{
		HAL_GPIO_WritePin(p_pin[3].pStrGPIO,p_pin[3].GPIO_PIN,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(p_pin[3].pStrGPIO,p_pin[3].GPIO_PIN,GPIO_PIN_RESET);
	}
}



static void spi_delay(int ten_us)
{
	while(ten_us--);
}

extern int ll_spi_write(struct SPI_t *p_spi,int regaddr,const char *src,int len,int ext_flg);
extern int ll_spi_read(struct SPI_t *p_spi,int regaddr,char *src,int len,int ext_flg);
extern int ll_read_t(struct SPI_t *p_spi,int regaddr,int ext_flg);
extern int ll_write_t(struct SPI_t *p_spi,int regaddr,int value,int ext_flg);
extern int ll_write_strobe(struct SPI_t *p_spi,int regaddr,int ext_flg);
extern int ll_write_sp_cmd(struct SPI_t *p_spi,int regaddr,int ext_flg);
extern int ll_chip_select(struct SPI_t *p_spi,int level);
#endif
