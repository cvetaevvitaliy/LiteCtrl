#include "ring_buffer.h"
#include "trf796xx_adapter.h"
#include "sz_libc.h"
#include "trf796xx_module.h"
#include "trf796xx_reg.h"
#include "delay_work.h"
#include "lite_timer.h"
#include "module_common.h"
#include "check_out_card.h"
#include "LiteMasterResource.h"

static struct trf796xx_uid_t trf796xx_cur={0};
static struct trf796xx_uid_t trf796xx_next={0};

struct trf796xx_priv_t
{
	int step;
	int read_blk_flg;
	int irq_cnt;
	int read_card_cnt;
	int enable_count_time;
	int time_cnt;
	int count_time;
	int count_enable;
	int short_time;
	int long_time;
	int threshold_time;
	int filter_time;
	int asy_time;
	int cnt_print_flg;
	int read_time_out;
};
static struct trf796xx_priv_t trf_796xx;
static int start_time;
static int trf7962_time_out = 0;
static int trf7962_select_flg = 0;
struct delay_work trf7962_dlw_st;
struct delay_work trf7962_dlw_st_second;

void send_read_block();
void init_trf796xx_sys(); 
static void trf796xx_thread(struct delay_work *p_dw);
static void trf796xx_second_thread(struct delay_work *p_dw);

enum
{

	CHIP_DISABLE  = 0,
	CHIP_ENABLE   = 1,
	INIT_DEVICE   = 2,
	IRQ_ENABLE    = 3,
	SEND_READ_BLK_CMD=4,
	SEND_SILENCE_1_CMD =5,
	SEND_SILENCE_2_CMD=6,
	SEND_SILENCE_3_CMD=7,
};

static void reset_trt7962_time()
{
	trf7962_time_out = HAL_GetTick();
}
char pri_buf[64] = {0};
void printf_fifo(char *buf,int len)
{
	int cnt=0;
	int pri_len=0;
	int ch;
	sz_memset(pri_buf,0,64);
	for(cnt=0;cnt<len-1;cnt++)
	{
		pri_len +=sz_snprintf(&pri_buf[pri_len],64 - pri_len,\
			"%02X-",(char)buf[cnt]);
	}
    pri_len +=sz_snprintf(&pri_buf[pri_len],64 - pri_len,\
			"%02X.",(char)buf[cnt]);
	sz_printk("FIFO:%s\n",pri_buf);
}


//static char trf7962_read_fifo_tmp[16];
static int trf7962_read_fifo(char *buf,int len,int flg)
{
    int cnt;
	char *pt_end,*pos;
//	sz_memset(buf,0,len);
//	sz_memset(trf7962_read_fifo_tmp,0,16);
    cnt = trf796xx_read_reg_t(FIFOStatus);
    cnt = (0x0F&cnt)+1;
	if(buf==0||len <cnt)
	{
		sz_printk("trf7962 read buffer error\n");
		return -1;
	}
	
    trf796xx_read_block(FIFO,buf,cnt);
	#if 0
	if(flg==0)

	{
		pos = trf7962_read_fifo_tmp;
		pt_end = trf7962_read_fifo_tmp+cnt-1;
		while(pt_end>=pos)
		{
			*buf++=*pt_end--;
	    }
		cnt-=2;
	}
	else
	{
		pos = trf7962_read_fifo_tmp+1;
		pt_end = pos+2;
		while(pt_end>pos)
		{
			*buf++=*pos++;
	    }
		//cnt=2;
	}
	#endif
    return cnt;
}

void set_read_card_count_flg(int new_flg)
{
	sz_printk("card count flag:%d(%s)\n",new_flg,new_flg==0?"enable":"disable");
	trf_796xx.count_enable = new_flg;
	trf_796xx.irq_cnt = 0;
	trf_796xx.read_card_cnt = 0;
	trf_796xx.enable_count_time = -1;
}

void set_read_card_count_short_time(int new_time)
{
	sz_printk("short_time:%dms\n",new_time);
	trf_796xx.short_time= new_time;
}
void set_read_card_count_long_time(int new_time)
{
	sz_printk("long_time:%dms\n",new_time);
	trf_796xx.long_time= new_time;
}

