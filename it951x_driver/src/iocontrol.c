/**
 * Copyright (c) 2006-2016 ITEtech Corporation. All rights reserved.
 *
 * Module Name:
 *     iocontrol.cpp
 *
 * Abstract:
    Demodulator and Modulator IOCTL Query and Set functions
 */


#include "it951x-core.h"

/*****************************************************************************
*
*  Function:   DemodIOCTLFun
*
*  Arguments:  handle             - The handle of demodulator or modulator.
*              IOCTLCode               - Device IO control code
*              pIOBuffer               - buffer containing data for the IOCTL
*
*  Returns:    Error_NO_ERROR: successful, non-zero error code otherwise.
*
*  Notes:
*
*****************************************************************************/
/* IRQL:DISPATCH_LEVEL */
DWORD DemodIOCTLFun(
    void *       handle,
    Dword        IOCTLCode,
    unsigned long       pIOBuffer
    )
{
    Dword error = Error_NO_ERROR;

    switch (IOCTLCode)
    {
		case IOCTL_ITE_MOD_ADJUSTOUTPUTGAIN: 
		{
			//Byte GainValue;
			TxSetGainRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxSetGainRequest)))
				return -EFAULT;

			if(((IT9510INFO*) handle)->isExtLo){		//	Use ADRF6755.
				if(Request.GainValue > -48 && Request.GainValue <= 0) {
					printk("ADRF6755 Request.GainValue: %d\n", (Byte) abs(Request.GainValue) );
					Request.error = IT9510User_adjustADRF6755Gain((IT9510INFO*) handle, (Byte) abs(Request.GainValue));
				} else {
					printk("ADRF6755 adjust gain out of range(%d)\n", Request.GainValue);
				}
			} else {
				printk("IT9510INFO Request.GainValue: %d\n", Request.GainValue);
				Request.error = IT9510_adjustOutputGain((IT9510INFO*) handle, &Request.GainValue);
			}
			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxSetGainRequest)))
				return -EFAULT;
				
/*			PTxSetGainRequest pRequest = (PTxSetGainRequest) pIOBuffer;

			if(((IT9510INFO*) handle)->isExtLo){		//	Use ADRF6755.
				if(pRequest->GainValue > -48 && pRequest->GainValue <= 0) {
					printk("ADRF6755 pRequest->GainValue: %d\n", (Byte) abs(pRequest->GainValue) );
					pRequest->error = IT9510User_adjustADRF6755Gain((IT9510INFO*) handle, (Byte) abs(pRequest->GainValue));
				} else {
					printk("ADRF6755 adjust gain out of range(%d)\n", pRequest->GainValue);
				}
			} else {
				printk("IT9510INFO pRequest->GainValue: %d\n", pRequest->GainValue);				
				pRequest->error = IT9510_adjustOutputGain((IT9510INFO*) handle, &pRequest->GainValue);
			}*/
			break;
		}
		case IOCTL_ITE_MOD_ENABLETXMODE:
		{
			TxModeRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxModeRequest)))
				return -EFAULT;

			Request.error = IT9510_setTxModeEnable((IT9510INFO*) handle, Request.OnOff);
			deb_data("IT951x TxMode RF %s\n", Request.OnOff?"ON":"OFF");	
			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxModeRequest)))
				return -EFAULT;
/*
			PTxModeRequest pRequest = (PTxModeRequest) pIOBuffer;
			pRequest->error = IT9510_setTxModeEnable((IT9510INFO*) handle, pRequest->OnOff);
			deb_data("IT951x TxMode RF %s\n", pRequest->OnOff?"ON":"OFF");	*/
			break;
		}
		case IOCTL_ITE_MOD_SETMODULE:
		{
			ChannelModulation temp;
			TxSetModuleRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxSetModuleRequest)))
				return -EFAULT;

			temp.constellation = Request.constellation;
			temp.highCodeRate = Request.highCodeRate;
			temp.interval = Request.interval;
			temp.transmissionMode = Request.transmissionMode;
			Request.error = IT9510_setTXChannelModulation((IT9510INFO*) handle, &temp);
			Request.error = IT9510_setTxModeEnable((IT9510INFO*) handle, 1);
			deb_data("IT951x TxMode RF ON\n");
			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxSetModuleRequest)))
				return -EFAULT;
			
/*			ChannelModulation temp;
			PTxSetModuleRequest pRequest = (PTxSetModuleRequest) pIOBuffer;
			temp.constellation = pRequest->constellation;
			temp.highCodeRate = pRequest->highCodeRate;
			temp.interval = pRequest->interval;
			temp.transmissionMode = pRequest->transmissionMode;
			pRequest->error = IT9510_setTXChannelModulation((IT9510INFO*) handle, &temp);
			pRequest->error = IT9510_setTxModeEnable((IT9510INFO*) handle, 1);
			deb_data("IT951x TxMode RF ON\n");*/
			break;
        }
        case IOCTL_ITE_DEMOD_ADDPIDAT:
        {
			AddPidAtRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(AddPidAtRequest)))
				return -EFAULT;
			
    		Request.error = Demodulator_addPidToFilter((Demodulator*) handle, Request.index, Request.pid.value);

			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(AddPidAtRequest)))
				return -EFAULT;

/*			//Byte temp0 = 0, temp1 = 1;
			//Dword status;	
			PAddPidAtRequest pRequest = (PAddPidAtRequest) pIOBuffer;
#if 0
			//clean 12 bytes garbage through 9517(TX).
			status = IT9510_writeRegisters ((IT9510INFO*) handle, Processor_OFDM, 0xF9A4, 1, &temp1);
			if(status)
			deb_data("DTV_WriteRegOFDM() return error!\n");

			status = IT9510_writeRegisters ((IT9510INFO*) handle, Processor_OFDM, 0xF9CC, 1, &temp0);
			if(status)
			deb_data("DTV_WriteRegOFDM() return error!\n");

			status = IT9510_writeRegisters ((IT9510INFO*) handle, Processor_OFDM, 0xF9A4, 1, &temp0);
			if(status)
			deb_data("DTV_WriteRegOFDM() return error!\n");

			status = IT9510_writeRegisters ((IT9510INFO*) handle, Processor_OFDM, 0xF9CC, 1, &temp1);
			if(status)
			deb_data("DTV_WriteRegOFDM() return error!\n");			
#endif			
    		pRequest->error = Demodulator_addPidToFilter((Demodulator*) handle, pRequest->index, pRequest->pid.value);*/
    		break;
        }
        case IOCTL_ITE_MOD_ADDPIDAT:
        {
			TxAddPidAtRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxAddPidAtRequest)))
				return -EFAULT;

			Request.error = IT9510_addPidToFilter((IT9510INFO*) handle, Request.index, Request.pid);
						
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxAddPidAtRequest)))
				return -EFAULT;
