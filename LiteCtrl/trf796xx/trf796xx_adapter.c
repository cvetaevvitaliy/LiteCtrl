#include "trf796xx_adapter.h"
#include "sz_libc.h"
#include "module_common.h"


#define RTF96XX_T 100
static GPIO_InitTypeDef  trf796xx_GPIO_InitStruct;
static GPIO_InitTypeDef  trf796xx_GPIO_InitStruct_pin7;
static void trf796xx_delay(int delay)
{
//	while(delay--)
//		__nop();
}

#if defined(USE_TRF7962_DELAY)
#define TRF7962_DELAY(ns) trf796xx_delay(ns)
#else
#define TRF7962_DELAY(...)
#endif


#define USE_TRF7962_LOCK
#if defined(USE_TRF7962_LOCK)
#define TRF7962_EN_INT() __enable_irq()
#define TRF7962_DI_INT() __disable_irq()
#else
#define TRF7962_EN_INT(...)
#define TRF7962_DI_INT(...)
#endif

void trf796xx_pin_init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//IRQ pin
	trf796xx_GPIO_InitStruct.Pin = GPIO_PIN_1;
    trf796xx_GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    trf796xx_GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    trf796xx_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &trf796xx_GPIO_InitStruct);
    HAL_NVIC_SetPriority(EXTI0_1_IRQn, 4, 2);
    
	//enable pin
	trf796xx_GPIO_InitStruct.Pin = GPIO_PIN_11;
    trf796xx_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    trf796xx_GPIO_InitStruct.Pull = GPIO_NOPULL;
    trf796xx_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &trf796xx_GPIO_InitStruct);
	
    //data clk
    trf796xx_GPIO_InitStruct.Pin = GPIO_PIN_15;
    trf796xx_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    trf796xx_GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    trf796xx_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &trf796xx_GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);



    //data bus
	trf796xx_GPIO_InitStruct.Pin = (GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|\
		                            GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
    trf796xx_GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    trf796xx_GPIO_InitStruct.Pull = GPIO_NOPULL;
    trf796xx_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOB, &trf796xx_GPIO_InitStruct); 
	
    //cond signal
	trf796xx_GPIO_InitStruct_pin7.Pin=GPIO_PIN_7;
	trf796xx_GPIO_InitStruct_pin7.Mode = GPIO_MODE_OUTPUT_PP;
    trf796xx_GPIO_InitStruct_pin7.Pull = GPIO_PULLDOWN;
    trf796xx_GPIO_InitStruct_pin7.Speed = GPIO_SPEED_FREQ_MEDIUM;

	//cnt
	GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 1, 1);
	
}
void trf796xx_enable_irq()
{
	sz_printk("7962 irq enable\n");
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}
void trf796xx_disable_irq()
{
	sz_printk("7962 irq disable\n");
	HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void trf796xx_enable()
{
    sz_printk("7962 chip enable\n");
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);
}

void trf796xx_disable()
{
	sz_printk("7962 chip disable\n");
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
}

static void set_trf796xx_data_port_for_input()
{
	trf796xx_GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOB, &trf796xx_GPIO_InitStruct);
}
static void set_trf796xx_data_port_for_output()
{
	trf796xx_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOB, &trf796xx_GPIO_InitStruct);
}

static void trf796xx_clk(unsigned int level)
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,level);
}
#define TRF796xxCLK_ON trf796xx_clk(1)
#define TRF796xxCLK_OFF trf796xx_clk(0)

static void trf796xx_set_pin7_mode(unsigned int level)
{
	trf796xx_GPIO_InitStruct_pin7.Mode = level;
	HAL_GPIO_Init(GPIOB, &trf796xx_GPIO_InitStruct_pin7);
}
#define SET_TRF_PIN7_INPUT  trf796xx_set_pin7_mode(0)
#define SET_TRF_PIN7_OUTPUT trf796xx_set_pin7_mode(1)

static void trf796xx_pin7(unsigned int level)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,level);
}
#define TRF_PIN7_ON  trf796xx_pin7(1)
#define TRF_PIN7_OFF trf796xx_pin7(0)

static void trf796xx_start_cond()
{
	TRF7962_DI_INT();
	SET_TRF_PIN7_OUTPUT;
	TRF_PIN7_OFF;
	TRF7962_DELAY(RTF96XX_T/3);
	TRF796xxCLK_ON;
	TRF7962_DELAY(RTF96XX_T/3);
	TRF_PIN7_ON;
	TRF7962_DELAY(RTF96XX_T/3);
	TRF796xxCLK_OFF;
	TRF7962_DELAY(RTF96XX_T/2);
	TRF_PIN7_OFF;
	SET_TRF_PIN7_INPUT;
	TRF7962_DELAY(RTF96XX_T/2);
}

static void trf796xx_stop_cond()
{
	SET_TRF_PIN7_OUTPUT;//CLK is low 
	TRF_PIN7_ON;
	TRF7962_DELAY(RTF96XX_T/3);
	TRF796xxCLK_ON;
	TRF7962_DELAY(RTF96XX_T/3);
	TRF_PIN7_OFF;
	TRF7962_DELAY(RTF96XX_T/3);
	TRF796xxCLK_OFF;
	TRF7962_DELAY(RTF96XX_T/2);
	SET_TRF_PIN7_INPUT;
	TRF7962_DELAY(RTF96XX_T/2);
	TRF7962_EN_INT();
}

