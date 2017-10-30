#ifndef _TERMINAL_H_
#define _TERMINAL_H_
extern void init_console();
extern int console_proc(char ch);
extern int register_console_write(int (*console_write_fun)(char *buf,int len));
extern int unregister_console_write();
extern int register_cmd(char *buf,int len);
extern int register_console_handl_data(int(*console_handl_fun)(int argc,char *argv[]));
extern void unregister_console_handl_data();
#endif