void set_read_card_count_filter_time(int new_time)
{
	sz_printk("filter_time:%dms\n",new_time);
	trf_796xx.filter_time= new_time;
}

void set_read_card_count_print(int new_flg)
{
	sz_printk("card_print:%d(%s)\n",new_flg,new_flg==0?"enable":"disable");
	trf_796xx.cnt_print_flg= new_flg;
}

void set_read_card_time_out(int new_time)
{
	if(new_time<=0 || new_time > 45)
	{
		new_time = 30;
	}
	sz_printk("read_time_out:%dms\n",new_time);
	trf_796xx.read_time_out= new_time;
}



static int read_complete = 0;
static char uid_buf[5]={3>>8,3<<4,0x24,0x01,0};
static char block_buf[5]={3>>8,3<<4,0,0x20,0};
static char keep_silenc_buf[14]={10>>8,10<<4,0x20,0x02,0};
static char read_multy_block[]={4>>8,4<<4,0x00,0x23,0x00,0x01};

static void memcpy_rev(char *dst,char *src,int len)
{
	char *dst_end = dst+len;
	src= src+len-1;
	while(dst<dst_end)
	{
		*dst++=*src--;
	}
}


void keep_silence_t(char *src,int len)
{
	trf796xx_write_cmd(Reset);
	trf796xx_write_cmd(TransmitCRC);
	trf796xx_write_block(0x1D,src,len);
	trf796xx_read_reg_t(IRQStatus);
	//printf_fifo(src,len);
}

void keep_silence(char *uid,int len)
{
	sz_memset(&keep_silenc_buf[4],0,8);
	//mcpy_rev(&keep_silenc_buf[4],uid,8);
	sz_memcpy(&keep_silenc_buf[4],uid,8);
	keep_silence_t(keep_silenc_buf,13);
}

int send_keep_silence()
{
	keep_silence(trf796xx_next.uid,8);
}


static void trf7962_read_multy_block()
{
	trf796xx_write_cmd(Reset);
	trf796xx_write_cmd(TransmitCRC);
	trf796xx_write_block(0x1D,read_multy_block,6);
	trf796xx_read_reg_t(IRQStatus);
}
static int TRF796XX_IT_PROC()
{
	int status_reg = trf796xx_read_reg_t(0x0C);
	int cnt;
	//sz_printk("1.IRQ:%#x\n",status_reg);
	if(status_reg==0x80)
	{
		if(read_complete ==0)
		{
			//sz_printk("ID\n");
		    send_read_uid(0);
		}
		else if(read_complete ==3)
		{
		   keep_silence(trf796xx_next.uid,8);
		   sz_printk("2.keep\n");
		   read_complete = 4;
		   
		}
		else if(read_complete ==4)
		{
//			send_read_uid(0);
			sz_printk("3.keep\n");
            lite_enable_timer();
//			read_complete = 5;
		}
		
        return 0;
	}
    if(status_reg==0xA0)
    {
		//trf796xx_write_cmd(Reset);
//		if(read_complete ==3)
//		{
//			sz_printk("1.keep\n");
//		}
		return 0;
    }
	if(status_reg==0xC0)
    {
		if(read_complete==0)
		{
			trf796xx_write_cmd(TransmitCRC);
		    trf796xx_write_block(0x1D,uid_buf, 5);
		}
		return 0;
    }
    if(status_reg == 0x60|| status_reg == 0x40)
    {
		
		if(read_complete==0)
		{
			int len;
		    len = trf7962_read_fifo(trf796xx_next.uid,16,0);
		    if(len>0) 
			{
				printf_fifo(trf796xx_next.uid,len);
				trf796xx_next.uid_len = len;
			    if(sz_memcmp(trf796xx_cur.uid,trf796xx_next.uid,16)==0)
			    {
					//sz_printk("read the same card\n");
					//sz_printk("2.use time:%dms\n",HAL_GetTick()-start_time);
					//send_read_uid(0);
					send_read_block();
			    }
				else
				{
					//printf_fifo(trf796xx_next.uid,len);
					sz_memset(trf796xx_cur.uid,0, 16);
					sz_memcpy(trf796xx_cur.uid,trf796xx_next.uid,16);
					trf796xx_cur.uid_len = trf796xx_next.uid_len;
					send_read_block();
				}
		    } 
		}
		else
		{
			int len;
		    len = trf7962_read_fifo(trf796xx_next.block0,16,1);
		    if(len>0) 
			{
					printf_fifo(trf796xx_next.block0,len);
					sz_printk("1.use time:%dms\n",HAL_GetTick()-start_time);
					trf796xx_next.blk_len = len;
					check_out_card(trf796xx_next.uid,trf796xx_next.uid_len,
						trf796xx_next.block0,trf796xx_next.blk_len);
					keep_silence(trf796xx_next.uid,8);
					read_complete = 3;
//					trf796xx_write_block(0x1D,keep_silenc_buf, 5);
//					read_complete=0;
                    //send_read_uid(0);
		    }
			else
			{
				sz_printk("read blk err=%d\n",len);
				send_read_uid(0);
			}
			
		}
//        cnt = trf796xx_read_reg_t(FIFOStatus);
//		cnt = (0x0F&cnt)+0x01;
//		sz_printk("1.read length:%d\n",cnt);
//
//		
//		trf796xx_disable_irq();
//	}
//	else if(status_reg == 0x40)
//	{
//		cnt = trf796xx_read_reg_t(FIFOStatus);
//		cnt = (0x0F&cnt)+0x01;
//		sz_printk("2.read length:%d\n",cnt);
//		trf796xx_disable_irq();
//		trf796xx_write_cmd(Reset);
	}
	else 
	{
//		sz_printk("restart.....\n");
//		init_trf796xx_sys();
//		start_time=HAL_GetTick();
//	    send_read_uid(-1);
	}
	return 0;	
}


