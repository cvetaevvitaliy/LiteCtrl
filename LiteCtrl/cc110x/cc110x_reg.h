#ifndef _CC110x_REG_H_
#define _CC110x_REG_H_
#include "sz_libc.h"
struct cc110x_reg_ss_t
{
	char regaddr;
	char value;
};

/**********************************************************************************************/
#define IOCFG2   0x00
#define IOCFG1  0x01
#define IOCFG0  0x02
#define FIFOTHR  0x03//rx和tx FIFO门限
#define SYNC1  0x04//同步词汇，高字节
#define SYNC0  0x05//同步词汇，低字节
#define PKTLEN  0x06//数据包长度
#define PKTCTRL1 0x07//数据包自动控制
#define PKTCTRL0 0x08//数据包自动控制
#define ADDR  0x09//设备地址
#define CHANNR  0x0a//信道数
#define FSCTRL1  0x0b//频率合成控制器
#define FSCTRL0  0x0c//频率控制词汇，高字节
#define FREQ2  0x0d//频率控制词汇，中间字节
#define FREQ1  0x0e//频率控制词汇，低字节
#define FREQ0  0x0f//调置器配置
#define MDMCFG4  0x10//调置器配置
#define MDMCFG3  0x11//调置器配置
#define MDMCFG2  0x12//调置器配置
#define MDMCFG1  0x13//调置器配置
#define MDMCFG0  0x14//调置器配置
#define DEVIATN  0x15//主通信控制状态机配置
#define MCSM2  0x16//主通信控制状态机配置
#define MCSM1  0x17//主通信控制状态机配置
#define MCSM0  0x18//频率偏移补偿配置
#define FOCCFG  0x19//位同步配置
#define BSCFG  0x1a//AGC控制
#define AGCCTRL2  0x1b//AGC控制
#define AGCCTRL1  0x1c//AGC控制
#define AGCCTRL0  0x1d//高字节时间0暂停
#define WOREVT1  0x1e//低字节时间0暂停
#define WOREVT0  0x1f//电磁波激活控制
#define WORCTRL  0x20//前末端RX配置
#define FREND1  0x21//前末端TX配置
#define FREND0  0x22//频率合成器校准
#define FSCAL3  0x23//频率合成器校准
#define FSCAL2  0x24//频率合成器校准
#define FSCAL1  0x25//频率合成器校准
#define FSCAL0  0x26//RC振荡器配置
#define RCCTRL1  0x27//RC振荡器配置
#define RCCTRL0  0x28//频率合成器校准控制
#define FSTEST  0x29//产品测试
#define PTEST  0x2a//AGC测试
#define AGCTEST  0x2b//不同的测试设置
#define TEST2  0x2c//不同的测试设置
#define TEST1  0x2d//不同的测试设置
#define TEST0  0x2e//

#define SRES  0x30//重启芯片
#define SFSTXON  0x31//开启和校准频率合成器
#define SXOFF  0x32//关闭晶体振荡器
#define SCAL  0x33//校准频率合成器并判断
#define SRX   0x34//启用SRX
#define STX   0x35//空闲状态:启用STX
#define SIDLE  0x36//离开RX/TX，关断频率合成器
#define SAFC  0x37//频率合成器AFC调节
#define SWOR  0x38//自动RX选举序列（电磁波激活）
#define SPWD  0x39//当CSN为高时进入功率降低模式
#define SFRX  0x3a//冲洗RX FIFO缓存
#define SFTX  0x3b//冲洗TX FIFO缓存
#define SWORRST  0x3c//重新设置真实时间时钟
#define SNOP  0x3d//无操作

#define PARTNUM  0x30//CC2550组成部分数目
#define VERSION  0x31//当前版本数
#define FREQEST  0x32//频率偏移估计
#define LQI   0x33//连接质量的解调器估计
#define RSSI  0x34//接收信号强度指示
#define MARCSTATE 0x35//控制状态机状态
#define WORTIME1 0x36//WOR计时器高字节
#define WORTIME0 0x37//WOR计时器低字节
#define PKTSTATUS 0x38//当前GDOx状态和数据包状态
#define VCO_VC_DAC 0x39//PLL校准模块的当前设定
#define TXBYTES  0x3a//TX FIFO中的下溢和比特数
#define RXBYTES  0x3b//RX FIFO中的下溢和比特数
#define PATABLE  0x3e
#define TX_FIFO  0x3f
#define RX_FIFO  0x3f
/**********************************************************************************************/

