#include "stm32l0xx_hal.h"
#include "terminal.h"
#include "lpuart.h"
#include "system.h"
#include "stm_console.h"
#include "sz_libc.h"
#include "delay_work.h"
#include "trf796xx_module.h"
#include "cc110x_module.h"
#include "lite_timer.h"
#include "device_mangement.h"
#include "led.h"
#include "check_out_card.h"
#include "cc110x_hal.h"
#include "adc.h"
#include "sysmode.h"
#include "check_out_card.h"

static void terminal_entry()
{
	int len;
	int i;
	unsigned char test_ch[16];
	len = lpuart_read(test_ch,16);
	for(i=0;i<len;i++)
	{
		console_proc((char)test_ch[i]);
	}
}

#define PRIN_BUF 128
//static char print_buf[PRIN_BUF];
struct prinf_bft
{

	char print_buf[PRIN_BUF];
    int flg;
};
struct prinf_bft prnt[4];
static int i=0;
static void get_format_buf(char **p_buf,int *len)
{
	*p_buf = prnt[i++%4].print_buf;
	*len = PRIN_BUF;
}
void mydelay(struct delay_work *pdlk)
{
	shedule_delay(pdlk,1000);//1second
}
struct delay_work st_deley_work;

static void init_timer()
{
	st_deley_work.handle = mydelay;
	shedule_init(&st_deley_work);
	shedule_delay(&st_deley_work,1000);// 1second
}
static int test_endain()
{
	int aa = 1;
	
	if(*(char*)&aa == 1)
	{

		sz_printk("Little endain\n");
	    return 0;
	}
	else
	{
		sz_printk("Big endain\n");
	    return 1;
	}
}


void set_default_param()
{
	char flg ;
	get_subdev_flg(&flg,1);
	if(flg==0)
	{
		device_defult_init();
	}
}
int main()
{
	HAL_Init();
	SystemClock_Config();//配置系统时钟
	init_lpuart(9600);//初始化GPIO 波特率
	start_lpuart();//使能串口
	set_default_param();
	
	sz_set_system_tick(HAL_GetTick);
	sz_set_libc_callback((int (*)(char *buf,int len))lpuart_printf,get_format_buf);//设置并启用C库
	init_delay_work();//定时器模块初始化
	start_console();//开启终端
	init_card();
	trf796xx_module_init();//TRF796xx模块初始化
	cc110x_module_init();
	//init_timer();//开始定时器函数
	lite_timer_init();
//    lite_enable_timer();
    printk_red("wellcome\n");
    test_endain();
    led_init();
    device_init();
	card_module_init();
	init_gloabel_info();
	while(1)
	{
		delay_work_entry();//定时器入口函数
		terminal_entry();//终端入口函数
		ShowLeds();
		trf7962_module_entery();
		cc110x_module_entery();
	}
	return 0;
}
