#ifndef __ADRF6755_H__
#define __ADRF6755_H__
#include "modulatorType.h"
#include "modulatorError.h"

#define Fpdf_KHz 40000 //40MHz
#define IIC_Addr 0x80


#define CR0		0x00
#define CR1		0x01
#define CR2		0x02
#define CR3		0x03
#define CR4		0x04
#define CR5		0x05
#define CR6		0x06
#define CR7		0x07
#define CR8		0x08
#define CR9		0x09
#define CR10	0x0A
#define CR11	0x0B
#define CR12	0x0C
#define CR13	0x0D
#define CR14	0x0E
#define CR15	0x0F
#define CR16	0x10
#define CR17	0x11
#define CR18	0x12
#define CR19	0x13
#define CR20	0x14
#define CR21	0x15
#define CR22	0x16
#define CR23	0x17
#define CR24	0x18
#define CR25	0x19
#define CR26	0x1A
#define CR27	0x1B
#define CR28	0x1C
#define CR29	0x1D
#define CR30	0x1E
#define CR31	0x1F
#define CR32	0x20
#define CR33	0x21

typedef struct {
	Handle userData;
	Byte gain;
	Byte Reg[34];
} ADRF6755INFO;


Dword ADRF6755_writeRegister(ADRF6755INFO* ADRF6755, Byte addr, Byte value);

Dword ADRF6755_readRegister(ADRF6755INFO* ADRF6755, Byte addr, Byte *value);

Dword ADRF6755_initialize (ADRF6755INFO* ADRF6755, Dword clk_KHz); 

Dword ADRF6755_setFrequency (ADRF6755INFO* ADRF6755, Dword frequency_KHz);

Dword ADRF6755_modulatorPowerOn (ADRF6755INFO* ADRF6755, Bool enable);

Dword ADRF6755_adjustOutputGain (ADRF6755INFO* ADRF6755, Byte gain);
#endif
