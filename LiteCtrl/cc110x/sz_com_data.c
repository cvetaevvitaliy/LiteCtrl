

void *(*_memcpy)(char *dst,char *src,int len);
/*
*|*****|*******|*************|*********|*****|*********|********|******|
*header length   pad. lengh    datalen0 data0 datalen1 data1 ....ender
*/
static void sz_pkzie(char *src,int len)
{
	*src++=len&0xff;
	*src++=(len>>8)&0xff;
	*src++=(len>>16)&0xff;
	*src++=(len>>24)&0xff;
}
static int sz_unpksize(char *src)
{
	int res = 0;
	res = *src++;
	res |= ((*src++))<<8;
	res |= ((*src++))<<16;
	res |= ((*src++))<<24;
	return res;
}
int sz_pakage(char *buf,int len,int argc,...)
{
	int  sub_len;
    char *sub_src;
	char *ap;
	int in_length;;
	int tmp=0;
	int i;
	ap = (char *)&argc+4;
	buf+=4;
	in_length = 1;
	for(i=0;i<argc;i++)
	{
	    sub_src = *((char**)ap);
		ap+=4;
		sub_len = *(int*)ap;
		ap+=4;
		sz_pkzie(buf+in_length,sub_len);
		in_length+=sizeof(int);
		_memcpy(buf+in_length,sub_src,sub_len);
		in_length+=sub_len;
	}
	tmp = in_length&(0x07);
//	printf("tmp:%d\n",tmp);
	if(tmp)
	{
		tmp = 8 - tmp;
		in_length+=tmp;
	}
//	printf("len:%d\n",in_length);
	buf-=4;
	sz_pkzie(buf,in_length);
	*(char*)(buf+4)=tmp;
	return in_length;
}

char hal_buf[64];
int sz_unpack(char *buf,int len)
{
	int cnt_len=0;
	int all_len;
	int padd_len;
	char *pstring;
	all_len  = sz_unpksize(buf);
	padd_len = *(char*)(buf+4);
	printf("package len:%d\n",all_len);
	printf("padding len:%d\n",padd_len);
	buf+=5;
	len-=padd_len+1;
	for(;cnt_len<len;)
	{
//		memset(hal_buf,0,64);
		all_len = sz_unpksize(buf+cnt_len);
		cnt_len+=4;
		pstring =buf+cnt_len;
//		memcpy(hal_buf,pstring,all_len);
//		printf("%2d,sting:%s\n",all_len,hal_buf);
        /***add your handle function call here***/
        //
		cnt_len+=all_len;
//		printf("cnt_len:%d\n",cnt_len);
	}
}
