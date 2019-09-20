#include "IT9510User.h"

#if IT9510User_INTERNAL
#include "ADF4351.h"
#include "ADRF6755.h"
#include "RFFC2072.h"
#include "orion_cal.h"

static Byte eagleDefaultTable[256] = {
	0x17, 0x03, 0xf1, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x8d, 0x04, 0x17, 0x95, 0x00, 0x01, 0x01, 0x02,
	0x03, 0x80, 0x00, 0xfa, 0xfa, 0x0a, 0x07, 0x00, 0x00, 0x41, 0x46, 0x30, 0x31, 0x30, 0x32, 0x30,
	0x32, 0x30, 0x37, 0x30, 0x30, 0x30, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x3a, 0x01, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x09, 0x04, 0x20, 0x03, 0x49, 0x00,
	0x54, 0x00, 0x45, 0x00, 0x20, 0x00, 0x54, 0x00, 0x65, 0x00, 0x63, 0x00, 0x68, 0x00, 0x2e, 0x00,
	0x2c, 0x00, 0x20, 0x00, 0x49, 0x00, 0x6e, 0x00, 0x63, 0x00, 0x2e, 0x00, 0x1c, 0x03, 0x44, 0x00,
	0x54, 0x00, 0x56, 0x00, 0x20, 0x00, 0x4d, 0x00, 0x6f, 0x00, 0x64, 0x00, 0x75, 0x00, 0x6c, 0x00,
	0x61, 0x00, 0x74, 0x00, 0x6f, 0x00, 0x72, 0x00, 0x20, 0x03, 0x41, 0x00, 0x46, 0x00, 0x30, 0x00,
	0x31, 0x00, 0x30, 0x00, 0x32, 0x00, 0x30, 0x00, 0x32, 0x00, 0x30, 0x00, 0x37, 0x00, 0x30, 0x00,
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x31, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x12, 0x03,
	0x4d, 0x00, 0x53, 0x00, 0x46, 0x00, 0x54, 0x00, 0x31, 0x00, 0x30, 0x00, 0x30, 0x00, 0x00, 0x00,
	0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

DeviceDescription IT9510HwDesc[] =
{
	//GPIOH1    GPIOH2    GPIOH3     GPIOH4   GPIOH5          GPIOH6        GPIOH7  GPIOH8  
	{ResetSlave, RfEnable, MuxSelect, UartTxd, PowerDownSlave, NA,			 IrDa,   intrEnable	},	//0:EVB-01 v01
	{ResetSlave, RfEnable, NA,        NA,	   PowerDownSlave, NA,			 IrDa,   UvFilter	},	//1:DB-01-01 v01
	{ResetSlave, RfEnable, MuxSelect, NA,      PowerDownSlave, NA,			 lnaGain,UvFilter	},	//2:DB-01-02 v01
	{ResetSlave, RfEnable, NA,        NA,      PowerDownSlave, NA,           IrDa,   UvFilter	},	//3:DB-01-01 v03
	{ResetSlave, RfEnable, MuxSelect, NA,      PowerDownSlave, NA,           NA,     UvFilter	},	//4:DB-01-02 v03
	{NA,         RfEnable, NA,        NA,      NA,             NA,           NA,     NA			},	//5:SD AV sender
	{NA,         RfEnable, LoClk,     LoData,  LoLe,           LnaPowerDown, IrDa,   UvFilter	},	//6:EVB-01 v02
	{NA,         RfEnable, MuxSelect, NA,      NA,             NA,           NA,     UvFilter   },	//7:1080P DTV CAM Yuelee
	{NA,         RfEnable, NA,        NA,      NA,             NA,           NA,     UvFilter   },	//8:1080P DTV CAM Sunnic
	{NA,         RfEnable, NA,        NA,      NA,             NA,           NA,     NA			},	//9:HD AV sender
	{NA,         RfEnable, NA,        NA,      NA,             NA,           NA,     UvFilter	},	//10:AV sender HC
	{ResetSlave, RfEnable, NA,        NA,      PowerDownSlave, NA,           IrDa,   UvFilter	},	//11:DB-01-01 v04
	{ResetSlave, RfEnable, NA,        NA,      PowerDownSlave, NA,           IrDa,   UvFilter	},	//12:4CH TX board
	{NA,         RfEnable, NA,        NA,      NA,             NA,           NA,     UvFilter	},	//13:HV100


};

DeviceDescription IT9510HwDescEx[] =
{
	//GPIOH1     GPIOH2    GPIOH3      GPIOH4   GPIOH5          GPIOH6         GPIOH7       GPIOH8  
	{ResetSlave, RfEnable, MuxSelect,  UartTxd, PowerDownSlave, NA,			   IrDa,        intrEnable   },	//0:EVB
	{ResetSlave, RfEnable, NA,         NA,	    PowerDownSlave, NA,			   IrDa,        UvFilter	 },	//1:dongle
	{ResetSlave, RfEnable, MuxSelect,  NA,      PowerDownSlave, NA,			   NA,          UvFilter	 },	//2:CAM
	{NA,         RfEnable, MuxSelect,  NA,      NA,             NA,            NA,          UvFilter	 },	//3:CAM_1
	{NA,         RfEnable, NA,         NA,      NA,             NA,            NA,          UvFilter	 },	//4:
	{NA,         RfEnable, NA,         NA,      NA,             NA,            NA,          NA           },	//5:
	{ResetSlave, RfEnable, Filter_1,   NA,      NA,             EepromSwitch,  NA,          NA           },	//6:Eagle2 + Bumblebee
	{ResetSlave, RfEnable, NA,         NA,      PowerDownSlave, NA,            NA,          UvFilter	 },	//7:DTV Bridge
	{ResetSlave, NA,       NA,         NA,      PowerDownSlave, NA,            IrDa,        UvFilter	 },	//8:dongle (Eagle2)
	{ResetSlave, RfEnable, Filter_1,   UartTxd, Filter_2,       NA,            IrDa,        UartRxd	     },	//9:Tx Module (Eagle2)
	{ResetSlave, RfEnable, Filter_1,   NA,      PowerDownSlave, NA,            IrDa,        NA		     }, //A:dongle_new (Eagle2)
	{ResetSlave, RfEnable, Filter_1,   UartTxd, Filter_2,       NA,            IrDa,        UartRxd	     }, //B:Tx Module (ADRF6755_20MHz)
	{ResetSlave, RfEnable, Filter_1n,  UartTxd, Filter_2n,      NA,            IrDa,        UartRxd	     }, //C:New Tx Module (test version, without orion)
	{ResetSlave, RfEnable, Filter_1n,  UartTxd, Filter_2n,      NA,            IrDa,        UartRxd	     }, //D:New Tx Module (test version, with orion)


};

DeviceDescription IT9510HwDescEx2[] =
{
	//GPIOH1     GPIOH2    GPIOH3      GPIOH4   GPIOH5          GPIOH6         GPIOH7       GPIOH8  
//	{ResetSlave, RfEnable, Filter_1n,  UartTxd,		Filter_2n,		NA,				IrDa,			UartRxd		}, // 0:old define
	{ResetSlave, RfEnable, NA,		   SpiEnable,	SpiClock,		SpiData,		NA,				NA			}, // 0:New Tx Module (RFFC2072 902~928M / 2.4G)
	{NA,		 NA,	   NA,		   NA,			NA,				NA,				NA,				NA			}, // 1:todo define for new
	{ResetSlave, RfEnable, ScanLock,   SpiEnable,	SpiClock,		SpiData,		Ant_Switch1,	Ant_Switch2	}, // 2:RFFC2072 + 64k EEPROM
	{ResetSlave, RfEnable, NA,		   SpiEnable,	SpiClock,		SpiData,		CalibOn,		ResetSlave_2}, // 3:RFFC2072 + IT9101, for ¤¤ÅA  AIR HD/TX_V20

};

static unsigned short checksum(Byte CFG_Length, Byte* image)
{
      unsigned short sum = 0;
      int i = 0;
      // No need to do divide operation for remainder, since it will 
      for (i=2; i<2+ CFG_Length; i++) 
		  sum = sum + image[i];
      return sum; 
}


Dword IT9510User_i2cSwitchToADRF6755 (IN  IT9510INFO*    modulator)
{	
	Dword error = ModulatorError_NO_ERROR;
	if(modulator->systemConfig.rfEnable != UNUSED){
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.rfEnable+1, 1); 		
		IT9510User_delay(30);
	}
	return error; 
}

Dword IT9510User_i2cSwitchToOrion (IN  IT9510INFO*    modulator)
{	
	Dword error = ModulatorError_NO_ERROR;

	if (modulator->deviceType == 0xB3) // use new GPIO define--GPIO 7/8
	{
		if (modulator->systemConfig.calibOn != UNUSED){
			error = IT9510_writeRegister(modulator, Processor_LINK, modulator->systemConfig.calibOn + 1, 0);
			if (error) goto exit;
		}
		IT9510User_delay(100);

		if (modulator->systemConfig.resetSlave_2 != UNUSED){
			error = IT9510_writeRegister(modulator, Processor_LINK, modulator->systemConfig.resetSlave_2 + 1, 1);
			if (error) goto exit;
		}
		IT9510User_delay(100);
	}
	else // use old GPIO define--GPIO 1/2
	{
	if(modulator->systemConfig.rfEnable != UNUSED){
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.rfEnable+1, 0); 		
			if (error) goto exit;
		}

		// reset orion to fix init fail issue
		if (modulator->systemConfig.resetSlave != UNUSED){
			error = IT9510_writeRegister(modulator, Processor_LINK, modulator->systemConfig.resetSlave + 1, 0); // TX(Orion reset) rest 
			if (error) goto exit;
		}
		IT9510User_delay(10);

		if (modulator->systemConfig.resetSlave != UNUSED){
			error = IT9510_writeRegister(modulator, Processor_LINK, modulator->systemConfig.resetSlave + 1, 1); // TX(Orion reset) rest 
			if (error) goto exit;
		}
		IT9510User_delay(10);
	}

exit:
	return error; 
}

