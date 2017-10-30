#include "sz_libc.h"
#include "ring_buffer.h"
#include "device_mangement.h"
#include "module_common.h"

static RING_BUF cards_rng;
static char cards_buf[64];
static char pre_card;


//�Ƶ�ƫ����
static char card_name[45][6] = { { "����" }, { "һ�f" }, { "���f" }, { "���f" }, { "���f" }, { "���f" }, { "���f" }, { "���f" }, { "���f" }, { "���f" },
{ "һͲ" }, { "��Ͳ" }, { "��Ͳ" }, { "��Ͳ" }, { "��Ͳ" }, { "��Ͳ" }, { "��Ͳ" }, { "��Ͳ" }, { "��Ͳ" },
{ "һ��" }, { "����" }, { "����" }, { "����" }, { "����" }, { "����" }, { "����" }, { "����" }, { "����" },
{ "��" }, { "��" }, { "��" }, { "��" }, { "��" }, { "�l" }, { "��" }, { "��" }, { "��" },
{ "��" }, { "��" }, { "÷" }, { "��" }, { "��" }, { "��" }, { "�ٴ�" }, { "�װ�" }
};

//static char card_name[45][8] = { { "error" }, { "YiWan" }, { "ErWan" }, { "SanWan" }, { "SiWan" }, { "WuWan" }, { "LiuWan" }, { "QiWan" }, { "BaWan" }, { "JiuWan" },
//{ "YiTong" }, { "ErTong" }, { "SanTong" }, { "SiTong" }, { "WuTong" }, { "LiuTong" }, { "QiTong" }, { "BaTong" }, { "JiuTong" },
//{ "YiTiao" }, { "ErTiao" }, { "SanTiao" }, { "SiTiao" }, { "WuTiao" }, { "LiuTiao" }, { "QiTiao" }, { "BaTiao" }, { "JiuTiao" },
//{ "Dong" }, { "Nan" }, { "Xi" }, { "Bei" }, { "Zhong" }, { "Fa" }, { "Men" }, { "Chun" }, { "Xia" },
//{ "Qiu" }, { "Tong" }, { "Mei" }, { "Lan" }, { "Ju" }, { "Chu" }, { "BaiTa" }, { "BaiBan" }
//};
#define Max_Card_number  44
//ǰ�˸���UID �� �ĸ���block 
// �߰�λ�� 0x00��ɫ 0x80��ɫ  �Ͱ�λƫ���� ��1-44��ȷ 0����
int check_out_card(char *uid_buf, int uid_len,char *blk_buf,int blk_len)
{
	int crc16Vlaue = 0;
	int blockValue = 0;
	if (uid_len > 12)  //���ȴ���
	{ 
		return 0; 
	}
	crc16Vlaue = Crc16Result((char *)uid_buf, uid_len);
	blockValue = sz_ctoi(blk_buf,blk_len);
	blockValue -= crc16Vlaue;
	if(blockValue<0)
		blockValue = -blockValue;
	if ((blockValue&0x7F)> Max_Card_number)  //�Ƶ�ƫ��������
	{ 
		return 0; 
	}
	//sz_printk("%s card value:%s\n",(blockValue&0x80)==0?"blue":"green",card_name[blockValue&0x7F]);
	return blockValue;
}



void input_card()
{
	
}
int getcards(char *src,int src_len)
{
	return pop_buf(&cards_rng,src,src_len);
}
int card_module_init()
{
	init_ring_buf(&cards_rng,cards_buf,64);
	return 0;
}
static int card_cnt = 0;
void store_card(char card)
{ 
	int ret = card & 0x7F;
	if(ret < 0 || ret > 44)
	{
		sz_printf("card num. is out num\n");
		return ;
	}
	sz_printk("%-dth:%s\n",++card_cnt,card_name[ret]);
    push_buf(&cards_rng,&card,1);
	if((pre_card == 0x13 && card == 0x93) ||(pre_card == 0x93 && card == 0x13))
	{
		device_setup(LITECTRL_FIRST_RUNNING,0);
		pre_card = 0x00;
	}
	pre_card = card; 
}


//����
//*in  ������Ҫ���ܵ�ID 
//*out ���������ID 
void decode_card(char *in,char *out)
{
   switch(in[0]/55)
   {
    case 0: 
    {
      out[0]=in[3];out[1]=in[0];out[2]=in[7];out[3]=in[4];out[4]=in[1];out[5]=in[2];out[6]=in[5];out[7]=in[6];    
      break;
    }
    case 1:
    {
      out[0]=in[2];out[1]=in[0];out[2]=in[5];out[3]=in[1];out[4]=in[7];out[5]=in[6];out[6]=in[3];out[7]=in[4];
      break;
    }
    case 2:
    {
     out[0]=in[7];out[1]=in[0];out[2]=in[3];out[3]=in[6];out[4]=in[5];out[5]=in[4];out[6]=in[2];out[7]=in[1];
      break;
    }
    case 3:
    {
      out[0]=in[4];out[1]=in[0];out[2]=in[1];out[3]=in[7];out[4]=in[2];out[5]=in[3];out[6]=in[6];out[7]=in[5];
      break;
    }
    case 4:
    {
      out[0]=in[6];out[1]=in[0];out[2]=in[4];out[3]=in[5];out[4]=in[3];out[5]=in[7];out[6]=in[1];out[7]=in[2];
      break;
    }
    default: break;  
   }
}