static void recv_irq()
{
	 switch(trf_796xx.step)
	 {
	 	case SEND_READ_BLK_CMD:
			
			break;
		case SEND_SILENCE_1_CMD:
			{
				#if 0
				int len;
                len = trf7962_read_fifo(trf796xx_next.block0,16,1);
				if(len>0)
				{
					printf_fifo(trf796xx_next.block0,len);
				}
				#else
				trf7962_read_fifo(trf796xx_next.block0,16,1);
				#endif
	 	    }
		case SEND_SILENCE_2_CMD:
			break;
	 }
}
static void trf7962_irq_handle()
{
	int status_reg = trf796xx_read_reg_t(0x0C);
	//sz_printk("1.IRQ:%#x\n",status_reg);
    switch(status_reg)
    {
		
		case 0x80:
			#if 1
			if(trf_796xx.step == SEND_SILENCE_2_CMD ||trf_796xx.step == SEND_SILENCE_3_CMD )
			{
				trf_796xx.read_blk_flg=0;
			}
			#endif
			trf796xx_read_reg_t(IRQStatus);
		    trf796xx_write_cmd(Reset);
			break;
		case 0xC0:
			break;
		case 0xA0:
//			if(trf_796xx.step == SEND_SILENCE_2_CMD ||trf_796xx.step == SEND_SILENCE_3_CMD )
//			{
//				trf_796xx.read_blk_flg=0;
//			}
			break;
		case 0x40:
		case 0x60:
			recv_irq();
			trf_796xx.read_blk_flg=0;
			break;
		default:
			break;
	}
}

