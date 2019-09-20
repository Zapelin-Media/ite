#ifndef __IT9510_H__
#define __IT9510_H__


#include "modulatorType.h"
#include "IT9510Type.h"
#include "IT9510User.h"
#include "modulatorError.h"
#include "IT9510Register.h"
#include "IT9510Variable.h"
#include "IT9510Version.h"
#include "IT9510Firmware.h"
#include "IQ_fixed_table.h"
#include "IT9510Tuner.h"
#include "IT9510PcrRestamp.h"

/**
 * Write one byte (8 bits) to a specific register in modulator.
 *
 * @param modulator the handle of modulator.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param registerAddress the address of the register to be written.
 * @param value the value to be written.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Eagle eagle;
 *
 *     // Set the value of register 0xA000 in modulator to 0.
 *     error = IT9510_writeRegister ((IT9510INFO*) &eagle, 0, Processor_LINK, 0xA000, 0);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword IT9510_writeRegister (
    IN  IT9510INFO*    modulator,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            value
);


/**
 * Write a sequence of bytes to the contiguous registers in modulator.
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 5.
 *
 * @param modulator the handle of modulator.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param registerAddress the start address of the registers to be written.
 * @param bufferLength the number of registers to be written.
 * @param buffer a byte array which is used to store values to be written.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte buffer[3] = { 0x00, 0x01, 0x02 };
 *     Eagle eagle;
 *
 *     // Set the value of register 0xA000 in modulator to 0.
 *     // Set the value of register 0xA001 in modulator to 1.
 *     // Set the value of register 0xA002 in modulator to 2.
 *     error = IT9510_writeRegisters ((IT9510INFO*) &eagle, Processor_LINK, 0xA000, 3, buffer);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword IT9510_writeRegisters (
    IN  IT9510INFO*    modulator,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
);


/**
 * Write a sequence of bytes to the contiguous registers in slave device
 * through specified interface (1, 2, 3).
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 6 (one more byte to specify tuner address).
 *
 * @param modulator the handle of modulator.
 * @param interfaceIndex the index of interface. The possible values are
 *        1~3.
 * @param slaveAddress the I2c address of slave device.
 * @param bufferLength the number of registers to be read.
 * @param buffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_writeGenericRegisters (
    IN  IT9510INFO*    modulator,
    IN  Byte            slaveAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
);


/**
 * Write a sequence of bytes to the contiguous cells in the EEPROM.
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 5 (firmware will detect EEPROM address).
 *
 * @param modulator the handle of modulator.
 * @param registerAddress the start address of the cells to be written.
 * @param registerAddressLength the valid bytes of registerAddress.
 * @param bufferLength the number of cells to be written.
 * @param buffer a byte array which is used to store values to be written.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte buffer[3] = { 0x00, 0x01, 0x02 };
 *     Eagle eagle;
 *
 *     // Set the value of cell 0x0000 in EEPROM to 0.
 *     // Set the value of cell 0x0001 in EEPROM to 1.
 *     // Set the value of cell 0x0002 in EEPROM to 2.
 *     error = IT9510_writeEepromValues ((IT9510INFO*) &eagle, 0, 0x0000, 3, buffer);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword IT9510_writeEepromValues (
    IN  IT9510INFO*    modulator,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
);


/**
 * Modify bits in the specific register.
 *
 * @param modulator the handle of modulator.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param registerAddress the address of the register to be written.
 * @param position the start position of bits to be modified (0 means the
 *        LSB of the specifyed register).
 * @param length the length of bits.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Eagle eagle;
 *
 *     // Modify the LSB of register 0xA000 in modulator to 0.
 *     error = IT9510_writeRegisterBits ((IT9510INFO*) &eagle, 0, Processor_LINK, 0xA000, 0, 1, 0);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword IT9510_writeRegisterBits (
    IN  IT9510INFO*    modulator,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            position,
    IN  Byte            length,
    IN  Byte            value
);


/**
 * Read one byte (8 bits) from a specific register in modulator.
 *
 * @param modulator the handle of modulator.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param registerAddress the address of the register to be read.
 * @param value the pointer used to store the value read from modulator
 *        register.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte value;
 *     Eagle eagle;
 *
 *     // Get the value of register 0xA000 in modulator.
 *     error = IT9510_readRegister ((IT9510INFO*) &eagle, 0, Processor_LINK, 0xA000, &value);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *     printf ("The value of 0xA000 is %2x", value);
 * </pre>
 */
