#include "stm32l0xx.h"
#include "LiteMasterResource.h"
#include "sz_libc.h"

#define EEPROM_BASE_ADDR    ((unsigned int)0x08080000U)   //start
#define EEPROM_BYTE_SIZE    (0x07FFU)  //2K




#define PEKEY1  0x89ABCDEF      
#define PEKEY2  0x02030405    


#define MASTER_ID_BASE (0)
#define MASTER_ADDR_BASE (12)
#define SUBEVICE_ADDR_BASE (13)
#define SUB_DEVICE_FLG (14)
#define SUB_DEVICE_POS (15)
#define C1101_CHANEL_BASE (16)
#define CARD_COUNT_FLG_BASE (17)
#define CARD_COUNT_SHORT_TIME_BASE (18)
#define CARD_COUNT_LONG_TIME_BASE (22)
#define CARD_COUNT_FILTER_TIME_BASE (26)
#define CARD_COUNT_PRINT_ENABLE_BASE (30)
/*
		EEPROM 以字节写
		Addr:偏移 0-2047
		Buffer:要写入的字符
		Length:长度
*/
static void EEPROM_WriteBytes(int Addr,char *Buffer,int Length)  
{  
    char *wAddr;  
    wAddr=(char *)(EEPROM_BASE_ADDR+Addr);  
	while(FLASH->SR&FLASH_SR_BSY){} 
    FLASH->PEKEYR=PEKEY1;                //unlock  
    FLASH->PEKEYR=PEKEY2;  
    while(FLASH->PECR&FLASH_PECR_PELOCK);  //等待解锁成功

    while(Length--)
	{  
        *wAddr++=*Buffer++;  
        while(FLASH->SR&FLASH_SR_BSY);  //等待编程完成
    }  
    FLASH->PECR|=FLASH_PECR_PELOCK;  //加锁寄存器
}

/*
		EEPROM 以字节读
		Addr:偏移 0-2047
		Buffer:存放要读字节的缓存区
		Length:长度
*/
static void EEPROM_ReadBytes(int Addr,char *Buffer,int Length)  
{  
    char *wAddr;  
    wAddr=(char *)(EEPROM_BASE_ADDR+Addr);  
    while(Length--)
	{  
        *Buffer++=*wAddr++;  
    }   
} 


void get_cpu_id(char *buf,int len)
{
    *(int*)buf=*(int*)(0x1FF80050U);
    buf+=4;
    *(int*)buf=*(int*)(0x1FF80054U);
    buf+=4;
    *(int*)buf=*(int*)(0x1FF80064U);
    buf+=4;
}



int get_master_id(char *buf,int len)
{
	EEPROM_ReadBytes(MASTER_ID_BASE,buf,12);
	return 12;
}

void set_master_id(char *buf,int len)
{
	int i;
	sz_printk("master id:");
	for(i=0;i<len;i++)
	{
	    sz_printf("%X",buf[i]);
	}
	sz_printf("\r\n");
	sz_printk("SETTIING_ACK:SET_PARITY_KEY_OK\n");
	//sz_printf("SETTIING_ACK:SET_PARITY_KEY_ERROR");
	EEPROM_WriteBytes(MASTER_ID_BASE,buf,12);
}

int get_master_addr(char *buf,int len)
{
	EEPROM_ReadBytes(MASTER_ADDR_BASE,buf,1);
	return 1;
}
void set_master_addr(char *buf,int len)
{
	EEPROM_WriteBytes(MASTER_ADDR_BASE,buf,1);
}

int get_subdev_addr(char *buf,int len)
{
	EEPROM_ReadBytes(SUBEVICE_ADDR_BASE,buf,1);
	return 1;
}

int set_subdev_addr(char *buf,int len)
{
	EEPROM_WriteBytes(SUBEVICE_ADDR_BASE,buf,1);
	return 1;
}

int get_subdev_flg(char *buf,int len)
{
	EEPROM_ReadBytes(SUB_DEVICE_FLG,buf,1);
	return 1;
}

void set_subdev_flg(char *buf,int len)
{
	EEPROM_WriteBytes(SUB_DEVICE_FLG,buf,1);
}

void set_subdev_pos(char *buf,int len)
{
	*buf = *buf&0x7F;
	EEPROM_WriteBytes(SUB_DEVICE_POS,buf,1);
}
int get_subdev_pos(char *buf,int len)
{
	EEPROM_ReadBytes(SUB_DEVICE_POS,buf,1);
	return len;
}

void set_subdev_chanle(char *buf,int len)
{
	EEPROM_WriteBytes(C1101_CHANEL_BASE,buf,1);
}

int get_cc1101_chanle(char *buf,int len)
{
	EEPROM_ReadBytes(C1101_CHANEL_BASE,buf,1);
	return len;
}


void set_subdev_card_cnt_flg(char *buf,int len)
{
	EEPROM_WriteBytes(CARD_COUNT_FLG_BASE,buf,1);
}