void trf796xx_module_init()
{
	int res;
	trf796xx_pin_init();
	trf_796xx.step = CHIP_DISABLE;
	shedule_init(&trf7962_dlw_st);
	shedule_init(&trf7962_dlw_st_second);
	trf7962_dlw_st.handle = trf796xx_thread;
	trf7962_dlw_st_second.handle = trf796xx_second_thread;
	shedule_delay(&trf7962_dlw_st,30);
	shedule_delay(&trf7962_dlw_st_second,30);

	res = 0;
	get_subdev_card_cnt_flg((char *)&res,1);
	set_read_card_count_flg(res);

	res = 0;
	get_subdev_card_cnt_short_time((char *)&res,4);
	set_read_card_count_short_time(sz_ctoi((char *)&res,4));

    res = 0;
	get_subdev_card_cnt_long_time((char *)&res,4);
	set_read_card_count_long_time(sz_ctoi((char *)&res,4));

    res = 0;
	get_subdev_card_cnt_filter_time((char *)&res,4);
	set_read_card_count_filter_time(sz_ctoi((char *)&res,4));

	res = 0;
	get_subdev_card_cnt_print_flg((char *)&res,4);
	set_read_card_count_print(sz_ctoi((char *)&res,4));

	res = 0;
	get_subdev_card_read_time_out_flg((char *)&res,4);
	set_read_card_time_out(sz_ctoi((char *)&res,4));
{
	char *pt;
	trf796xx_next.pos = 0;
	pt = trf796xx_next.block[0];
	for(res = 0;res < 3;res++)//static char keep_silenc_buf[14]={10>>8,10<<4,0x20,0x02,0};
	{
		pt = trf796xx_next.block[res];
		*pt++ = (char)(10>>8);
		*pt++ = (char)(10<<4);
		*pt++ = 0x20;
		*pt++ = 0x02;	
	}
}
	
}


extern void Initial7960s();
extern void _tmain();



void write_read_block_test()
{
	
char _tbuf[12];
	int i;
	sz_memset(_tbuf, 0, 12);
	for(i = 0;i<12;i++)
	    _tbuf[i] = i;
	trf796xx_write_block(0x00,_tbuf,10);
	sz_memset(_tbuf, 0, 12);
	trf796xx_read_block(0x00, _tbuf,10);
	for(i = 0;i<10;i++)
	    sz_printk("reg[%d]=%#x\n",i,_tbuf[i]);
}




//static char tmp_block[2][16]={{0},{0}};
//static int tem_blk_cnt = 0;

static int trf796xx_cmp_card()
{
	int i;
	char *pt = trf796xx_next.block[trf796xx_next.pos];
	//sz_printk("pos:%d\n",trf796xx_next.pos);
	for(i = 0;i < 3;i++)
	{

		if(pt == trf796xx_next.block[i])
		{
		    continue;	
		}
		else
		{
			if(sz_memcmp((const void *)&trf796xx_next.block[i][4], (const void *)&pt[4],7)==0)
            {
                return -1;
            }
		}
	}
    
    return 0;
}

void clear_store_repeat_card()
{
	int i;
	for(i=0;i<3;i++)
	{
		sz_memset(&trf796xx_next.block[i][4],0,4);
	}
}
static void trf796xx_second_thread(struct delay_work *p_dw)
{
	int current_time = HAL_GetTick();
	if(trf_796xx.enable_count_time==0 && trf_796xx.count_enable == 0)
	{
		
		if((current_time-trf_796xx.count_time)>trf_796xx.threshold_time)
		{
			int i,cnt;
			char ch = 0;
			DIS_ALL_INT();
			if(trf_796xx.cnt_print_flg==0)
			{
			    sz_printk("2.irq num.:%d,read num.:%d\n",trf_796xx.irq_cnt,trf_796xx.read_card_cnt);
			}
			cnt = trf_796xx.irq_cnt - trf_796xx.read_card_cnt;
			if(cnt > 0)
			{
				if(trf_796xx.cnt_print_flg==0)
				{
					sz_printk("2.aft push %d err_cards,read_card_cnt=%d\n",cnt,trf_796xx.read_card_cnt);
				}
				for(i=0;i<cnt;i++)
				{
					store_card(ch);
				}
				
			}
			trf_796xx.read_card_cnt = trf_796xx.irq_cnt;
			trf_796xx.enable_count_time = -1;
			EN_ALL_INT();
		}

		
	}

	if((current_time - trf_796xx.asy_time)>60*1000)
	{
		DIS_ALL_INT();
		trf_796xx.asy_time = current_time;
		trf_796xx.read_card_cnt = trf_796xx.irq_cnt;
		EN_ALL_INT();
		clear_store_repeat_card();
	}
	shedule_delay(p_dw,1);
}