/*
			PTxAddPidAtRequest pRequest = (PTxAddPidAtRequest) pIOBuffer;
			pRequest->error = IT9510_addPidToFilter((IT9510INFO*) handle, pRequest->index, pRequest->pid);*/
    		break;
        }
		case IOCTL_ITE_MOD_ACQUIRECHANNEL:
		{
			TxAcquireChannelRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxAcquireChannelRequest)))
				return -EFAULT;
	
			Request.error = IT9510_acquireTxChannel((IT9510INFO*) handle, Request.bandwidth, Request.frequency);

			if(((IT9510INFO*) handle)->deviceType == 0x9C){
	//			printk("IT9510User_RFFC2072AcquireChannel____1\n");
	//				IT9510User_RFFC2072AcquireChannel( &((IT9510INFO*) handle)->RFFC2072, Request.frequency);
	//				IT9510User_RFFC2072DeviceEnable( &((IT9510INFO*) handle)->RFFC2072, True);
			}

			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxAcquireChannelRequest)))
				return -EFAULT;

/*			PTxAcquireChannelRequest pRequest = (PTxAcquireChannelRequest) pIOBuffer;
			pRequest->error = IT9510_acquireTxChannel((IT9510INFO*) handle, pRequest->bandwidth, pRequest->frequency);*/
			break;
        }
        case IOCTL_ITE_DEMOD_RESETPID:
        {
			ResetPidRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(ResetPidRequest)))
				return -EFAULT;
			
            Request.error = Demodulator_resetPidFilter((Demodulator*) handle);
			            
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(ResetPidRequest)))
				return -EFAULT;
/*
            PResetPidRequest pRequest = (PResetPidRequest) pIOBuffer;
            pRequest->error = Demodulator_resetPidFilter((Demodulator*) handle);*/
    		break;
        }
        case IOCTL_ITE_MOD_RESETPID:
        {
			TxResetPidRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxResetPidRequest)))
				return -EFAULT;
			
            Request.error = IT9510_resetPidFilter((IT9510INFO*) handle);
			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxResetPidRequest)))
				return -EFAULT;
/*		
            PTxResetPidRequest pRequest = (PTxResetPidRequest) pIOBuffer;
            pRequest->error = IT9510_resetPidFilter((IT9510INFO*) handle);*/
    		break;
        }
        case IOCTL_ITE_DEMOD_ACQUIRECHANNEL:
        {
			AcquireChannelRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(AcquireChannelRequest)))
				return -EFAULT;
			
			Request.error = Demodulator_acquireChannel((Demodulator*) handle, Request.bandwidth, Request.frequency);
						            
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(AcquireChannelRequest)))
				return -EFAULT;
/*				
			PAcquireChannelRequest pRequest = (PAcquireChannelRequest) pIOBuffer;
			pRequest->error = Demodulator_acquireChannel ((Demodulator*) handle, pRequest->bandwidth, pRequest->frequency);*/
    		break;
        }
        case IOCTL_ITE_DEMOD_ISLOCKED:
        {
			IsLockedRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(IsLockedRequest)))
				return -EFAULT;
			
			Request.error = Demodulator_isLocked((Demodulator*) handle, &Request.locked);
									            
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(IsLockedRequest)))
				return -EFAULT;
/*
			PIsLockedRequest pRequest = (PIsLockedRequest) pIOBuffer;
			pRequest->error = Demodulator_isLocked ((Demodulator*) handle, &pRequest->locked);*/
    		break;
        }
        case IOCTL_ITE_DEMOD_GETCHANNELSTATISTIC:
        {
			GetChannelStatisticRequest Request;
			ChannelStatistic* channelStatistic;
			Dword postErrCnt;
			Dword postBitCnt;
			Word rsdAbortCnt;
			
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(GetChannelStatisticRequest)))
				return -EFAULT;
			
			channelStatistic = (ChannelStatistic*) Request.channelStatisticAddr;
			channelStatistic->postVitErrorCount = 0;
			channelStatistic->postVitBitCount = 0;
			channelStatistic->abortCount = 0;
			Request.error = Demodulator_getPostVitBer((Demodulator*) handle, &postErrCnt, &postBitCnt, &rsdAbortCnt);
			if (Request.error == Error_NO_ERROR) {
				channelStatistic->postVitErrorCount = postErrCnt;
				channelStatistic->postVitBitCount = postBitCnt;
				channelStatistic->abortCount = rsdAbortCnt;
			}
			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(GetChannelStatisticRequest)))
				return -EFAULT;
				
/*
			PGetChannelStatisticRequest pRequest = (PGetChannelStatisticRequest) pIOBuffer;
			ChannelStatistic* channelStatistic = (ChannelStatistic*) pRequest->channelStatisticAddr;
			Dword postErrCnt;
			Dword postBitCnt;
			Word rsdAbortCnt;

			channelStatistic->postVitErrorCount = 0;
			channelStatistic->postVitBitCount = 0;
			channelStatistic->abortCount = 0;
			pRequest->error = Demodulator_getPostVitBer ((Demodulator*) handle, &postErrCnt, &postBitCnt, &rsdAbortCnt);
			if (pRequest->error == Error_NO_ERROR) {
				channelStatistic->postVitErrorCount = postErrCnt;
				channelStatistic->postVitBitCount = postBitCnt;
				channelStatistic->abortCount = rsdAbortCnt;
			}*/
    		break;
        }
        case IOCTL_ITE_DEMOD_GETSTATISTIC:
        {
			GetStatisticRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(GetStatisticRequest)))
				return -EFAULT;

			Request.error = Demodulator_getStatistic((Demodulator*) handle, &Request.statistic);
			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(GetStatisticRequest)))
				return -EFAULT;