Dword IT9510User_i2cSwitchToOrion2(IN  IT9510INFO*    modulator)
{
	Dword error = ModulatorError_NO_ERROR;

	if (modulator->deviceType == 0xB3) // use new GPIO define--GPIO 7/8
	{
		if (modulator->systemConfig.resetSlave_2 != UNUSED){
			error = IT9510_writeRegister(modulator, Processor_LINK, modulator->systemConfig.resetSlave_2 + 1, 0);
			if (error) goto exit;
		}
		IT9510User_delay(100);

		if (modulator->systemConfig.calibOn != UNUSED){
			error = IT9510_writeRegister(modulator, Processor_LINK, modulator->systemConfig.calibOn + 1, 1);
			if (error) goto exit;
		}
		IT9510User_delay(100);
	}
	else // use old GPIO define--GPIO 1/2
	{
		// Reset Orion to avoid I2C bus fail
		if (modulator->systemConfig.resetSlave != UNUSED){
			error = IT9510_writeRegister(modulator, Processor_LINK, modulator->systemConfig.resetSlave + 1, 0); // TX(Orion reset) rest 
			if (error) goto exit;
		}
		IT9510User_delay(10);

		if (modulator->systemConfig.resetSlave != UNUSED){
			error = IT9510_writeRegister(modulator, Processor_LINK, modulator->systemConfig.resetSlave + 1, 1); // TX(Orion reset) rest 
			if (error) goto exit;
		}
		IT9510User_delay(10);
	}

exit:
	return error; 
}