static void trf796xx_thread(struct delay_work *p_dw)
{
    switch(trf_796xx.step)
    {
		
		case CHIP_DISABLE:
			GO_NEXET_STEP(trf_796xx.step);
			trf796xx_disable();
			shedule_delay(&trf7962_dlw_st,30);
		    break;
	    case CHIP_ENABLE:
			GO_NEXET_STEP(trf_796xx.step);
			trf796xx_enable();
			shedule_delay(&trf7962_dlw_st,30);
			break;
		case INIT_DEVICE:
			GO_NEXET_STEP(trf_796xx.step);
			init_trf796xx_sys() ;	
		case IRQ_ENABLE:
			GO_NEXET_STEP(trf_796xx.step);
			trf796xx_enable_irq();
			//send_read_uid(0);
			trf7962_select_flg = 1;
			shedule_delay(&trf7962_dlw_st,100);
			break;
		case SEND_READ_BLK_CMD:
			GO_NEXET_STEP(trf_796xx.step);
			trf7962_read_multy_block();
		    shedule_delay(&trf7962_dlw_st,3);
			trf_796xx.read_blk_flg = -1;
			trf_796xx.time_cnt = 0;
			//sz_printk("send read uid\n");
			break;
		case SEND_SILENCE_1_CMD:
			
//			if(trf_796xx.read_blk_flg==0)
//			{
//				GO_NEXET_STEP(trf_796xx.step);
//				trf796xx_next.block0[8]=0xE0;
//				keep_silence(&trf796xx_next.block0[1],8);
//				shedule_delay(&trf7962_dlw_st,45);
//				sz_printk("1.send keep s\n");
//			}
//			else
//			{
//				GOTO_STEP(trf_796xx.step,SEND_READ_BLK_CMD);
//				shedule_delay(&trf7962_dlw_st,1);
//			}
			#if 1
			if(trf_796xx.read_blk_flg==0)
			{
				char *pt = trf796xx_next.block[trf796xx_next.pos];
				if(trf_796xx.count_enable == 0)
				{
					int cnt;	
					char ca_ch;
					DIS_ALL_INT();
					trf_796xx.enable_count_time = -1;
					decode_card(trf796xx_next.block0+1,pt+4);
					//printf_fifo(pt+4,8);
					ca_ch = pt[11]&0x7F;
					if(ca_ch<0 || ca_ch > 44)
					{
						//sz_printk("read a invalid card\n");
						//trf_796xx.read_card_cnt ++;
						GOTO_STEP(trf_796xx.step,SEND_READ_BLK_CMD);
					}
					else
					{
					    if(trf796xx_cmp_card()==0)
    					{
    						//printf_fifo(trf796xx_next.block0,8);
    						trf796xx_next.repeat_flg = 0;
    						trf_796xx.read_card_cnt ++;
    						if(trf_796xx.cnt_print_flg==0)
    						{
    						    sz_printk("1.irq num.:%d,read num.:%d\n",trf_796xx.irq_cnt,trf_796xx.read_card_cnt);
    						}
    						cnt = trf_796xx.read_card_cnt - trf_796xx.irq_cnt;
    						if(cnt>=0)
    						{
    					    	store_card(pt[11]);//¥Ê≈∆
    						}
    						else
    						{
    							int i;
        						char ch = 0;
        						cnt = -cnt;
    							if(trf_796xx.cnt_print_flg==0)
    							{
    	    					    sz_printk("1.aft push %d err_cards,read_card_cnt=%d\n",cnt,trf_796xx.read_card_cnt);
    							}
    	    					for(i=0;i<cnt;i++)
    	    					{
    	    						store_card(ch);
    	    					}
    							store_card(pt[11]);
    	    					trf_796xx.read_card_cnt = trf_796xx.irq_cnt;
    						}
    					}
    					else
    					{
    						//sz_printk("read a repeat card\n");
    						trf796xx_next.repeat_flg = -1;
							//trf_796xx.read_card_cnt = trf_796xx.irq_cnt;
    					}
						
                        pt[11]=0xE0;
                        keep_silence_t(pt,14);
						//sz_printk("1.send keep\n");
                        GO_NEXET_STEP(trf_796xx.step);
                        trf_796xx.time_cnt = 0;
                        trf_796xx.read_blk_flg = -1;
					}
					EN_ALL_INT();
					trf_796xx.asy_time = HAL_GetTick();
					shedule_delay(&trf7962_dlw_st,1);
				}
			    else
			    {
					
			    }
			}
			else
			{
                if(trf_796xx.time_cnt > trf_796xx.read_time_out)
                {					
                    GOTO_STEP(trf_796xx.step,SEND_READ_BLK_CMD);
					shedule_delay(&trf7962_dlw_st,1);
                }
                else 
				{
					trf_796xx.time_cnt++;
					shedule_delay(&trf7962_dlw_st,1);
                }				
			}
			#endif
			break;
		case SEND_SILENCE_2_CMD:
//            keep_silence(&trf796xx_next.block0[1],8);
//			shedule_delay(&trf7962_dlw_st,45);
//			GOTO_STEP(trf_796xx.step,SEND_READ_BLK_CMD);
//			sz_printk("2.send keep s\n");
			#if 1
			if(trf_796xx.read_blk_flg==0)
			{
				//HAL_Delay(30);
				int gg=trf796xx_next.pos;
    			char *pt = trf796xx_next.block[gg];
				if(trf796xx_next.repeat_flg == 0)
				{
    				int gg=trf796xx_next.pos;
    				char *pt = trf796xx_next.block[gg];
    				gg = (gg+1)%3;
    				trf796xx_next.pos =gg;
				}
				//GOTO_STEP(trf_796xx.step,SEND_READ_BLK_CMD);
    			GO_NEXET_STEP(trf_796xx.step);
				//keep_silence(&trf796xx_next.block0[1],8);
				keep_silence_t(pt,14);
				//sz_printk("2.send keep\n");
				trf_796xx.time_cnt = 0;
				trf_796xx.read_blk_flg = -1;
				shedule_delay(&trf7962_dlw_st,1);
			}
			else
			{
                if(trf_796xx.time_cnt > 40)
                {
					if(trf796xx_next.repeat_flg == 0)
				    {
    					int gg=trf796xx_next.pos;
    					gg = (gg+1)%3;
    					trf796xx_next.pos =gg;
					}
                    GOTO_STEP(trf_796xx.step,SEND_READ_BLK_CMD);
					shedule_delay(&trf7962_dlw_st,1);
                }
                else 
				{
					trf_796xx.time_cnt++;
					shedule_delay(&trf7962_dlw_st,1);
					
                }				
			}
            #endif

			
			break;
		case SEND_SILENCE_3_CMD:
			if(trf_796xx.read_blk_flg==0)
			{
		        shedule_delay(&trf7962_dlw_st,1);
				GOTO_STEP(trf_796xx.step,SEND_READ_BLK_CMD);
				//sz_printk("3.send keep end\n",HAL_GetTick());
			}
			else
			{
                if(trf_796xx.time_cnt > 40)
                {
                    GOTO_STEP(trf_796xx.step,SEND_READ_BLK_CMD);
					shedule_delay(&trf7962_dlw_st,1);
                }
                else 
				{
					trf_796xx.time_cnt++;
					shedule_delay(&trf7962_dlw_st,1);
                }				
			}
			break;
		default:
			break;
    }
    
}