static void trf796xx_write_port(int value)
{
	HAL_GPIO_WritePin(GPIOB,0xFF&(~value),GPIO_PIN_RESET);
	set_trf796xx_data_port_for_output();
	HAL_GPIO_WritePin(GPIOB,0xFF&value,GPIO_PIN_SET);
	TRF7962_DELAY(RTF96XX_T/10);
	TRF796xxCLK_ON;
	TRF7962_DELAY(RTF96XX_T);
	TRF796xxCLK_OFF;
	TRF7962_DELAY(RTF96XX_T);
}



static int trf796xx_read_port()
{
	int value;
	set_trf796xx_data_port_for_input();
	TRF7962_DELAY(RTF96XX_T/2);
	TRF796xxCLK_ON;
	TRF7962_DELAY(RTF96XX_T/2);
	value = GPIOB->IDR&0xFF;
	TRF796xxCLK_OFF;
	TRF7962_DELAY(RTF96XX_T);
    return value;
}

int trf796xx_write_reg_t(int addr,int value)
{
	trf796xx_start_cond();
    trf796xx_write_port(addr&0x1F);//0001,1111
    trf796xx_write_port(value);
	trf796xx_stop_cond();
	return 0;
}

int trf796xx_read_reg_t(int addr)
{
	int ret_;
	trf796xx_start_cond();
	trf796xx_write_port(((addr&0x1F)|0x40));//0101,1111
	ret_ = trf796xx_read_port();
	trf796xx_stop_cond();
	return ret_;
}

int trf796xx_write_regs(struct trf796xx_data_t *p_data,int len)
{
	struct trf796xx_data_t *p_end = p_data + len;
	trf796xx_start_cond();
	for(;p_data<p_end;++p_data)
	{
		trf796xx_write_port((int)(p_data->addr&0x1F));//0001,1111
	    trf796xx_write_port((int)(p_data->value&0xFF));
	}
	trf796xx_stop_cond();
	return len;
}

int trf796xx_read_regs(struct trf796xx_data_t *p_data,int len)
{
	struct trf796xx_data_t *p_end = p_data + len;
	trf796xx_start_cond();
	for(;p_data<p_end;++p_data)
	{
		trf796xx_write_port((int)((p_data->addr&0x1F)|0x40));//0101,1111
		p_data->value = (short)trf796xx_read_port();
	}
	trf796xx_stop_cond();
	return len;
}

int trf796xx_write_cmd(int cmd)
{
	trf796xx_start_cond();
	trf796xx_write_port((cmd&0x1F)|0x80);//cmd 1001,1111
	trf796xx_stop_cond();
	return 0;
}

int trf796xx_write_cmds(const char *buf,int len)
{
	const char *end = buf+len;
	int addr;
	trf796xx_start_cond();
    for(;buf<end;++buf)
    {
		addr = (int)*buf;
		trf796xx_write_port((addr&0x1F)|0x80);//cmd 1001,1111
    }
	trf796xx_stop_cond();
	return len;
}

int trf796xx_write_block(int addr,const char *buf,int len)
{
	const char *end = buf+len;
	trf796xx_start_cond();
	trf796xx_write_port((addr&0x1F)|0x20);//cmd 0011,1111
    for(;buf<end;++buf)
    {
		trf796xx_write_port((int)(*buf));
    }
	TRF_PIN7_ON;
	TRF7962_DELAY(RTF96XX_T);
	TRF_PIN7_OFF;
	TRF7962_DELAY(RTF96XX_T);
	TRF7962_EN_INT();
	return len;
}


int trf796xx_read_block(int addr,char *buf,int len)
{
	char *end = buf+len;
	trf796xx_start_cond();
	trf796xx_write_port((addr&0x1F)|0x60);//cmd 011,,11111
    for(;buf<end;++buf)
    {
		*buf = (char)(trf796xx_read_port()&0xFF);
    }
	SET_TRF_PIN7_OUTPUT;
	TRF_PIN7_ON;
	TRF7962_DELAY(RTF96XX_T);
	TRF_PIN7_OFF;
	TRF7962_DELAY(RTF96XX_T);
	TRF7962_EN_INT();
	return len;
}

int trf796xx_write_raw_datas(const char *buf,int len)
{
	const char *end = buf+len;
	trf796xx_start_cond();
    for(;buf<end;++buf)
    {
		trf796xx_write_port((int)(*buf));
    }
	trf796xx_stop_cond();
	return len;
}



int printf_reg(int addr)
{
	int ret_;
	ret_=trf796xx_read_reg_t(addr);
	sz_printk("addr[%#x]=%#x\n",addr,ret_);
	return ret_;
}

#if 1
int trf796xx_read_single(char *p_buf,int len)
{
	char *p_buf_end=p_buf+len;
	trf796xx_start_cond();
	for(;p_buf<p_buf_end;++p_buf)
	{
		trf796xx_write_port((int)(*p_buf&0x1F));//0101,1111
		*p_buf = (char)trf796xx_read_port();
	}
	trf796xx_stop_cond();
	return len;
}

int trf796xx_write_single(const char *p_buf,int len)
{
	const char *p_buf_end=p_buf+len;
	trf796xx_start_cond();
	for(;p_buf<p_buf_end;++p_buf)
	{
		trf796xx_write_port((int)((*p_buf&0x1F)|0x20));//0101,1111
	}
	trf796xx_stop_cond();
	return len;
}


int read_irq_pin()
{
	return HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1);
//	return HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==0?1:0;
}
void set_irq_pin_to_input()
{
	  GPIO_InitTypeDef  GPIO_InitStruct;
    HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);//
    GPIO_InitStruct.Pin = (GPIO_PIN_1);
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH  ;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
}
void set_irq_pin_to_extit()
{
    //__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Pin = (GPIO_PIN_1);
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH  ;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);//
}
#endif