Dword IT9510User_getDeviceTypeSetting (
    IN  IT9510INFO*    modulator,
    IN  Byte          pcbIndex,
	IN  SystemConfig* Config  
) {
	DeviceDescription HwSetting;
	SystemConfig PCBconfig;
	Dword error = 0;
	
	PCBconfig.resetSlave	= UNUSED;
	PCBconfig.rfEnable		= UNUSED;
	PCBconfig.loClk			= UNUSED;
	PCBconfig.loData		= UNUSED;
	PCBconfig.loLe			= UNUSED;
	PCBconfig.lnaPowerDown	= UNUSED;
	PCBconfig.irDa			= UNUSED;
	PCBconfig.uvFilter		= UNUSED;
	PCBconfig.chSelect0		= UNUSED;
	PCBconfig.chSelect1		= UNUSED;		
	PCBconfig.chSelect2		= UNUSED;
	PCBconfig.chSelect3		= UNUSED;
	PCBconfig.muxSelect		= UNUSED;
	PCBconfig.uartTxd		= UNUSED;
	PCBconfig.powerDownSlave= UNUSED;
	PCBconfig.intrEnable	= UNUSED;
	PCBconfig.lnaGain		= UNUSED;
	PCBconfig.dataRateCheck	= UNUSED;
	PCBconfig.slaveLock		= UNUSED;
	PCBconfig.filter_1		= UNUSED;
	PCBconfig.filter_2		= UNUSED;
	PCBconfig.uartRxd		= UNUSED;
	PCBconfig.eepromSwitch  = UNUSED;
	PCBconfig.filter_1n		= UNUSED;
	PCBconfig.filter_2n		= UNUSED;
	PCBconfig.spiEnable		= UNUSED;
	PCBconfig.spiClock		= UNUSED;
	PCBconfig.spiData		= UNUSED;
	PCBconfig.scanLock		= UNUSED;
	PCBconfig.ant_Switch1	= UNUSED;
	PCBconfig.ant_Switch2	= UNUSED;
	PCBconfig.calibOn		= UNUSED;
	PCBconfig.resetSlave_2	= UNUSED;
	

	if(pcbIndex<IndexEnd){
		HwSetting = IT9510HwDesc[pcbIndex];

	}else if((pcbIndex & 0x0F) < PCBExIndexEnd){
		if(pcbIndex == 0xDA)
			HwSetting = IT9510HwDescEx[(pcbIndex & 0x0F)+1]; // GPIO setting , Device type 0xDA=0xDB
		else if ((pcbIndex & 0xF0) == 0xB0)
			HwSetting = IT9510HwDescEx2[(pcbIndex & 0x0F)];
		else
			HwSetting = IT9510HwDescEx[(pcbIndex & 0x0F)];

		if((pcbIndex & 0xF0) == 0x50 || (pcbIndex & 0xF0) == 0xD0) //ADRF6755
			modulator->slaveIICAddr = 0x80;	//ADRF6755 i2c address
		printk("Set SlaveII address to : 0x%X\n", modulator->slaveIICAddr);
		modulator->deviceType = pcbIndex;
		
	}else{
		error = ModulatorError_INVALID_SYSTEM_CONFIG;
		HwSetting = IT9510HwDesc[3];
	}

	if(HwSetting.GPIO1 != NA){
		if(HwSetting.GPIO1 == ResetSlave)
			PCBconfig.resetSlave = GPIOH1;
		else if(HwSetting.GPIO1 == RfEnable)
			PCBconfig.rfEnable = GPIOH1;
		else if(HwSetting.GPIO1 == LoClk)
			PCBconfig.loClk = GPIOH1;
		else if(HwSetting.GPIO1 == LoData)
			PCBconfig.loData = GPIOH1;
		else if(HwSetting.GPIO1 == LoLe)
			PCBconfig.loLe = GPIOH1;
		else if(HwSetting.GPIO1 == LnaPowerDown)
			PCBconfig.lnaPowerDown = GPIOH1;
		else if(HwSetting.GPIO1 == IrDa)
			PCBconfig.irDa = GPIOH1;
		else if(HwSetting.GPIO1 == UvFilter)
			PCBconfig.uvFilter = GPIOH1;
		else if(HwSetting.GPIO1 == ChSelect3)
			PCBconfig.chSelect3 = GPIOH1;
		else if(HwSetting.GPIO1 == ChSelect2)
			PCBconfig.chSelect2 = GPIOH1;
		else if(HwSetting.GPIO1 == ChSelect1)
			PCBconfig.chSelect1 = GPIOH1;
		else if(HwSetting.GPIO1 == ChSelect0)
			PCBconfig.chSelect0 = GPIOH1;
		else if(HwSetting.GPIO1 == PowerDownSlave)
			PCBconfig.powerDownSlave = GPIOH1;
		else if(HwSetting.GPIO1 == UartTxd)
			PCBconfig.uartTxd = GPIOH1;
		else if(HwSetting.GPIO1 == MuxSelect)
			PCBconfig.muxSelect = GPIOH1;
		else if(HwSetting.GPIO1 == lnaGain)
			PCBconfig.lnaGain = GPIOH1;
		else if(HwSetting.GPIO1 == intrEnable)
			PCBconfig.intrEnable = GPIOH1;
		else if(HwSetting.GPIO1 == DataRateCheck)
			PCBconfig.dataRateCheck = GPIOH1;
		else if(HwSetting.GPIO1 == SlaveLock)
			PCBconfig.slaveLock = GPIOH1;
		else if(HwSetting.GPIO1 == Filter_1)
			PCBconfig.filter_1 = GPIOH1;
		else if(HwSetting.GPIO1 == Filter_2)
			PCBconfig.filter_2 = GPIOH1;
		else if(HwSetting.GPIO1 == UartRxd)
			PCBconfig.uartRxd = GPIOH1;
		else if(HwSetting.GPIO1 == EepromSwitch)
			PCBconfig.eepromSwitch = GPIOH1;
		else if(HwSetting.GPIO1 == Filter_1n)
			PCBconfig.filter_1n = GPIOH1;
		else if(HwSetting.GPIO1 == Filter_2n)
			PCBconfig.filter_2n = GPIOH1;
		else if (HwSetting.GPIO1 == SpiEnable)
			PCBconfig.spiEnable = GPIOH1;
		else if (HwSetting.GPIO1 == SpiClock)
			PCBconfig.spiClock = GPIOH1;
		else if (HwSetting.GPIO1 == SpiData)
			PCBconfig.spiData = GPIOH1;
		else if (HwSetting.GPIO1 == ScanLock)
			PCBconfig.scanLock = GPIOH1;
		else if (HwSetting.GPIO1 == Ant_Switch1)
			PCBconfig.ant_Switch1 = GPIOH1;
		else if (HwSetting.GPIO1 == Ant_Switch2)
			PCBconfig.ant_Switch2 = GPIOH1;
		else if (HwSetting.GPIO1 == CalibOn)
			PCBconfig.calibOn = GPIOH1;
		else if (HwSetting.GPIO1 == ResetSlave_2)
			PCBconfig.resetSlave_2 = GPIOH1;

	}


	if(HwSetting.GPIO2 != NA){
		if(HwSetting.GPIO2 == ResetSlave)
			PCBconfig.resetSlave = GPIOH2;
		else if(HwSetting.GPIO2 == RfEnable)
			PCBconfig.rfEnable = GPIOH2;
		else if(HwSetting.GPIO2 == LoClk)
			PCBconfig.loClk = GPIOH2;
		else if(HwSetting.GPIO2 == LoData)
			PCBconfig.loData = GPIOH2;
		else if(HwSetting.GPIO2 == LoLe)
			PCBconfig.loLe = GPIOH2;
		else if(HwSetting.GPIO2 == LnaPowerDown)
			PCBconfig.lnaPowerDown = GPIOH2;
		else if(HwSetting.GPIO2 == IrDa)
			PCBconfig.irDa = GPIOH2;
		else if(HwSetting.GPIO2 == UvFilter)
			PCBconfig.uvFilter = GPIOH2;
		else if(HwSetting.GPIO2 == ChSelect3)
			PCBconfig.chSelect3 = GPIOH2;
		else if(HwSetting.GPIO2 == ChSelect2)
			PCBconfig.chSelect2 = GPIOH2;
		else if(HwSetting.GPIO2 == ChSelect1)
			PCBconfig.chSelect1 = GPIOH2;
		else if(HwSetting.GPIO2 == ChSelect0)
			PCBconfig.chSelect0 = GPIOH2;
		else if(HwSetting.GPIO2 == PowerDownSlave)
			PCBconfig.powerDownSlave = GPIOH1;
		else if(HwSetting.GPIO2 == UartTxd)
			PCBconfig.uartTxd = GPIOH2;
		else if(HwSetting.GPIO2 == MuxSelect)
			PCBconfig.muxSelect = GPIOH2;
			else if(HwSetting.GPIO2 == lnaGain)
			PCBconfig.lnaGain = GPIOH2;
		else if(HwSetting.GPIO2 == intrEnable)
			PCBconfig.intrEnable = GPIOH2;
		else if(HwSetting.GPIO2 == DataRateCheck)
			PCBconfig.dataRateCheck = GPIOH2;
		else if(HwSetting.GPIO2 == SlaveLock)
			PCBconfig.slaveLock = GPIOH2;		
		else if(HwSetting.GPIO2 == Filter_1)
			PCBconfig.filter_1 = GPIOH2;
		else if(HwSetting.GPIO2 == Filter_2)
			PCBconfig.filter_2 = GPIOH2;
		else if(HwSetting.GPIO2 == UartRxd)
			PCBconfig.uartRxd = GPIOH2;
		else if(HwSetting.GPIO2 == EepromSwitch)
			PCBconfig.eepromSwitch = GPIOH2;
		else if(HwSetting.GPIO2 == Filter_1n)
			PCBconfig.filter_1n = GPIOH2;
		else if(HwSetting.GPIO2 == Filter_2n)
			PCBconfig.filter_2n = GPIOH2;
		else if (HwSetting.GPIO2 == SpiEnable)
			PCBconfig.spiEnable = GPIOH2;
		else if (HwSetting.GPIO2 == SpiClock)
			PCBconfig.spiClock = GPIOH2;
		else if (HwSetting.GPIO2 == SpiData)
			PCBconfig.spiData = GPIOH2;
		else if (HwSetting.GPIO2 == ScanLock)
			PCBconfig.scanLock = GPIOH2;
		else if (HwSetting.GPIO2 == Ant_Switch1)
			PCBconfig.ant_Switch1 = GPIOH2;
		else if (HwSetting.GPIO2 == Ant_Switch2)
			PCBconfig.ant_Switch2 = GPIOH2;
		else if (HwSetting.GPIO2 == CalibOn)
			PCBconfig.calibOn = GPIOH2;
		else if (HwSetting.GPIO2 == ResetSlave_2)
			PCBconfig.resetSlave_2 = GPIOH2;

	}

	if(HwSetting.GPIO3 != NA){
		if(HwSetting.GPIO3 == ResetSlave)
			PCBconfig.resetSlave = GPIOH3;
		else if(HwSetting.GPIO3 == RfEnable)
			PCBconfig.rfEnable = GPIOH3;
		else if(HwSetting.GPIO3 == LoClk)
			PCBconfig.loClk = GPIOH3;
		else if(HwSetting.GPIO3 == LoData)
			PCBconfig.loData = GPIOH3;
		else if(HwSetting.GPIO3 == LoLe)
			PCBconfig.loLe = GPIOH3;
		else if(HwSetting.GPIO3 == LnaPowerDown)
			PCBconfig.lnaPowerDown = GPIOH3;
		else if(HwSetting.GPIO3 == IrDa)
			PCBconfig.irDa = GPIOH3;
		else if(HwSetting.GPIO3 == UvFilter)
			PCBconfig.uvFilter = GPIOH3;
		else if(HwSetting.GPIO3 == ChSelect3)
			PCBconfig.chSelect3 = GPIOH3;
		else if(HwSetting.GPIO3 == ChSelect2)
			PCBconfig.chSelect2 = GPIOH3;
		else if(HwSetting.GPIO3 == ChSelect1)
			PCBconfig.chSelect1 = GPIOH3;
		else if(HwSetting.GPIO3 == ChSelect0)
			PCBconfig.chSelect0 = GPIOH3;
		else if(HwSetting.GPIO3 == PowerDownSlave)
			PCBconfig.powerDownSlave = GPIOH1;
		else if(HwSetting.GPIO3 == UartTxd)
			PCBconfig.uartTxd = GPIOH3;
		else if(HwSetting.GPIO3 == MuxSelect)
			PCBconfig.muxSelect = GPIOH3;
		else if(HwSetting.GPIO3 == lnaGain)
			PCBconfig.lnaGain = GPIOH3;
		else if(HwSetting.GPIO3 == intrEnable)
			PCBconfig.intrEnable = GPIOH3;
		else if(HwSetting.GPIO3 == DataRateCheck)
			PCBconfig.dataRateCheck = GPIOH3;
		else if(HwSetting.GPIO3 == SlaveLock)
			PCBconfig.slaveLock = GPIOH3;		
		else if(HwSetting.GPIO3 == Filter_1)
			PCBconfig.filter_1 = GPIOH3;
		else if(HwSetting.GPIO3 == Filter_2)
			PCBconfig.filter_2 = GPIOH3;
		else if(HwSetting.GPIO3 == UartRxd)
			PCBconfig.uartRxd = GPIOH3;
		else if(HwSetting.GPIO3 == EepromSwitch)
			PCBconfig.eepromSwitch = GPIOH3;
		else if(HwSetting.GPIO3 == Filter_1n)
			PCBconfig.filter_1n = GPIOH3;
		else if(HwSetting.GPIO3 == Filter_2n)
			PCBconfig.filter_2n = GPIOH3;
		else if (HwSetting.GPIO3 == SpiEnable)
			PCBconfig.spiEnable = GPIOH3;
		else if (HwSetting.GPIO3 == SpiClock)
			PCBconfig.spiClock = GPIOH3;
		else if (HwSetting.GPIO3 == SpiData)
			PCBconfig.spiData = GPIOH3;
		else if (HwSetting.GPIO3 == ScanLock)
			PCBconfig.scanLock = GPIOH3;
		else if (HwSetting.GPIO3 == Ant_Switch1)
			PCBconfig.ant_Switch1 = GPIOH3;
		else if (HwSetting.GPIO3 == Ant_Switch2)
			PCBconfig.ant_Switch2 = GPIOH3;
		else if (HwSetting.GPIO3 == CalibOn)
			PCBconfig.calibOn = GPIOH3;
		else if (HwSetting.GPIO3 == ResetSlave_2)
			PCBconfig.resetSlave_2 = GPIOH3;

	}

	if(HwSetting.GPIO4 != NA){
		if(HwSetting.GPIO4 == ResetSlave)
			PCBconfig.resetSlave = GPIOH4;
		else if(HwSetting.GPIO4 == RfEnable)
			PCBconfig.rfEnable = GPIOH4;
		else if(HwSetting.GPIO4 == LoClk)
			PCBconfig.loClk = GPIOH4;
		else if(HwSetting.GPIO4 == LoData)
			PCBconfig.loData = GPIOH4;
		else if(HwSetting.GPIO4 == LoLe)
			PCBconfig.loLe = GPIOH4;
		else if(HwSetting.GPIO4 == LnaPowerDown)
			PCBconfig.lnaPowerDown = GPIOH4;
		else if(HwSetting.GPIO4 == IrDa)
			PCBconfig.irDa = GPIOH4;
		else if(HwSetting.GPIO4 == UvFilter)
			PCBconfig.uvFilter = GPIOH4;
		else if(HwSetting.GPIO4 == ChSelect3)
			PCBconfig.chSelect3 = GPIOH4;
		else if(HwSetting.GPIO4 == ChSelect2)
			PCBconfig.chSelect2 = GPIOH4;
		else if(HwSetting.GPIO4 == ChSelect1)
			PCBconfig.chSelect1 = GPIOH4;
		else if(HwSetting.GPIO4 == ChSelect0)
			PCBconfig.chSelect0 = GPIOH4;
		else if(HwSetting.GPIO4 == PowerDownSlave)
			PCBconfig.powerDownSlave = GPIOH4;
		else if(HwSetting.GPIO4 == UartTxd)
			PCBconfig.uartTxd = GPIOH4;
		else if(HwSetting.GPIO4 == MuxSelect)
			PCBconfig.muxSelect = GPIOH4;
		else if(HwSetting.GPIO4 == lnaGain)
			PCBconfig.lnaGain = GPIOH4;
		else if(HwSetting.GPIO4 == intrEnable)
			PCBconfig.intrEnable = GPIOH4;
		else if(HwSetting.GPIO4 == DataRateCheck)
			PCBconfig.dataRateCheck = GPIOH4;
		else if(HwSetting.GPIO4 == SlaveLock)
			PCBconfig.slaveLock = GPIOH4;
		else if(HwSetting.GPIO4 == Filter_1)
			PCBconfig.filter_1 = GPIOH4;
		else if(HwSetting.GPIO4 == Filter_2)
			PCBconfig.filter_2 = GPIOH4;
		else if(HwSetting.GPIO4 == UartRxd)
			PCBconfig.uartRxd = GPIOH4;
		else if(HwSetting.GPIO4 == EepromSwitch)
			PCBconfig.eepromSwitch = GPIOH4;
		else if(HwSetting.GPIO4 == Filter_1n)
			PCBconfig.filter_1n = GPIOH4;
		else if(HwSetting.GPIO4 == Filter_2n)
			PCBconfig.filter_2n = GPIOH4;
		else if (HwSetting.GPIO4 == SpiEnable)
			PCBconfig.spiEnable = GPIOH4;
		else if (HwSetting.GPIO4 == SpiClock)
			PCBconfig.spiClock = GPIOH4;
		else if (HwSetting.GPIO4 == SpiData)
			PCBconfig.spiData = GPIOH4;
		else if (HwSetting.GPIO4 == ScanLock)
			PCBconfig.scanLock = GPIOH4;
		else if (HwSetting.GPIO4 == Ant_Switch1)
			PCBconfig.ant_Switch1 = GPIOH4;
		else if (HwSetting.GPIO4 == Ant_Switch2)
			PCBconfig.ant_Switch2 = GPIOH4;
		else if (HwSetting.GPIO4 == CalibOn)
			PCBconfig.calibOn = GPIOH4;
		else if (HwSetting.GPIO4 == ResetSlave_2)
			PCBconfig.resetSlave_2 = GPIOH4;

	}


	if(HwSetting.GPIO5 != NA){
		if(HwSetting.GPIO5 == ResetSlave)
			PCBconfig.resetSlave = GPIOH5;
		else if(HwSetting.GPIO5 == RfEnable)
			PCBconfig.rfEnable = GPIOH5;
		else if(HwSetting.GPIO5 == LoClk)
			PCBconfig.loClk = GPIOH5;
		else if(HwSetting.GPIO5 == LoData)
			PCBconfig.loData = GPIOH5;
		else if(HwSetting.GPIO5 == LoLe)
			PCBconfig.loLe = GPIOH5;
		else if(HwSetting.GPIO5 == LnaPowerDown)
			PCBconfig.lnaPowerDown = GPIOH5;
		else if(HwSetting.GPIO5 == IrDa)
			PCBconfig.irDa = GPIOH5;
		else if(HwSetting.GPIO5 == UvFilter)
			PCBconfig.uvFilter = GPIOH5;
		else if(HwSetting.GPIO5 == ChSelect3)
			PCBconfig.chSelect3 = GPIOH5;
		else if(HwSetting.GPIO5 == ChSelect2)
			PCBconfig.chSelect2 = GPIOH5;
		else if(HwSetting.GPIO5 == ChSelect1)
			PCBconfig.chSelect1 = GPIOH5;
		else if(HwSetting.GPIO5 == ChSelect0)
			PCBconfig.chSelect0 = GPIOH5;
		else if(HwSetting.GPIO5 == PowerDownSlave)
			PCBconfig.powerDownSlave = GPIOH5;
		else if(HwSetting.GPIO5 == UartTxd)
			PCBconfig.uartTxd = GPIOH5;
		else if(HwSetting.GPIO5 == MuxSelect)
			PCBconfig.muxSelect = GPIOH5;
		else if(HwSetting.GPIO5 == lnaGain)
			PCBconfig.lnaGain = GPIOH5;
		else if(HwSetting.GPIO5 == intrEnable)
			PCBconfig.intrEnable = GPIOH5;
		else if(HwSetting.GPIO5 == DataRateCheck)
			PCBconfig.dataRateCheck = GPIOH5;
		else if(HwSetting.GPIO5 == SlaveLock)
			PCBconfig.slaveLock = GPIOH5;
		else if(HwSetting.GPIO5 == Filter_1)
			PCBconfig.filter_1 = GPIOH5;
		else if(HwSetting.GPIO5 == Filter_2)
			PCBconfig.filter_2 = GPIOH5;
		else if(HwSetting.GPIO5 == UartRxd)
			PCBconfig.uartRxd = GPIOH5;
		else if(HwSetting.GPIO5 == EepromSwitch)
			PCBconfig.eepromSwitch = GPIOH5;
		else if(HwSetting.GPIO5 == Filter_1n)
			PCBconfig.filter_1n = GPIOH5;
		else if(HwSetting.GPIO5 == Filter_2n)
			PCBconfig.filter_2n = GPIOH5;
		else if (HwSetting.GPIO5 == SpiEnable)
			PCBconfig.spiEnable = GPIOH5;
		else if (HwSetting.GPIO5 == SpiClock)
			PCBconfig.spiClock = GPIOH5;
		else if (HwSetting.GPIO5 == SpiData)
			PCBconfig.spiData = GPIOH5;
		else if (HwSetting.GPIO5 == ScanLock)
			PCBconfig.scanLock = GPIOH5;
		else if (HwSetting.GPIO5 == Ant_Switch1)
			PCBconfig.ant_Switch1 = GPIOH5;
		else if (HwSetting.GPIO5 == Ant_Switch2)
			PCBconfig.ant_Switch2 = GPIOH5;
		else if (HwSetting.GPIO5 == CalibOn)
			PCBconfig.calibOn = GPIOH5;
		else if (HwSetting.GPIO5 == ResetSlave_2)
			PCBconfig.resetSlave_2 = GPIOH5;

	}

	if(HwSetting.GPIO6 != NA){
		if(HwSetting.GPIO6 == ResetSlave)
			PCBconfig.resetSlave = GPIOH6;
		else if(HwSetting.GPIO6 == RfEnable)
			PCBconfig.rfEnable = GPIOH6;
		else if(HwSetting.GPIO6 == LoClk)
			PCBconfig.loClk = GPIOH6;
		else if(HwSetting.GPIO6 == LoData)
			PCBconfig.loData = GPIOH6;
		else if(HwSetting.GPIO6 == LoLe)
			PCBconfig.loLe = GPIOH6;
		else if(HwSetting.GPIO6 == LnaPowerDown)
			PCBconfig.lnaPowerDown = GPIOH6;
		else if(HwSetting.GPIO6 == IrDa)
			PCBconfig.irDa = GPIOH6;
		else if(HwSetting.GPIO6 == UvFilter)
			PCBconfig.uvFilter = GPIOH6;
		else if(HwSetting.GPIO6 == ChSelect3)
			PCBconfig.chSelect3 = GPIOH6;
		else if(HwSetting.GPIO6 == ChSelect2)
			PCBconfig.chSelect2 = GPIOH6;
		else if(HwSetting.GPIO6 == ChSelect1)
			PCBconfig.chSelect1 = GPIOH6;
		else if(HwSetting.GPIO6 == ChSelect0)
			PCBconfig.chSelect0 = GPIOH6;
		else if(HwSetting.GPIO6 == PowerDownSlave)
			PCBconfig.powerDownSlave = GPIOH6;
		else if(HwSetting.GPIO6 == UartTxd)
			PCBconfig.uartTxd = GPIOH6;
		else if(HwSetting.GPIO6 == MuxSelect)
			PCBconfig.muxSelect = GPIOH6;
		else if(HwSetting.GPIO6 == lnaGain)
			PCBconfig.lnaGain = GPIOH6;
		else if(HwSetting.GPIO6 == intrEnable)
			PCBconfig.intrEnable = GPIOH6;
		else if(HwSetting.GPIO6 == DataRateCheck)
			PCBconfig.dataRateCheck = GPIOH6;
		else if(HwSetting.GPIO6 == SlaveLock)
			PCBconfig.slaveLock = GPIOH6;		
		else if(HwSetting.GPIO6 == Filter_1)
			PCBconfig.filter_1 = GPIOH6;
		else if(HwSetting.GPIO6 == Filter_2)
			PCBconfig.filter_2 = GPIOH6;
		else if(HwSetting.GPIO6 == UartRxd)
			PCBconfig.uartRxd = GPIOH6;
		else if(HwSetting.GPIO6 == EepromSwitch)
			PCBconfig.eepromSwitch = GPIOH6;
		else if(HwSetting.GPIO6 == Filter_1n)
			PCBconfig.filter_1n = GPIOH6;
		else if(HwSetting.GPIO6 == Filter_2n)
			PCBconfig.filter_2n = GPIOH6;
		else if (HwSetting.GPIO6 == SpiEnable)
			PCBconfig.spiEnable = GPIOH6;
		else if (HwSetting.GPIO6 == SpiClock)
			PCBconfig.spiClock = GPIOH6;
		else if (HwSetting.GPIO6 == SpiData)
			PCBconfig.spiData = GPIOH6;
		else if (HwSetting.GPIO6 == ScanLock)
			PCBconfig.scanLock = GPIOH6;
		else if (HwSetting.GPIO6 == Ant_Switch1)
			PCBconfig.ant_Switch1 = GPIOH6;
		else if (HwSetting.GPIO6 == Ant_Switch2)
			PCBconfig.ant_Switch2 = GPIOH6;
		else if (HwSetting.GPIO6 == CalibOn)
			PCBconfig.calibOn = GPIOH6;
		else if (HwSetting.GPIO6 == ResetSlave_2)
			PCBconfig.resetSlave_2 = GPIOH6;

	}

	if(HwSetting.GPIO7 != NA){
		if(HwSetting.GPIO7 == ResetSlave)
			PCBconfig.resetSlave = GPIOH7;
		else if(HwSetting.GPIO7 == RfEnable)
			PCBconfig.rfEnable = GPIOH7;
		else if(HwSetting.GPIO7 == LoClk)
			PCBconfig.loClk = GPIOH7;
		else if(HwSetting.GPIO7 == LoData)
			PCBconfig.loData = GPIOH7;
		else if(HwSetting.GPIO7 == LoLe)
			PCBconfig.loLe = GPIOH7;
		else if(HwSetting.GPIO7 == LnaPowerDown)
			PCBconfig.lnaPowerDown = GPIOH7;
		else if(HwSetting.GPIO7 == IrDa)
			PCBconfig.irDa = GPIOH7;
		else if(HwSetting.GPIO7 == UvFilter)
			PCBconfig.uvFilter = GPIOH7;
		else if(HwSetting.GPIO7 == ChSelect3)
			PCBconfig.chSelect3 = GPIOH7;
		else if(HwSetting.GPIO7 == ChSelect2)
			PCBconfig.chSelect2 = GPIOH7;
		else if(HwSetting.GPIO7 == ChSelect1)
			PCBconfig.chSelect1 = GPIOH7;
		else if(HwSetting.GPIO7 == ChSelect0)
			PCBconfig.chSelect0 = GPIOH7;
		else if(HwSetting.GPIO7 == PowerDownSlave)
			PCBconfig.powerDownSlave = GPIOH7;
		else if(HwSetting.GPIO7 == UartTxd)
			PCBconfig.uartTxd = GPIOH7;
		else if(HwSetting.GPIO7 == MuxSelect)
			PCBconfig.muxSelect = GPIOH7;
		else if(HwSetting.GPIO7 == lnaGain)
			PCBconfig.lnaGain = GPIOH7;
		else if(HwSetting.GPIO7 == intrEnable)
			PCBconfig.intrEnable = GPIOH7;
		else if(HwSetting.GPIO7 == DataRateCheck)
			PCBconfig.dataRateCheck = GPIOH7;
		else if(HwSetting.GPIO7 == SlaveLock)
			PCBconfig.slaveLock = GPIOH7;
		else if(HwSetting.GPIO7 == Filter_1)
			PCBconfig.filter_1 = GPIOH7;
		else if(HwSetting.GPIO7 == Filter_2)
			PCBconfig.filter_2 = GPIOH7;
		else if(HwSetting.GPIO7 == UartRxd)
			PCBconfig.uartRxd = GPIOH7;
		else if(HwSetting.GPIO7 == EepromSwitch)
			PCBconfig.eepromSwitch = GPIOH7;
		else if(HwSetting.GPIO7 == Filter_1n)
			PCBconfig.filter_1n = GPIOH7;
		else if(HwSetting.GPIO7 == Filter_2n)
			PCBconfig.filter_2n = GPIOH7;
		else if (HwSetting.GPIO7 == SpiEnable)
			PCBconfig.spiEnable = GPIOH7;
		else if (HwSetting.GPIO7 == SpiClock)
			PCBconfig.spiClock = GPIOH7;
		else if (HwSetting.GPIO7 == SpiData)
			PCBconfig.spiData = GPIOH7;
		else if (HwSetting.GPIO7 == ScanLock)
			PCBconfig.scanLock = GPIOH7;
		else if (HwSetting.GPIO7 == Ant_Switch1)
			PCBconfig.ant_Switch1 = GPIOH7;
		else if (HwSetting.GPIO7 == Ant_Switch2)
			PCBconfig.ant_Switch2 = GPIOH7;
		else if (HwSetting.GPIO7 == CalibOn)
			PCBconfig.calibOn = GPIOH7;
		else if (HwSetting.GPIO7 == ResetSlave_2)
			PCBconfig.resetSlave_2 = GPIOH7;
	}

	if(HwSetting.GPIO8 != NA){
		if(HwSetting.GPIO8 == ResetSlave)
			PCBconfig.resetSlave = GPIOH8;
		else if(HwSetting.GPIO8 == RfEnable)
			PCBconfig.rfEnable = GPIOH8;
		else if(HwSetting.GPIO8 == LoClk)
			PCBconfig.loClk = GPIOH8;
		else if(HwSetting.GPIO8 == LoData)
			PCBconfig.loData = GPIOH8;
		else if(HwSetting.GPIO8 == LoLe)
			PCBconfig.loLe = GPIOH8;
		else if(HwSetting.GPIO8 == LnaPowerDown)
			PCBconfig.lnaPowerDown = GPIOH8;
		else if(HwSetting.GPIO8 == IrDa)
			PCBconfig.irDa = GPIOH8;
		else if(HwSetting.GPIO8 == UvFilter)
			PCBconfig.uvFilter = GPIOH8;
		else if(HwSetting.GPIO8 == ChSelect3)
			PCBconfig.chSelect3 = GPIOH8;
		else if(HwSetting.GPIO8 == ChSelect2)
			PCBconfig.chSelect2 = GPIOH8;
		else if(HwSetting.GPIO8 == ChSelect1)
			PCBconfig.chSelect1 = GPIOH8;
		else if(HwSetting.GPIO8 == ChSelect0)
			PCBconfig.chSelect0 = GPIOH8;
		else if(HwSetting.GPIO8 == PowerDownSlave)
			PCBconfig.powerDownSlave = GPIOH8;
		else if(HwSetting.GPIO8 == UartTxd)
			PCBconfig.uartTxd = GPIOH8;
		else if(HwSetting.GPIO8 == MuxSelect)
			PCBconfig.muxSelect = GPIOH8;
		else if(HwSetting.GPIO8 == lnaGain)
			PCBconfig.lnaGain = GPIOH8;
		else if(HwSetting.GPIO8 == intrEnable)
			PCBconfig.intrEnable = GPIOH8;
		else if(HwSetting.GPIO8 == DataRateCheck)
			PCBconfig.dataRateCheck = GPIOH8;
		else if(HwSetting.GPIO8 == SlaveLock)
			PCBconfig.slaveLock = GPIOH8;		
		else if(HwSetting.GPIO8 == Filter_1)
			PCBconfig.filter_1 = GPIOH8;
		else if(HwSetting.GPIO8 == Filter_2)
			PCBconfig.filter_2 = GPIOH8;
		else if(HwSetting.GPIO8 == UartRxd)
			PCBconfig.uartRxd = GPIOH8;
		else if(HwSetting.GPIO8 == EepromSwitch)
			PCBconfig.eepromSwitch = GPIOH8;
		else if(HwSetting.GPIO8 == Filter_1n)
			PCBconfig.filter_1n = GPIOH8;
		else if(HwSetting.GPIO8 == Filter_2n)
			PCBconfig.filter_2n = GPIOH8;
		else if (HwSetting.GPIO8 == SpiEnable)
			PCBconfig.spiEnable = GPIOH8;
		else if (HwSetting.GPIO8 == SpiClock)
			PCBconfig.spiClock = GPIOH8;
		else if (HwSetting.GPIO8 == SpiData)
			PCBconfig.spiData = GPIOH8;
		else if (HwSetting.GPIO8 == ScanLock)
			PCBconfig.scanLock = GPIOH8;
		else if (HwSetting.GPIO8 == Ant_Switch1)
			PCBconfig.ant_Switch1 = GPIOH8;
		else if (HwSetting.GPIO8 == Ant_Switch2)
			PCBconfig.ant_Switch2 = GPIOH8;
		else if (HwSetting.GPIO8 == CalibOn)
			PCBconfig.calibOn = GPIOH8;
		else if (HwSetting.GPIO8 == ResetSlave_2)
			PCBconfig.resetSlave_2 = GPIOH8;

	}


	*Config = PCBconfig;

	return (error);
}