#define CC1101_IOCFG2  0x00
#define CC1101_IOCFG1  0x01
#define CC1101_IOCFG0  0x02
#define CC1101_FIFOTHR  0x03//rx和tx FIFO门限
#define CC1101_SYNC1  0x04//同步词汇，高字节
#define CC1101_SYNC0  0x05//同步词汇，低字节
#define CC1101_PKTLEN  0x06//数据包长度
#define CC1101_PKTCTRL1 0x07//数据包自动控制
#define CC1101_PKTCTRL0 0x08//数据包自动控制
#define CC1101_ADDR  0x09//设备地址
#define CC1101_CHANNR  0x0a//信道数
#define CC1101_FSCTRL1  0x0b//频率合成控制器
#define CC1101_FSCTRL0  0x0c//频率控制词汇，高字节
#define CC1101_FREQ2  0x0d//频率控制词汇，中间字节
#define CC1101_FREQ1  0x0e//频率控制词汇，低字节
#define CC1101_FREQ0  0x0f//调置器配置
#define CC1101_MDMCFG4  0x10//调置器配置
#define CC1101_MDMCFG3  0x11//调置器配置
#define CC1101_MDMCFG2  0x12//调置器配置
#define CC1101_MDMCFG1  0x13//调置器配置
#define CC1101_MDMCFG0  0x14//调置器配置
#define CC1101_DEVIATN  0x15//主通信控制状态机配置
#define CC1101_MCSM2  0x16//主通信控制状态机配置
#define CC1101_MCSM1  0x17//主通信控制状态机配置
#define CC1101_MCSM0  0x18//频率偏移补偿配置
#define CC1101_FOCCFG  0x19//位同步配置
#define CC1101_BSCFG  0x1a//AGC控制
#define CC1101_AGCCTRL2  0x1b//AGC控制
#define CC1101_AGCCTRL1  0x1c//AGC控制
#define CC1101_AGCCTRL0  0x1d//高字节时间0暂停
#define CC1101_WOREVT1  0x1e//低字节时间0暂停
#define CC1101_WOREVT0  0x1f//电磁波激活控制
#define CC1101_WORCTRL  0x20//前末端RX配置
#define CC1101_FREND1  0x21//前末端TX配置
#define CC1101_FREND0  0x22//频率合成器校准
#define CC1101_FSCAL3  0x23//频率合成器校准
#define CC1101_FSCAL2  0x24//频率合成器校准
#define CC1101_FSCAL1  0x25//频率合成器校准
#define CC1101_FSCAL0  0x26//RC振荡器配置
#define CC1101_RCCTRL1  0x27//RC振荡器配置
#define CC1101_RCCTRL0  0x28//频率合成器校准控制
#define CC1101_FSTEST  0x29//产品测试
#define CC1101_PTEST  0x2a//AGC测试
#define CC1101_AGCTEST  0x2b//不同的测试设置
#define CC1101_TEST2  0x2c//不同的测试设置
#define CC1101_TEST1  0x2d//不同的测试设置
#define CC1101_TEST0  0x2e//
        
#define CC1101_SRES  0x30//重启芯片
#define CC1101_SFSTXON  0x31//开启和校准频率合成器
#define CC1101_SXOFF  0x32//关闭晶体振荡器
#define CC1101_SCAL  0x33//校准频率合成器并判断
#define CC1101_SRX   0x34//启用SRX
#define CC1101_STX   0x35//空闲状态:启用STX
#define CC1101_SIDLE  0x36//离开RX/TX，关断频率合成器
#define CC1101_SAFC  0x37//频率合成器AFC调节
#define CC1101_SWOR  0x38//自动RX选举序列（电磁波激活）
#define CC1101_SPWD  0x39//当CSN为高时进入功率降低模式
#define CC1101_SFRX  0x3a//冲洗RX FIFO缓存
#define CC1101_SFTX  0x3b//冲洗TX FIFO缓存
#define CC1101_SWORRST  0x3c//重新设置真实时间时钟
#define CC1101_SNOP  0x3d//无操作
        
