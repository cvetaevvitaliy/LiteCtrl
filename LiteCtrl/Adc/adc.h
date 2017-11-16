#ifndef _ADC_H_
#define _ADC_H_
extern unsigned short get_adc_value();
extern void init_adc_timer();
extern void adc_start();
extern void adc_stop();
extern int get_channel();
#endif