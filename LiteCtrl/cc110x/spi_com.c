#include "stm32l0xx_hal.h"
#include "ring_buffer.h"
#include "spi_com.h"
#include "module_common.h"

//HAL_SPI_TransmitReceive_DMA

/*******************************************************************************
*function:wirte a data to slave  
*p_spi:a spi device context
*data:the data you wanto write
*ext_flg:some special data need more than one or more clk
*******************************************************************************/

static int ll_spi_write_read(struct SPI_t *p_spi,int value,int ext_flg)
{
	int i;
	int data_size = p_spi->data_size;
	int mask;
	int ret_v = 0;
	
	if(p_spi->first == MSB)
	{
		mask = (data_size == DATA_SZIE_8?0x80:0x8000);
		for(i=0;i<data_size;i++)
    	{	
    		if((value&mask)!=0)
				p_spi->mosi(p_spi->priv,1);
			else
				p_spi->mosi(p_spi->priv,0);
			value  <<= 1;
			p_spi->delay(p_spi->clk_all/3);
			p_spi->clk(p_spi->priv,1);
			p_spi->delay(p_spi->clk_all/3);//just wait a little time wait for the clk stateble
			ret_v <<= 1;
			if(p_spi->miso(p_spi->priv)==1)
				ret_v=ret_v|0x01;
			p_spi->clk(p_spi->priv,0);
			p_spi->delay(p_spi->clk_all/3);
//			p_spi->delay(p_spi->clk_all/6);
    	}
	}
	else
	{
		mask = 0x01;
		
    }
	for(i=0;i<ext_flg;i++)
	{
		p_spi->clk(p_spi->priv,0);
		p_spi->delay(p_spi->clk_half);
		p_spi->clk(p_spi->priv,1);
		p_spi->delay(p_spi->clk_half);
	}
	p_spi->delay(p_spi->clk_all);
	return ret_v;
}
/*********************************************************************************/

int ll_spi_write(struct SPI_t *p_spi,int regaddr,const char *src,int len,int ext_flg)
{
	const char *end = src+len;
	DIS_ALL_INT();
	p_spi->chip_select(p_spi->priv,0);
	p_spi->delay(p_spi->clk_all);
	while(p_spi->miso(p_spi->priv)==1);
	ll_spi_write_read(p_spi,regaddr,ext_flg);
	while(src<end)
	{
		ll_spi_write_read(p_spi,(int)(*src),ext_flg);
		++src;
	}
	p_spi->delay(p_spi->clk_all);
	p_spi->chip_select(p_spi->priv,1);
	p_spi->delay(p_spi->clk_all);
	EN_ALL_INT();
	return len;
}

int ll_spi_read(struct SPI_t *p_spi,int regaddr,char *src,int len,int ext_flg)
{
	char *end = src+len;
    DIS_ALL_INT();
	p_spi->chip_select(p_spi->priv,0);
	p_spi->delay(p_spi->clk_all);
	while(p_spi->miso(p_spi->priv  )==1);
	ll_spi_write_read(p_spi,regaddr,ext_flg);
	while(src<end)
	{
		*src = ll_spi_write_read(p_spi,0x00,ext_flg);
		++src;
	}
	p_spi->delay(p_spi->clk_all);
	p_spi->chip_select(p_spi->priv,1);
	p_spi->delay(p_spi->clk_all);
	EN_ALL_INT();
	return len;
}

int  ll_read_t(struct SPI_t *p_spi,int regaddr,int ext_flg)
{
	int ret_;
	DIS_ALL_INT();
	p_spi->chip_select(p_spi->priv,0);
	p_spi->delay(p_spi->clk_all);
	while(p_spi->miso(p_spi->priv)==1);
	ll_spi_write_read(p_spi,regaddr,ext_flg);
	ret_= ll_spi_write_read(p_spi,0x00,ext_flg);
	p_spi->delay(p_spi->clk_all);
	p_spi->chip_select(p_spi->priv,1);
	p_spi->delay(p_spi->clk_all);
	EN_ALL_INT();
	return ret_;
}

int  ll_write_t(struct SPI_t *p_spi,int regaddr,int value,int ext_flg)
{
	DIS_ALL_INT();
	p_spi->chip_select(p_spi->priv,0);
	p_spi->delay(p_spi->clk_all);
	while(p_spi->miso(p_spi->priv)==1);
	ll_spi_write_read(p_spi,regaddr,ext_flg);
	ll_spi_write_read(p_spi,value,ext_flg);
	p_spi->delay(p_spi->clk_all);
	p_spi->chip_select(p_spi->priv,1);
	p_spi->delay(p_spi->clk_all);
	EN_ALL_INT();
	return 0;
}

int ll_write_strobe(struct SPI_t *p_spi,int regaddr,int ext_flg)
{
	DIS_ALL_INT();
	p_spi->chip_select(p_spi->priv,0);
	p_spi->delay(p_spi->clk_all);
	while(p_spi->miso(p_spi->priv)==1);
	ll_spi_write_read(p_spi,regaddr,ext_flg);
	p_spi->delay(p_spi->clk_all);
	p_spi->chip_select(p_spi->priv,1);
	p_spi->delay(p_spi->clk_all);
	EN_ALL_INT();
	return 0;
}

int ll_write_sp_cmd(struct SPI_t *p_spi,int regaddr,int ext_flg)
{
	DIS_ALL_INT();
	p_spi->chip_select(p_spi->priv,0);
	p_spi->delay(p_spi->clk_all);
	while(p_spi->miso(p_spi->priv)==1);
	ll_spi_write_read(p_spi,regaddr,ext_flg);
	while(p_spi->miso(p_spi->priv)==1);
	p_spi->delay(p_spi->clk_all);
	p_spi->chip_select(p_spi->priv,1);
	p_spi->delay(p_spi->clk_all);
	EN_ALL_INT();
	return 0;
}

int ll_chip_select(struct SPI_t *p_spi,int level)
{
	p_spi->chip_select(p_spi->priv,level);
	return 0;
}