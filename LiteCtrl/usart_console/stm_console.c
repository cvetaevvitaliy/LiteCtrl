#include "lpuart.h"
#include "terminal.h"
#include "sz_libc.h"
#include "trf796xx_module.h"
#include "cc110x_hal.h"
#include "LiteMasterResource.h"
#include "console_cmd.h"
#include "device_mangement.h"
#include "module_common.h"

const char *read_id_card_t="read_id_card_t";
const char *read_id_card="read_id_card";
const char *read_id_block="read_id_bock";
static const char *cc1101_send_test="cc1101_send";
static const char *str_get_cpu_id="get_cpu_id";
static const char *str_echo="echo";
static const char *str_cat="cat";
static const char *str_lst="ls";
static const char *str_setup="setup";
static const char *str_flash_format="flash_format";
char temp[32];

static int strcmp_t(char *str1,char *str2)
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
char tx_buffer[28]={0x0,0,0,0,0,0,0,0};
int device_pakage_match_set_addr_respond();
static int console_do_cmd(int argc,char *argv[])
{
	if(strcmp_t((char*)read_id_card,argv[0])==0)
	{
		sz_printk("read card id\n");
		send_read_uid(0);
	}
	else if(strcmp_t((char*)read_id_card_t,argv[0])==0)
	{
		send_read_id_t();

	}
	else if(strcmp_t((char*)read_id_block,argv[0])==0)
	{
		sz_printk("read card block\n");
		send_read_block();
	}
	else if(strcmp_t((char*)cc1101_send_test,argv[0])==0)
	{
//		int i;
//		struct cc1101_msg_list_t *p_cc110_msg;
//		for(i=0;i<3;i++)
//		{
//    		p_cc110_msg= cc1101_msg_alloc_send_msg();
//    		if(p_cc110_msg)
//    		{
//    			sz_memcpy(p_cc110_msg->buf,tx_buffer, 28);
//    			p_cc110_msg->len = 28;
//    			cc101_msg_send_commit(p_cc110_msg,5,10);
//    		}
//		}
        device_pakage_match_set_addr_respond();
	}
	else if(strcmp_t((char*)str_get_cpu_id,argv[0])==0)
	{
		char tmp_buf[16]={0};
        get_cpu_id(tmp_buf,16);
		printf_fifo(tmp_buf,12);
	}
	else if(strcmp_t((char*)str_echo,argv[0])==0)
	{
		console_cmd_echo(argc-1,&argv[1]);
	}
	else if(strcmp_t((char*)str_cat,argv[0])==0)
	{
		console_cmd_cat(argc-1,&argv[1]);
		//init_trf796xx_sys();
		//send_read_uid(0);
	}
	else if(strcmp_t((char*)str_lst,argv[0])==0)
	{
		send_keep_silence();
	}
	else if(strcmp_t((char*)str_setup,argv[0])==0)
	{
		if(argc<2)
		{
		    device_setup(LITECTRL_NORMAL_WORK,0);
		}
		else
		{
			int channel = sz_strtoul(argv[1],NULL,10);
			sz_printk("match channel:%d\n",channel);
			device_setup(LITECTRL_FIRST_RUNNING,channel);
		}
	}
    else if(strcmp_t((char*)str_flash_format,argv[0])==0)
	{
		device_defult_init();
    }
	return 0;
}

int start_console()
{
	register_console_write((int (*)(char *,int))lpuart_printf);
	init_console();
	register_console_handl_data(console_do_cmd);
	
	register_cmd((char*)read_id_card,sz_strlen(read_id_card));
	register_cmd((char*)read_id_card_t,sz_strlen(read_id_card_t));
	register_cmd((char*)read_id_block,sz_strlen(read_id_block));
	register_cmd((char*)cc1101_send_test,sz_strlen(cc1101_send_test));
	register_cmd((char*)str_get_cpu_id,sz_strlen(str_get_cpu_id));
	register_cmd((char*)str_echo,sz_strlen(str_echo));
	register_cmd((char*)str_cat,sz_strlen(str_cat));
	register_cmd((char*)str_lst,sz_strlen(str_lst));
    register_cmd((char*)str_setup,sz_strlen(str_setup));
	register_cmd((char*)str_flash_format,sz_strlen(str_flash_format));
	
	return 0;
}
