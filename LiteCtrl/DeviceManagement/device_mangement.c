#include "device_mangement.h"
#include "device_pakage.h"
#include "cc110x_hal.h"
#include "delay_work.h"
#include "module_common.h"
#include "cc110x_module.h"
#include "LiteMasterResource.h"
#include "device_info.h"
#include "device_handle_data.h"
#include "LiteMasterResource.h"
#include "led.h"
#include "cc110x_hal.h"
#include "sz_libc.h"
#include "sysmode.h"

static int device_mode;

struct delay_work device_dwk;

static int device_step=0;

static int device_init_flg = DEVICE_UNINIT;


static void device_dwk_fun(struct delay_work *p_dwk);


static void set_device_init_flg(int new_flg)
{
	device_init_flg = new_flg;
}
static int get_device_init_flg()
{
	return device_init_flg;
}

int get_device_mode()
{
	return device_mode;
}

void set_device_mode(int new_mode)
{
	device_mode = new_mode;
}

static int time_out_cnt = 0;
void resset_online_time()
{
	time_out_cnt  = -1;
}
void set_online_time()
{
	SetLedState(0,ONE_FLASH_ON);
	time_out_cnt  = 0;
}

int get_online_time()
{
	return time_out_cnt;
}

void device_exit()
{
	if(get_device_init_flg() != DEVICE_INIT)
		return ;
	cancel_delay_work(&device_dwk);
	set_device_init_flg(DEVICE_UNINIT);
	switch(get_device_mode())
	{
		case LITECTRL_NORMAL_WORK:
			cc110x_module_exit();
			break;
		case LITECTRL_FIRST_RUNNING:
			cc110x_module_exit();
			break; 
		default: 
			break; 
	}
}


int device_setup(int mode,int channel)
{
	device_exit();
	set_device_mode(mode);
	set_device_init_flg(DEVICE_INIT);
	switch(get_device_mode())
	{
		case LITECTRL_NORMAL_WORK:	
			cc110x_start_work(get_subdevice()->subaddr,get_subdevice()->chanel);
			device_step = 0;
			resset_online_time();
			shedule_delay(&device_dwk,2000);// 3s
			break;
		case LITECTRL_FIRST_RUNNING://firt 
			cc110x_start_work(get_subdevice()->subaddr,channel);
			//shedule_delay(&device_dwk,3000);// 3s
			break; 
		case LITECTRL_BOARD_CHEKING:
			shedule_delay(&device_dwk,1000);
			cc110x_start_work(get_subdevice()->subaddr,channel);
			break;
		default:
			break; 
	}
	return 0;
}

//get the system information

static int get_device_running_mode()
{
	return 0;
}

static void mode_select()
{
	int mod = read_sysmode();
	switch(mod)
	{
		case 1:
			init_adc_timer();
			adc_start();
			HAL_Delay(100);
			device_setup(LITECTRL_BOARD_CHEKING,get_channel());
			adc_stop();
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			mod = mod - 1;
			set_subdev_pos((char*)&mod,1);
			device_setup(LITECTRL_PRODUCE_CHEKING,0);  
			break;
		default:
			device_setup(LITECTRL_NORMAL_WORK,0);
			break;
	}
}
void device_init()
{
	char buf;
	init_sysmde_pin();
	shedule_init(&device_dwk);
    device_dwk.handle = device_dwk_fun;
	device_init_sys_info();
    set_device_attr(get_subdevice()->position_id);
	set_cc1101_handle_recv_data(cc1101_handle_data);
	set_device_init_flg(DEVICE_UNINIT);
	get_subdev_flg(&buf,1);
    mode_select();
	//device_setup(LITECTRL_BOARD_CHEKING,0);
}



static void dlw_normal_running(struct delay_work *p_dwk) 
{
	static int reconn = 0;
	int channel;
	static int step = 0;
    switch(step)
    {
		case 0:
			if(get_online_time()!=-1)
			{
				shedule_delay(p_dwk,5*1000);
				resset_online_time();
				break;
			}
			step = 1;
		case 1:
            if(get_online_time()==-1)
            {
            	sz_printk("scaning ...\n");
            	cc110x_stop_work();
            	channel = get_subdevice()->chanel+1;
				//channel = get_subdevice()->chanel;
            	if(channel >=16)
            		channel = 8;
            	get_subdevice()->chanel = channel;
            	cc110x_start_work(get_subdevice()->subaddr,channel);
            	reconn++;
            	shedule_delay(p_dwk,2*1000);
            	break ;
            }
			step=0;
			get_subdevice()->chanel-=1;
			shedule_delay(p_dwk,5*1000);
			set_scan_cnt();
			break;
		defualt:
			break;
    }
}

static void board_check(struct delay_work *p_dwk)
{
	static int status = 0;
	switch(status)

	{
		case 0:
			sz_printk("subdev_init\n");
			GO_NEXET_STEP(status);
			shedule_delay(p_dwk,2*1000);
			//shedule_delay(p_dwk,100);
			break;
		case 1:
			sz_printk("subdev_433_rx_signal:%2d",0-RSSI_calculated(cc1101_get_rx_sig_strength()));
			sz_printf(" 433\r\n");
			sz_printk("subdev_433_tx_signal:%2d",0-RSSI_calculated(cc1101_get_tx_sig_strength()));
			sz_printf(" 433\r\n");
			sz_printk("subdev_read_card:%2d",18);
			sz_printf(" card\r\n");
			sz_printk("subdev_finish\n");
		default:
			break;
	}
}
static void device_dwk_fun(struct delay_work *p_dwk)
{
    switch(get_device_mode())
    {
		case LITECTRL_NORMAL_WORK:
			dlw_normal_running(p_dwk);
			break;
		case LITECTRL_FIRST_RUNNING:
			dlw_normal_running(p_dwk);
			break;
		case LITECTRL_BOARD_CHEKING:
			board_check(p_dwk);
			break;
		default:
			sz_printk("----------\n");
			break;
    }
}