Dword IT9510User_setSystemConfig (
    IN  IT9510INFO*    modulator,
	IN  SystemConfig  systemConfig
) {
	Dword error = 0;
	Byte pinCnt = 0;
	
	if(systemConfig.resetSlave != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.resetSlave+1, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.resetSlave+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.resetSlave+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
		IT9510User_delay(10);
	}

   	if(systemConfig.lnaPowerDown != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.lnaPowerDown+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.lnaPowerDown+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.loClk != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.loClk+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.loClk+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.loData != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.loData+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.loData+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.loLe != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.loLe+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.loLe+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.muxSelect != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.muxSelect+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.muxSelect+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.powerDownSlave != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.powerDownSlave+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.powerDownSlave+3, 1);//gpiox_on
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.powerDownSlave+1, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.rfEnable != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.rfEnable+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.rfEnable+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.uartTxd != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.uartTxd+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.uartTxd+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.uvFilter != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.uvFilter+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.uvFilter+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.filter_1 != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.filter_1+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.filter_1+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.filter_2 != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.filter_2+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.filter_2+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.lnaGain != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.lnaGain+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.lnaGain+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.eepromSwitch != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.eepromSwitch+1, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.eepromSwitch+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.eepromSwitch+3, 1);//gpiox_on
		if (error) goto exit;

		pinCnt++;
	}

	if(systemConfig.filter_1n != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.filter_1n+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.filter_1n+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.filter_2n != UNUSED){ //output
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.filter_2n+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, systemConfig.filter_2n+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if (systemConfig.scanLock != UNUSED){ //output
		error = IT9510_writeRegister(modulator, Processor_LINK, systemConfig.scanLock + 2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister(modulator, Processor_LINK, systemConfig.scanLock + 3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if (systemConfig.ant_Switch1 != UNUSED){ //output
		error = IT9510_writeRegister(modulator, Processor_LINK, systemConfig.ant_Switch1 + 2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister(modulator, Processor_LINK, systemConfig.ant_Switch1 + 3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if (systemConfig.ant_Switch2 != UNUSED){ //output
		error = IT9510_writeRegister(modulator, Processor_LINK, systemConfig.ant_Switch2 + 2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister(modulator, Processor_LINK, systemConfig.ant_Switch2 + 3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if (systemConfig.calibOn != UNUSED){ //output
		error = IT9510_writeRegister(modulator, Processor_LINK, systemConfig.calibOn + 2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister(modulator, Processor_LINK, systemConfig.calibOn + 3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if (systemConfig.resetSlave_2 != UNUSED){ //output
		error = IT9510_writeRegister(modulator, Processor_LINK, systemConfig.resetSlave_2 + 2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister(modulator, Processor_LINK, systemConfig.resetSlave_2 + 3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(pinCnt>8)
		error = ModulatorError_INVALID_SYSTEM_CONFIG;

	if(error == ModulatorError_NO_ERROR)
		modulator->systemConfig = systemConfig;
exit:
    return (ModulatorError_NO_ERROR);
}

Dword IT9510User_getTsInputType (
	IN  IT9510INFO*    modulator,
	OUT  TsInterface*  tsInStreamType
) {
	Dword error = ModulatorError_NO_ERROR;
	Byte temp = 0;
	*tsInStreamType = (TsInterface)temp;

	error = IT9510_readRegister (modulator, Processor_LINK, 0x4979, &temp);//has eeprom ??
	if((temp == 1) && (error == ModulatorError_NO_ERROR)){
		error = IT9510_readRegister (modulator, Processor_LINK, 0x49CA, &temp);
		if(error == ModulatorError_NO_ERROR){
			*tsInStreamType = (TsInterface)temp;
		}
	}
	return (error);
}

Dword IT9510User_getDeviceType (
	IN  IT9510INFO*    modulator,
	OUT  Byte*		  deviceType	   
){	
	Dword error = ModulatorError_NO_ERROR;
	Byte temp;

	
	error = IT9510_readRegister (modulator, Processor_LINK, 0x4979, &temp);//has eeprom ??
	if((temp == 1) && (error == ModulatorError_NO_ERROR)){  // eeprom
		error = IT9510_readRegister (modulator, Processor_LINK, 0x49D5, &temp);	
		if(error == ModulatorError_NO_ERROR){	// read eeprom value
			*deviceType = temp;	
			error = ModulatorError_NO_ERROR;		
		}else if(temp == 0){ // No value
			*deviceType = -1;
			error = ModulatorError_INVALID_DEV_TYPE;
		}
	}
	else    // no eeprom, use default.
	{
		*deviceType = IT9510User_DEVICETYPE;
		error = ModulatorError_USB_READ_FAIL;
	}
	
	return(error);
}



#endif

Dword IT9510User_memoryCopy (
    IN  IT9510INFO*    modulator,
    IN  void*           dest,
    IN  void*           src,
    IN  Dword           count
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  memcpy(dest, src, (size_t)count);
     *  return (0);
     */
	memcpy(dest, src, count);
    return (ModulatorError_NO_ERROR);
}

Dword IT9510User_delay (
    IN  Dword           dwMs
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  delay(dwMs);
     *  return (0);
     */
    if(dwMs != 0)
		msleep(dwMs);
    return (ModulatorError_NO_ERROR);
}

Dword IT9510User_printf (const char* format,...){

	printk(format);
	return (ModulatorError_NO_ERROR);
}

Dword IT9510User_enterCriticalSection (
	void
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
    return (ModulatorError_NO_ERROR);
}


Dword IT9510User_leaveCriticalSection (
	void
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
    return (ModulatorError_NO_ERROR);
}


Dword IT9510User_mpegConfig (
    IN  IT9510INFO*    modulator
) {
    /*
     *  ToDo:  Add code here
     *
     */
    return (ModulatorError_NO_ERROR);
}


Dword IT9510User_busTx (
    IN  IT9510INFO*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  short i;
     *
     *  start();
     *  write_i2c(uc2WireAddr);
     *  ack();
     *  for (i = 0; i < bufferLength; i++) {
     *      write_i2c(*(ucpBuffer + i));
     *      ack();
     *  }
     *  stop();
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	Dword error = 0;
#if IT9510User_INTERNAL
	if(!modulator->dev_disconnect) 
	{
		if(modulator->busId == Bus_USB)
			error = Usb2_writeControlBus((Modulator*)modulator,bufferLength,buffer);
		else
			error = ModulatorError_INVALID_BUS_TYPE;
	}
#endif
    return (error);
}


Dword IT9510User_busRx (
    IN  IT9510INFO*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  short i;
     *
     *  start();
     *  write_i2c(uc2WireAddr | 0x01);
     *  ack();
     *  for (i = 0; i < bufferLength - 1; i++) {
     *      read_i2c(*(ucpBuffer + i));
     *      ack();
     *  }
     *  read_i2c(*(ucpBuffer + bufferLength - 1));
     *  nack();
     *  stop();
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
    //return (Af9035u2i_readControlBus(modulator,bufferLength,buffer));

	Dword error = 0;
#if IT9510User_INTERNAL
	if(!modulator->dev_disconnect) 
	{
		if(modulator->busId == Bus_USB)
			error = Usb2_readControlBus((Modulator*)modulator,bufferLength,buffer);
		else
			error = ModulatorError_INVALID_BUS_TYPE;
	}
#endif
	return (error);
}


Dword IT9510User_setBus (
    IN  IT9510INFO*    modulator
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
    Dword error = ModulatorError_NO_ERROR;

    return(error);
}


 Dword IT9510User_Initialization  (
    IN  IT9510INFO*    modulator
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	Dword error = 0;
	modulator->isExtLo = False;

#if IT9510User_INTERNAL
	 error = IT9510User_setSystemConfig(modulator, modulator->systemConfig);
	 if (error) goto exit;

	if(modulator->tsInterfaceType == PARALLEL_TS_INPUT || modulator->tsInterfaceType == SERIAL_TS_INPUT){
		
		if(modulator->systemConfig.resetSlave != UNUSED){
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.resetSlave+1, 1); //RX(IT9133) rest 
			if (error) goto exit;
		}

		IT9510User_delay(10);
		if(modulator->systemConfig.powerDownSlave != UNUSED){
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.powerDownSlave+1, 0); //RX(IT9133) power up
			if (error) goto exit;
		}
	}

	if(modulator->systemConfig.lnaGain != UNUSED){
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.lnaGain+1, 0); //lna Gain
		if (error) goto exit;
	}

	if (modulator->systemConfig.resetSlave_2 != UNUSED){
		error = IT9510_writeRegister(modulator, Processor_LINK, modulator->systemConfig.resetSlave_2 + 1, 0); // orion reset
		if (error) goto exit;
	}

	if (modulator->systemConfig.calibOn != UNUSED){
		error = IT9510_writeRegister(modulator, Processor_LINK, modulator->systemConfig.calibOn + 1, 1); // orion calibration/switch 
		if (error) goto exit;
	}

	if(modulator->systemConfig.loClk != UNUSED)
		ADF4351_busInit((Modulator*)modulator);

#ifdef __ADRF6755_H__
	//modulator->slaveIICAddr = 0x80;
	if(modulator->slaveIICAddr == 0x80 || modulator->slaveIICAddr == 0xC0){
		modulator->ADRF6755.userData = modulator;
		IT9510User_i2cSwitchToADRF6755(modulator);
		if(modulator->deviceType >= 0xDA)
			error = ADRF6755_initialize( &(modulator->ADRF6755), 20000);
		else
			error = ADRF6755_initialize(&(modulator->ADRF6755), 40000);
		if(error) {
			printk("ADRF6755 initialize fail \n");
			goto exit;
		}
		IT9510User_printf("ADRF6755 initialize ok \n");
		modulator->isExtLo = True;
		modulator->ADRF6755.gain = 5;
	}
#endif
#ifdef _ORION_CAL_H
	
	//----- orion cal dc
	if(modulator->deviceType == 0xDB || modulator->deviceType == 0x9B ||
	   modulator->deviceType == 0xDD /* || modulator->deviceType == 0xB3 */) // cancel the 0xb3, otherwise i2c always switch to orion until IT9510User_setTxModeEnable() is finish, EEPROM can't use duration the time .
	{
		IT9510User_i2cSwitchToOrion(modulator);
		IT9510User_printf("orion initialize  \n");
		if(modulator->isExtLo == True)
			error = orion_cal_init(modulator, 20000);	
		else
			error = orion_cal_init(modulator, 12000);
		if (error) goto exit;
	}
#endif
	if(modulator->systemConfig.rfEnable != UNUSED){
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.rfEnable+1, 0); //RF out power down
		if (error) goto exit;
	}

#ifdef __RFFC2072_H__
	if (modulator->deviceType == 0x9C || modulator->deviceType == 0xB0 ||
		modulator->deviceType == 0xB2 || modulator->deviceType == 0xB3)
	{
		(modulator->RFFC2072).userData = modulator;
		RFFC2072_initialize(&(modulator->RFFC2072));

		if (modulator->deviceType == 0xB2) 
			IT9510User_LoadRFGainTable(modulator);
	}
#endif

	IT9510User_LoadDCCalibrationTable(modulator);
	
exit:
#endif
    return (error);

 }


Dword IT9510User_Finalize  (
    IN  IT9510INFO*    modulator
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	Dword error = 0;
#if IT9510User_INTERNAL
	if(modulator->busId == Bus_USB)
		error = Usb2_exitDriver((Modulator*)modulator);
	else
		error = ModulatorError_INVALID_BUS_TYPE;
#endif
    return (error);

 }
 

Dword IT9510User_acquireChannel (
	IN  IT9510INFO*    modulator,
	IN  Word          bandwidth,
	IN  Dword         frequency
){

	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	Dword error = 0;
#ifdef __ADRF6755_H__	
	Byte val[6];
	TransmissionModes tempMode; /** Number of carriers used for OFDM signal						*/
	Interval	tempGI;  
#endif 
#if IT9510User_INTERNAL

	if(modulator->systemConfig.loClk != UNUSED)
		ADF4351_setFrequency((Modulator*)modulator, frequency);//External Lo control

	
	if(modulator->systemConfig.filter_1n != UNUSED){
		if(frequency <= 950000){ // 			
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.filter_1n+1, 1);			
		}else {
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.filter_1n+1, 0);		
		}
		if (error) goto exit;

	}

	if(modulator->systemConfig.filter_2n != UNUSED){
		if(frequency <= 250000){ // 			
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.filter_2n+1, 0);
			if (error) goto exit;

		}else if((frequency > 250000)&&(frequency <= 950000)){
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.filter_2n+1, 1);
			if (error) goto exit;
		}else if((frequency > 950000)&&(frequency <= 1400000)){
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.filter_2n+1, 1);
			if (error) goto exit;
		}else if(frequency > 1400000){
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.filter_2n+1, 0);
			if (error) goto exit;		
		}

	}
	
	if(modulator->systemConfig.uvFilter != UNUSED){
		if(frequency < 250000){ // <=250000KHz v-filter gpio set to Lo
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.uvFilter+1, 0); 
		}else{
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.uvFilter+1, 1); 
		}
		if (error) goto exit;
	} 

		
	if(modulator->systemConfig.filter_1 != UNUSED){
		if(frequency <= 250000)
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.filter_1+1, 1);
		else
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.filter_1+1, 0);
		if (error) goto exit;	

	}

	if(modulator->systemConfig.filter_2 != UNUSED){
		if(frequency <= 150000){ // 			
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.filter_2+1, 1);
		}else if((frequency > 150000)&&(frequency <= 250000)){
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.filter_2+1, 0);
		}else if((frequency > 250000)&&(frequency <= 400000)){
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.filter_2+1, 1);
		}else if(frequency > 400000){
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.filter_2+1, 0);			
		}
		if (error) goto exit;
	}
	

