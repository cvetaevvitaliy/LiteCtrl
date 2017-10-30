#ifndef _LPUAR_H_
#define _LPUAR_H_
#include "stm32l0xx_hal.h"
extern void init_lpuart_gpio();
extern void init_lpuart(uint32_t BaudRate);
extern int lpuart_printf(uint8_t *buf,int len);
extern int lpuart_read(uint8_t *buf,int len);
extern void start_lpuart();
extern void stop_lpuart();
#endif