Dword IT9510_readRegister (
    IN  IT9510INFO*    modulator,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    OUT Byte*           value
);


/**
 * Read a sequence of bytes from the contiguous registers in modulator.
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 5.
 *
 * @param modulator the handle of modulator.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param registerAddress the address of the register to be read.
 * @param bufferLength the number of registers to be read.
 * @param buffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte buffer[3];
 *     Eagle eagle;
 *
 *     // Get the value of register 0xA000, 0xA001, 0xA002 in modulator.
 *     error = IT9510_readRegisters ((IT9510INFO*) &eagle, 0, Processor_LINK, 0xA000, 3, buffer);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *     printf ("The value of 0xA000 is %2x", buffer[0]);
 *     printf ("The value of 0xA001 is %2x", buffer[1]);
 *     printf ("The value of 0xA002 is %2x", buffer[2]);
 * </pre>
 */
Dword IT9510_readRegisters (
    IN  IT9510INFO*    modulator,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            bufferLength,
    OUT Byte*           buffer
);


/**
 * Read a sequence of bytes from the contiguous registers in slave device
 * through specified interface (1, 2, 3).
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 6 (one more byte to specify tuner address).
 *
 * @param modulator the handle of modulator.
 * @param slaveAddress the I2c address of slave device.
 * @param bufferLength the number of registers to be read.
 * @param buffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_readGenericRegisters (
    IN  IT9510INFO*    modulator,
    IN  Byte            slaveAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
);


/**
 * Read a sequence of bytes from the contiguous cells in the EEPROM.
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 5 (firmware will detect EEPROM address).
 *
 * @param modulator the handle of modulator.
 * @param registerAddress the start address of the cells to be read.
 * @param bufferLength the number of cells to be read.
 * @param buffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte buffer[3];
 *     Eagle eagle;
 *
 *     // Get the value of cell 0x0000, 0x0001, 0x0002 in EEPROM.
 *     error = IT9510_readEepromValues ((IT9510INFO*) &eagle, 0, 0x0000, 3, buffer);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *     printf ("The value of 0x0000 is %2x", buffer[0]);
 *     printf ("The value of 0x0001 is %2x", buffer[1]);
 *     printf ("The value of 0x0002 is %2x", buffer[2]);
 * </pre>
 */
Dword IT9510_readEepromValues (
    IN  IT9510INFO*    modulator,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    OUT Byte*           buffer
);


/**
 * Read bits of the specified register.
 *
 * @param modulator the handle of modulator.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param registerAddress the address of the register to be read.
 * @param position the start position of bits to be read (0 means the
 *        LSB of the specifyed register).
 * @param length the length of bits.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte value;
 *     Eagle eagle;
 *
 *     // Read the LSB of register 0xA000 in modulator.
 *     error = IT9510_readRegisterBits ((IT9510INFO*) &eagle, 0, Processor_LINK, 0xA000, 0, 1, &value);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *     printf ("The value of LSB of 0xA000 is %2x", value);
 * </pre>
 */
Dword IT9510_readRegisterBits (
    IN  IT9510INFO*    modulator,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            position,
    IN  Byte            length,
    OUT Byte*           value
);


/**
 * Get the version of firmware.
 *
 * @param modulator the handle of modulator.
 * @param version the version of firmware.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Dword version;
 *     Eagle eagle;
 *
 *     // Get the version of Link layer firmware.
 *     error = IT9510_getFirmwareVersion ((IT9510INFO*) &eagle, Processor_LINK, &version);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("The version of firmware is : %X", version);
 * </pre>
 */
Dword IT9510_getFirmwareVersion (
    IN  IT9510INFO*    modulator,
    IN  Processor       processor,
    OUT Dword*          version
);



