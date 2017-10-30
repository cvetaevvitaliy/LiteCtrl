#ifndef _TRF796XX_REG_H_
#define _TRF796XX_REG_H_
//rtf796xx_demo.h
#define RXERROR 0x0f
#define NOERROR 0X00
//Reader commands
#define Idle                    0x00
#define SoftInit                0x03
#define InitialRFCollision      0x04
#define ResponseRFCollisionN    0x05
#define ResponseRFCollision0    0x06
#define Reset                   0x0F
#define TransmitNoCRC           0x10
#define TransmitCRC             0x11
#define DelayTransmitNoCRC      0x12
#define DelayTransmitCRC        0x13
#define TransmitNextSlot        0x14
#define CloseSlotSequence       0x15
#define StopDecoders            0x16
#define RunDecoders             0x17
#define ChectInternalRF         0x18
#define CheckExternalRF         0x19
#define AdjustGain              0x1A

//Reader addresses
#define ChipStateControl        0x00
#define ISOControl              0x01
#define ISO14443Boptions        0x02
#define ISO14443Aoptions        0x03
#define TXtimerEPChigh          0x04
#define TXtimerEPClow           0x05
#define TXPulseLenghtControl    0x06
#define RXNoResponseWaitTime    0x07
#define RXWaitTime              0x08
#define ModulatorControl        0x09
#define RXSpecialSettings       0x0A
#define RegulatorControl        0x0B
#define IRQStatus               0x0C
#define IRQMask                 0x0D
#define CollisionPosition       0x0E
#define RSSILevels              0x0F
#define RAMStartAddress         0x10    //RAM is 7 bytes long (0x10 - 0x16)
#define NFCID                   0x17
#define NFCTargetLevel          0x18
#define NFCTargetProtocol       0x19
#define TestSetting1            0x1A
#define TestSetting2            0x1B
#define FIFOStatus              0x1C
#define TXLenghtByte1           0x1D
#define TXLenghtByte2           0x1E
#define FIFO                    0x1F


#define BIT0
#define BIT1
#define BIT2
#define BIT3
#define BIT4
#define BIT5
#define BIT6
#define BIT7

#endif