/*
			PGetStatisticRequest pRequest = (PGetStatisticRequest) pIOBuffer;
			pRequest->error = Demodulator_getStatistic ((Demodulator*) handle, &pRequest->statistic);*/
    		break;
        }
        case IOCTL_ITE_MOD_CONTROLPIDFILTER:
        {
			TxControlPidFilterRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxControlPidFilterRequest)))
				return -EFAULT;

			Request.error = IT9510_controlPidFilter((IT9510INFO*) handle, Request.control, Request.enable);
						
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxControlPidFilterRequest)))
				return -EFAULT;
/*
			PTxControlPidFilterRequest pRequest = (PTxControlPidFilterRequest) pIOBuffer;
			pRequest->error = IT9510_controlPidFilter ((IT9510INFO*) handle, pRequest->control, pRequest->enable);*/
    		break;
        }
        case IOCTL_ITE_DEMOD_CONTROLPIDFILTER:
        {
			ControlPidFilterRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(ControlPidFilterRequest)))
				return -EFAULT;

			Request.error = Demodulator_controlPidFilter((Demodulator*) handle, Request.control);
			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(ControlPidFilterRequest)))
				return -EFAULT;
/*
			PControlPidFilterRequest pRequest = (PControlPidFilterRequest) pIOBuffer;
			pRequest->error = Demodulator_controlPidFilter ((Demodulator*) handle, pRequest->control);*/
    		break;
        }
        case IOCTL_ITE_DEMOD_CONTROLPOWERSAVING:
        {
			ControlPowerSavingRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(ControlPowerSavingRequest)))
				return -EFAULT;

			Request.error = Demodulator_controlPowerSaving((Demodulator*) handle, Request.control);
			Request.error = IT9510_writeRegisterBits((IT9510INFO*) handle, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, !Request.control);
						
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(ControlPowerSavingRequest)))
				return -EFAULT;
/*	
			PControlPowerSavingRequest pRequest = (PControlPowerSavingRequest) pIOBuffer;
			pRequest->error = Demodulator_controlPowerSaving ((Demodulator*) handle, pRequest->control);
			pRequest->error = IT9510_writeRegisterBits((IT9510INFO*) handle, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, !pRequest->control);*/
    		break;
        }
        case IOCTL_ITE_MOD_CONTROLPOWERSAVING:
        {
			TxControlPowerSavingRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxControlPowerSavingRequest)))
				return -EFAULT;

			Request.error = IT9510_setTxModeEnable((IT9510INFO*) handle, Request.control);
			Request.error = IT9510_controlPowerSaving((IT9510INFO*) handle, Request.control);
			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxControlPowerSavingRequest)))
				return -EFAULT;
/*
			PTxControlPowerSavingRequest pRequest = (PTxControlPowerSavingRequest) pIOBuffer;
			pRequest->error = IT9510_setTxModeEnable((IT9510INFO*) handle, pRequest->control);
			pRequest->error = IT9510_controlPowerSaving ((IT9510INFO*) handle, pRequest->control);*/
    		break;
        }
        case IOCTL_ITE_DEMOD_GETDRIVERINFO:
        {
			DemodDriverInfo Request;
			DWORD dwFWVerionLink = 0;
			DWORD dwFWVerionOFDM = 0;
			
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(DemodDriverInfo)))
				return -EFAULT;
				
            strcpy((char *)Request.DriverVerion, DRIVER_RELEASE_VERSION);
			sprintf((char *)Request.APIVerion, "%X.%X.%X.%X", (BYTE)(Version_NUMBER>>8), (BYTE)(Version_NUMBER), Version_DATE, Version_BUILD);
			Request.error = Demodulator_getFirmwareVersion ((Demodulator*) handle, Processor_LINK, &dwFWVerionLink);
			sprintf((char *)Request.FWVerionLink, "%d.%d.%d.%d", (BYTE)(dwFWVerionLink>>24), (BYTE)(dwFWVerionLink>>16), (BYTE)(dwFWVerionLink>>8), (BYTE)dwFWVerionLink);
			deb_data("Demodulator_getFirmwareVersion Processor_LINK %s\n", (char *)Request.FWVerionLink);
			
			Request.error = Demodulator_getFirmwareVersion ((Demodulator*) handle, Processor_OFDM, &dwFWVerionOFDM);
			sprintf((char *)Request.FWVerionOFDM, "%d.%d.%d.%d", (BYTE)(dwFWVerionOFDM>>24), (BYTE)(dwFWVerionOFDM>>16), (BYTE)(dwFWVerionOFDM>>8), (BYTE)dwFWVerionOFDM);
			deb_data("Demodulator_getFirmwareVersion Processor_OFDM %s\n", (char *)Request.FWVerionOFDM);
			//strcpy((char *)Request.DateTime, ""__DATE__" "__TIME__"");
			strcpy((char *)Request.Company, "ITEtech");
#if IT9133Rx
			strcpy((char *)Request.SupportHWInfo, "OmegaII DVBT");
#else
			strcpy((char *)Request.SupportHWInfo, "SAMBA ISDB-T");
#endif			
			Request.error = Error_NO_ERROR;
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(DemodDriverInfo)))
				return -EFAULT;
