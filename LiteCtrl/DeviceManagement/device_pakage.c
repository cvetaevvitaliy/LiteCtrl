#include "device_pakage.h"
#include "sz_libc.h"
//exchange master id
int cc1101_sub_pakage_match_set_addr(char *buf,int buf_len,struct master_device_t *pmaster,struct sub_device_t *psub)
{
	char *p_buf = buf;
	
	*p_buf = pmaster->masteraddr;//or 0xff
	++p_buf;
	
	sz_itoc(p_buf,pmaster->cseq);
	p_buf+=4;
	
	*p_buf = CC1101_STOM_SETADDR;
	++p_buf;

	*p_buf = (char)psub->position_id;
	++p_buf;
	
	return p_buf-buf;
}

int cc1101_sub_pakage_send_card(char *buf,int buf_len,struct master_device_t *pmaster,struct sub_device_t *psub,
	char *card,int card_cnt)
{
	char *p_buf = buf;
	*p_buf = (char)pmaster->masteraddr;//
	++p_buf;
	
	sz_itoc(p_buf,psub->cseq);
	p_buf+=4;
	
	*p_buf = CC1101_STOM_CARD;
	++p_buf;

	*p_buf = (char)psub->position_id;
	++p_buf;
	
	sz_memcpy(p_buf,card,card_cnt);
	p_buf+=card_cnt;
	
	return p_buf-buf;
}


int cc1101_master_pakage_send_sig_ack(char *buf,int buf_len,char sig_str)
{
	char *p_buf = buf;

	//addr
	*p_buf = 0xFF;//
	++p_buf;
	
	//cseq
	//sz_itoc(p_buf,psub->cseq);
	//sz_printk("sub %d cseq:%d\n",psub->position_id,psub->cseq);
	p_buf+=4;
	
	//cmd
	*p_buf = CC1101_COMM_REPORT_SIG_ACK;
	++p_buf;
	
	//position
	//*p_buf = (char)psub->position_id;
	++p_buf;
	
	//master id
	//sz_memcpy(p_buf,pmaster->master_local_id,12);
	//p_buf+=12;

	*p_buf = sig_str;
	++p_buf;
	return p_buf-buf;
}

