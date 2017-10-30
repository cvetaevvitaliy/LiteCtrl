#ifndef _CC110X_MODULE_H_
#define _CC110X_MODULE_H_

extern int cc110x_module_init();
extern int cc110x_module_entery();

extern int cc110x_start_work(int addr,int chanel);
extern int cc110x_stop_work();
extern void cc110x_module_exit();

#endif