/*
			PDemodDriverInfo pDriverInfo = (PDemodDriverInfo)pIOBuffer;
			DWORD dwFWVerionLink = 0;
			DWORD dwFWVerionOFDM = 0;
			
            strcpy((char *)pDriverInfo->DriverVerion, DRIVER_RELEASE_VERSION);
			sprintf((char *)pDriverInfo->APIVerion, "%X.%X.%X.%X", (BYTE)(Version_NUMBER>>8), (BYTE)(Version_NUMBER), Version_DATE, Version_BUILD);
			pDriverInfo->error = Demodulator_getFirmwareVersion ((Demodulator*) handle, Processor_LINK, &dwFWVerionLink);
			sprintf((char *)pDriverInfo->FWVerionLink, "%d.%d.%d.%d", (BYTE)(dwFWVerionLink>>24), (BYTE)(dwFWVerionLink>>16), (BYTE)(dwFWVerionLink>>8), (BYTE)dwFWVerionLink);
			deb_data("Demodulator_getFirmwareVersion Processor_LINK %s\n", (char *)pDriverInfo->FWVerionLink);
			
			pDriverInfo->error = Demodulator_getFirmwareVersion ((Demodulator*) handle, Processor_OFDM, &dwFWVerionOFDM);
			sprintf((char *)pDriverInfo->FWVerionOFDM, "%d.%d.%d.%d", (BYTE)(dwFWVerionOFDM>>24), (BYTE)(dwFWVerionOFDM>>16), (BYTE)(dwFWVerionOFDM>>8), (BYTE)dwFWVerionOFDM);
			deb_data("Demodulator_getFirmwareVersion Processor_OFDM %s\n", (char *)pDriverInfo->FWVerionOFDM);
			//strcpy((char *)pDriverInfo->DateTime, ""__DATE__" "__TIME__"");
			strcpy((char *)pDriverInfo->Company, "ITEtech");
#if IT9133Rx
			strcpy((char *)pDriverInfo->SupportHWInfo, "OmegaII DVBT");
#else
			strcpy((char *)pDriverInfo->SupportHWInfo, "SAMBA ISDB-T");
#endif			
			pDriverInfo->error = Error_NO_ERROR;
*/
    		break;
        }
        case IOCTL_ITE_MOD_GETDRIVERINFO:
        {
			TxModDriverInfo Request;
			DWORD dwFWVerionLink = 0;
			DWORD dwFWVerionOFDM = 0;

			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxModDriverInfo)))
				return -EFAULT;

            strcpy((char *)Request.DriverVerion, DRIVER_RELEASE_VERSION);
			sprintf((char *)Request.APIVerion, "%X.%X.%X.%X", (BYTE)(IT9510_Version_NUMBER>>8), (BYTE)(IT9510_Version_NUMBER), IT9510_Version_DATE, IT9510_Version_BUILD);

			Request.error = IT9510_getFirmwareVersion ((IT9510INFO*) handle, Processor_LINK, &dwFWVerionLink);
			sprintf((char *)Request.FWVerionLink, "%d.%d.%d.%d", (BYTE)(dwFWVerionLink>>24), (BYTE)(dwFWVerionLink>>16), (BYTE)(dwFWVerionLink>>8), (BYTE)dwFWVerionLink);
			deb_data("Modulator_getFirmwareVersion Processor_LINK %s\n", (char *)Request.FWVerionLink);
 
			Request.error = IT9510_getFirmwareVersion ((IT9510INFO*) handle, Processor_OFDM, &dwFWVerionOFDM);
			sprintf((char *)Request.FWVerionOFDM, "%d.%d.%d.%d", (BYTE)(dwFWVerionOFDM>>24), (BYTE)(dwFWVerionOFDM>>16), (BYTE)(dwFWVerionOFDM>>8), (BYTE)dwFWVerionOFDM);
			deb_data("Modulator_getFirmwareVersion Processor_OFDM %s\n", (char *)Request.FWVerionOFDM);

			strcpy((char *)Request.Company, "ITEtech");
			strcpy((char *)Request.SupportHWInfo, "EagleII DVBT & ISDB-T");

			Request.error = Error_NO_ERROR;
			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxModDriverInfo)))
				return -EFAULT;
				
/*
			PTxModDriverInfo pDriverInfo = (PTxModDriverInfo)pIOBuffer;
			DWORD dwFWVerionLink = 0;
			DWORD dwFWVerionOFDM = 0;
			

            strcpy((char *)pDriverInfo->DriverVerion, DRIVER_RELEASE_VERSION);
			sprintf((char *)pDriverInfo->APIVerion, "%X.%X.%X.%X", (BYTE)(IT9510_Version_NUMBER>>8), (BYTE)(IT9510_Version_NUMBER), IT9510_Version_DATE, IT9510_Version_BUILD);

			pDriverInfo->error = IT9510_getFirmwareVersion ((IT9510INFO*) handle, Processor_LINK, &dwFWVerionLink);
			sprintf((char *)pDriverInfo->FWVerionLink, "%d.%d.%d.%d", (BYTE)(dwFWVerionLink>>24), (BYTE)(dwFWVerionLink>>16), (BYTE)(dwFWVerionLink>>8), (BYTE)dwFWVerionLink);
			deb_data("Modulator_getFirmwareVersion Processor_LINK %s\n", (char *)pDriverInfo->FWVerionLink);
 
			pDriverInfo->error = IT9510_getFirmwareVersion ((IT9510INFO*) handle, Processor_OFDM, &dwFWVerionOFDM);
			sprintf((char *)pDriverInfo->FWVerionOFDM, "%d.%d.%d.%d", (BYTE)(dwFWVerionOFDM>>24), (BYTE)(dwFWVerionOFDM>>16), (BYTE)(dwFWVerionOFDM>>8), (BYTE)dwFWVerionOFDM);
			deb_data("Modulator_getFirmwareVersion Processor_OFDM %s\n", (char *)pDriverInfo->FWVerionOFDM);

			//strcpy((char *)pDriverInfo->DateTime, ""__DATE__" "__TIME__"");
			strcpy((char *)pDriverInfo->Company, "ITEtech");
			strcpy((char *)pDriverInfo->SupportHWInfo, "EagleII DVBT & ISDB-T");

			pDriverInfo->error = Error_NO_ERROR;
*/
    		break;
        }
        case IOCTL_ITE_MOD_WRITEREGISTERS:
        {
			TxWriteRegistersRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxWriteRegistersRequest)))
				return -EFAULT;

            Request.error = IT9510_writeRegisters((IT9510INFO*) handle, Request.processor, Request.registerAddress, Request.bufferLength, Request.buffer);
			           			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxWriteRegistersRequest)))
				return -EFAULT;