static struct trf796xx_data_t str_regs[]=
{
#if 1
	{ChipStateControl,0x31},
	{ISOControl,0x01},
	{ModulatorControl,0x01},
	{RXNoResponseWaitTime,0x0E},
	{RXWaitTime,0x1F},
    {RXSpecialSettings,0x40},
    {RegulatorControl,0x06}
#else
    {ChipStateControl,0x21},
	{ISOControl,0x01},
	{ModulatorControl,0x01},
	{RXNoResponseWaitTime,0x0E},
	{RXWaitTime,0x1F},
    {RXSpecialSettings,0x40},
    {RegulatorControl,0x06}
#endif
};


static void init_trf796xx_chip()
{
	trf796xx_write_regs(str_regs,sizeof(str_regs)/sizeof(struct trf796xx_data_t));
}
void init_trf796xx_sys()
{
	sz_printk("7962 init device\n");
	trf796xx_write_cmd(SoftInit);
//	trf796xx_write_reg_t(ChipStateControl, 0x2C);
//	trf796xx_write_reg_t(ISOControl, 0x05);
//	trf796xx_write_reg_t(ModulatorControl, 0x07);
//	//trf796xx_write_reg_t(RXNoResponseWaitTime, 0x01);
//	//trf796xx_write_reg_t(RXWaitTime, 0x01);
//	trf796xx_write_reg_t(RXSpecialSettings, 0x42);//
//	trf796xx_write_reg_t(RegulatorControl, 0x06);

#if 0
    trf796xx_write_reg_t(ChipStateControl, 0x3E);	
    trf796xx_write_reg_t(ISOControl, 0x01);	
    trf796xx_write_reg_t(ModulatorControl, 0x31);	
    //	trf796xx_write_reg_t(RXNoResponseWaitTime, 0x0E);
    //	trf796xx_write_reg_t(RXWaitTime, 0x1F);  
    trf796xx_write_reg_t(RXNoResponseWaitTime, 0x01);	
    trf796xx_write_reg_t(RXWaitTime, 0x01);		
    trf796xx_write_reg_t(RXSpecialSettings, 0x40);
    trf796xx_write_reg_t(RegulatorControl, 0x06);
#else
    init_trf796xx_chip();
#endif 
   
    trf796xx_read_reg_t(0x0C);
}