/**
 * Load the IR table for USB device.
 *
 * @param modulator the handle of modulator.
 * @param tableLength The length of IR table.
 * @param table The content of IR table.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword IT9510_loadIrTable (
    IN  IT9510INFO*    modulator,
    IN  Word            tableLength,
    IN  Byte*           table
);


/**
 * First, download firmware from host to modulator. Actually, firmware is
 * put in firmware.h as a part of source code. Therefore, in order to
 * update firmware the host have to re-compile the source code.
 * Second, setting all parameters which will be need at the beginning.
 *
 * @param modulator the handle of modulator.
 * @param streamType The format of TS interface type.
 * @param busId The id of bus type.1:Bus_I2C 2:Bus_USB
 * @param i2cAddr The address of i2c bus.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Modulator eagle;
 *
 *     // Initialize demodulators.
 *     // SAW Filter  : 8MHz
 *     // Stream Type : IP Datagram.
 *     error = IT9510_TXinitialize ((IT9510INFO*) &eagle, SERIAL_TS_INPUT, Bus_I2C, 0x38);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword IT9510_initialize (
    IN  IT9510INFO*    modulator,
    IN  TsInterface   streamType,
	IN  Byte            busId,
	IN  Byte            i2cAddr
);


/**
 * Power off the demodulators.
 *
 * @param modulator the handle of modulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Modulator eagle;
 *
 *     // Finalize demodulators.
 *     error = IT9510_finalize ((IT9510INFO*) &eagle);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword IT9510_finalize (
    IN  IT9510INFO*    modulator
);


/**
 * Reset modulator.
 *
 * @param modulator the handle of modulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Modulator eagle;
 *
 *     // Reset modulator.
 *     error = IT9510_reset ((IT9510INFO*) &eagle);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword IT9510_reset (
    IN  IT9510INFO*    modulator
);



/**
 * Set Tx channel modulation related information.
 *
 * @param modulator the handle of modulator.
 * @param channelModulation The modulation of channel.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword IT9510_setTXChannelModulation (
    IN  IT9510INFO*            modulator,
    IN  ChannelModulation*      channelModulation
);


Dword IT9510_setISDBTChannelModulation (
    IN  IT9510INFO*            modulator,
    IN  ISDBTModulation      isdbtModulation
) ;
/**
 * Set TX mode enable(output data).
 *
 * @param modulator the handle of modulator.
 * @param enable The flag of enable(1:on / 0:off).
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 */
Dword IT9510_setTxModeEnable (
    IN  IT9510INFO*            modulator,
    IN  Byte                    enable
);

/**
 * Set output frequency.
 *
 * @param modulator the handle of modulator.
 * @param frequency the channel frequency in KHz.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 */
Dword IT9510_setFrequency (
	IN  IT9510INFO*    modulator,
	IN  Dword           frequency
);

/**
 * Specify the bandwidth of channel and tune the channel to the specific
 * frequency. Afterwards, host could use output parameter dvbH to determine
 * if there is a DVB-H signal.
 * In DVB-T mode, after calling this function the output parameter dvbH
 * should return False and host could use output parameter "locked" to check
 * if the channel has correct TS output.
 * In DVB-H mode, after calling this function the output parameter dvbH should
 * return True and host could start get platform thereafter.
 *
 * @param modulator the handle of modulator.
 * @param bandwidth The channel bandwidth.
 *        DVB-T: 5000, 6000, 7000, and 8000 (KHz).
 * @param frequency the channel frequency in KHz.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Eagle eagle;
 *
 *     error = IT9510_acquireTxChannel ((IT9510INFO*) &eagle, 8000, 666000);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");*
 *    
 * </pre>
 */

Dword IT9510_acquireTxChannel (
    IN  IT9510INFO*      modulator,
    IN  Word            bandwidth,
    IN  Dword           frequency
);

/**
* Specify the bandwidth of channel and tune the channel to the specific
* frequency. Afterwards, host could use output parameter dvbH to determine
* if there is a DVB-H signal.
* In DVB-T mode, after calling this function the output parameter dvbH
* should return False and host could use output parameter "locked" to check
* if the channel has correct TS output.
* In DVB-H mode, after calling this function the output parameter dvbH should
* return True and host could start get platform thereafter.
*
* @param modulator the handle of modulator.
* @param bandwidth The channel bandwidth.
*        DVB-T: 5000, 6000, 7000, and 8000 (KHz).
* @param frequency1 the channel frequency in KHz(for Eagle2, less than 950 MHz).
* @param frequency2 the channel frequency in KHz(for RF2072, for 2.4G) .
* @return Error_NO_ERROR: successful, non-zero error code otherwise.
*/

Dword IT9510_acquireTxChannelDual(
	IN  IT9510INFO*      modulator,
	IN  Word            bandwidth,
	IN  Dword           frequency1,
	IN  Dword			frequency2
	);