/*
			PTxWriteRegistersRequest pRequest = (PTxWriteRegistersRequest) pIOBuffer;
            pRequest->error = IT9510_writeRegisters ((IT9510INFO*) handle, pRequest->processor, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);*/
    		break;
        }
        case IOCTL_ITE_DEMOD_WRITEREGISTERS:
        {
			WriteRegistersRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(WriteRegistersRequest)))
				return -EFAULT;

            Request.error = Demodulator_writeRegisters((Demodulator*) handle, Request.processor, Request.registerAddress, Request.bufferLength, Request.buffer);
			                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(WriteRegistersRequest)))
				return -EFAULT;
/*
			PWriteRegistersRequest pRequest = (PWriteRegistersRequest) pIOBuffer;
            pRequest->error = Demodulator_writeRegisters ((Demodulator*) handle, pRequest->processor, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);*/
    		break;
        }
        case IOCTL_ITE_MOD_WRITEEEPROMVALUES:
        {
			TxWriteEepromValuesRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxWriteEepromValuesRequest)))
				return -EFAULT;

			Request.error = IT9510_writeEepromValues((IT9510INFO*) handle, Request.registerAddress, Request.bufferLength, Request.buffer);
						                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxWriteEepromValuesRequest)))
				return -EFAULT;
/*
			PTxWriteEepromValuesRequest pRequest = (PTxWriteEepromValuesRequest) pIOBuffer;
			pRequest->error = IT9510_writeEepromValues ((IT9510INFO*) handle, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);*/
    		break;
        }
		case IOCTL_ITE_DEMOD_SETDECRYPT:
		{
			TxSetDecryptRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxSetDecryptRequest)))
				return -EFAULT;

#if IT9133Rx
			Request.error = Demodulator_setDecrypt ((Demodulator*) handle, (Dword) Request.decryptKey,  Request.decryptEnable);
#else
			// No Defined. TODO: Support Decryption for ISDBT RX.
			printk("Not support Decryption for ISDBT RX\n");
			Request.error = Error_TPS_DECRYPTION_UNSUPPORTED;
#endif
		                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxSetDecryptRequest)))
				return -EFAULT;

/*
			PTxSetDecryptRequest pRequest = (PTxSetDecryptRequest) pIOBuffer;
#if IT9133Rx
			pRequest->error = Demodulator_setDecrypt ((Demodulator*) handle, (Dword) pRequest->decryptKey,  pRequest->decryptEnable);
#else
			// No Defined. TODO: Support Decryption for ISDBT RX.
			printk("Not support Decryption for ISDBT RX\n");
			pRequest->error = Error_TPS_DECRYPTION_UNSUPPORTED;
#endif*/
			break;
		}
        case IOCTL_ITE_DEMOD_READREGISTERS:
        {
			ReadRegistersRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(ReadRegistersRequest)))
				return -EFAULT;

			Request.error = Demodulator_readRegisters((Demodulator*) handle, Request.processor, Request.registerAddress, Request.bufferLength, Request.buffer);
			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(ReadRegistersRequest)))
				return -EFAULT;
/*
			PReadRegistersRequest pRequest = (PReadRegistersRequest) pIOBuffer;
			pRequest->error = Demodulator_readRegisters ((Demodulator*) handle, pRequest->processor, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);*/
    		break;
        }
        case IOCTL_ITE_MOD_READREGISTERS:
        {
			TxReadRegistersRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxReadRegistersRequest)))
				return -EFAULT;

			Request.error = IT9510_readRegisters((IT9510INFO*) handle, Request.processor, Request.registerAddress, Request.bufferLength, Request.buffer);
															                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxReadRegistersRequest)))
				return -EFAULT;
/*
			PTxReadRegistersRequest pRequest = (PTxReadRegistersRequest) pIOBuffer;
			pRequest->error = IT9510_readRegisters ((IT9510INFO*) handle, pRequest->processor, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);*/
    		break;
        }
        case IOCTL_ITE_MOD_READEEPROMVALUES:
        {
			TxReadEepromValuesRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxReadEepromValuesRequest)))
				return -EFAULT;

			Request.error = IT9510_readEepromValues((IT9510INFO*) handle, Request.registerAddress, Request.bufferLength, Request.buffer);
																		                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxReadEepromValuesRequest)))
				return -EFAULT;
/*
			PTxReadEepromValuesRequest pRequest = (PTxReadEepromValuesRequest) pIOBuffer;
			pRequest->error = IT9510_readEepromValues ((IT9510INFO*) handle, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);*/
    		break;
        }
        case IOCTL_ITE_MOD_GETGAINRANGE:
        {
			TxGetGainRangeRequest Request;

			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxGetGainRangeRequest)))
				return -EFAULT;
				
			if(((IT9510INFO*) handle)->isExtLo) {
				Request.maxGain = 0;
				Request.minGain	= -47;
				Request.error = Error_NO_ERROR;
			} else {
				Request.error = IT9510_getGainRange ((IT9510INFO*) handle, (Dword) Request.frequency, (Word) Request.bandwidth, &Request.maxGain, &Request.minGain);
			}
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxGetGainRangeRequest)))
				return -EFAULT;
/*
			PTxGetGainRangeRequest pRequest = (PTxGetGainRangeRequest) pIOBuffer;		
			
			if(((IT9510INFO*) handle)->isExtLo) {
				pRequest->maxGain = 0;
				pRequest->minGain	= -47;
				pRequest->error = Error_NO_ERROR;
			} else {
				pRequest->error = IT9510_getGainRange ((IT9510INFO*) handle, (Dword) pRequest->frequency, (Word) pRequest->bandwidth, &pRequest->maxGain, &pRequest->minGain);
			}*/				

			break;
		}
        case IOCTL_ITE_MOD_GETTPS:
        {
			TxGetTPSRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxGetTPSRequest)))
				return -EFAULT;

			Request.error = IT9510_getTPS((IT9510INFO*) handle, &Request.tps);
																					                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxGetTPSRequest)))
				return -EFAULT;
