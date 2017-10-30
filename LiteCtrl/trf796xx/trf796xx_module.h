#ifndef _TRF796xx_MODULE_H_
#define _TRF796xx_MODULE_H_

struct trf796xx_uid_t
{
	char uid[16];
	int uid_len;
	char block0[16];
	char block[3][16];
	int pos;
	int repeat_flg;
	char card_num;
	int blk_len;
};
extern void trf796xx_module_init();
extern void trf796xx_handle_irq();
extern void send_read_uid(int flg);
extern void send_read_id_t();
extern void send_read_block();
extern int trf7962_module_entery();
extern void printf_fifo(char *buf,int len);
extern void init_trf796xx_sys();
extern int send_keep_silence();
extern void set_read_card_count_flg(int new_flg);
extern void set_read_card_count_short_time(int new_time);
extern void set_read_card_count_long_time(int new_time);
extern void set_read_card_count_filter_time(int new_time);
extern void set_read_card_count_print(int new_flg);
#endif
