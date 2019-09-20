#ifndef __IT9510User_H__
#define __IT9510User_H__


//#include <stdio.h>
#include "IT9510.h"
#include "modulatorType.h"
#if IT9510User_INTERNAL
#include "i2cimpl.h"
#include "usb2impl.h"
#include "af9035u2iimpl.h"
#endif
#include <linux/string.h>
#include "it951x-core.h"

#define IT9510User_MAX_PKT_SIZE               255


#define IT9510User_RETRY_MAX_LIMIT            10


/** Define I2C master speed, the default value 0x07 means 366KHz (1000000000 / (24.4 * 16 * IT9510User_I2C_SPEED)). */
#define IT9510User_IIC_SPEED              0x07

/** Define I2C address of secondary chip when Diversity mode or PIP mode is active. */
#define IT9510User_IIC_ADDRESS            0x38
#define IT9510User_SlaveIIC_ADDRESS      0x3A//0x80
#define IT9510User_DEVICETYPE			 0x01//0xD9

/** Define USB frame size */

#define IT9510User_USB20_MAX_PACKET_SIZE_EP4      256
#define IT9510User_USB20_MAX_PACKET_SIZE_EP5      512
#define IT9510User_USB20_FRAME_SIZE_EP4           URB_BUFSIZE_TX_CMD //(188 * 1)
#define IT9510User_USB20_FRAME_SIZE_EP5           URB_BUFSIZE_RX //(188 * 348)
#define IT9510User_USB20_FRAME_SIZE_DW        (IT9510User_USB20_FRAME_SIZE / 4)
#define IT9510User_USB11_MAX_PACKET_SIZE      64
#define IT9510User_USB11_FRAME_SIZE           (188 * 21)
#define IT9510User_USB11_FRAME_SIZE_DW        (IT9510User_USB11_FRAME_SIZE / 4)
#define IT9510User_MAXFRAMESIZE			63

#if IT9510User_INTERNAL

#ifndef __EAGLEUSER_H__
typedef enum {
	NA = 0,
	ResetSlave,
	RfEnable,
	LoClk,
	LoData,
	LoLe,
	LnaPowerDown,
	IrDa,
	UvFilter,
	ChSelect3,
	ChSelect2,
	ChSelect1,
	ChSelect0,
	PowerDownSlave,
	UartTxd,
	MuxSelect,
	lnaGain,
	intrEnable,
	DataRateCheck,
	SlaveLock,
	Filter_1,
	Filter_2,
	UartRxd,
	EepromSwitch,
	Filter_1n,
	Filter_2n,
	SpiEnable,
	SpiClock,
	SpiData,
	ScanLock,
	Ant_Switch1,
	Ant_Switch2,
	CalibOn,
	ResetSlave_2
} HwFunction;


typedef struct {
    HwFunction          GPIO1;
    HwFunction          GPIO2;
    HwFunction          GPIO3;
    HwFunction          GPIO4;
	HwFunction          GPIO5;
    HwFunction          GPIO6;
    HwFunction          GPIO7;
    HwFunction          GPIO8;
} DeviceDescription;

typedef enum {
	EVB01v01 = 0,
	DB0101v01,
	DB0102v01,
	DB0101v03,
	DB0102v02,
	SDAVsender,
	EVB01v02,
	DTVCAM_Yuelee,
	DTVCAM_Sunnic,
	HDAVsender,
	AVsenderHC,
	DB0101v04,
	MutiChTxBoard,
	HV100,
	IndexEnd
} PCBIndex;

typedef enum {
	EVB = 0,
	Dongle,
	CAM,
	CAM_1,
	Type4,
	Type5,
	ADF4351,
	DTVBridge,
	Eagle2Dongle,
	TxModule,
	Eagle2Dongle_V2,
	TxModule_20M,
	TxModule_NEW,
	TxModule_NEW_orion,
	PCBExIndexEnd
} PCBExIndex;

#endif
Dword IT9510User_getDeviceTypeSetting (
    IN  IT9510INFO*    modulator,
    IN  Byte          pcbIndex,
	IN  SystemConfig* Config  
);


Dword IT9510User_setSystemConfig (
    IN  IT9510INFO*    modulator,
	IN  SystemConfig  systemConfig
);

Dword IT9510User_getTsInputType (
	IN  IT9510INFO*    modulator,
	OUT  TsInterface*  tsInStreamType
);

Dword IT9510User_getDeviceType (
	IN  IT9510INFO*    modulator,
	OUT  Byte*		  deviceType	   
);