/*
			PTxGetTPSRequest pRequest = (PTxGetTPSRequest) pIOBuffer;		
			pRequest->error = IT9510_getTPS ((IT9510INFO*) handle, &pRequest->tps);*/
			break;
		}
        case IOCTL_ITE_MOD_SETTPS:
        {
			TxSetTPSRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxSetTPSRequest)))
				return -EFAULT;

			Request.error = IT9510_setTPS((IT9510INFO*) handle, Request.tps, Request.actualInfo);
																								                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxSetTPSRequest)))
				return -EFAULT;
/*
			PTxSetTPSRequest pRequest = (PTxSetTPSRequest) pIOBuffer;		
			pRequest->error = IT9510_setTPS ((IT9510INFO*) handle, pRequest->tps, pRequest->actualInfo);*/
			break;
		}
        case IOCTL_ITE_MOD_GETOUTPUTGAIN:
        {
			TxGetOutputGainRequest Request;
			Byte tmp_gain;
			
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxGetOutputGainRequest)))
				return -EFAULT;

			if(((IT9510INFO*) handle)->isExtLo){
				Request.error = IT9510User_getADRF6755Gain((IT9510INFO*) handle, &tmp_gain);
				Request.gain = 0-tmp_gain;
			} else {
				Request.error = IT9510_getOutputGain ((IT9510INFO*) handle, &Request.gain);
			}
			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxGetOutputGainRequest)))
				return -EFAULT;
/*
			PTxGetOutputGainRequest pRequest = (PTxGetOutputGainRequest) pIOBuffer;		
			Byte tmp_gain;
			if(((IT9510INFO*) handle)->isExtLo){
				pRequest->error = IT9510User_getADRF6755Gain((IT9510INFO*) handle, &tmp_gain);
				pRequest->gain = 0-tmp_gain;
			} else {
				pRequest->error = IT9510_getOutputGain ((IT9510INFO*) handle, &pRequest->gain);
			}*/
			break;
		}
        case IOCTL_ITE_MOD_SENDHWPSITABLE:
        {
			TxSendHwPSITableRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxSendHwPSITableRequest)))
				return -EFAULT;

			Request.error = IT9510_sendHwPSITable((IT9510INFO*) handle, (Byte*) Request.pbufferAddr);
																														                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxSendHwPSITableRequest)))
				return -EFAULT;
/*
			PTxSendHwPSITableRequest pRequest = (PTxSendHwPSITableRequest) pIOBuffer;	
			pRequest->error = IT9510_sendHwPSITable ((IT9510INFO*) handle, (Byte*) pRequest->pbufferAddr);*/
			break;
		}
        case IOCTL_ITE_MOD_ACCESSFWPSITABLE:
        {
			TxAccessFwPSITableRequest Request;
			Byte* pbufferAddrUser;
			Byte pbufferAddrKernel[188];
			
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxAccessFwPSITableRequest)))
				return -EFAULT;

			pbufferAddrUser = (Byte*) Request.pbufferAddr;
			copy_from_user(pbufferAddrKernel, pbufferAddrUser, 188);					
			Request.error = IT9510_accessFwPSITable ((IT9510INFO*) handle, Request.psiTableIndex, (Byte*) pbufferAddrKernel);

			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxAccessFwPSITableRequest)))
				return -EFAULT;

/*
			PTxAccessFwPSITableRequest pRequest = (PTxAccessFwPSITableRequest) pIOBuffer;
			Byte* pbufferAddrUser = (Byte*) pRequest->pbufferAddr;
			Byte pbufferAddrKernel[188];

			copy_from_user(pbufferAddrKernel, pbufferAddrUser, 188);					
			pRequest->error = IT9510_accessFwPSITable ((IT9510INFO*) handle, pRequest->psiTableIndex, (Byte*) pbufferAddrKernel);*/
			break;
		}		
        case IOCTL_ITE_MOD_SETFWPSITABLETIMER:
        {
			TxSetFwPSITableTimerRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxSetFwPSITableTimerRequest)))
				return -EFAULT;

			Request.error = IT9510_setFwPSITableTimer((IT9510INFO*) handle, Request.psiTableIndex, Request.timer);
																																	                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxSetFwPSITableTimerRequest)))
				return -EFAULT;
/*
			PTxSetFwPSITableTimerRequest pRequest = (PTxSetFwPSITableTimerRequest) pIOBuffer;	
			pRequest->error = IT9510_setFwPSITableTimer ((IT9510INFO*) handle, pRequest->psiTableIndex, pRequest->timer);*/
			break;
		}
		case IOCTL_ITE_MOD_SETDCCALIBRATIONVALUE:
		{
			TxSetDCCalibrationValueRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxSetDCCalibrationValueRequest)))
				return -EFAULT;

			Request.error = IT9510_setDCCalibrationValue((IT9510INFO*) handle, Request.dc_i, Request.dc_q);
			deb_data("Set DC Calibration dc_i %d, dc_q %d\n", Request.dc_i, Request.dc_q);
																														                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxSetDCCalibrationValueRequest)))
				return -EFAULT;
/*
			PTxSetDCCalibrationValueRequest pRequest = (PTxSetDCCalibrationValueRequest) pIOBuffer;		
			pRequest->error = IT9510_setDCCalibrationValue ((IT9510INFO*) handle, pRequest->dc_i, pRequest->dc_q);
			deb_data("Set DC Calibration dc_i %d, dc_q %d\n", pRequest->dc_i, pRequest->dc_q);*/
			break;
		}
		case IOCTL_ITE_MOD_GETCHIPTYPE:
		{
			TxGetChipTypeRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxGetChipTypeRequest)))
				return -EFAULT;

			Request.error = IT9510_getChipType((IT9510INFO*) handle, &Request.chipType);

			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxGetChipTypeRequest)))
				return -EFAULT;