#define CC1101_PARTNUM  0x30//CC2550组成部分数目
#define CC1101_VERSION  0x31//当前版本数
#define CC1101_FREQEST  0x32//频率偏移估计
#define CC1101_LQI   0x33//连接质量的解调器估计
#define CC1101_RSSI  0x34//接收信号强度指示
#define CC1101_MARCSTATE 0x35//控制状态机状态
#define CC1101_WORTIME1 0x36//WOR计时器高字节
#define CC1101_WORTIME0 0x37//WOR计时器低字节
#define CC1101_PKTSTATUS 0x38//当前GDOx状态和数据包状态
#define CC1101_VCO_VC_DAC 0x39//PLL校准模块的当前设定
#define CC1101_TXBYTES  0x3a//TX FIFO中的下溢和比特数
#define CC1101_RXBYTES  0x3b//RX FIFO中的下溢和比特数
#define CC1101_PATABLE  0x3e
#define CC1101_TX_FIFO  0x3f
#define CC1101_RX_FIFO  0x3f
static void print_b(char *buf,int len)
{
	int i;
	if(len < 0)
		return ;
	
	sz_printf("Data:");
	len = len -1;
	for(i=0;i<len;i++)
	{
		sz_printf("%#x-",buf[i]);
	}
	sz_printf("%#x\r\n",buf[i]);
}

/**********************************************************************************************/