#endif
/**
 * Memory copy Function
 */
Dword IT9510User_memoryCopy (
    IN  IT9510INFO*    modulator,
    IN  void*           dest,
    IN  void*           src,
    IN  Dword           count
);


/**
 * Delay Function
 */
Dword IT9510User_delay (
    IN  Dword           dwMs
);

/**
 * printf Function
 */
Dword IT9510User_printf (const char* format,...);

/**
 * Enter critical section
 */
Dword IT9510User_enterCriticalSection (void);


/**
 * Leave critical section
 */
Dword IT9510User_leaveCriticalSection (void);


/**
 * Config MPEG2 interface
 */
Dword IT9510User_mpegConfig (
    IN  IT9510INFO*    modulator
);


/**
 * Write data via "Control Bus"
 * I2C mode : uc2WireAddr mean modulator chip address, the default value is 0x38
 * USB mode : uc2WireAddr is useless, don't have to send this data
 */
Dword IT9510User_busTx (
    IN  IT9510INFO*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
);


/**
 * Read data via "Control Bus"
 * I2C mode : uc2WireAddr mean modulator chip address, the default value is 0x38
 * USB mode : uc2WireAddr is useless, don't have to send this data
 */
Dword IT9510User_busRx (
    IN  IT9510INFO*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);


Dword IT9510User_setBus (
	IN  IT9510INFO*	modulator
);

Dword IT9510User_Initialization  (
    IN  IT9510INFO*    modulator
); 

Dword IT9510User_Finalize  (
    IN  IT9510INFO*    modulator
);

Dword IT9510User_acquireChannel (
	IN  IT9510INFO*    modulator,
	IN  Word          bandwidth,
	IN  Dword         frequency
);

Dword IT9510User_acquireChannelDual(
	IN  IT9510INFO*    modulator,
	IN  Word          bandwidth,
	IN  Dword         frequency
	);

Dword IT9510User_setTxModeEnable (
	IN  IT9510INFO*            modulator,
	IN  Byte                    enable	
);

Dword IT9510User_getChannelIndex (
	IN  IT9510INFO*            modulator,
	IN  Byte*                    index	
);

Dword IT9510User_updateEepromData(
	IN  IT9510INFO*     modulator,
    IN  Word            registerAddress,
    IN  Byte            writeBufferLength,
    IN  Byte*           writeBuffer
);

Dword IT9510User_LoadDCCalibrationTable (
	IN  IT9510INFO*            modulator									  
);

Dword IT9510User_rfPowerOn (
	IN  IT9510INFO*            modulator,
	IN  Bool                   isPowerOn	
);

Dword IT9510User_LoadRFGainTable(
	IN  IT9510INFO*            modulator
	);

Dword IT9510User_getOutputGainInfo(
	IN  IT9510INFO*    modulator,
	IN  Dword			frequency,
	OUT Byte*			index		
	);

Dword IT9510User_adjustOutputGain(
	IN  IT9510INFO*    modulator,
	IN  int			  *gain
	);

Dword IT9510User_adjustADRF6755Gain(
	IN  IT9510INFO*     modulator,
    IN  Byte            gian    
);

Dword IT9510User_getADRF6755Gain(
	IN  IT9510INFO*     modulator,
    IN  Byte*            gain
);


#ifdef __RFFC2072_H__
Dword IT9510User_RFFC2072AcquireChannel (
	IN	RFFC2072INFO *RFFC2072,
	IN  Dword         frequency
);

Dword IT9510User_RFFC2072DeviceEnable (
	IN  RFFC2072INFO *RFFC2072,
	IN  Bool enable
);

Dword IT9510User_RFFC2072SetMixerCurrent (
	IN  RFFC2072INFO *RFFC2072,
	IN  Byte value
);

Dword IT9510User_RFFC2072SetChargePumpCurrent (
	IN  RFFC2072INFO *RFFC2072,
	IN  Byte value
);

Dword IT9510User_RFFC2072SetChargePumpLeakage (
	IN  RFFC2072INFO *RFFC2072,
	IN  Byte value
);

Dword IT9510User_RFFC2072ReadRegister (
	IN  RFFC2072INFO *RFFC2072,
	Byte regAddr,
	IN  Word* value
);


Dword IT9510User_RFFC2072WriteRegister (
	IN  RFFC2072INFO *RFFC2072,
	Byte regAddr,
	IN  Word value
);

Dword IT9510User_RFFC2072SetReLock (
	IN  RFFC2072INFO *RFFC2072
);
#endif

#endif

