#ifndef __USER_H__
#define __USER_H__


//#include <stdio.h> //for Linux
#include "IT9510Type.h"
#include "error.h"




#define User_USE_INTERRUPT              0
#define User_USE_DRIVER                 0

#define User_OMEGA2_API_RELEASE		1 

#define User_MAX_PKT_SIZE               255
#define User_USE_SHORT_CMD              0

#define User_RETRY_MAX_LIMIT            1


/** Define I2C master speed, the default value 0x07 means 366KHz (1000000000 / (24.4 * 16 * User_I2C_SPEED)). */
#define User_I2C_SPEED              0x07

/** Define I2C address of secondary chip when Diversity mode or PIP mode is active. */
//#define User_I2C_ADDRESS			0x3A//0x38
#define User_Chip2_I2C_ADDRESS      0x3A//0x3c

/** Define USB frame size */
#define User_USB20_MAX_PACKET_SIZE      512
#ifdef DVB_USB_ADAP_NEED_PID_FILTER 
#define User_USB20_FRAME_SIZE           (188 * 21)
#else
#define User_USB20_FRAME_SIZE           (188 * 348)
#endif	
#define User_USB20_FRAME_SIZE_DW        (User_USB20_FRAME_SIZE / 4)
#define User_USB11_MAX_PACKET_SIZE      64
#define User_USB11_FRAME_SIZE           (188 * 21)
#define User_USB11_FRAME_SIZE_DW        (User_USB11_FRAME_SIZE / 4)





/**
 * Memory copy Function
 */
Dword User_memoryCopy (
    IN  Demodulator*    demodulator,
    IN  void*           dest,
    IN  void*           src,
    IN  Dword           count
);


/**
 * Delay Function
 */
Dword User_delay (
    IN  Demodulator*    demodulator,
    IN  Dword           dwMs
);


/**
 * Enter critical section
 */
Dword User_enterCriticalSection (
    IN  Demodulator*    demodulator
);


/**
 * Leave critical section
 */
Dword User_leaveCriticalSection (
    IN  Demodulator*    demodulator
);


/**
 * Config MPEG2 interface
 */
Dword User_mpegConfig (
    IN  Demodulator*    demodulator
);


/**
 * Write data via "Control Bus"
 * I2C mode : uc2WireAddr mean demodulator chip address, the default value is 0x38
 * USB mode : uc2WireAddr is useless, don't have to send this data
 */
Dword User_busTx (
    IN  Demodulator*    demodulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
);


/**
 * Read data via "Control Bus"
 * I2C mode : uc2WireAddr mean demodulator chip address, the default value is 0x38
 * USB mode : uc2WireAddr is useless, don't have to send this data
 */
Dword User_busRx (
    IN  Demodulator*    demodulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);


/**
 * Read data via "Data Bus"
 * I2C mode : uc2WireAddr mean demodulator chip address, the default value is 0x38
 * USB mode : uc2WireAddr is useless, don't have to send this data
 */
Dword User_busRxData (
    IN  Demodulator*    demodulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);
#endif
