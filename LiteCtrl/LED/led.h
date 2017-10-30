#ifndef _LED_H_
#define _LED_H_
enum
{
	IDLE = 0,
	ALLAY_OPEN = 1,
	ALLAY_CLOSE = 2,
	ALLAY_FLASH =3,
	ONE_FLASH_ON,
	LED_STA_END
};

extern void ShowLeds();
extern void SetLedState(int which,int state);
//extern void SetLedFlashTime(int which,int miliseconds);
extern void led_clk();
extern void led_init();
#endif