void send_read_uid(int flg)
{
	//sz_printk("22222\n");
	//sz_printk("start:%d\n",HAL_GetTick());
	if(flg==0)
	{
	    start_time=HAL_GetTick();
	}
	read_complete = 0;
	trf796xx_read_reg_t(0x0C);
	trf796xx_write_cmd(Reset);;//REST FIFO
	trf796xx_write_cmd(TransmitCRC);//CRC
	trf796xx_write_block(0x1D,uid_buf, 5);
}

void send_read_id_t()
{
	trf796xx_write_block(0x1D,uid_buf, 5);
}

void send_read_block()
{
	read_complete = 1;
	trf796xx_write_cmd(Reset);//REST FIFO
	trf796xx_write_cmd(TransmitCRC);//CRC
	trf796xx_write_block(0x1D,block_buf, 5);
}

extern void TRF7960(void);    
void EXTI0_1_IRQHandler(void)
{
    #if 0
	TRF796XX_IT_PROC();
	reset_trt7962_time();
	#else
	trf7962_irq_handle();
	#endif
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1); 
}

int trf7962_module_entery()
{
	
}

//static int cards_cnt = 0;
static int filter_time;
void EXTI4_15_IRQHandler(void)
{
	int current_time = HAL_GetTick();
	trf_796xx.asy_time = current_time;
    if(trf_796xx.count_enable == 0)
    {
        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12)==0)
        { 
        	//sz_printk("%dst cards begin\n",trf_796xx.irq_cnt+1);
			filter_time = current_time;
        }
        else
        {
        	int ret = current_time - filter_time;
        	if(ret<trf_796xx.filter_time)
    		{
				if(trf_796xx.cnt_print_flg==0)
				{
    			    sz_printk("filter time:%d\n",ret);
				}
    		}
			else
			{	
				if(ret<80)
				{
					trf_796xx.threshold_time = trf_796xx.long_time;
				}
				else
				{
					trf_796xx.threshold_time = trf_796xx.short_time;
				}
	        	trf_796xx.irq_cnt++;
				if(trf_796xx.cnt_print_flg==0)
				{
	        	    sz_printk("3.irq num.:%d,read num.:%d\n",trf_796xx.irq_cnt,trf_796xx.read_card_cnt);
				}
				ret = trf_796xx.irq_cnt - trf_796xx.read_card_cnt;
				if(ret>0)
				{
					trf_796xx.enable_count_time = 0;
		        	trf_796xx.count_time = current_time;
					//trf_796xx.read_card_cnt++;
				}
//	        	else if(ret == 1)
//	        	{
//		        	trf_796xx.enable_count_time = 0;
//		        	trf_796xx.count_time = current_time;
//	        	}
//				else
//				{
//					//do nothing
//				}
			}
        }
    }
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12); 
}
