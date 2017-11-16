#include "console_cmd.h"
#include "sz_libc.h"
#include "LiteMasterResource.h"
#include "led.h"
#include "cc110x_hal.h"
#include "stm32l0xx_hal.h"
#include "device_info.h"
#include "module_common.h"
#include "trf796xx_module.h"
#include "adc.h"
#include "sysmode.h"
#include "check_out_card.h"
#include "device_info.h"

static int sz_strcmp_t(char *str1,char *str2)
{
	char *end;
	int len1;
	int len2;
	len1 = sz_strlen(str1);
	len2 = sz_strlen(str2);
	if( len1 != len2)
		return -1;
	end = str1 + len1;
	while(str1 < end)
	{
		if(*str1 != *str2)
			break;
		str1++;
		str2++;
	}
	if(str1 == end)
		return 0;
	else
		return -1;
}


static void format()
{
	
}

static int get_ch_hex(char ch)
{
	if(ch > 0x2F && ch < 0x3A)
	{
		return (ch - 0x30);
	} 
	else if(ch > 0x40 && ch < 0x47)
	{
		return ((ch - 0x37));
	}
	else if(ch > 0x60 && ch < 0x67)
	{
		return (ch - 0x47);
	}
	else
	{
		return -1;
	}
}

int console_cmd_echo(char argc,char *argv[])
{
	int i;
//	for(i=0;i<argc;i++)
//	    sz_printk("%d.%s\n",i,argv[i]);
	if(argc<2)
		return -1;
	if(sz_strcmp_t("position",argv[1])==0)
	{
		

	}
	else if(sz_strcmp_t("system",argv[1])==0)
	{


		
	}
	else if(sz_strcmp_t("maddr",argv[1])==0)
	{
		sz_printk("write addr=%s\n",argv[0]);
		set_master_addr(argv[0],4);
	}
	else if(sz_strcmp_t("pos",argv[1])==0)
	{
		argv[0][0]=argv[0][0]-'0';
		set_subdev_pos(argv[0],1);
		get_subdevice()->position_id = (int)argv[0][0];
		sz_printk("SETTIING_ACK:SET_POS_OK\n");
		//sz_printk("SETTIING_ACK:SET_POS_ERROR\n");
	}
	else if(sz_strcmp_t("card_cnt_flg",argv[1])==0)
	{
	   argv[0][0]=argv[0][0]-'0';
	    set_subdev_card_cnt_flg(argv[0],1); 
		set_read_card_count_flg((int)argv[0][0]);
	}
	else if(sz_strcmp_t("led0",argv[1])==0)
	{
		int cmd = argv[0][0]=argv[0][0]-'0';
		switch(cmd)
		{
			case 0:
				SetLedState(0,ALLAY_OPEN);
				break;
			case 1:
				SetLedState(0,ALLAY_CLOSE);
				break;
			case 2:
				SetLedState(0,ALLAY_FLASH);
				break;
			case 3:
				SetLedState(0,ONE_FLASH_ON);
				break;
			default:
				break;
		}
	}
	else if(sz_strcmp_t("subdev",argv[1])==0)
	{
		int value = sz_strtoul(argv[0],0,10);
		set_subdev_flg((char*)&value,1);
	}
	else if(sz_strcmp_t("short_time",argv[1])==0)
	{
		int flg;
		flg = sz_strtoul((const char *)argv[0],0,10);
		set_subdev_card_cnt_short_time((char*)&flg,4);
		set_read_card_count_short_time(flg);
	}
	else if(sz_strcmp_t("long_time",argv[1])==0)
	{
		int flg;
		flg = sz_strtoul((const char *)argv[0],0,10);
		set_subdev_card_cnt_long_time((char*)&flg,4);
		set_read_card_count_long_time(flg);
		
	}
	else if(sz_strcmp_t("filter_time",argv[1])==0)
	{
		int flg;
		flg = sz_strtoul((const char *)argv[0],0,10);
		set_subdev_card_cnt_filter_time((char*)&flg,4);
		set_read_card_count_filter_time(flg);
		
	}
	else if(sz_strcmp_t("card_print",argv[1])==0)
	{
		int flg;
		flg = sz_strtoul((const char *)argv[0],0,10);
		set_subdev_card_cnt_print_flg((char*)&flg,1);
		set_read_card_count_print(flg);
		
	}
	else if(sz_strcmp_t("read_time_out",argv[1])==0)
	{
		int flg;
		flg = sz_strtoul((const char *)argv[0],0,10);
		set_subdev_card_read_time_out_flg((char*)&flg,1);
		set_read_card_time_out(flg);
		
	}
	else if(sz_strcmp_t("onlie_card_print",argv[1])==0)
	{
		int flg;
		flg = sz_strtoul((const char *)argv[0],0,10);
		set_subdev_card_online_print_flg((char*)&flg,1);
		set_card_info_pf(flg);
		
	}
    else if(sz_strcmp_t("mst_cpu_id",argv[1])==0)
    {
		char tmp[12]={0};
		char *pt=argv[0];
		int i;
		int mst_addr;
		int sub_addr;
		for(i=0;i<12;i++)
		{
			
			tmp[i]=(get_ch_hex(*pt++)<<4)|(get_ch_hex(*pt++));
			//pt+=2;
		}
		set_master_id(tmp,12);		
		mst_addr = Crc16Result(tmp,12);
		mst_addr = mst_addr %256;
		if(mst_addr<0 || mst_addr > 250)
		{

			mst_addr = 128;
		}
		sub_addr = mst_addr+(get_subdevice()->position_id);
		sz_printk("master addr:%#x,sub addr:%#x\n",mst_addr,sub_addr);
		set_master_addr((char *) &mst_addr, 4);
		set_subdev_addr((char *) &sub_addr, 4);
    }

	return 0;
}

