#include "device_pakage.h"
#include "device_info.h"
#include "cc110x_hal.h"
#include "cc110x_adapter.h"
#include "sz_libc.h"
#include "LiteMasterResource.h"
#include "device_mangement.h"
#include "check_out_card.h"
#include "led.h"
//static int l_cseq,l_bm_crc32;
static char back_card_buf[64];
static char back_cads_len=0;

static void back_up_pakage(char *buf,int len)
{
	sz_memset(back_card_buf,0,64);
	sz_memcpy(back_card_buf,buf,len);
	back_cads_len = len;
}
static int device_send_old_pakage()
{
//	int len;
    struct cc1101_msg_list_t *p_cc110_msg;
    p_cc110_msg= cc1101_msg_alloc_send_msg();
    if(p_cc110_msg!=0)
    {
		p_cc110_msg->len = back_cads_len;
		sz_memcpy(p_cc110_msg->buf,back_card_buf,back_cads_len);
    	cc101_msg_send_commit(p_cc110_msg,1,5);
		return 0;
    }
    return -1; 
}

static int device_pakage_cards()
{
	int len;
    struct cc1101_msg_list_t *p_cc110_msg;
	char cards[8]={0};
//	static int cnt_=0;
	
    p_cc110_msg= cc1101_msg_alloc_send_msg();
    if(p_cc110_msg)
    {
		#if 1
		len = getcards(cards,8);
		if(len<=0)
		{
			len = 1;
			cards[0] = 0xFF;
		}
		len = cc1101_sub_pakage_send_card(p_cc110_msg->buf,p_cc110_msg->max,
			get_masterdevice(),get_subdevice(),(char*)cards,len);
		#else
		sz_memset(cards,0,8);
	    //len = sz_snprintf(cards,8,"%d",cnt_++);
	    cards[0] = cnt_++;
	    cnt_=cnt_%44;
		len = cc1101_sub_pakage_send_card(p_cc110_msg->buf,p_cc110_msg->len,
			get_masterdevice(),get_subdevice(),(char*)cards,1);
		#endif
		p_cc110_msg->len = len;
		back_up_pakage(p_cc110_msg->buf,p_cc110_msg->len);
    	cc101_msg_send_commit(p_cc110_msg,1,5);
		return 0;
    }
    return -1; 
}

int device_pakage_match_set_addr_respond()
{
	int len;
    struct cc1101_msg_list_t *p_cc110_msg;
//	static int cnt_=0;
    p_cc110_msg= cc1101_msg_alloc_send_msg();
    if(p_cc110_msg)
    {
    	len = cc1101_sub_pakage_match_set_addr(p_cc110_msg->buf,p_cc110_msg->max,
			get_masterdevice(),get_subdevice());
		p_cc110_msg->len = len;
    	cc101_msg_send_commit(p_cc110_msg,5,10);
		sz_printk("send set addr respond success\n");
		return 0;
    }
	sz_printk("send set addr respond fail\n");
    return -1; 
}

static int send_special_cmd_ack()
{
	int len;
    struct cc1101_msg_list_t *p_cc110_msg;
    p_cc110_msg= cc1101_msg_alloc_send_msg();
    if(p_cc110_msg)
    {
		//sz_printk("\n\nsend pos:%d\n",which);
    	len = cc1101_master_pakage_send_sig_ack(p_cc110_msg->buf,p_cc110_msg->max,cc1101_get_rx_sig_strength());
		p_cc110_msg->len = len;
    	cc101_msg_send_commit(p_cc110_msg,2,5);
		return 0;
    }
    return -1; 
}

int cc1101_handle_data(char *buf,int buf_len)
{
	char *p_buf = buf;
	int addr;
	int cseq;
	int cmd;
	int pos;
	addr = (int)*p_buf;
	++p_buf;
	cseq = sz_ctoi(p_buf,4);
	p_buf+=4;
    cmd = (int)*p_buf;
	++p_buf;
    pos = (int)*p_buf;
	++p_buf;
	buf_len-=7;
	//sz_printk("cseq=%d,cmd=%#x,pos=%d\n",cseq,cmd,pos);
    switch(cmd)
    {
		case CC1101_MTOS_GET_CARD:
			
			if(sz_memcmp(p_buf,get_masterdevice()->master_local_id,12)!=0)//不是本机的主机发出的
            {
               //sz_printk("1.is not my master\n");
                return -1;
            }
			if(addr==0xFF || addr==0x00)
			{
				set_online_time();
				break;
			}
			if(cseq!=get_subdevice()->cseq)
			{
				get_subdevice()->cseq = cseq;
				device_pakage_cards();
			}
			else
			{

				device_send_old_pakage();
			}
			set_online_time();
			break;
		case CC1101_MTOS_MATCH:
//			if(get_device_mode()==LITECTRL_NORMAL_WORK)
//				break;
			if(get_masterdevice()->cseq == cseq)
			{
				sz_printk("recv same match broadcast\n");
				break;
			}
			sz_printk("recv match broadcast\n");
			sz_memset(get_masterdevice()->master_local_id,0,12);
			sz_memcpy(get_masterdevice()->master_local_id,p_buf,12);
			set_master_id(p_buf,12);
			device_init_sys_info();
			get_masterdevice()->cseq = cseq;
			break;
		case CC1101_MTOS_SETADDR:
//			if(get_device_mode()==LITECTRL_NORMAL_WORK)
//				break;
			//sz_printk("pos is %d\n",get_subdevice()->position_id);
			
			//sz_printk("pos:%#x(%#x)\n",pos,get_subdevice()->position_id);
			if((pos&0x80)==0x80)
			{
				sz_printk("set position\n");
                set_subdev_pos((char*)&pos,1);//write the posiion
                get_subdevice()->position_id = pos&0x7F;
			}
			else if((pos&0x7f)!=get_subdevice()->position_id)
			{
				sz_printk("is not my pos cmd\n");
				break;
			}
			if(sz_memcmp(get_masterdevice()->master_local_id,p_buf,12)!=0)
			{
//				sz_printk("recv set addr broadcast\n");
//				sz_printk("2.is not master\n");
				break;
			}
			
			if(get_masterdevice()->cseq == cseq)
			{
				//sz_printk("recv same set addr broadcast\n");
				break;
			}
			sz_printk("recv set addr broadcast\n");

			
			p_buf+=12;
			set_master_addr(p_buf,1);
			
			++p_buf;
			set_subdev_addr(p_buf,1);
			cc110x_write_t(ADDR,(int)*p_buf);
			device_init_sys_info();
			get_masterdevice()->cseq = cseq;
			device_pakage_match_set_addr_respond();
			{
				int res = 240;
			    set_subdev_flg((char*)&res,1);
			}
			break;
		case CC1101_MTOS_SET_CARD_CNT:
			if(pos !=get_subdevice()->position_id)
			{
				//sz_printk("set card count is not my pos cmd\n");
				break;
			}
			if(sz_memcmp(get_masterdevice()->master_local_id,p_buf,12)!=0)
			{
				//sz_printk("set card count is not master\n");
				break;
			}
			p_buf+=12;
			set_subdev_card_cnt_flg(p_buf,1);
			device_init_sys_info();
			get_masterdevice()->cseq = cseq;
			set_online_time();
			break;
		case CC1101_COMM_REPORT_SIG:
			//sz_printk("sig.\n");
			//p_buf+=12;
			cc1101_set_tx_sig_strength((int)(*p_buf));
			send_special_cmd_ack();
			break;
	 	default:
			sz_printk("dst_addr:%#x,recv error cmd:%#x\n",addr,cmd);
			break;
    }
	return 0;
	
}



