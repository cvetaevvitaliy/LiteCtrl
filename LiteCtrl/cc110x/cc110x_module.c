#include "stm32l0xx_hal.h"
#include "cc110x_adapter.h"
#include "sz_libc.h"
#include "cc110x_reg.h"
//#include "led.h"
#include "module_common.h"
#include "cc110x_hal.h"


//static int m_cc110x_step;
static int m_work_state = MODULE_STA_STOP;

static int init_cc110x_chip(int addr,int chanel);

static const struct cc110x_reg_ss_t cc110x_reg[]=   
{
    {FSCTRL1      ,0x08},   // FSCTRL1   Frequency synthesizer control.
    {FSCTRL0      ,0x00},   // FSCTRL0   Frequency synthesizer control.
    {FREQ2	      ,0x10},   // FREQ2     Frequency control word, high byte.
    {FREQ1        ,0xA7},   // FREQ1     Frequency control word, middle byte.
    {FREQ0        ,0x62},   // FREQ0     Frequency control word, low byte.
//    {FREQ2	      ,0x0F},   // FREQ2     Frequency control word, high byte.
//    {FREQ1        ,0x62},   // FREQ1     Frequency control word, middle byte.
//    {FREQ0        ,0x76},   // FREQ0     Frequency control word, low byte.
    {MDMCFG4      ,0x5B},   // MDMCFG4   Modem configuration.
    {MDMCFG3      ,0xF8},   // MDMCFG3   Modem configuration.
    {MDMCFG2      ,0x03},   // MDMCFG2   Modem configuration.
    {MDMCFG1      ,0x22},   // MDMCFG1   Modem configuration.
    {MDMCFG0      ,0xF8},   // MDMCFG0   信道间隔200khz.
    //{CHANNR		  ,0x00},   // CHANNR    信道号.
    {DEVIATN	  ,0x47},   // DEVIATN   偏差Deviation 0x35=20k.
    {FREND1    	  ,0xB6},   // FREND1    Front end RX configuration.
    {FREND0       ,0x10},   // FREND0    Front end RX configuration.
    {MCSM0        ,0x18},   // MCSM0     Main Radio Control State Machine configuration.
    {FOCCFG       ,0x1D},   // FOCCFG    Frequency Offset Compensation Configuration.
    {BSCFG     	  ,0x1C},   // BSCFG     Bit synchronization Configuration.
    {AGCCTRL2     ,0xC7},   // AGCCTRL2  AGC control.
    {AGCCTRL1     ,0x00},   // AGCCTRL1  AGC control.
    {AGCCTRL0     ,0xF2},   // AGCCTRL0  AGC control.
    {FSCAL3       ,0xEA},   // FSCAL3    Frequency synthesizer calibration.
    {FSCAL2       ,0x2A},   // FSCAL2    Frequency synthesizer calibration.
    {FSCAL1       ,0x00},   // FSCAL1    Frequency synthesizer calibration.
    {FSCAL0       ,0x11},   // FSCAL0    Frequency synthesizer calibration.
    {FSTEST       ,0x59},   // FSTEST    Frequency synthesizer calibration.
    {TEST2        ,0x81},   // TEST2     Various test settings.
    {TEST1        ,0x35},   // TEST1     Various test settings.
    {TEST0        ,0x09},   // TEST0     Various test settings.
    {IOCFG2		  ,0x0B},   // IOCFG2    GDO2 output pin configuration.
    {IOCFG0       ,0x06},   // IOCFG0D   GDO0 output pin configuration. Refer to SmartRF?Studio User Manual for detailed pseudo register explanation.
    {PKTCTRL1     ,0x07},   // PKTCTRL1  启用地址滤波.
    {PKTCTRL0     ,0x05},   // PKTCTRL0  TX启用CRC 可变长度.
//    {ADDR         ,0x18},   // ADDR      设备地址.
//    {PKTLEN       ,0x0c},    // PKTLEN    最大包的长度.
    {SYNC1        ,0x16},
	{SYNC0        ,0x18}	
};
static const char chanel_num[16]=	{
	0x00,0x01,0x02,0x0C,//0-3
	0x10,0x11,0x12,0x1F,//4-7
	0x20,0x21,0x22,0x2F,//8-11
	0x40,0x41,0x42,0x4F//12-15
		}; 
void cc110x_settings(int addr,int chanel) 
{
    int i=0;
	int cnt=sizeof(cc110x_reg)/sizeof(struct cc110x_reg_ss_t);
	sz_printk("sizeof(cc110x_reg)=%d\n",cnt);
	for(i=0;i<cnt;i++)
	{
		cc110x_write_t(cc110x_reg[i].regaddr,cc110x_reg[i].value);
	}
	if(chanel < 0 && chanel >= 16)
	{
		chanel = 0;
		sz_printk("the channel is byond\n");
	}
	cc110x_write_t(ADDR,(char)addr);
	cc110x_write_t(CHANNR,(char)chanel_num[chanel]);
	sz_printk("device addr:%#x,channel:%#x\n",(char)addr,(char)chanel_num[chanel]);
	
}

//INT8U PaTabel[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};
const char PaTabel[8] = {0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0};
static int init_cc110x_chip(int addr,int chanel)
{
    cc110x_reset(SRES);
    cc110x_settings(addr,chanel);
    cc110x_write(PATABLE, PaTabel, 8);
	return 0;
}

static void set_cc110x_work_state(int new_state)
{
	m_work_state = new_state;
}

static int get_cc110x_work_state()
{
	return m_work_state;
}

int cc110x_start_work(int addr,int chanel)
{
	if(get_cc110x_work_state()==MODULE_STA_WORK)
		return -1;
	set_cc110x_work_state(MODULE_STA_WORK);
	init_cc110x_chip(addr,chanel);
	cc1101_msg_start_work();
	return 0;
}

int cc110x_stop_work()
{
	if(get_cc110x_work_state() == MODULE_STA_STOP)
		return -1;
	set_cc110x_work_state(MODULE_STA_STOP);
	cc1101_msg_stop_work();
	return 0;
}
int cc110x_module_init()
{
	cc110x_init();
	cc1101_msg_init();
	set_cc110x_work_state(MODULE_STA_STOP);
	return 0;
}

void cc110x_module_exit()
{
	cc110x_stop_work();
}

int cc110x_module_entery()
{
	cc1101_msg_entery();
	return 0;
}