int get_subdev_card_cnt_flg(char *buf,int len)
{
	EEPROM_ReadBytes(CARD_COUNT_FLG_BASE,buf,1);
	return 1;
}

void set_subdev_card_cnt_short_time(char *buf,int len)
{
	EEPROM_WriteBytes(CARD_COUNT_SHORT_TIME_BASE,buf,4);
}

int get_subdev_card_cnt_short_time(char *buf,int len)
{
	EEPROM_ReadBytes(CARD_COUNT_SHORT_TIME_BASE,buf,4);
	return 1;
}


void set_subdev_card_cnt_long_time(char *buf,int len)
{
	EEPROM_WriteBytes(CARD_COUNT_LONG_TIME_BASE,buf,4);
}

int get_subdev_card_cnt_long_time(char *buf,int len)
{
	EEPROM_ReadBytes(CARD_COUNT_LONG_TIME_BASE,buf,4);
	return 4;
}

void set_subdev_card_cnt_filter_time(char *buf,int len)
{
	EEPROM_WriteBytes(CARD_COUNT_FILTER_TIME_BASE,buf,4);
}

int get_subdev_card_cnt_filter_time(char *buf,int len)
{
	EEPROM_ReadBytes(CARD_COUNT_FILTER_TIME_BASE,buf,4);
	return 4;
}

void set_subdev_card_cnt_print_flg(char *buf,int len)
{
	EEPROM_WriteBytes(CARD_COUNT_PRINT_ENABLE_BASE,buf,1);
}

int get_subdev_card_cnt_print_flg(char *buf,int len)
{
	EEPROM_ReadBytes(CARD_COUNT_PRINT_ENABLE_BASE,buf,1);
	return 1;
}


void device_defult_init()
{
	int res;

	res = 1;
	set_subdev_flg((char*)&res,1);
	
	res = 100;
	set_subdev_card_cnt_short_time((char*)&res,4);

	res = 200;
	set_subdev_card_cnt_long_time((char*)&res,4);

	res = 30;
	set_subdev_card_cnt_filter_time((char*)&res,4);

	res = 1;
	set_subdev_card_cnt_print_flg((char*)&res,1);
}


void print_system()
{
	char buf[12];
	int i;
	for(i=0;i<2;i++)
	{
	    sz_printf("*********************************");
	}
	sz_printf("\r\n");
	sz_memset(buf,0, 12);
	get_subdev_flg(buf,1);
	sz_printf("*subdev         :%#x\r\n",sz_ctoi(buf,1)&0xFF);

	sz_memset(buf,0, 12);
	get_master_addr(buf,1);
	sz_printf("*master addr    :%#x\r\n",sz_ctoi(buf,1)&0xFF);

	sz_memset(buf,0, 12);
	get_subdev_addr(buf,1);
	sz_printf("*sub addr       :%#x\r\n",sz_ctoi(buf,1)&0xFF);

	sz_memset(buf,0, 12);
	get_master_id(buf,12);
    sz_printf("*master id      :");
	for(i=0;i<12;i++)
	{
	    sz_printf("%X",buf[i]);
	}
	sz_printf("\r\n");

	sz_memset(buf,0, 12);
	get_subdev_pos(buf,1);
	sz_printf("*pos            :%d",(int)buf[0]);
    switch((int)buf[0])
    {
		case 1:
			sz_printf("(East)\r\n");
			break;
		case 2:
			sz_printf("(South)\r\n");
			break;
		case 3:
			sz_printf("(West)\r\n");
			break;
		case 4:
			sz_printf("(North)\r\n");
			break;
		default:
			sz_printf("(Unknow)\r\n");
			break;
    }

	sz_memset(buf,0, 12);
	get_subdev_card_cnt_flg(buf,1);
	i = sz_ctoi(buf,1);
	sz_printf("*card_cnt_flg   :%d(%s)\r\n",i,i== 0x00?"enable":"disable");

    sz_memset(buf,0, 12);
	get_subdev_card_cnt_short_time(buf,4);
	sz_printf("*short_time     :%dms\r\n",sz_ctoi(buf,4));

    sz_memset(buf,0, 12);
	get_subdev_card_cnt_long_time(buf,4);
	sz_printf("*long_time      :%dms\r\n",sz_ctoi(buf,4));

    sz_memset(buf,0, 12);
	get_subdev_card_cnt_filter_time(buf,4);
	sz_printf("*filter_time    :%dms\r\n",sz_ctoi(buf,4));
	
	sz_memset(buf,0, 12);
	get_subdev_card_cnt_print_flg(buf,1);
	sz_printf("*card_print     :%d(%s)\r\n",sz_ctoi(buf,1),sz_ctoi(buf,1)==0?"enable":"diable");
	
	for(i=0;i<2;i++)
	{
	    sz_printf("*********************************");
	}
	sz_printf("\r\n");
}