//#define CCxxx0_IOCFG2       0x00        // GDO2 output pin configuration
//#define CCxxx0_IOCFG1       0x01        // GDO1 output pin configuration
//#define CCxxx0_IOCFG0       0x02        // GDO0 output pin configuration
//#define CCxxx0_FIFOTHR      0x03        // RX FIFO and TX FIFO thresholds
//#define CCxxx0_SYNC1        0x04        // Sync word, high INT8U
//#define CCxxx0_SYNC0        0x05        // Sync word, low INT8U
//#define CCxxx0_PKTLEN       0x06        // Packet length
//#define CCxxx0_PKTCTRL1     0x07        // Packet automation control
//#define CCxxx0_PKTCTRL0     0x08        // Packet automation control
//#define CCxxx0_ADDR         0x09        // Device address
//#define CCxxx0_CHANNR       0x0A        // Channel number
//#define CCxxx0_FSCTRL1      0x0B        // Frequency synthesizer control
//#define CCxxx0_FSCTRL0      0x0C        // Frequency synthesizer control
//#define CCxxx0_FREQ2        0x0D        // Frequency control word, high INT8U
//#define CCxxx0_FREQ1        0x0E        // Frequency control word, middle INT8U
//#define CCxxx0_FREQ0        0x0F        // Frequency control word, low INT8U
//#define CCxxx0_MDMCFG4      0x10        // Modem configuration
//#define CCxxx0_MDMCFG3      0x11        // Modem configuration
//#define CCxxx0_MDMCFG2      0x12        // Modem configuration
//#define CCxxx0_MDMCFG1      0x13        // Modem configuration
//#define CCxxx0_MDMCFG0      0x14        // Modem configuration
//#define CCxxx0_DEVIATN      0x15        // Modem deviation setting
//#define CCxxx0_MCSM2        0x16        // Main Radio Control State Machine configuration
//#define CCxxx0_MCSM1        0x17        // Main Radio Control State Machine configuration
//#define CCxxx0_MCSM0        0x18        // Main Radio Control State Machine configuration
//#define CCxxx0_FOCCFG       0x19        // Frequency Offset Compensation configuration
//#define CCxxx0_BSCFG        0x1A        // Bit Synchronization configuration
//#define CCxxx0_AGCCTRL2     0x1B        // AGC control
//#define CCxxx0_AGCCTRL1     0x1C        // AGC control
//#define CCxxx0_AGCCTRL0     0x1D        // AGC control
//#define CCxxx0_WOREVT1      0x1E        // High INT8U Event 0 timeout
//#define CCxxx0_WOREVT0      0x1F        // Low INT8U Event 0 timeout
//#define CCxxx0_WORCTRL      0x20        // Wake On Radio control
//#define CCxxx0_FREND1       0x21        // Front end RX configuration
//#define CCxxx0_FREND0       0x22        // Front end TX configuration
//#define CCxxx0_FSCAL3       0x23        // Frequency synthesizer calibration
//#define CCxxx0_FSCAL2       0x24        // Frequency synthesizer calibration
//#define CCxxx0_FSCAL1       0x25        // Frequency synthesizer calibration
//#define CCxxx0_FSCAL0       0x26        // Frequency synthesizer calibration
//#define CCxxx0_RCCTRL1      0x27        // RC oscillator configuration
//#define CCxxx0_RCCTRL0      0x28        // RC oscillator configuration
//#define CCxxx0_FSTEST       0x29        // Frequency synthesizer calibration control
//#define CCxxx0_PTEST        0x2A        // Production test
//#define CCxxx0_AGCTEST      0x2B        // AGC test
//#define CCxxx0_TEST2        0x2C        // Various test settings
//#define CCxxx0_TEST1        0x2D        // Various test settings
//#define CCxxx0_TEST0        0x2E        // Various test settings
//
// Strobe commands
//#define CCxxx0_SRES         0x30        // Reset chip.
//#define CCxxx0_SFSTXON      0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
//                                        // If in RX/TX: Go to a wait state where only the synthesizer is
//                                        // running (for quick RX / TX turnaround).
//#define CCxxx0_SXOFF        0x32        // Turn off crystal oscillator.
//#define CCxxx0_SCAL         0x33        // Calibrate frequency synthesizer and turn it off
//                                        // (enables quick start).
//#define CCxxx0_SRX          0x34        // Enable RX. Perform calibration first if coming from IDLE and
//                                        // MCSM0.FS_AUTOCAL=1.
//#define CCxxx0_STX          0x35        // In IDLE state: Enable TX. Perform calibration first if
//                                        // MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
//                                        // Only go to TX if channel is clear.
//#define CCxxx0_SIDLE        0x36        // Exit RX / TX, turn off frequency synthesizer and exit
//                                        // Wake-On-Radio mode if applicable.
//#define CCxxx0_SAFC         0x37        // Perform AFC adjustment of the frequency synthesizer
//#define CCxxx0_SWOR         0x38        // Start automatic RX polling sequence (Wake-on-Radio)
//#define CCxxx0_SPWD         0x39        // Enter power down mode when CSn goes high.
//#define CCxxx0_SFRX         0x3A        // Flush the RX FIFO buffer.
//#define CCxxx0_SFTX         0x3B        // Flush the TX FIFO buffer.
//#define CCxxx0_SWORRST      0x3C        // Reset real time clock.
//#define CCxxx0_SNOP         0x3D        // No operation. May be used to pad strobe commands to two
//                                        // INT8Us for simpler software.
//
//#define CCxxx0_PARTNUM      0x30
//#define CCxxx0_VERSION      0x31
//#define CCxxx0_FREQEST      0x32
//#define CCxxx0_LQI          0x33
//#define CCxxx0_RSSI         0x34
//#define CCxxx0_MARCSTATE    0x35
//#define CCxxx0_WORTIME1     0x36
//#define CCxxx0_WORTIME0     0x37
//#define CCxxx0_PKTSTATUS    0x38
//#define CCxxx0_VCO_VC_DAC   0x39
//#define CCxxx0_TXBYTES      0x3A
//#define CCxxx0_RXBYTES      0x3B
//
//#define CCxxx0_PATABLE      0x3E
//#define CCxxx0_TXFIFO       0x3F
//#define CCxxx0_RXFIFO       0x3F
/**********************************************************************************************/
#define 	BYTES_IN_RXFIFO     0x7F  						//接收缓冲区的有效字节数
#define 	CRC_OK              0x80 						//CRC校验通过位标志
#endif
