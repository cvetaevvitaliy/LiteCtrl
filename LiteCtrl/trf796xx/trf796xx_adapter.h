#ifndef _TRF796XX_ADAPTER_H_
#define _TRF796XX_ADAPTER_H_
#include "stm32l0xx_hal.h"
struct trf796xx_data_t
{
	short addr;//16bits 
	short value;//16bis
};

extern void trf796xx_pin_init();
extern void trf796xx_enable_irq();
extern void trf796xx_disable_irq();
extern void trf796xx_enable();
extern void trf796xx_disable();

extern int trf796xx_write_reg_t(int addr,int value);
extern int trf796xx_read_reg_t(int addr);
extern int trf796xx_write_cmd(int cmd);
extern int trf796xx_write_cmds(const char *buf,int len);
extern int trf796xx_write_regs(struct trf796xx_data_t *,int);
extern int trf796xx_read_regs(struct trf796xx_data_t *,int);
extern int  trf796xx_write_block(int,const char *,int);
extern int  trf796xx_read_block(int,char *,int);
extern int trf796xx_write_raw_datas(const char *buf,int len);
extern int printf_reg(int addr);


extern int read_irq_pin();
extern void set_irq_pin_to_input();
extern void set_irq_pin_to_extit();
#define IRQ read_irq_pin()
extern int trf796xx_read_single(char *p_buf,int len);
extern int trf796xx_write_single(const char *p_buf,int len);
#endif