/**
 * reset PSB buffer
 *
 * @param modulator the handle of modulator.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 */
Dword IT9510_resetPSBBuffer (
	IN  IT9510INFO*    modulator
);

/**
 * Set the output stream type of chip. Because the device could output in
 * many stream type, therefore host have to choose one type before receive
 * data.
 *
 * Note: After host know all the available channels, and want to change to
 *       specific channel, host have to choose output mode before receive
 *       data. Please refer the example of IT9510_setTsInterface.
 *
 * @param modulator the handle of modulator.
 * @param streamType the possible values are
 *        DVB-T:    PARALLEL_TS_INPUT
 *                  SERIAL_TS_INPUT
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Eagle eagle;
 *
 *     error = IT9510_setTsInterface ((IT9510INFO*) &eagle, SERIAL_TS_INPUT)
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword IT9510_setTsInterface (
    IN  IT9510INFO*    modulator,
    IN  TsInterface   streamType
);

/**
 *
 * @param modulator the handle of modulator.
 * @param code the value of IR raw code, the size should be 4 or 6,
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword IT9510_getIrCode (
    IN  IT9510INFO*    modulator,
    OUT Dword*          code
);


/**
 * Return to boot code
 *
 * @param modulator the handle of modulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword IT9510_TXreboot (
    IN  IT9510INFO*    modulator
);



/**
 *
 * @param modulator the handle of modulator.
 * @param contorl 1: Power up, 0: Power down;
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword IT9510_controlPowerSaving (
    IN  IT9510INFO*    modulator,
    IN  Byte            control
);



/**
 * Control PID fileter
 *
 * @param modulator the handle of modulator.
 * @param control 0: tha same PID pass 1: difference PID pass.
 * @param enable 0: FID filter Disable, 1: FID filter Enable.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword IT9510_controlPidFilter (
    IN  IT9510INFO*    modulator,
    IN  Byte            control,
	IN  Byte            enable
);


/**
 * Reset PID filter.
 *
 * @param modulator the handle of modulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Eagle eagle;
 *
 *     error = IT9510_resetPidFilter ((IT9510INFO*) &eagle, 0);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword IT9510_resetPidFilter (
    IN  IT9510INFO*    modulator
);


/**
 * Add PID to PID filter.
 *
 * @param modulator the handle of modulator.
 * @param index the index of PID filter.
 * @param pid the PID that will be add to PID filter.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Pid pid;
 *     Eagle eagle;
 *
 *     pid.value = 0x0000;
 *
 *     // Add PID to PID filter.
 *     error = IT9510_addPidToFilter ((IT9510INFO*) &eagle, 0, 1, pid);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword IT9510_addPidToFilter (
    IN  IT9510INFO*    modulator,
    IN  Byte            index,
    IN  Pid             pid
);


Dword IT9510_addPidToISDBTPidFilter (
    IN  IT9510INFO*    modulator,
    IN  Byte            index,
    IN  Pid             pid,
	IN	TransportLayer  layer
);

/**
 * Access & send HW PSI table.
 *
 * @param modulator the handle of modulator.
 * @param pbuffer the data that will be write to HW PSI table.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_sendHwPSITable (
	IN  IT9510INFO*    modulator,
	IN  Byte*            pbuffer
);

/**
 * Access FW PSI table.
 *
 * @param modulator the handle of modulator.
 * @param the index of FW PSI table. The possible values are
 *        1~5.
 * @param pbuffer the data that will be write to HW PSI table.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_accessFwPSITable (
	IN  IT9510INFO*    modulator,
	IN  Byte		  psiTableIndex,
	IN  Byte*         pbuffer
);

/**
 * set FW PSI table output timer.
 *
 * @param modulator the handle of modulator.
 * @param the index of FW PSI table. The possible values are
 *        1~5.
 * @param timer the timer unit is ms.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_setFwPSITableTimer (
	IN  IT9510INFO*    modulator,
	IN  Byte		  psiTableIndex,
	IN  Word          timer_ms
); 

/**
 * set Slave IIC Address.
 *
 * @param modulator the handle of modulator.
 * @param SlaveAddress Slave device IIC Addr.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_setSlaveIICAddress (
    IN  IT9510INFO*    modulator,
	IN  Byte          SlaveAddress
);

/**
 * run IT9517 Calibration.
 *
 * @param modulator the handle of modulator.
 * @param bandwidth channel bandwidth
 * @param frequency channel frequency
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_runTxCalibration (
	IN  IT9510INFO*    modulator,
	IN  Word            bandwidth,
    IN  Dword           frequency
);

/**
 * set IT9517 IQ value.
 *
 * @param modulator the handle of modulator.
 * @param bandwidth channel bandwidth
 * @param frequency channel frequency
 * @
 */