#ifdef __ADRF6755_H__	
	//modulator->slaveIICAddr = 0x80;
	if(modulator->isExtLo == True) { //for ADRF6755 only

		if(modulator->outputMode == ISDBT) {
		
			tempGI = modulator->isdbtModulation.interval;
			tempMode = modulator->isdbtModulation.transmissionMode;
		}else{
			tempGI = modulator->channelModulation.interval;
			tempMode = modulator->channelModulation.transmissionMode;

		}


		if(tempMode == TransmissionMode_2K && tempGI == Interval_1_OVER_32)
			frequency = frequency - 30; //offset 30KHz 
		IT9510User_i2cSwitchToADRF6755(modulator);
		error = ADRF6755_setFrequency( &(modulator->ADRF6755), frequency);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFB24, 0x04); 
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFB25, 0x34); 
		if (error) goto exit;
		error = IT9510_writeRegister (modulator,  Processor_OFDM, 0xFBBC, 0x22);
		val[0] = 0xf4;
		val[1] = 1;
		val[2] = 0;
		val[3] = 0;
		val[4] = 0xf4;
		val[5] = 1;

		error = IT9510_writeRegisters(modulator, Processor_OFDM, 0xF752, 6, val);
		if (error) goto exit;
		error = ADRF6755_adjustOutputGain ( &(modulator->ADRF6755), modulator->ADRF6755.gain);

	}
