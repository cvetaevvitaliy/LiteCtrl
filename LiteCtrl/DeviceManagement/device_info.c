#include "device_info.h"
#include "LiteMasterResource.h"
#include "sz_libc.h"
#
static struct master_device_t master_device;
static struct sub_device_t  sub_device;
static int device_attr;
static int online[4];

const char *_VERSION = "0.0.1.20171116";
const char *inner_version = "0.0.0";
const char *update_time = "2017.11.16 16:36";

int print_version()
{
	printf_green("Subdev Soft Version:%s\r\n",_VERSION);
}

int print_inner_version()
{
	printf_green("\r\nInner Version   :%s\r\n",inner_version);
	printf_green("Last update time:%s\r\n",update_time);
}


int device_init_sys_info()
{
	char buf;
	get_master_id(master_device.master_local_id,12);
	master_device.cseq = -1;
	get_master_addr(&buf,1);
	master_device.masteraddr = sz_ctoi(&buf,1)&0xFF;
    sz_printk("maseter addr:%#X\n",master_device.masteraddr);

	//get_cpu_id(sub_device.subdev_local_id,12);
	sub_device.subaddr=0x28;
	sub_device.cseq = -1;
	get_subdev_addr(&buf,1);
	sub_device.subaddr = sz_ctoi(&buf,1)&0xFF;
	sz_printk("subdev addr:%#x\n",sub_device.subaddr);
	get_subdev_pos(&buf,1);
	sub_device.position_id = (int)buf;
	get_subdev_card_cnt_flg(&buf,1);
	sub_device.card_cnt = sz_ctoi(&buf,1)&0xFF;
	sub_device.chanel = 8;
	return 0;
}
void set_device_attr(int attr)
{
	device_attr = attr;
}

int get_device_attr()
{
	return device_attr;
}

struct sub_device_t *get_subdevice()
{
	return &sub_device;	
}

struct master_device_t *get_masterdevice()
{
	return &master_device;
}