/*
			PTxGetChipTypeRequest pRequest = (PTxGetChipTypeRequest) pIOBuffer;		
			pRequest->error = IT9510_getChipType ((IT9510INFO*) handle, &pRequest->chipType);*/
			break;
		}		
        case IOCTL_ITE_MOD_SETISDBTCHANNELMODULATION:
        {
			TXSetISDBTChannelModulationRequest Request;
			ISDBTModulation* isdbtModulationUser;
			ISDBTModulation isdbtModulationKernel;
			
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TXSetISDBTChannelModulationRequest)))
				return -EFAULT;

			isdbtModulationUser = (ISDBTModulation*) Request.isdbtModulationAddr;
			copy_from_user(&isdbtModulationKernel, isdbtModulationUser, sizeof(ISDBTModulation));
			Request.error = IT9510_setISDBTChannelModulation ((IT9510INFO*) handle, isdbtModulationKernel);
			Request.error = IT9510_setTxModeEnable((IT9510INFO*) handle, 1);
			deb_data("IT951x TxMode RF ON\n");
																																	                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TXSetISDBTChannelModulationRequest)))
				return -EFAULT;
/*
			PTXSetISDBTChannelModulationRequest pRequest = (PTXSetISDBTChannelModulationRequest) pIOBuffer;		
			ISDBTModulation* isdbtModulationUser = (ISDBTModulation*) pRequest->isdbtModulationAddr;
			ISDBTModulation isdbtModulationKernel;

			copy_from_user(&isdbtModulationKernel, isdbtModulationUser, sizeof(ISDBTModulation));
			pRequest->error = IT9510_setISDBTChannelModulation ((IT9510INFO*) handle, isdbtModulationKernel);
			pRequest->error = IT9510_setTxModeEnable((IT9510INFO*) handle, 1);
			deb_data("IT951x TxMode RF ON\n");*/
			break;
		}		
        case IOCTL_ITE_MOD_SETTMCCINFO:
        {
			TXSetTMCCInfoRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TXSetTMCCInfoRequest)))
				return -EFAULT;

			Request.error = IT9510_setTMCCInfo((IT9510INFO*) handle, Request.TmccInfo, Request.actualInfo);
																																	                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TXSetTMCCInfoRequest)))
				return -EFAULT;
/*
			PTXSetTMCCInfoRequest pRequest = (PTXSetTMCCInfoRequest) pIOBuffer;		
			pRequest->error = IT9510_setTMCCInfo ((IT9510INFO*) handle, pRequest->TmccInfo, pRequest->actualInfo);*/
			break;
		}		
        case IOCTL_ITE_MOD_GETTMCCINFO:
        {
			TXGetTMCCInfoRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TXGetTMCCInfoRequest)))
				return -EFAULT;

			Request.error = IT9510_getTMCCInfo((IT9510INFO*) handle, &Request.TmccInfo);
																																	                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TXGetTMCCInfoRequest)))
				return -EFAULT;
/*
			PTXGetTMCCInfoRequest pRequest = (PTXGetTMCCInfoRequest) pIOBuffer;		
			pRequest->error = IT9510_getTMCCInfo ((IT9510INFO*) handle, &pRequest->TmccInfo);*/
			break;
		}		
        case IOCTL_ITE_MOD_GETTSINPUTBITRATE:
        {
			TXGetTSinputBitRateRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TXGetTSinputBitRateRequest)))
				return -EFAULT;

			Request.error = IT9510_getTSinputBitRate((IT9510INFO*) handle, &Request.BitRate_Kbps);
																																				                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TXGetTSinputBitRateRequest)))
				return -EFAULT;
/*
			PTXGetTSinputBitRateRequest pRequest = (PTXGetTSinputBitRateRequest) pIOBuffer;		
			pRequest->error = IT9510_getTSinputBitRate ((IT9510INFO*) handle, &pRequest->BitRate_Kbps);*/
			break;
		}
        case IOCTL_ITE_MOD_ADDPIDTOISDBTPIDFILTER:
        {
			TXAddPidToISDBTPidFilterRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TXAddPidToISDBTPidFilterRequest)))
				return -EFAULT;

			Request.error = IT9510_addPidToISDBTPidFilter((IT9510INFO*) handle, Request.index, Request.pid, Request.layer);
																																							                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TXAddPidToISDBTPidFilterRequest)))
				return -EFAULT;
/*
			PTXAddPidToISDBTPidFilterRequest pRequest = (PTXAddPidToISDBTPidFilterRequest) pIOBuffer;		
			pRequest->error = IT9510_addPidToISDBTPidFilter ((IT9510INFO*) handle, pRequest->index, pRequest->pid, pRequest->layer);*/
			break;
		}
		case IOCTL_ITE_MOD_SETPCRMODE:
		{
			TxSetPcrModeRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxSetPcrModeRequest)))
				return -EFAULT;

			Request.error = IT9510_setPcrMode((IT9510INFO*) handle, Request.mode);
			deb_data("Enable PCR Mode %d\n", Request.mode);
																																										                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxSetPcrModeRequest)))
				return -EFAULT;
/*
			PTxSetPcrModeRequest pRequest = (PTxSetPcrModeRequest) pIOBuffer;	

			pRequest->error = IT9510_setPcrMode ((IT9510INFO*) handle, pRequest->mode);
			deb_data("Enable PCR Mode %d\n", pRequest->mode);*/
			break;
		}
		case IOCTL_ITE_MOD_GETDTVMODE:
		{
			PTxGetDTVModeRequest pRequest = (PTxGetDTVModeRequest) pIOBuffer;		
			Byte temp = 0;
			
			error = IT9510_readRegister ((IT9510INFO*) handle, Processor_LINK, 0x4979, &temp);// has eeprom ?
			if((temp == 1) && (error == ModulatorError_NO_ERROR)){  // eeprom, read value
				pRequest->error = IT9510_readRegisters ((IT9510INFO*) handle, Processor_LINK, 0x49C5, 1, &pRequest->DTVMode);	
			} else {    // no eeprom, return Error
				pRequest->DTVMode = 0;
				pRequest->error = ModulatorError_USB_READ_FAIL;
			}
			break;
		}
		case IOCTL_ITE_MOD_GETFREQUENCYINDEX:
		{
			TxGetFrequencyIndexRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxGetFrequencyIndexRequest)))
				return -EFAULT;

			Request.error = IT9510User_getChannelIndex((IT9510INFO*) handle, &(Request.frequencyindex));
																																							                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxGetFrequencyIndexRequest)))
				return -EFAULT;