#endif

#ifdef __RFFC2072_H__

	if (modulator->deviceType == 0x9C || modulator->deviceType == 0xB2 ||
		modulator->deviceType == 0xB3)
	{	
		if(frequency>LO_Frequency)
			error = IT9510_setFrequency(modulator, frequency - LO_Frequency);
		else
			error = IT9510_setFrequency(modulator, LO_Frequency - frequency);

		if (error) goto exit;

		error = IT9510User_RFFC2072AcquireChannel(&(modulator->RFFC2072), LO_Frequency);
		if (error) goto exit;

		if (modulator->deviceType == 0xB2)
		{
			Byte bIndex = 0;
			int gain_value;

			if (IT9510User_getOutputGainInfo(modulator, frequency, &bIndex) == 0)
			{
			// setting output gain
			// analog gain
			gain_value = modulator->rfGainInfo.ptrGaintable[bIndex].alanogGainValue;
			error = IT9510_writeRegister(modulator, Processor_OFDM, 0xFBBC, gain_value);
			if (error) goto exit;

			// digital gain
			gain_value = modulator->rfGainInfo.ptrGaintable[bIndex].digitalGainValue;
			error = IT9510_adjustOutputGain(modulator, &gain_value);
			if (error) goto exit;
		}
		}

	}
#endif
exit:
#endif
	return (error);
}

Dword IT9510User_acquireChannelDual(
	IN  IT9510INFO*    modulator,
	IN  Word          bandwidth,
	IN  Dword         frequency
	){

	/*
	*  ToDo:  Add code here
	*
	*  // If no error happened return 0, else return error code.
	*  return (0);
	*/
	Dword error = 0;

#ifdef __RFFC2072_H__

	if (modulator->deviceType == 0xB0){

		error = IT9510User_RFFC2072AcquireChannel(&(modulator->RFFC2072), frequency);
		if (error) goto exit;
	}

#endif

exit:

	return (error);

}
Dword IT9510User_setTxModeEnable (
	IN  IT9510INFO*            modulator,
	IN  Byte                    enable	
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	Dword error = ModulatorError_NO_ERROR;
#if IT9510User_INTERNAL
#ifdef __ADRF6755_H__	
	Byte val[6];
	Byte temp = 0;
	TransmissionModes tempMode; /** Number of carriers used for OFDM signal						*/
	Interval	tempGI; 
#endif
	Dword         frequency;
	if(enable){
		
#ifdef __ADRF6755_H__		
		//modulator->slaveIICAddr = 0x80;
		if(modulator->isExtLo == True){
			IT9510User_i2cSwitchToADRF6755(modulator);

			error = ADRF6755_modulatorPowerOn( &(modulator->ADRF6755), True);
			if (error) goto exit;

			error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFB24, 0x04); 
			if (error) goto exit;
			error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFB25, 0x34); 
			if (error) goto exit;
			error = IT9510_writeRegister (modulator,  Processor_OFDM, 0xFBBC, 0x22);
			val[0] = 0xf4;
			val[1] = 1;
			val[2] = 0;
			val[3] = 0;
			val[4] = 0xf4;
			val[5] = 1;

			error = IT9510_writeRegisters(modulator, Processor_OFDM, 0xF752, 6, val);
			if (error) goto exit;
			
			error = ADRF6755_adjustOutputGain ( &(modulator->ADRF6755), modulator->ADRF6755.gain);
			if (error) goto exit;
			//IT9510User_delay(100);
		}
#endif