Dword IT9510_setIQValue(
	IN  IT9510INFO*    modulator,
    IN  int dAmp,
	IN  int dPhi
);

/**
 * adjust Output Gain.
 *
 * @param modulator the handle of modulator.
 * @param gain adjust output gain value.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_adjustOutputGain (
	IN  IT9510INFO*    modulator,
	IN  int			  *gain	   
); 

/**
 * get Output Gain.
 *
 * @param modulator the handle of modulator.
 * @param gain : output gain value.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_getOutputGain (
	IN  IT9510INFO*    modulator,
	OUT  int			  *gain	   
);


/**
 * Get the Min/Max Gain range of the specified frequency and bandwidth to IT9500 TX.
 * @param modulator the handle of modulator.
 * @param frequency: Specify the frequency, in KHz.
 * @param bandwidth: Specify the bandwidth, in KHz.
 * @param pMaxGain:  The Maximum Gain/Attenuation can be set, in dB
 * @param pMinGain:  The Minumum Gain/Attenuation can be set, in dB 
 * @return:          ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_getGainRange (
	IN  IT9510INFO*    modulator,
	IN  Dword           frequency,
	IN  Word            bandwidth,    
	OUT int*			maxGain,
	OUT int*			minGain
);

/**
 * suspend Mode.
 *
 * @param modulator the handle of modulator.
 * @param enable 1:suspend mode/0: resume.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_suspendMode (
    IN  IT9510INFO*    modulator,
    IN  Byte          enable
);


/**
 * SET TPS settings
 *
 * @param modulator the handle of modulator.
 * @param TPS: Transmission Parameter Signalling 
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_setTPS (
    IN  IT9510INFO*    modulator,
    IN  TPS           tps,
	IN  Bool		  actualInfo
);

/**
 * GET current TPS settings
 *
 * @param modulator the handle of modulator.
 * @param pTPS: Transmission Parameter Signalling 
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_getTPS (
    IN  IT9510INFO*    modulator,
    IN  pTPS           pTps
);

/**
 * set IQ fixed table point
 *
 * @param modulator the handle of modulator.
 * @param IQ_table_ptr: IQ fixed table point
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_setIQtable (
	IN  IT9510INFO*    modulator,
    IN  CalibrationInfo calibrationInfo
);

Dword IT9510_setDCtable (
	IN  IT9510INFO*    modulator,
    IN  DCInfo dcInfo
);

Dword IT9510_setRFGaintable(
	IN  IT9510INFO*    modulator,
	IN  RFGainInfo rfGainInfo
	);

/**
 * set dc Value
 *
 * @param modulator the handle of modulator.
 * @param dc_i & dc_q:dc_i & dc_q value
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_setDCCalibrationValue (
	IN  IT9510INFO*	modulator,
    IN	int			dc_i,
	IN	int			dc_q
);

/**
 * get dc Value
 *
 * @param modulator the handle of modulator.
 * @param dc_i & dc_q: return dc_i & dc_q value
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 * 
 */
Dword IT9510_getDCCalibrationValue (
	IN  IT9510INFO*	modulator,
    IN	int*		dc_i,
	IN	int*		dc_q
);

Dword IT9510_setTMCCInfo (
    IN  IT9510INFO*    modulator,
    IN  TMCCINFO      TmccInfo,
	IN  Bool		  actualInfo
);
/*
Dword IT9510_setTMCCInfo2 (
    IN  IT9510INFO*    modulator,
    IN  TMCCINFO      TmccInfo
);*/

Dword IT9510_getTMCCInfo (
    IN  IT9510INFO*    modulator,
    IN  pTMCCINFO     pTmccInfo
);


Dword IT9510_getTSinputBitRate (
    IN  IT9510INFO*    modulator,
    IN  Word*     BitRate_Kbps
);



Dword IT9510_setPcrMode (
    IN  IT9510INFO*		modulator,
    IN  PcrMode			mode
);

Dword IT9510_setPcrModeEnable (
    IN  IT9510INFO*		modulator,
    IN  Byte			enable
);

