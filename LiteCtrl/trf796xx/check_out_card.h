#ifndef _CHECK_OUT_CARD_H_
#define _CHECK_OUT_CARD_H_
extern int check_out_card(char *uid_buf, int uid_len,char *blk_buf,int blk_len);
extern void store_card(char card);
extern void input_card();
extern int getcards(char *src,int src_len);
extern int card_module_init();
extern void decode_card(char *in,char *out);
#endif
