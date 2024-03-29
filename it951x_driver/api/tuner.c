#include "IT9510Type.h"
#if IT9510User_INTERNAL
#if IT9133Rx
/**
 * @(#)Afatech_OMEGA.cpp
 *
 * ==========================================================
 * Version: 2.0
 * Date:    2009.06.15
 * ==========================================================
 *
 * ==========================================================
 * History:
 *
 * Date         Author      Description
 * ----------------------------------------------------------
 *
 * 2009.06.15   M.-C. Ho    new tuner
 * ==========================================================
 *
 * Copyright 2009 Afatech, Inc. All rights reserved.
 *
 */


#include "error.h"
#include "user.h"
#include "register.h"
#define __OMAGADEMOD_H__
#include "standard.h"
#include "tuner.h"
#include "Omega.h"
#include "Firmware_Afa_Omega_Script.h"
#include "Firmware_Afa_Omega_LNA_Config_1_Script.h"
#include "Firmware_Afa_Omega_LNA_Config_2_Script.h"
#include "Firmware_Afa_Omega_Script_V2.h"
#include "Firmware_Afa_Omega_LNA_Config_1_Script_V2.h"
#include "Firmware_Afa_Omega_LNA_Config_2_Script_V2.h"
#include "Firmware_Afa_Omega_LNA_Config_3_Script_V2.h"
#include "Firmware_Afa_Omega_LNA_Config_4_Script_V2I.h"
#include "Firmware_Afa_Omega_LNA_Config_5_Script_V2I.h"
#include "Firmware_Afa_Omega_LNA_Config_3_Script_V2W.h"

Demodulator* Afatech_OMEGA_demodulator;

Dword OMEGA_open (
	IN  Demodulator*	demodulator
) {
	Dword error = Error_NO_ERROR;
	Afatech_OMEGA_demodulator = demodulator;
	error = omega_init();

	return (error);
}

Dword OMEGA_close (
	IN  Demodulator*	demodulator
) {
	Dword error = Error_NO_ERROR;
	if(demodulator!=NULL)
		error = Error_NO_ERROR;
	return (Error_NO_ERROR);
}

Dword OMEGA_set (
	IN  Demodulator*	demodulator,
	IN  Word			bandwidth,
	IN  Dword			frequency
) {
	Dword error = Error_NO_ERROR;
	Afatech_OMEGA_demodulator = demodulator;
	error = omega_setfreq((unsigned int)bandwidth, (unsigned int)frequency);
	
	return (error);
}


TunerDescription tunerDescription= {
    OMEGA_open,
    OMEGA_close,
    OMEGA_set,
    NULL,
    NULL,
    OMEGA_ADDRESS,			/** tuner i2c address */
    2,						/** length of tuner register address */
    0,						/** tuner if */
    False,					/** spectrum inverse */
    0x38,					/** tuner id */
};

