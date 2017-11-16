#ifndef _CHECK_OUT_CARD_H_
#define _CHECK_OUT_CARD_H_
extern int check_out_card(char *uid_buf, int uid_len,char *blk_buf,int blk_len);
extern void store_card(char card);
extern void init_card();
extern int getcards(char *src,int src_len);
extern int card_module_init();
extern void decode_card(char *in,char *out);
extern void set_card_info_pf(int new_f);
#endif