int console_cmd_cat(char argc,char *argv[])
{
//	int i;
//	for(i=0;i<argc;i++)
//	    sz_printk("%d.%s\n",i,argv[i]);
	if(sz_strcmp_t("position",argv[0])==0)
	{
		

	}
	else if(sz_strcmp_t("system",argv[0])==0)
	{
		print_system();
	}
	else if(sz_strcmp_t("maddr",argv[0])==0)
	{
		char buf[4]={0};
		get_master_addr(buf,4);
        sz_printf("maddr = %s\r\n",buf);
	}
	else if(sz_strcmp_t("msg",argv[0])==0)
	{
		msg_printf();
	}
	else if(sz_strcmp_t("card_cnt_flg",argv[0])==0)
	{
		int flg;
		get_subdev_card_cnt_flg((char*)&flg,1);
		sz_printk("read card count:%d(%s)\n",flg,flg == 0x00?"enable":"disable");
	}
	else if(sz_strcmp_t("short_time",argv[0])==0)
	{
		int flg;
		get_subdev_card_cnt_short_time((char*)&flg,4);
		sz_printk("short_time:%dms\n",flg);
	}
	else if(sz_strcmp_t("long_time",argv[0])==0)
	{
		int flg;
		get_subdev_card_cnt_long_time((char*)&flg,4);
		sz_printk("long_time:%dms\n",flg);
	}
	else if(sz_strcmp_t("filter_time",argv[0])==0)
	{
		int flg;
		get_subdev_card_cnt_filter_time((char*)&flg,4);
		sz_printk("filter_time:%dms\n",flg);
		
	}
	else if(sz_strcmp_t("card_print",argv[0])==0)
	{
		int flg;
		get_subdev_card_cnt_print_flg((char*)&flg,1);
		sz_printk("card_print flg:%d(%s)\n",flg,flg == 0?"enable":"disable");
		
	}
	else if(sz_strcmp_t("cc1101",argv[0])==0)
	{
		printf_cc1101_info();		
	}
	else if(sz_strcmp_t("systime",argv[0])==0)
	{
		sz_printk("system time:%d\n",HAL_GetTick());		
	}
	else if(sz_strcmp_t("adc",argv[0])==0)
	{
		sz_printk("adc:%d\n",get_adc_value());		
	}
	else if(sz_strcmp_t("sysmod",argv[0])==0)
	{
		read_sysmode();		
	}
	else if(sz_strcmp_t("version",argv[0])==0)
	{
		print_version();
	}

	else if(sz_strcmp_t("inner_version",argv[0])==0)
	{
		print_inner_version();
	}
	return 0;
}

