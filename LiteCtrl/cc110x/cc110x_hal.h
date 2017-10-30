#ifndef _CC110X_HAL_H_
#define _CC110X_HAL_H_
#include "list.h"
enum
{
	CC110X_STATE_IDLE = 0,
	CC110X_STATE_SEND_A,
	CC110X_STATE_SEND_B,
	CC110X_STATE_SEND_C,
	CC110X_STATE_RECV,
	CC110X_STATE_SEND,
	CC110x_STATE_END,
};

enum
{
	CC1101_STATE_SEND_IDLE = 0,
	CC1101_STATE_SEND_WORK,
};
struct send_ifn_t
{
	short send_cnt;
	short  time_interval;
};
union cc1101_msg_ifn_t
{
	struct send_ifn_t send_ifn;
	int reseve;
};
struct cc1101_msg_list_t
{
	struct list_head m_list;
	union cc1101_msg_ifn_t cc1101_msg_ifn;
	char *buf;
	int len;
	int max;
	int num;
};

extern int cc1101_msg_init();
extern void cc1101_msg_entery();
extern int cc1101_msg_start_work();
extern int cc1101_msg_stop_work();


extern int cc101_msg_send_commit(struct cc1101_msg_list_t *p_cc110_msg,int cnt,int time_interve);
extern struct cc1101_msg_list_t *cc1101_msg_alloc_send_msg();
extern void cc1101_msg_free_send_msg(struct cc1101_msg_list_t *p_cc110_msg);
extern void set_cc1101_handle_recv_data(int (*cb_)(char *buf,int buf_len));
extern void msg_printf();
extern void init_gloabel_info();
extern void set_pakage_cnt(int flg,int strength);
extern void set_scan_cnt();
extern void printf_cc1101_info();
extern int  RSSI_calculated(int rssidata);
extern int cc1101_get_rx_sig_strength();
extern void cc1101_set_tx_sig_strength(int sig_s);
extern int  cc1101_get_tx_sig_strength();
#endif

