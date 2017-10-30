#ifndef _DEVICE_MANAGEMENT_H_
#define _DEVICE_MANAGEMENT_H_
#include "device_pakage.h"
#include "module_common.h"
extern void device_init();
extern int device_setup(int mode,int channel);
extern void set_online_time();
extern int get_device_mode();
#endif