Dword OMEGA_supportLNA (
    IN  Demodulator*    demodulator,
    IN  Byte            supporttype
 ) {
     Dword error = Error_INVALID_DEV_TYPE;
     Byte chip_version = 0;
	 Dword chip_Type;
	 Byte var[2];
	
	error = Standard_readRegister(demodulator, Processor_LINK, chip_version_7_0, &chip_version);
	if (error) goto exit;
	error = Standard_readRegisters(demodulator, Processor_LINK, chip_version_7_0+1, 2, var);
	if (error) goto exit;
	chip_Type = var[1]<<8 | var[0];	

    error = Error_INVALID_DEV_TYPE;
	if(chip_Type==0x9135 && chip_version == 2){	
		switch (supporttype){
			case OMEGA_NORMAL:			
				tunerDescription.tunerScriptTable = V2_OMEGA_scripts;
				tunerDescription.tunerScriptSetsTable = V2_OMEGA_scriptSets;
                tunerDescription.tunerId = 0x60;				
				error = Error_NO_ERROR;
				break;
			case OMEGA_LNA_Config_1:
				tunerDescription.tunerScriptTable = V2_OMEGA_LNA_Config_1_scripts;
				tunerDescription.tunerScriptSetsTable = V2_OMEGA_LNA_Config_1_scriptSets;
				tunerDescription.tunerId = 0x61;
				error = Error_NO_ERROR;
				break;
			case OMEGA_LNA_Config_2:
				tunerDescription.tunerScriptTable = V2_OMEGA_LNA_Config_2_scripts;
				tunerDescription.tunerScriptSetsTable = V2_OMEGA_LNA_Config_2_scriptSets;
				tunerDescription.tunerId = 0x62;
				error = Error_NO_ERROR;
				break;				
			case OMEGA_LNA_Config_3:
				tunerDescription.tunerScriptTable = V2W_OMEGA_LNA_Config_3_scripts;
				tunerDescription.tunerScriptSetsTable = V2W_OMEGA_LNA_Config_3_scriptSets;
				tunerDescription.tunerId = 0x63;
				error = Error_NO_ERROR;
				break;
			case OMEGA_LNA_Config_4:
				tunerDescription.tunerScriptTable = V2I_OMEGA_LNA_Config_4_scripts;
				tunerDescription.tunerScriptSetsTable = V2I_OMEGA_LNA_Config_4_scriptSets;
				tunerDescription.tunerId = 0x64;
				error = Error_NO_ERROR;
				break;
				
			case OMEGA_LNA_Config_5:
				tunerDescription.tunerScriptTable = V2I_OMEGA_LNA_Config_5_scripts;
				tunerDescription.tunerScriptSetsTable = V2I_OMEGA_LNA_Config_5_scriptSets;
				tunerDescription.tunerId = 0x65;
				error = Error_NO_ERROR;
				break;
			default:				
		        break;
	   }
	}else{
		switch (supporttype){
			case OMEGA_NORMAL:			
				tunerDescription.tunerScriptTable = OMEGA_scripts;
				tunerDescription.tunerScriptSetsTable = OMEGA_scriptSets;
				tunerDescription.tunerId = 0x38;
				error = Error_NO_ERROR;
				break;
			case OMEGA_LNA_Config_1:
				tunerDescription.tunerScriptTable = OMEGA_LNA_Config_1_scripts;
				tunerDescription.tunerScriptSetsTable = OMEGA_LNA_Config_1_scriptSets;
				tunerDescription.tunerId = 0x51;
				error = Error_NO_ERROR;
				break;
			case OMEGA_LNA_Config_2:
				tunerDescription.tunerScriptTable = OMEGA_LNA_Config_2_scripts;
				tunerDescription.tunerScriptSetsTable = OMEGA_LNA_Config_2_scriptSets;
				tunerDescription.tunerId = 0x52;
				error = Error_NO_ERROR;
				break;
			default:
				
				break;

		 }
	}
exit:
    return (error); 	
}
#else
/**
 * @(#)Afatech_SAMBA.cpp
 *
 * ==========================================================
 * Version: 2.0
 * Date:    2009.06.15
 * ==========================================================
 *
 * ==========================================================
 * History:
 *
 * Date         Author      Description
 * ----------------------------------------------------------
 *
 * 2009.06.15   M.-C. Ho    new tuner
 * ==========================================================
 *
 * Copyright 2009 Afatech, Inc. All rights reserved.
 *
 */


//#include <stdio.h>
//#include "type.h"
#include "error.h"
#include "user.h"
#include "register.h"
#define __SAMBADEMOD_H__
#include "standard.h"
#include "tuner.h"
#include "samba.h"
#include "Firmware_Afa_Samba_Script.h"
#include "Firmware_Afa_Samba_Config_1_Script.h"

Demodulator* Afatech_SAMBA_demodulator;

Dword SAMBA_open (
	IN  Demodulator*	demodulator
) {
	Dword error = Error_NO_ERROR;
	Afatech_SAMBA_demodulator = demodulator;
	error = samba_init();

	return (error);
}

Dword SAMBA_close (
	IN  Demodulator*	demodulator
) {
	Dword error = Error_NO_ERROR;
	if(demodulator!=NULL)
		error = Error_NO_ERROR;
	return (Error_NO_ERROR);
}

Dword SAMBA_set (
	IN  Demodulator*	demodulator,
	IN  Word			bandwidth,
	IN  Dword			frequency
) {
	Dword error = Error_NO_ERROR;
	Afatech_SAMBA_demodulator = demodulator;
	error = samba_setfreq((unsigned int)bandwidth, (unsigned int)frequency);
	
	return (error);
}


TunerDescription tunerDescription= {
    SAMBA_open,
    SAMBA_close,
    SAMBA_set,
    NULL,
    NULL,
    SAMBA_ADDRESS,			/** tuner i2c address */
    2,						/** length of tuner register address */
    0,						/** tuner if */
    False,					/** spectrum inverse */
    0x70,					/** tuner id */
};

Dword SAMBA_setConfig  (
    IN  Demodulator*    demodulator,
    IN  Byte            configtype
 ) {
    Dword error = Error_INVALID_DEV_TYPE;
	if(demodulator!=NULL) {	
		switch (configtype){
			case 0: //samba_NORMAL:			
				tunerDescription.tunerScriptTable = SAMBA_scripts;
				tunerDescription.tunerScriptSetsTable = SAMBA_scriptSets;
				tunerDescription.tunerId = 0x70;
				error = Error_NO_ERROR;
				break;
			case 1: //samba_LNA_Config_1:
				tunerDescription.tunerScriptTable = SAMBA_Config_1_scripts;
				tunerDescription.tunerScriptSetsTable = SAMBA_Config_1_scriptSets;
				tunerDescription.tunerId = 0x71;
				error = Error_NO_ERROR;
				break;
			
			default:
				
				break;
		}
	} else {
		error = Error_NULL_HANDLE_PTR;
	}

    return (error); 	
}



#endif
#endif
