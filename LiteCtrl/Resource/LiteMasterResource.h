#ifndef _LITE_MASTER_RESOURCE_H_
#define _LITE_MASTER_RESOURCE_H_
extern void get_cpu_id(char *buf,int len);
extern int get_master_id(char *buf,int len);
extern void set_master_id(char *buf,int len);
extern int get_master_addr(char *buf,int len);
extern void set_master_addr(char *buf,int len);
extern int get_subdev_addr(char *buf,int len);
extern int set_subdev_addr(char *buf,int le);
extern int get_subdev_flg(char *buf,int len);
extern void set_subdev_flg(char *buf,int len);
extern void set_subdev_pos(char *buf,int len);
extern int get_cc1101_pos(char *buf,int len);
extern void set_subdev_chanle(char *buf,int len);
extern int get_cc1101_chanle(char *buf,int len);
extern void set_subdev_card_cnt_flg(char *buf,int len);
extern int get_subdev_card_cnt_flg(char *buf,int len);
extern void set_subdev_card_cnt_short_time(char *buf,int len);
extern int get_subdev_card_cnt_short_time(char *buf,int len);
extern void set_subdev_card_cnt_long_time(char *buf,int len);
extern int get_subdev_card_cnt_long_time(char *buf,int len);
extern void set_subdev_card_cnt_filter_time(char *buf,int len);
extern int get_subdev_card_cnt_filter_time(char *buf,int len);
extern void print_system();
extern void device_defult_init();
extern int get_subdev_card_cnt_print_flg(char *buf,int len);
extern void set_subdev_card_cnt_print_flg(char *buf,int len);
extern int get_subdev_pos(char *buf,int len);

#endif
