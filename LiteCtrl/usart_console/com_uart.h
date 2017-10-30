#ifndef _COM_UART_H_
#define _COM_UART_H_
#include "stm32l0xx_hal.h"
#include "ring_buffer.h"
extern void handl_uart_send(UART_HandleTypeDef *pUartHand,RING_BUF *pRbf);
extern void handl_uart_recv(UART_HandleTypeDef *pUartHand,RING_BUF *pRbf);
extern int uart_printf(UART_HandleTypeDef *pUartHand,RING_BUF *pRbf,uint8_t *buf,int len);
#endif
