#ifndef __IT9510_TYPE_H__
#define __IT9510_TYPE_H__

//#ifndef __MODULATOR_TYPE_H__
#include "modulatorType.h"
//#endif
#include "ADRF6755.h"
#include "RFFC2072.h"
#include <linux/kthread.h>

#define IT9510User_INTERNAL	 1
#define IT9133Rx	1

#define IT9517Cmd_buildCommand(command, processor)  (command + (Word) (processor << 12))
#define IT9510_MAX_BIT  

typedef enum {
   filter = 0,	
   LayerB = 1,
   LayerA = 2,
   LayerAB = 3
} TransportLayer;


/**
 * The type defination of Constellation.
 */
typedef enum {
    DownSampleRate_21_OVER_1 = 0,		/** Signal uses FEC coding ratio of 21/1						*/
    DownSampleRate_21_OVER_2,			/** Signal uses FEC coding ratio of 21/2						*/
    DownSampleRate_21_OVER_3,			/** Signal uses FEC coding ratio of 21/3						*/
    DownSampleRate_21_OVER_4,			/** Signal uses FEC coding ratio of 21/4						*/
    DownSampleRate_21_OVER_5,			/** Signal uses FEC coding ratio of 21/5						*/
    DownSampleRate_21_OVER_6,			/** Signal uses FEC coding ratio of 21/6						*/
} DownSampleRate;

typedef enum {
	DVBT = 1,
    ISDBT
} OutputMode;

typedef enum {
	NullPacketModeDisable = 0,
	SequentialMode = 1,
    NormalMode
} NullPacketMode;

typedef enum {
	PcrModeDisable = 0,
	PcrMode1 = 1,
    PcrMode2,
	PcrMode3
} PcrMode;

typedef enum {
	ARIB_STD_B31 = 0,					/** System based on this specification							*/
	ISDB_TSB							/** System for ISDB-TSB											*/
} SystemIdentification;


typedef struct {
	Constellation	constellation;      /** Constellation scheme (FFT mode) in use						*/
	CodeRate		codeRate;		    /** FEC coding ratio of high-priority stream					*/
} TMCC;

typedef struct {
    Dword		frequency;              /** Channel frequency in KHz.									*/
    Bandwidth	bandwidth;
	TransmissionModes transmissionMode; /** Number of carriers used for OFDM signal						*/
	Interval	interval;               /** Fraction of symbol length used as guard (Guard Interval)	*/
	TMCC		layerA;
	TMCC		layerB;	
	Bool		isPartialReception;
} ISDBTModulation;

typedef struct _TMCCINFO{
	TMCC					layerA;
	TMCC					layerB;	
	Bool					isPartialReception;
	SystemIdentification	systemIdentification;
} TMCCINFO, *pTMCCINFO;


typedef struct {
    PcrMode pcrMode;
	Word bandwidth;
	ChannelModulation channelModulation;
	ISDBTModulation   isdbtModulation;
	OutputMode outputMode; //0:un 1:DVB 2:isdbt; 
	Dword   packetTimeJitter_ps;
	Dword	pcrExtJitter;
	int		positive; //-1:- / 1:+
} PCRCALINFO;

/**
 * The data structure of IT9510
 */
typedef struct {
    /** Basic structure */
    Handle userData;
    Byte busId;
	Byte i2cAddr;
    Byte* firmwareCodes;
    Segment* firmwareSegments;
    Word*  firmwarePartitions;
    Word* scriptSets;
    ValueSet* scripts;
    TsInterface tsInterfaceType;
    Word bandwidth;
    Dword frequency;    
    Bool booted;
	Byte slaveIICAddr;  
	ChannelModulation channelModulation;
	CalibrationInfo calibrationInfo;
	DCInfo dcInfo;
#if IT9510User_INTERNAL
	SystemConfig systemConfig;	
#endif
	ISDBTModulation   isdbtModulation;
	OutputMode outputMode; //0:un 1:DVB 2:isdbt; 
	NullPacketMode nullPacketMode;
	PcrMode pcrMode;
	Bool isExtLo;
#if IT9510User_INTERNAL
	Byte deviceType;
#endif
	PCRCALINFO	pcrCalInfo;
	RFGainInfo rfGainInfo;
	
	// Fix shared memery for DC table at multi-device issue. By JK.
	DCtable dc_table[7];
	DCtable ofs_table[7];
	RFGainTable rfGain_table[50];
	
#ifdef __ADRF6755_H__
	ADRF6755INFO ADRF6755;
#endif	

#ifdef __RFFC2072_H__
	RFFC2072INFO RFFC2072;
#endif

// for RFFC2072 board issue. Add by JK.
#if RF_RELOCK_MONITOR
	struct task_struct *relock_thread;	// for RFFC2072 board.
	Byte start_detect_relock_status;
#endif
// check whether device is disconnect. Add by JK.
	Bool dev_disconnect;
} IT9510INFO;


extern const Byte IT9510_bitMask[8];
#define IT9510_REG_MASK(pos, len)                (IT9510_bitMask[len-1] << pos)
#define IT9510_REG_CLEAR(temp, pos, len)         (temp & (~IT9510_REG_MASK(pos, len)))
#define IT9510_REG_CREATE(val, temp, pos, len)   ((val << pos) | (IT9510_REG_CLEAR(temp, pos, len)))
#define IT9510_REG_GET(value, pos, len)          ((value & IT9510_REG_MASK(pos, len)) >> pos)

#endif