#ifdef _ORION_CAL_H
		if(modulator->isExtLo == True)
			frequency =  (modulator->frequency/1000)*1000; // fixed Accuracy issue
		else
			frequency =  modulator->frequency;
		//----- orion cal dc
		if(modulator->deviceType == 0xDB || modulator->deviceType == 0x9B ||
		   modulator->deviceType == 0xDD || modulator->deviceType == 0xB3)
		{
#ifdef __ADRF6755_H__
			if(modulator->isExtLo == True){
				temp = modulator->ADRF6755.gain;
				IT9510User_i2cSwitchToADRF6755(modulator);
				error = ADRF6755_setFrequency( &(modulator->ADRF6755), frequency);
				if (error) goto exit;			
				
			}
#endif
			IT9510User_i2cSwitchToOrion(modulator);
			IT9510User_delay(20);
#ifdef __ADRF6755_H__
			if(modulator->isExtLo == True){
				temp = modulator->ADRF6755.gain;
				error = ADRF6755_adjustOutputGain ( &(modulator->ADRF6755), 0);
				if (error) goto exit;
			}
#endif
			
			IT9510User_printf("orion initialize  \n");
			if(modulator->isExtLo == True)
				error = orion_cal_init(modulator, 20000);	
			else
				error = orion_cal_init(modulator, 12000);
			if (error) {
				printk("orion_cal_init fail:[%lu]\n", error);
				goto exit;
			}

		// cal dc with orion iqik module, Michael, 20140502
				
			IT9510User_delay(200);
			IT9510User_printf("orion_calDC.....  \n");
			if(modulator->isExtLo == True)
				error = orion_calDC(modulator, frequency, False);
			else
				error = orion_calDC(modulator, frequency, True);
			if (error) goto exit;
				
			IT9510User_i2cSwitchToOrion2(modulator);

#ifdef __ADRF6755_H__		
			
			if(modulator->isExtLo == True){
				error = ADRF6755_adjustOutputGain (&(modulator->ADRF6755), temp);
				if (error) goto exit;
				IT9510User_delay(20);
				frequency = modulator->frequency;		
				if(modulator->outputMode == ISDBT) {
				
					tempGI = modulator->isdbtModulation.interval;
					tempMode = modulator->isdbtModulation.transmissionMode;
				}else{
					tempGI = modulator->channelModulation.interval;
					tempMode = modulator->channelModulation.transmissionMode;

				}


				if(tempMode == TransmissionMode_2K && tempGI == Interval_1_OVER_32)
					frequency = frequency - 30; //offset 30KHz 
				if(modulator->systemConfig.rfEnable != UNUSED){
						error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.rfEnable+1, 1); //RF power up 
						if (error) goto exit;
				}
				

				IT9510User_i2cSwitchToADRF6755(modulator);
				error = ADRF6755_setFrequency( &(modulator->ADRF6755), frequency);
				if (error) goto exit;
								
				
			}
#endif
		}
#endif
	
		
		IT9510User_printf("TX ON !!.....  \n");
		
	}else{
		
#ifdef __ADRF6755_H__
		//modulator->slaveIICAddr = 0x80;
		if(modulator->isExtLo == True){
			IT9510User_i2cSwitchToADRF6755(modulator);
			error = ADRF6755_modulatorPowerOn( &(modulator->ADRF6755), False);
		}
#endif


	}

	if(modulator->systemConfig.rfEnable != UNUSED){
			error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.rfEnable+1, enable); //RF power on 
			if (error) goto exit;
	}
#ifdef __RFFC2072_H__
	if (modulator->deviceType == 0x9C || modulator->deviceType == 0xB0 ||
		modulator->deviceType == 0xB2 || modulator->deviceType == 0xB3)
	{
		RFFC2072_enableDevice(&(modulator->RFFC2072),(Bool)enable);
	}
#endif
exit :
#endif
	return (error);
}


Dword IT9510User_getChannelIndex (
	IN  IT9510INFO*            modulator,
	IN  Byte*                    index	
) {

	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	Dword error = ModulatorError_NO_ERROR;
#if IT9510User_INTERNAL
	Byte Freqindex = 0;
	Byte temp = 0;
	// get HW setting
	if(modulator->systemConfig.muxSelect != UNUSED){
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.muxSelect+1, 1); //MUX
		if (error) goto exit;
	}

	if(modulator->systemConfig.chSelect0 != UNUSED){ //input
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect0+2, 0);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect0+3, 1);//gpiox_on
		if (error) goto exit;
	}

	if(modulator->systemConfig.chSelect1 != UNUSED){ //input
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect1+2, 0);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect1+3, 1);//gpiox_on
		if (error) goto exit;					
	}

	if(modulator->systemConfig.chSelect2 != UNUSED){ //input
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect2+2, 0);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect2+3, 1);//gpiox_on
		if (error) goto exit;					
	}

	if(modulator->systemConfig.chSelect3 != UNUSED){ //input
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect3+2, 0);//gpiox_en
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect3+3, 1);//gpiox_on
		if (error) goto exit;					
	}
	//--- get HW freq setting
	if(modulator->systemConfig.chSelect0 != UNUSED){
		error = IT9510_readRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect0, &temp); 					
	}
	if (error) goto exit;
	Freqindex = Freqindex | (temp);

	if(modulator->systemConfig.chSelect1 != UNUSED){
		error = IT9510_readRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect1, &temp); 
	}
	if (error) goto exit;
	Freqindex = Freqindex | (temp<<1);

	if(modulator->systemConfig.chSelect2 != UNUSED){
		error = IT9510_readRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect2, &temp); 					
	}
	if (error) goto exit;
	Freqindex = Freqindex | (temp<<2);

	if(modulator->systemConfig.chSelect3 != UNUSED){
		error = IT9510_readRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect3, &temp); 					
	}
	if (error) goto exit;
	Freqindex = Freqindex | (temp<<3);

	error = IT9510_readRegister (modulator, Processor_LINK, 0x49E5, &temp);
	if (error) goto exit;
	Freqindex = Freqindex | (temp<<4);
	//--------------------


	error = IT9510User_setSystemConfig (modulator, modulator->systemConfig);
	if (error) goto exit;


	if(modulator->systemConfig.muxSelect != UNUSED){
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.muxSelect+1, 0); //MUX					
	}

	*index = Freqindex;
exit :
#endif
	return (error);
}


Dword IT9510User_updateEepromData(
	IN  IT9510INFO*     modulator,
    IN  Word            registerAddress,
    IN  Byte            writeBufferLength,
    IN  Byte*           writeBuffer
) {

	Dword error = ModulatorError_NO_ERROR;
	Byte buff[256];
	Word            tempAddress = 0;
    Byte            tempLength = 16;
	Byte i = 0, temp = 0;
	Word wMemAddr = registerAddress;
	Byte bSize = writeBufferLength;

	unsigned short check1, check2;

	error = IT9510_readRegister (modulator, Processor_LINK, 0x4979, &temp);//has eeprom ??
	if((temp != 1) || (error != ModulatorError_NO_ERROR)){ 
		error = ModulatorError_READ_EEPROM_FAIL;
		goto exit;

	}

	if((wMemAddr + bSize)>255){ //length>256 ??
		error = ModulatorError_INVALID_BUF_LEN;
		goto exit;
	}

	for(i=0 ;i<16;i++) {
		tempAddress = i*16;			
		error = IT9510_readEepromValues(modulator, tempAddress, tempLength, &buff[tempAddress]);
		if (error)	goto exit;		
	}

	check1 = checksum(buff[2], buff);
	check2 = (buff[0]<<8 | buff[1]);
	if(check1 != check2){
		IT9510User_printf("eeprom checksum error, load default image!!\n");
		IT9510User_memoryCopy(modulator, buff, eagleDefaultTable, 256);
	}



	if((wMemAddr+bSize) > buff[2])
		buff[2] =(Byte) (wMemAddr+bSize); //change len

	for(i=0; i<bSize; i++){
		buff[wMemAddr + i] = writeBuffer[i];
	}

	check1 = checksum(buff[2], buff); // get new checksum
	buff[0] = (Byte)(check1>>8);
	buff[1] = (Byte)(check1);

	tempAddress = 0;
	tempLength = 16;
	for(i=0 ;i<16;i++) {
		tempAddress = i*16;	
		error = IT9510_writeEepromValues(modulator, tempAddress, tempLength, &buff[tempAddress]);
		if (error)	goto exit;	
	}
exit:
	return (error);
}

