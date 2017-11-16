#include "sz_libc.h"
#include "ring_buffer.h"
#include "device_mangement.h"
#include "module_common.h"
#include "trf796xx_module.h"

#define _NEW_
static RING_BUF cards_rng;
static char cards_buf[64];
static char pre_card;


//牌的偏移量
static char card_name[45][6] = { { "错误" }, { "一萬" }, { "二萬" }, { "三萬" }, { "四萬" }, { "五萬" }, { "六萬" }, { "七萬" }, { "八萬" }, { "九萬" },
{ "一筒" }, { "二筒" }, { "三筒" }, { "四筒" }, { "五筒" }, { "六筒" }, { "七筒" }, { "八筒" }, { "九筒" },
{ "一条" }, { "二条" }, { "三条" }, { "四条" }, { "五条" }, { "六条" }, { "七条" }, { "八条" }, { "九条" },
{ "东" }, { "南" }, { "西" }, { "北" }, { "中" }, { "發" }, { "门" }, { "春" }, { "夏" },
{ "秋" }, { "东" }, { "梅" }, { "兰" }, { "菊" }, { "竹" }, { "百搭" }, { "白板" }
};

//static char card_name[45][8] = { { "error" }, { "YiWan" }, { "ErWan" }, { "SanWan" }, { "SiWan" }, { "WuWan" }, { "LiuWan" }, { "QiWan" }, { "BaWan" }, { "JiuWan" },
//{ "YiTong" }, { "ErTong" }, { "SanTong" }, { "SiTong" }, { "WuTong" }, { "LiuTong" }, { "QiTong" }, { "BaTong" }, { "JiuTong" },
//{ "YiTiao" }, { "ErTiao" }, { "SanTiao" }, { "SiTiao" }, { "WuTiao" }, { "LiuTiao" }, { "QiTiao" }, { "BaTiao" }, { "JiuTiao" },
//{ "Dong" }, { "Nan" }, { "Xi" }, { "Bei" }, { "Zhong" }, { "Fa" }, { "Men" }, { "Chun" }, { "Xia" },
//{ "Qiu" }, { "Tong" }, { "Mei" }, { "Lan" }, { "Ju" }, { "Chu" }, { "BaiTa" }, { "BaiBan" }
//};
#define Max_Card_number  44


static int card_info_pf = 1;
void set_card_info_pf(int new_f)
{
	sz_printk("onlie_card_print old:%d,new:%d\n",card_info_pf , new_f);
	card_info_pf = new_f;
}

//前八个是UID 后 四个是block 
// 高八位的 0x00蓝色 0x80绿色  低八位偏移量 （1-44正确 0错误）
int check_out_card(char *uid_buf, int uid_len,char *blk_buf,int blk_len)
{
	int crc16Vlaue = 0;
	int blockValue = 0;
	if (uid_len > 12)  //长度错误
	{ 
		return 0; 
	}
	crc16Vlaue = Crc16Result((char *)uid_buf, uid_len);
	blockValue = sz_ctoi(blk_buf,blk_len);
	blockValue -= crc16Vlaue;
	if(blockValue<0)
		blockValue = -blockValue;
	if ((blockValue&0x7F)> Max_Card_number)  //牌的偏移量过大
	{ 
		return 0; 
	}
	//sz_printk("%s card value:%s\n",(blockValue&0x80)==0?"blue":"green",card_name[blockValue&0x7F]);
	return blockValue;
}



void init_card()
{
	int res;
	res = 0;
	get_subdev_card_online_print_flg((char *)&res,4);
	set_card_info_pf(sz_ctoi((char *)&res,4));
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
	if(card_info_pf>0)
	{
	    sz_printk("%-dth:%s(%d)\n",++card_cnt,card_name[ret],ret);
	}
    push_buf(&cards_rng,&card,1);
	if((pre_card == 0x0E && card == 0x8E) ||(pre_card == 0x8E && card == 0x0E))
	{
		device_setup(LITECTRL_FIRST_RUNNING,0);
		pre_card = 0x00;
	}
	pre_card = card; 
}

#ifdef _NEW_
static int key[2]={0x3824cd57,0x8637acbf};
#endif
//加密牌
//*in  输入要加密的ID
//*out 输出要加密后的IDs
//void encode_card(char *buf,char *out)
//{
//	char *src = out;
//	char *end = out + 8;
//	char tmp[8];
//	char *dst = tmp;
//	while(src<end)
//	{
//		  *dst = *src ;
//          ++src;
//		  ++dst;
//	}
//	
//   switch(buf[1]/55)
//   {
//    case 0: 
//    {
//      out[3]=buf[0];out[0]=buf[1];out[7]=buf[2];out[4]=buf[3];out[1]=buf[4];out[2]=buf[5];out[5]=buf[6];out[6]=buf[7];     
//      break;
//    }
//    case 1:
//    {
//      out[2]=buf[0];out[0]=buf[1];out[5]=buf[2];out[1]=buf[3];out[7]=buf[4];out[6]=buf[5];out[3]=buf[6];out[4]=buf[7];
//      break;
//    }
//    case 2:
//    {
//      out[7]=buf[0];out[0]=buf[1];out[3]=buf[2];out[6]=buf[3];out[5]=buf[4];out[4]=buf[5];out[2]=buf[6];out[1]=buf[7];
//      break;
//    }
//    case 3:
//    {
//      out[4]=buf[0];out[0]=buf[1];out[1]=buf[2];out[7]=buf[3];out[2]=buf[4];out[3]=buf[5];out[6]=buf[6];out[5]=buf[7];
//      break;
//    }
//    case 4:
//    {
//      out[6]=buf[0];out[0]=buf[1];out[4]=buf[2];out[5]=buf[3];out[3]=buf[4];out[7]=buf[5];out[1]=buf[6];out[2]=buf[7];
//      break;
//    }
//	#ifdef _NEW_
//	{
//		
//    	*(int*)out = (*(int*)out)^key[0];
//    	*(int*)(out+4) = (*(int*)(out+4))^key[1];
//	}
//	#endif
//    default: break;  
//   }
//}

//解牌
//*in  输入需要解密的ID 
//*out 输出真正的ID 
//小机端主要字节对齐
void decode_card(char *buf,char *out)
{
	
	char *src = buf;
	char *end = src + 8;
	char in[8];
	char *dst = in;
	while(src<end)
	{
		  *dst = *src ;
          ++src;
		  ++dst;
	}
	#ifdef _NEW_
    *(int*)in = (*(int*)in)^key[0];
    *(int*)(in+4) = (*(int*)(in+4))^key[1];
	#endif
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