Dword IT9510_setNullPacketMode (
    IN  IT9510INFO*		modulator,
    IN  NullPacketMode	mode
);
/**
 * check Ts Buffer Overflow
 *
 * @param modulator the handle of modulator.
 * @param overflow: return 1:overflow
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_isTsBufferOverflow (
	IN  IT9510INFO*	modulator,
    OUT	Bool		*overflow	
);

/**
 * get chip type
 *
 * @param modulator the handle of modulator.
 * @param chipType: 9503 or 9507 
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_getChipType (
	IN  IT9510INFO*	modulator,
    OUT	Word		*chipType	
);

/**
 * set OFS Value
 *
 * @param modulator the handle of modulator.
 * @param ofs_i & ofs_q:ofs_i & ofs_q value
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_setOFSCalibrationValue (
	IN  IT9510INFO*	modulator,
    IN	Byte		ofs_i,
	IN	Byte		ofs_q
);

/**
 * get OFS Value
 *
 * @param modulator the handle of modulator.
 * @param ofs_i & ofs_q: return ofs_i & ofs_q value
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_getOFSCalibrationValue (
	IN  IT9510INFO*	modulator,
    OUT	Byte*		ofs_i,
	OUT	Byte*		ofs_q
);

/**
 * enable AES function
 *
 * @param modulator the handle of modulator.
 * @param enable: 1: on  0:off 
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */

Dword IT9510_aesEncryptionEnable (
	IN  IT9510INFO*	modulator,
    IN	Bool		enable
	
);

/**
 * set AES key
 *
 * @param modulator the handle of modulator.
 * @param key: 128 bits AES key
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_setAesEncryptionKey (
	IN  IT9510INFO*	modulator,
    IN	Byte*		key
	
);


/**
 * set Encryption Start Address
 *
 * @param modulator the handle of modulator.
 * @param addr[5:0]: 188 packet Encryption start address (0~43)
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */

Dword IT9510_setEncryptionStartAddress (
	IN  IT9510INFO*	modulator,
    IN	Byte		addr
	
);

/**
 * set Sine Tone output
 *
 * @param modulator the handle of modulator.
 * @param on_off: 0: off 1:on
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_setSineTone (
	IN  IT9510INFO*	modulator, 
    IN	Bool		on_off
	
);

/**
 * enable Tps Encryption function
 *
 * @param modulator the handle of modulator.
 * @param key: Tps Encryption key
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_enableTpsEncryption (
	IN  IT9510INFO*	modulator,
    IN	Dword		key
	
);

/**
 * disable Tps Encryption function
 *
 * @param modulator the handle of modulator.
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_disableTpsEncryption (
	IN  IT9510INFO*	modulator
);


/**
 * write IT9560's EEPROM data
 *
 * @param modulator the handle of modulator.
 * @param slaveAddress: EEPROM Chip I2C Address(e.g :0xA8)
 * @param startAddress: register start address
 * @param buffer: data source
 * @param key: Tps Encryption key
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_writeIT9560EEPROM(
	IN  IT9510INFO*	modulator,
	IN  Byte        slaveAddress,
	IN  Word        startAddress,
	IN  unsigned char* buffer, 
	IN  int writeSize
);

/**
 * read IT9560's EEPROM data
 *
 * @param modulator the handle of modulator.
 * @param slaveAddress: EEPROM Chip I2C Address(e.g :0xA8)
 * @param startAddress: register start address
 * @param buffer: read data buffer
 * @param key: Tps Encryption key
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_readIT9560EEPROM(
	IN  IT9510INFO*	modulator,
	IN  Byte        slaveAddress,
	IN  Word        startAddress,
	OUT unsigned char* buffer, 
	IN  int readSize
);

/**
 * get IT9560 EEPROM Fw Version
 *
 * @param modulator the handle of modulator.
 * @param slaveAddress: EEPROM Chip I2C Address(e.g :0xA8)
 * @param version: IT9560 FW version in EEPROM(8 Bytes) 
 * @return: ERROR_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword IT9510_getIT9560FwVersion(
	IN  IT9510INFO*	modulator,
	IN  Byte        slaveAddress,
	OUT Byte *version
);





Dword IT9510_setPcrInfo(
	IN  IT9510INFO*	modulator,
	IN  PCRCALINFO	pcrCalInfo
);

#endif