/*
			PTxGetFrequencyIndexRequest pRequest = (PTxGetFrequencyIndexRequest) pIOBuffer;		
			pRequest->error = IT9510User_getChannelIndex ((IT9510INFO*) handle, &(pRequest->frequencyindex));*/
			break;
		}
		case IOCTL_ITE_MOD_ENABLETPSENCRYPTION:
		{
			TxEnableTpsEncryptionRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxEnableTpsEncryptionRequest)))
				return -EFAULT;
			
			Request.error = IT9510_enableTpsEncryption((IT9510INFO*) handle, (Dword) Request.key);
																																										                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxEnableTpsEncryptionRequest)))
				return -EFAULT;
/*
			PTxEnableTpsEncryptionRequest pRequest = (PTxEnableTpsEncryptionRequest) pIOBuffer;		
			pRequest->error = IT9510_enableTpsEncryption ((IT9510INFO*) handle, (Dword) pRequest->key);*/
			break;
		}
		case IOCTL_ITE_MOD_DISABLETPSENCRYPTION:
		{
			TxDisableTpsEncryptionRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxDisableTpsEncryptionRequest)))
				return -EFAULT;
			
			Request.error = IT9510_disableTpsEncryption ((IT9510INFO*) handle);
																																							                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxDisableTpsEncryptionRequest)))
				return -EFAULT;
/*
			PTxDisableTpsEncryptionRequest pRequest = (PTxDisableTpsEncryptionRequest) pIOBuffer;		
			pRequest->error = IT9510_disableTpsEncryption ((IT9510INFO*) handle);*/
			break;
		}
        case IOCTL_ITE_BB_IT9560_GETSTATISTIC:
        {
			IT9560GetStatisticRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(IT9560GetStatisticRequest)))
				return -EFAULT;

			Request.error = IT9510_getIT9560FwVersion((IT9510INFO*) handle, 0xa8, Request.FwVersion);
			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(IT9560GetStatisticRequest)))
				return -EFAULT;
    		break;
        }
        case IOCTL_ITE_BB_IT9560_WRITEEEPROMVALUES:
        {
			IT9560WriteEepromValuesRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(IT9560WriteEepromValuesRequest)))
				return -EFAULT;
			
//			printk("[WRITEEEPROMVALUES] Request.slaveAddress: %x, Request.startAddressOffset: %u, Request.writeSize: %u\n", Request.slaveAddress, Request.startAddressOffset, Request.writeSize);
			Request.error = IT9510_writeIT9560EEPROM((IT9510INFO*) handle, Request.slaveAddress, Request.startAddressOffset, Request.buffer, Request.writeSize);

			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(IT9560WriteEepromValuesRequest)))
				return -EFAULT;
    		break;
        }
        case IOCTL_ITE_BB_IT9560_READEEPROMVALUES:
        {
			IT9560ReadEepromValuesRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(IT9560ReadEepromValuesRequest)))
				return -EFAULT;

			Request.error = IT9510_readIT9560EEPROM((IT9510INFO*) handle, Request.slaveAddress, Request.startAddressOffset, Request.buffer, Request.readSize);
//			printk("[READEEPROMVALUES] Request.slaveAddress: %x, Request.startAddressOffset: %u, Request.writeSize: %u\n", Request.slaveAddress, Request.startAddressOffset, Request.readSize);
			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(IT9560ReadEepromValuesRequest)))
				return -EFAULT;
    		break;
        }
        case IOCTL_ITE_BB_IT9560_WRITEREGISTERS:
        {
			IT9560WriteRegistersRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(IT9560WriteRegistersRequest)))
				return -EFAULT;

//			printk("[WRITEREGISTERS] Request.processor: %d, Request.registerAddress: %X, Request.bufferLength: %d\n", Request.processor, Request.registerAddress, Request.bufferLength);
            Request.error = IT9510_writeRegisters((IT9510INFO*) handle, Request.processor, Request.registerAddress, Request.bufferLength, Request.buffer);
			           			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(IT9560WriteRegistersRequest)))
				return -EFAULT;
    		break;
        }
        case IOCTL_ITE_BB_IT9560_READREGISTERS:
        {
			IT9560ReadRegistersRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(IT9560ReadRegistersRequest)))
				return -EFAULT;

//			printk("[READREGISTERS] Request.processor: %d, Request.registerAddress: %X, Request.bufferLength: %d\n", Request.processor, Request.registerAddress, Request.bufferLength);
			Request.error = IT9510_readRegisters((IT9510INFO*) handle, Request.processor, Request.registerAddress, Request.bufferLength, Request.buffer);
															                       			
			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(IT9560ReadRegistersRequest)))
				return -EFAULT;
    		break;
        }
		case IOCTL_ITE_MOD_ACQUIRECHANNELDUAL:
		{
			TxAcquireChannelDualRequest Request;
			if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(TxAcquireChannelDualRequest)))
				return -EFAULT;
	
			Request.error = IT9510_acquireTxChannelDual((IT9510INFO*) handle, Request.bandwidth, Request.frequency1, Request.frequency2);

			if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(TxAcquireChannelDualRequest)))
				return -EFAULT;

			break;
        }
        default:
        {
            //DBGPRINT(MP_WARN, (TEXT("DemodInformation--->[not suported!!!]\n")));
            deb_data("NOT SUPPORTED IOCONTROL! 0x%08x\n", (unsigned int)IOCTLCode);
            return ENOTTY;
        }
    }
    
    return error;
}