Dword IT9510User_LoadDCCalibrationTable (
	IN  IT9510INFO*            modulator									  
) {
    Dword error = ModulatorError_NO_ERROR;
	DCInfo dcInfo;
	Byte DCvalue[16];
	Byte OFS_I_value[7];
	Byte OFS_Q_value[7];
	Byte eeprom = 0;
	Byte DCcalibration = 0;
	Byte index = 0;
//	Byte i =0;

	//-------------set DC Calibration table
	error = IT9510_readRegister (modulator, Processor_LINK, 0x4979, &eeprom);//has eeprom ??
	if (error) goto exit;
	error = IT9510_readRegister (modulator, Processor_LINK, 0x49D6, &DCcalibration);//has DCcalibration ??
	if (error) goto exit;

	if((eeprom ==1) && ((DCcalibration & 0x80) == 0x80)){
		error = IT9510_readRegisters (modulator, Processor_LINK, 0x49D6, 12, DCvalue);//DC calibration value
		if (error) goto exit;
	
		error = IT9510_readRegisters (modulator, Processor_LINK, 0x49CC, 5, OFS_I_value);//OFS_I calibration value
		if (error) goto exit;

		error = IT9510_readRegisters (modulator, Processor_LINK, 0x49E2, 5, OFS_Q_value);//OFS_Q calibration value
		if (error) goto exit;

		error = IT9510_readRegister (modulator, Processor_LINK, 0x49C9, &DCvalue[12]);//DC_6_i calibration value
		if (error) goto exit;
		error = IT9510_readRegister (modulator, Processor_LINK, 0x49D1, &DCvalue[13]);//DC_6_q calibration value
		if (error) goto exit;
		error = IT9510_readRegister (modulator, Processor_LINK, 0x49CB, &DCvalue[14]);//DC_7_i calibration value
		if (error) goto exit;
		error = IT9510_readRegister (modulator, Processor_LINK, 0x49E7, &DCvalue[15]);//DC_7_q calibration value
		if (error) goto exit;

		error = IT9510_readRegister (modulator, Processor_LINK, 0x49AB, &OFS_I_value[5]);//OFS_6_i calibration value
		if (error) goto exit;
		error = IT9510_readRegister (modulator, Processor_LINK, 0x49E8, &OFS_Q_value[5]);//OFS_6_q calibration value
		if (error) goto exit;
		error = IT9510_readRegister (modulator, Processor_LINK, 0x49C3, &OFS_I_value[6]);//OFS_7_i calibration value
		if (error) goto exit;
		error = IT9510_readRegister (modulator, Processor_LINK, 0x49E9, &OFS_Q_value[6]);//OFS_7_q calibration value
		if (error) goto exit;

		for(index = 1; index<8; index++){
		
			if(index == 1){
				modulator->dc_table[index-1].startFrequency = 200000;
				modulator->ofs_table[index-1].startFrequency = 200000;
			} else if(index == 2){
				modulator->dc_table[index-1].startFrequency = 325000;
				modulator->ofs_table[index-1].startFrequency = 325000;
			} else if(index == 3){
				modulator->dc_table[index-1].startFrequency = 500000;
				modulator->ofs_table[index-1].startFrequency = 500000;
			} else if(index == 4){
				modulator->dc_table[index-1].startFrequency = 700000;
				modulator->ofs_table[index-1].startFrequency = 700000;
			} else if(index == 5){
				modulator->dc_table[index-1].startFrequency = 875000;
				modulator->ofs_table[index-1].startFrequency = 875000;
			} else if(index == 6){
				modulator->dc_table[index-1].startFrequency = 1250000;
				modulator->ofs_table[index-1].startFrequency = 1250000;
			} else if(index == 7){
				modulator->dc_table[index-1].startFrequency = 2400000;
				modulator->ofs_table[index-1].startFrequency = 2400000;
			}


			modulator->dc_table[index-1].i = DCvalue[(index*2)];
			if(((DCvalue[0] >> (index-1) ) &0x01) == 0)
				modulator->dc_table[index-1].i = modulator->dc_table[index-1].i * -1;

			modulator->dc_table[index-1].q = DCvalue[1 + (index*2)];
			if(((DCvalue[1] >> (index-1) ) &0x01) == 0)
				modulator->dc_table[index-1].q = modulator->dc_table[index-1].q * -1;
			
			
			modulator->ofs_table[index-1].i = OFS_I_value[index-1];
			modulator->ofs_table[index-1].q = OFS_Q_value[index-1];



		}

		dcInfo.ptrDCtable = modulator->dc_table;
		dcInfo.ptrOFStable = modulator->ofs_table;
		dcInfo.tableGroups = 7;

		error = IT9510_setDCtable(modulator, dcInfo);
	}
exit:
	return error;
}

Dword IT9510User_rfPowerOn (
	IN  IT9510INFO*            modulator,
	IN  Bool                   isPowerOn	
){
	Dword error = ModulatorError_NO_ERROR;

	if(modulator->systemConfig.rfEnable != UNUSED){
		error = IT9510_writeRegister (modulator, Processor_LINK, modulator->systemConfig.rfEnable+1, (Byte)isPowerOn); //RF power on/off 
		if (error) goto exit;
	}
exit:
	return error;

}

Word EepromProtocolChecksum(Byte* p, Byte count)
{
	Word sum;
	Byte i;

	sum = 0;
	for (i = 0; i < count; i++) {
		if (i & 1)
			sum += p[i];		// add low byte.
		else
			sum += ((Word)p[i]) << 8;	// add high byte.
	}

	return ~sum;
}

Dword IT9510User_LoadRFGainTable(
	IN  IT9510INFO*            modulator
	)
{
	Dword error = ModulatorError_NO_ERROR;
	RFGainInfo rfGain_Info;
	Byte eeprom = 0;
	Word tempAddress = 0;
	Byte tempLength = 16;
	Byte tab_count, i, tmp_val;
	Word tmp_data1, tmp_data2;
	Byte eepromData[256];
	Word chk_tmp = 0, chk_page = 0;
	const Byte header_len = 4;
	const Byte sec_unit_size = 4; // eeprom: 2 byte for freq, 1 byte for digital and analog gain
	Byte tab_size = sizeof(modulator->rfGain_table) / sizeof(RFGainTable);

	rfGain_Info.tableIsValid = false;

	//-------------get RF Gain table
	error = IT9510_readRegister(modulator, Processor_LINK, 0x4979, &eeprom);//has eeprom ??
	if (error) goto exit;

	if (eeprom == 0)
	{
		error = ModulatorError_NO_SUCH_TABLE;
		goto exit;
	}

	// read 256 byte from eeprom
	for (i = 0; i<16; i++) {
		tempAddress = i * 16;
		// get page 2 data
		error = IT9510_readEepromValues(modulator, 0x200 + tempAddress, tempLength, &eepromData[tempAddress]);
		if (error)	goto exit;
	}

	// checksum whether correct
	chk_tmp = EepromProtocolChecksum(eepromData, 256 - 2);

	chk_page = (eepromData[254] << 8) + eepromData[255];

	if (chk_tmp != chk_page)
	{
		error = ModulatorError_WRONG_CHECKSUM;
		goto exit;
	}
	// check eeprom data
	if (eepromData[0] == 0)
	{
		error = ModulatorError_NO_SUCH_TABLE;
		goto exit;
	}

	// get the table count
	tab_count = eepromData[1];

	if (tab_count > tab_size || tab_count == 1)
	{
		error = ModulatorError_INVALID_DATA_LENGTH;
		goto exit;
	}

	for (i = 0; i < tab_count; i++)
	{
		// get freq
		modulator->rfGain_table[i].rawFrequency = (eepromData[header_len + i * sec_unit_size] << 8) +
										eepromData[header_len + i * sec_unit_size + 1];
		// get alalog gain
		modulator->rfGain_table[i].alanogGainValue = eepromData[header_len + i * sec_unit_size + 2];

		//get digital gain
		tmp_val = eepromData[header_len + i * sec_unit_size + 3];
		modulator->rfGain_table[i].digitalGainValue = tmp_val & 0x80 ? ((tmp_val & 0x7F) * -1) : tmp_val;
	}

	// calculate the start frequency for gain control
	for (i = 0; i < tab_count - 1; i++)
	{
		tmp_data1 = modulator->rfGain_table[i].rawFrequency;
		tmp_data2 = modulator->rfGain_table[i + 1].rawFrequency;

		modulator->rfGain_table[i].startFrequency = (tmp_data1 + tmp_data2) * 1000 / 2; // use K Hz
	}

	rfGain_Info.tableIsValid = true;
	rfGain_Info.tableCount = tab_count;
	rfGain_Info.ptrGaintable = modulator->rfGain_table;

	error = IT9510_setRFGaintable(modulator, rfGain_Info);

exit:
	return error;
}

Dword IT9510User_getOutputGainInfo(
	IN  IT9510INFO*    modulator,
	IN  Dword			frequency,
	OUT Byte*			index
	)
{
	Dword error = ModulatorError_NO_ERROR;
	Byte tab_count, i, tmpVal;

	RFGainInfo rfGain_Info = modulator->rfGainInfo;

	if (rfGain_Info.tableIsValid == false)
	{
		error = ModulatorError_NOT_SUPPORT;
		goto exit;
	}

	tab_count  = rfGain_Info.tableCount;

	// search the start frequency
	tmpVal = 0;

	for (i = 0; i < tab_count - 1; i++)
	{
		if (rfGain_Info.ptrGaintable[i].startFrequency == frequency)
		{
			tmpVal = i + 1;
			break;
		}
		else if (rfGain_Info.ptrGaintable[i].startFrequency > frequency)
		{
			tmpVal = i;
			break;
		}
	}

	// check frequency whether more than max table value
	if (i == tab_count - 1 && tmpVal == 0)
		tmpVal = i;

	// return output gain index
	*index = tmpVal;

exit:
	return error;
}

Dword IT9510User_adjustOutputGain(
	IN  IT9510INFO*    modulator,
	IN  int			  *gain
	)
{
	Dword error = ModulatorError_NO_ERROR;

#ifdef __RFFC2072_H__
	if (modulator->deviceType == 0xB2)
	{
		Byte bIndex = 0;
		Dword frequency = modulator->frequency;

		if (modulator->frequency < LO_Frequency)
			frequency = modulator->frequency + LO_Frequency;
		else
			frequency = modulator->frequency;

		if (IT9510User_getOutputGainInfo(modulator, frequency, &bIndex) == 0)
		{
		// save the digital gain when different with eeprom 
		modulator->rfGainInfo.ptrGaintable[bIndex].digitalGainValue = *gain;
	}
	}
#endif

	return error;
}

Dword IT9510User_adjustADRF6755Gain(
	IN  IT9510INFO*     modulator,
    IN  Byte            gian    
){
#ifdef __ADRF6755_H__	
	return ADRF6755_adjustOutputGain( &(modulator->ADRF6755), gian);
#else
	return 0;
#endif
}

Dword IT9510User_getADRF6755Gain(
	IN  IT9510INFO*     modulator,
    IN  Byte*           gain
){
#ifdef __ADRF6755_H__	
	*gain = modulator->ADRF6755.gain;
#endif
	return 0;
}

#ifdef __RFFC2072_H__
Dword IT9510User_RFFC2072AcquireChannel (
	IN	RFFC2072INFO *RFFC2072,
	IN  Dword         frequency
){

	return (RFFC2072_setOperatingFrequency(RFFC2072,frequency));

}

Dword IT9510User_RFFC2072DeviceEnable (
	IN  RFFC2072INFO *RFFC2072,
	IN  Bool enable
){

	return (RFFC2072_enableDevice(RFFC2072,enable));

}



Dword IT9510User_RFFC2072SetMixerCurrent (
	IN  RFFC2072INFO *RFFC2072,
	IN  Byte value
){

	return (RFFC2072_setMixerCurrent(RFFC2072,value));

}


Dword IT9510User_RFFC2072SetChargePumpCurrent (
	IN  RFFC2072INFO *RFFC2072,
	IN  Byte value
){

	return (RFFC2072_setChargePumpCurrent(RFFC2072,value));

}


Dword IT9510User_RFFC2072SetChargePumpLeakage (
	IN  RFFC2072INFO *RFFC2072,
	IN  Byte value
){

	return (RFFC2072_setChargePumpLeakage(RFFC2072,value));

}

Dword IT9510User_RFFC2072ReadRegister (
	IN  RFFC2072INFO *RFFC2072,
	Byte regAddr,
	IN  Word* value
){

	return (RFFC2072_readRegister(RFFC2072, regAddr, value));

}


Dword IT9510User_RFFC2072WriteRegister (
	IN  RFFC2072INFO *RFFC2072,
	Byte regAddr,
	IN  Word value
){

	return (RFFC2072_writeRegister(RFFC2072, regAddr, value));

}

Dword IT9510User_RFFC2072SetReLock (
	IN  RFFC2072INFO *RFFC2072
){

	return (RFFC2072_setReLock(RFFC2072));
}
#endif
