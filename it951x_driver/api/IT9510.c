#include "IT9510.h"

Byte IT9517Cmd_sequence = 0;

#if ((IT9510_DVB_OFDM_VERSION2 < 10)||(IT9510_DVB_OFDM_VERSION3 < 8))
#error Firmware version too old.  Please update Firmware version.
#endif

#ifndef __IT9507_H__
const Byte Eagle_bitMask[8] = {
	0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF
};
#endif

const Byte IT9510_bitMask[8] = {
	0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF
};

/** local functions */
static unsigned int IT9510_fN_min[9] = {
	53000, 74000, 111000, 148000, 222000, 296000, 445000, 573000, 950000
};

Dword IT9510_getModulationDataRate (
    IN  IT9510INFO*			modulator,
	IN  Word				bandwidth,
	IN  ChannelModulation	channelModulation,
	IN  ISDBTModulation		isdbtModulation,
	IN  OutputMode			outputMode,
	OUT Dword*				dataRate_bps
){
	Dword   error = ModulatorError_NO_ERROR;
	unsigned long *dataRateArray;
	
	unsigned short array_idx = 0;
	
	Constellation FFT;
	Interval interval;
	CodeRate CR;

	if(outputMode == ISDBT){
		FFT = isdbtModulation.layerA.constellation;
		interval = isdbtModulation.interval;
		CR = isdbtModulation.layerA.codeRate;
	}else{
		FFT = channelModulation.constellation;
		interval = channelModulation.interval;
		CR = channelModulation.highCodeRate;
	}

	if(bandwidth == 8000)
	{
		if(outputMode == ISDBT){
			dataRateArray = ISDBT_8mhz_datarate_bps;					
		}else{
			dataRateArray = DVBT_8mhz_datarate_bps;			
		}
	}
	else if(bandwidth == 7000)
	{
		if(outputMode == ISDBT){
			dataRateArray = ISDBT_7mhz_datarate_bps;					
		}else{
			dataRateArray = DVBT_7mhz_datarate_bps;			
		}
	}
	else if(bandwidth == 6000)
	{
		if(outputMode == ISDBT){
			dataRateArray = ISDBT_6mhz_datarate_bps;					
		}else{
			dataRateArray = DVBT_6mhz_datarate_bps;			
		}
	}
	else if(bandwidth == 5000)
	{
		if(outputMode == ISDBT){
			dataRateArray = ISDBT_5mhz_datarate_bps;					
		}else{
			dataRateArray = DVBT_5mhz_datarate_bps;			
		}
	}
	else if(bandwidth == 4000)
	{
		if(outputMode == ISDBT){
			dataRateArray = ISDBT_4mhz_datarate_bps;					
		}else{
			dataRateArray = DVBT_4mhz_datarate_bps;			
		}
	}
	else if(bandwidth == 3000)
	{
		if(outputMode == ISDBT){
			dataRateArray = ISDBT_3mhz_datarate_bps;					
		}else{
			dataRateArray = DVBT_3mhz_datarate_bps;			
		}
	}
	else if(bandwidth == 2000)
	{
		if(outputMode == ISDBT){
			dataRateArray = ISDBT_2mhz_datarate_bps;					
		}else{
			dataRateArray = DVBT_2mhz_datarate_bps;			
		}
	}
	else if(bandwidth == 1000)
	{
		if(outputMode == ISDBT){
			dataRateArray = ISDBT_1mhz_datarate_bps;					
		}else{
			dataRateArray = DVBT_1mhz_datarate_bps;			
		}
	}
	else //default
	{
		*dataRate_bps = 0;
		goto exit;
	}

	//Constellation selection register
	switch(FFT)
	{
		case Constellation_64QAM: // 64QAM
			array_idx += 40;	
			break;

		case Constellation_16QAM: // 16QAM
			array_idx += 20;
			break;

			case Constellation_QPSK: // QPSK
			array_idx += 0;
			
			break;
	}

	//FEC selection register
	switch(CR)
	{
		case CodeRate_7_OVER_8: // 7/8
			array_idx += (4*4);
			break;

		case CodeRate_5_OVER_6: // 5/6
			array_idx += (4*3);
			break;

		case CodeRate_3_OVER_4: // 3/4
			array_idx += (4*2);
			break;

		case CodeRate_2_OVER_3: // 2/3
			array_idx += (4*1);
			
			break;

		case CodeRate_1_OVER_2: // 1/2
			array_idx += 0;			
			break;
	}

	//Guard Interval selection register
	switch(interval)
	{
		case Interval_1_OVER_4: // 1/4
			array_idx += 0;			
			break;

		case Interval_1_OVER_8: // 1/8
			array_idx += 1;			
			break;

		case Interval_1_OVER_16: // 1/16
			array_idx += 2;			
			break;

		case Interval_1_OVER_32: // 1/32
			array_idx += 3;			
			break;
	}

	*dataRate_bps = dataRateArray[array_idx];
		

	
exit:
	return (error);
}

Dword IT9510_getPacketTimeJitter(
	IN  IT9510INFO*	modulator,
	OUT Dword*		pcrOffset
){

	Dword error;
	Dword offset;
	Dword dataRate_org;
	Dword dataRate_target;
	Dword temp;
	
	offset = (modulator->pcrCalInfo.packetTimeJitter_ps * 100) / 3617;

	error = IT9510_getModulationDataRate(modulator, 
										 modulator->bandwidth, 
										 modulator->channelModulation, 
										 modulator->isdbtModulation, 
										 modulator->outputMode, 
										 &dataRate_target);
	if(error) goto exit;
	
	error = IT9510_getModulationDataRate(modulator, 
										 modulator->pcrCalInfo.bandwidth, 
										 modulator->pcrCalInfo.channelModulation,
										 modulator->pcrCalInfo.isdbtModulation, 
										 modulator->pcrCalInfo.outputMode, 
										 &dataRate_org);
	if(error) goto exit;
	
	
	temp = (dataRate_org * 100) / dataRate_target;
    *pcrOffset = (offset * temp) / 100;


	//printf("dataRate_org=%d / dataRate_target=%d / pcrOffset=%d \n",dataRate_org,dataRate_target,*pcrOffset);


exit:
	return ModulatorError_NO_ERROR;	
}






Dword IT9510_setPcrModeEnable (
    IN  IT9510INFO*		modulator,
    IN  Byte			enable
){
	Dword   error = ModulatorError_NO_ERROR;
	unsigned char *basehex;
	unsigned char *exthex;
	unsigned short basehex_idx = 0;
	unsigned short exthex_idx = 0;
	Constellation FFT;
	Interval interval;
	CodeRate CR;
	Dword   offset;
	Dword   PCR_EXT;

	if(modulator->outputMode == ISDBT){
		FFT = modulator->isdbtModulation.layerA.constellation;
		interval = modulator->isdbtModulation.interval;
		CR = modulator->isdbtModulation.layerA.codeRate;
	}else{
		FFT = modulator->channelModulation.constellation;
		interval = modulator->channelModulation.interval;
		CR = modulator->channelModulation.highCodeRate;
	}

	if(modulator->bandwidth == 8000)
	{
		if(modulator->outputMode == ISDBT){
			basehex = ISDBT_basehex_8mhz;
			exthex = ISDBT_exthex_8mhz;		
		}else{
			basehex = DVBT_basehex_8mhz;
			exthex = DVBT_exthex_8mhz;
		}
	}
	else if(modulator->bandwidth == 7000)
	{
		if(modulator->outputMode == ISDBT){
			basehex = ISDBT_basehex_7mhz;
			exthex = ISDBT_exthex_7mhz;		
		}else{
			basehex = DVBT_basehex_7mhz;
			exthex = DVBT_exthex_7mhz;
		}
	}
	else if(modulator->bandwidth == 6000)
	{
		if(modulator->outputMode == ISDBT){
			basehex = ISDBT_basehex_6mhz;
			exthex = ISDBT_exthex_6mhz;		
		}else{
			basehex = DVBT_basehex_6mhz;
			exthex = DVBT_exthex_6mhz;
		}
	}
	else if(modulator->bandwidth == 5000)
	{
		if(modulator->outputMode == ISDBT){
			basehex = ISDBT_basehex_5mhz;
			exthex = ISDBT_exthex_5mhz;		
		}else{
			basehex = DVBT_basehex_5mhz;
			exthex = DVBT_exthex_5mhz;
		}
	}
	else if(modulator->bandwidth == 4000)
	{
		if(modulator->outputMode == ISDBT){
			basehex = ISDBT_basehex_4mhz;
			exthex = ISDBT_exthex_4mhz;		
		}else{
			basehex = DVBT_basehex_4mhz;
			exthex = DVBT_exthex_4mhz;
		}
	}
	else if(modulator->bandwidth == 3000)
	{
		if(modulator->outputMode == ISDBT){
			basehex = ISDBT_basehex_3mhz;
			exthex = ISDBT_exthex_3mhz;		
		}else{
			basehex = DVBT_basehex_3mhz;
			exthex = DVBT_exthex_3mhz;
		}
	}
	else if(modulator->bandwidth == 2000)
	{
		if(modulator->outputMode == ISDBT){
			basehex = ISDBT_basehex_2mhz;
			exthex = ISDBT_exthex_2mhz;		
		}else{
			basehex = DVBT_basehex_2mhz;
			exthex = DVBT_exthex_2mhz;
		}
	}
	else if(modulator->bandwidth == 1000)
	{
		if(modulator->outputMode == ISDBT){
			basehex = ISDBT_basehex_1mhz;
			exthex = ISDBT_exthex_1mhz;		
		}else{
			basehex = DVBT_basehex_1mhz;
			exthex = DVBT_exthex_1mhz;
		}
	}
	else //default
	{
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_ts0_pcrmode, 0); //disable pcr mode
		goto exit;
	}

	//Constellation selection register
	switch(FFT)
	{
		case Constellation_64QAM: // 64QAM
			basehex_idx += 40;
			exthex_idx += (40*3);
			break;

		case Constellation_16QAM: // 16QAM
			basehex_idx += 20;
			exthex_idx += (20*3);
			break;

			case Constellation_QPSK: // QPSK
			//basehex_idx += 0;
			//exthex_idx += 0;
			break;
	}

	//FEC selection register
	switch(CR)
	{
		case CodeRate_7_OVER_8: // 7/8
			basehex_idx += (4*4);
			exthex_idx += (4*4*3);
			break;

		case CodeRate_5_OVER_6: // 5/6
			basehex_idx += (4*3);
			exthex_idx += (4*3*3);
			break;

		case CodeRate_3_OVER_4: // 3/4
			basehex_idx += (4*2);
			exthex_idx += (4*2*3);
			break;

		case CodeRate_2_OVER_3: // 2/3
			basehex_idx += (4*1);
			exthex_idx += (4*1*3);
			break;

		case CodeRate_1_OVER_2: // 1/2
			//basehex_idx += 0;
			//exthex_idx += 0;
			break;
	}

	//Guard Interval selection register
	switch(interval)
	{
		case Interval_1_OVER_4: // 1/4
			//basehex_idx += 0;
			//exthex_idx += 0;
			break;

		case Interval_1_OVER_8: // 1/8
			basehex_idx += 1;
			exthex_idx += (1*3);
			break;

		case Interval_1_OVER_16: // 1/16
			basehex_idx += 2;
			exthex_idx += (2*3);
			break;

		case Interval_1_OVER_32: // 1/32
			basehex_idx += 3;
			exthex_idx += (3*3);
			break;
	}

	
	if((enable != 0 ) && (modulator->pcrMode != PcrModeDisable)) {
		//--------------------------- set pcr -------------
		error = IT9510_getPacketTimeJitter(modulator, &offset);
		if (error) goto exit;


		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_ts0_pcrmode, 0);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_pcr_reload, 0);
		if (error) goto exit;
		//---#PCR Base
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_pcr_base_add_7_0, (Byte)basehex[basehex_idx]);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_pcr_base_add_15_8, (Byte)0);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_pcr_base_add_23_16, (Byte)0);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_pcr_base_add_31_24, (Byte)0);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_pcr_base_add_32, (Byte)0);
		if (error) goto exit;

		//--- #PCR Extension
		if(modulator->pcrCalInfo.positive == 1)
			PCR_EXT = (exthex[(exthex_idx)]<<16 | exthex[(exthex_idx+1)]<<8 | exthex[exthex_idx+2]) + offset;
		else if(modulator->pcrCalInfo.positive == -1)
			PCR_EXT = (exthex[(exthex_idx)]<<16 | exthex[(exthex_idx+1)]<<8 | exthex[exthex_idx+2]) - offset;
		else
			PCR_EXT = (exthex[(exthex_idx)]<<16 | exthex[(exthex_idx+1)]<<8 | exthex[exthex_idx+2]);

		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_pcr_ext_add_7_0, (Byte)PCR_EXT);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_pcr_ext_add_15_8, (Byte)(PCR_EXT>>8));
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_pcr_ext_add_18_16, (Byte)(PCR_EXT>>16));
		if (error) goto exit;


		//--- #PCR diff 100ms will auto-reload
		error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFA4D, 0x28);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFA4E, 0x23);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFA4F, 0x00);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFA50, 0x00);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFA51, 0x00);
		if (error) goto exit;
		
		//---enable PCR Mode
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_ts0_pcrmode, (Byte)modulator->pcrMode );
		if (error) goto exit;

//-----------------------------------------
	}else{

		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_ts0_pcrmode, 0);

	}
exit:
	return (error);
}


//-------------------------------------------

unsigned int IT9510_getLoFreq(unsigned int rf_freq_kHz)
{
	unsigned int nc, nv, mv, lo_freq;
	
	//unsigned int freq_code;
	unsigned long tmp_tg, tmp_cal, tmp_m;

	unsigned int m_bdry;
	unsigned long tmp_numer;
	unsigned int g_fxtal_kHz = 2000;
	unsigned int g_fdiv =3;
	
	//m_bdry = 3480;
	m_bdry = 3660;
	tmp_numer = (unsigned long)g_fxtal_kHz * (unsigned long)m_bdry;
	
	IT9510_fN_min[7] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 4));	
	IT9510_fN_min[6] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 6));
	IT9510_fN_min[5] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 8));
	IT9510_fN_min[4] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 12));
	IT9510_fN_min[3] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 16));
	IT9510_fN_min[2] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 24));
	IT9510_fN_min[1] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 32));



	//*nc = IT9510_get_nc(rf_freq_kHz);
	if ((rf_freq_kHz <= IT9510_fN_min[1]))										{nc = 0;}	/*74*/
	else if ((rf_freq_kHz > IT9510_fN_min[1]) && (rf_freq_kHz <= IT9510_fN_min[2]))	{nc = 1;}	/*74 111*/	
	else if ((rf_freq_kHz > IT9510_fN_min[2]) && (rf_freq_kHz <= IT9510_fN_min[3]))	{nc = 2;}	/*111 148*/
	else if ((rf_freq_kHz > IT9510_fN_min[3]) && (rf_freq_kHz <= IT9510_fN_min[4]))	{nc = 3;}	/*148 222*/	
	else if ((rf_freq_kHz > IT9510_fN_min[4]) && (rf_freq_kHz <= IT9510_fN_min[5]))	{nc = 4;}	/*222 296*/	
	else if ((rf_freq_kHz > IT9510_fN_min[5]) && (rf_freq_kHz <= IT9510_fN_min[6]))	{nc = 5;}	/*296 445*/
	else if ((rf_freq_kHz > IT9510_fN_min[6]) && (rf_freq_kHz <= IT9510_fN_min[7]))	{nc = 6;}	/*445 573*/
	else if ((rf_freq_kHz > IT9510_fN_min[7]) && (rf_freq_kHz <= IT9510_fN_min[8]))	{nc = 7;}	/*573 890*/
	else 																	{nc = 8;}	/*L-band*/
	
	//*nv = IT9510_get_nv(*nc);

	switch(nc) {
		case 0:	nv = 48;	break;
		case 1:	nv = 32;	break;
		case 2:	nv = 24;	break;
		case 3:	nv = 16;	break;
		case 4:	nv = 12;	break;
		case 5:	nv = 8;	break;
		case 6:	nv = 6;	break;
		case 7:	nv = 4;	break;
		case 8: nv = 2; break;	/*L-band*/
		default:	nv = 2;	break;
	}



	if((nc)==8)
		nc = 0;
	tmp_tg = (unsigned long)rf_freq_kHz * (unsigned long)(nv) * (unsigned long)g_fdiv;
	tmp_m = (tmp_tg / (unsigned long)g_fxtal_kHz);
	tmp_cal = tmp_m * (unsigned long)g_fxtal_kHz;
	if ((tmp_tg-tmp_cal) >= (g_fxtal_kHz>>1)) {tmp_m = tmp_m+1;}
	mv = (unsigned int) (tmp_m);

	lo_freq = (((nc)&0x07) << 13) + (mv);
	
	return lo_freq;
}


Dword IT9517Cmd_addChecksum (
    IN  IT9510INFO*    modulator,
    OUT Dword*          bufferLength,
    OUT Byte*           buffer
) {
    Dword error  = ModulatorError_NO_ERROR;
    Dword loop   = (*bufferLength - 1) / 2;
    Dword remain = (*bufferLength - 1) % 2;
    Dword i;
    Word  checksum = 0;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    for (i = 0; i < loop; i++)
        checksum = checksum + (Word) (buffer[2 * i + 1] << 8) + (Word) (buffer[2 * i + 2]);
    if (remain)
        checksum = checksum + (Word) (buffer[*bufferLength - 1] << 8);
    
    checksum = ~checksum;
    buffer[*bufferLength]     = (Byte) ((checksum & 0xFF00) >> 8);
    buffer[*bufferLength + 1] = (Byte) (checksum & 0x00FF);
    buffer[0]                 = (Byte) (*bufferLength + 1);
    *bufferLength            += 2;

    return (error);
}


Dword IT9517Cmd_removeChecksum (
    IN  IT9510INFO*    modulator,
    OUT Dword*          bufferLength,
    OUT Byte*           buffer
) {
    Dword error    = ModulatorError_NO_ERROR;
    Dword loop     = (*bufferLength - 3) / 2;
    Dword remain   = (*bufferLength - 3) % 2;
    Dword i;
    Word  checksum = 0;
	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    for (i = 0; i < loop; i++)
        checksum = checksum + (Word) (buffer[2 * i + 1] << 8) + (Word) (buffer[2 * i + 2]);
    if (remain)
        checksum = checksum + (Word) (buffer[*bufferLength - 3] << 8);    
    
    checksum = ~checksum;
    if (((Word)(buffer[*bufferLength - 2] << 8) + (Word)(buffer[*bufferLength - 1])) != checksum) {
        error = ModulatorError_WRONG_CHECKSUM;
        goto exit;
    }
    if (buffer[2])
        error = ModulatorError_FIRMWARE_STATUS | buffer[2];
    
    buffer[0]      = (Byte) (*bufferLength - 3);
    *bufferLength -= 2;

exit :
    return (error);
}

Dword IT9517Cmd_reboot (
    IN  IT9510INFO*    modulator
) {
    Dword       error = ModulatorError_NO_ERROR;
    Word        command;
    Byte        buffer[255];
    Dword       bufferLength,cnt;
       
    command   = IT9517Cmd_buildCommand (Command_REBOOT, Processor_LINK);
    buffer[1] = (Byte) (command >> 8);
    buffer[2] = (Byte) command;
    buffer[3] = (Byte) IT9517Cmd_sequence++;
    bufferLength = 4;
    error = IT9517Cmd_addChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;
   
	for (cnt = 0; cnt < IT9510User_RETRY_MAX_LIMIT; cnt++) {
		error = IT9510User_busTx (modulator, bufferLength, buffer);
		if (error == 0) break;
		IT9510User_delay (1);
	}
    if (error) goto exit;

exit :
    
    return (error);
}


Dword IT9517Cmd_sendCommand (
    IN  IT9510INFO*    modulator,
    IN  Word            command,
    IN  Processor       processor,
    IN  Dword           writeBufferLength,
    IN  Byte*           writeBuffer,
    IN  Dword           readBufferLength,
    OUT Byte*           readBuffer
) {
    Dword       error = ModulatorError_NO_ERROR;
    Byte        buffer[255];
    Dword       bufferLength;
    Dword       remainLength;
    Dword       sendLength;
    Dword       i, k, cnt;
    
    Dword       maxFrameSize = IT9510User_MAXFRAMESIZE;
    
    if ((writeBufferLength + 6) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }

    if ((readBufferLength + 5) > IT9510User_MAX_PKT_SIZE) {
        error  = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }

    if ((readBufferLength + 5) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }


    if (writeBufferLength == 0) {
        command   = IT9517Cmd_buildCommand (command, processor);
        buffer[1] = (Byte) (command >> 8);
        buffer[2] = (Byte) command;
        buffer[3] = (Byte) IT9517Cmd_sequence++;
        bufferLength = 4;
        error = IT9517Cmd_addChecksum (modulator, &bufferLength, buffer);
        if (error) goto exit;

        // send command packet
        i = 0;
        sendLength = 0;
        remainLength = bufferLength;
        while (remainLength > 0) {
            i = (remainLength > IT9510User_MAX_PKT_SIZE) ? (IT9510User_MAX_PKT_SIZE) : (remainLength);        

			for (cnt = 0; cnt < IT9510User_RETRY_MAX_LIMIT; cnt++) {
				error = IT9510User_busTx (modulator, i, &buffer[sendLength]);
				if (error == 0) break;
				IT9510User_delay (1);
			}
            if (error) goto exit;

            sendLength   += i;
            remainLength -= i;
        }
    } else {
        command   = IT9517Cmd_buildCommand (command, processor);
        buffer[1] = (Byte) (command >> 8);
        buffer[2] = (Byte) command;
        buffer[3] = (Byte) IT9517Cmd_sequence++;
        for (k = 0; k < writeBufferLength; k++)
            buffer[k + 4] = writeBuffer[k];
        
        
        bufferLength = 4 + writeBufferLength;
        error = IT9517Cmd_addChecksum (modulator, &bufferLength, buffer);
        if (error) goto exit;

        
        /** send command */
        i = 0;
        sendLength = 0;
        remainLength = bufferLength;
        while (remainLength > 0) {
            i     = (remainLength > IT9510User_MAX_PKT_SIZE) ? (IT9510User_MAX_PKT_SIZE) : (remainLength);        

			for (cnt = 0; cnt < IT9510User_RETRY_MAX_LIMIT; cnt++) {
				error = IT9510User_busTx (modulator, i, &buffer[sendLength]);
				if (error == 0) break;
				IT9510User_delay (1);
			}
            if (error) goto exit;

            sendLength   += i;
            remainLength -= i;
        }
    }

    bufferLength = 5 + readBufferLength;

	for (cnt = 0; cnt < IT9510User_RETRY_MAX_LIMIT; cnt++) {
		error = IT9510User_busRx (modulator, bufferLength, buffer);
		if (error == 0) break;
		IT9510User_delay (1);
	}
    if (error) goto exit;

    error = IT9517Cmd_removeChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

    if (readBufferLength) {
        for (k = 0; k < readBufferLength; k++) {
            readBuffer[k] = buffer[k + 3];
        }
    }

exit :
    
    return (error);
}

Dword IT9510_calOutputGain (
	IN  IT9510INFO*    modulator,
	IN  Byte		  *defaultValue,
	IN  int			  *gain	   
) {
	Dword error = ModulatorError_NO_ERROR;
	int amp_mul;
	int c1value = 0;
	int c2value = 0;
	int c3value = 0;	
	int c1value_default;
	int c2value_default;
	int c3value_default;	
	
	Dword amp_mul_max1 = 0;
	Dword amp_mul_max2 = 0;
	Dword amp_mul_max3 = 0;
	int amp_mul_max = 0;
	int i = 0;
	
	int gain_X10 = *gain * 10;
	
	Bool overflow = False;
	
	if(modulator == NULL){
		error = ModulatorError_NULL_HANDLE_PTR;
		goto exit;
	}

	c1value_default = defaultValue[1]<<8 | defaultValue[0];
	c2value_default = defaultValue[3]<<8 | defaultValue[2];
	c3value_default = defaultValue[5]<<8 | defaultValue[4];	
	
	if (c1value_default>1023) c1value_default = c1value_default-2048;
	if (c2value_default>1023) c2value_default = c2value_default-2048;
	if (c3value_default>1023) c3value_default = c3value_default-2048;

	amp_mul_max1 = 10000*1023/abs(c1value_default);
	if(c2value_default != 0)
		amp_mul_max2 = 10000*1023/abs(c2value_default);
	else
		amp_mul_max2 = 0xFFFFFFFF;
    amp_mul_max3 = 10000*1023/abs(c3value_default);


	if (amp_mul_max1<amp_mul_max3) {
		if (amp_mul_max1<amp_mul_max2) {
				amp_mul_max = (int)amp_mul_max1;
			} else {
				amp_mul_max = (int)amp_mul_max2;
			}
	  } else if (amp_mul_max3<amp_mul_max2) {
        	amp_mul_max =(int)amp_mul_max3;
   	  } else {
   	  	amp_mul_max =(int)amp_mul_max2;
   	  	}

	if(gain_X10>0){
		//d_amp_mul = 1;
		amp_mul = 10000;
		for(i = 0 ; i<gain_X10 ; i+=10){
			if (amp_mul_max>amp_mul) {
				amp_mul = (amp_mul * 11220)/10000;
				c1value = (c1value_default * amp_mul)/10000;
				c2value = (c2value_default* amp_mul)/10000;
				c3value = (c3value_default * amp_mul)/10000;
			}
			if(c1value>0x03ff){
				c1value=0x03ff;
				overflow = True;				
			}
			
			if(c3value>0x03ff){
				c3value=0x03ff;
				overflow = True;				
			}

			if(overflow)
				break;
		}
			
		
	}else if(gain_X10<0){
		//d_amp_mul = 1;
		amp_mul = 10000;
		for(i = 0 ; i>gain_X10 ; i-=10){
			if (amp_mul_max>amp_mul) {
				//d_amp_mul *= 0.501;
				amp_mul = (amp_mul * 8910)/10000;
				
				c1value = (c1value_default * amp_mul)/10000;
				c2value = (c2value_default * amp_mul)/10000;
				c3value = (c3value_default * amp_mul)/10000;
			}
			if(c1value==0){
				overflow = True;
			}
			
			if(c3value==0){
				overflow = True;
			}

			if(overflow)
				break;			
		}
		
	}else{
		c1value = c1value_default;
		c2value = c2value_default;
		c3value = c3value_default;

	}
	if (c1value<0) {c1value=c1value+2048;}
	if (c2value<0) {c2value=c2value+2048;}
	if (c3value<0) {c3value=c3value+2048;}
	c1value = (c1value%2048);
	c2value = (c2value%2048);
	c3value = (c3value%2048);
	*gain = i/10;
exit:	
	return (error);
}

Dword IT9510_selectBandwidth (
	IN  IT9510INFO*    modulator,
	IN  Word          bandwidth          /** KHz              */
) {
	Dword error ;
	
	Byte temp1;
	Byte temp2;
	Byte temp3;
	Byte temp4;
	Byte temp5;
	//Byte temp;
	
	error = ModulatorError_NO_ERROR;
	temp1 = 0;
	temp2 = 0;
	temp3 = 0;
	temp4 = 0;
	temp5 = 0;
	
	switch (bandwidth) {

		case 1000:              /** 1M */
			temp1 = 0x5E;	//0xFBB6
			temp2 = 0x03;	//0xFBB7
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			break;

       	case 1500:              /** 1.5M */
			temp1 = 0x6E;	//0xFBB6
			temp2 = 0x03;	//0xFBB7
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			break; 

		case 2000:              /** 2M */
			temp1 = 0x5E;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			break;

		case 2500:              /** 2M */
			temp1 = 0x66;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			break;

		case 3000:              /** 3M */
			temp1 = 0x6E;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			break;

		case 4000:              /** 4M */
			temp1 = 0x5E;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp4 = 0x03;	//0xD814
			temp5 = 0x01;	//0xF741
			break;

		case 5000:              /** 5M */
			temp1 = 0x66;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp4 = 0x03;	//0xD814
			temp5 = 0x01;	//0xF741
			break;

		case 6000:              /** 6M */
			temp1 = 0x6E;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp4 = 0x03;	//0xD814
			temp5 = 0x01;	//0xF741
			break;

		case 7000:              /** 7M */
			temp1 = 0x76;	//0xFBB6
			temp2 = 0x02;	//0xFBB7
			temp4 = 0x03;	//0xD814
			temp5 = 0x02;	//0xF741
			break;

		case 8000:              /** 8M */
			temp1 = 0x1E;	//0xFBB6
			temp2 = 0x02;	//0xFBB7
			temp4 = 0x03;	//0xD814
			temp5 = 0x02;	//0xF741
			break;

		default:
			
			error = ModulatorError_INVALID_BW;
			break;
	}

	if(error == ModulatorError_NO_ERROR){
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_afe_mem15, temp1);
		if (error) goto exit;

		error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_afe_mem16, 0, 2, temp2);
		if (error) goto exit;

		error = IT9510_writeRegister (modulator,  Processor_OFDM, p_IT9510_reg_intp_ds, temp5);
		if (error) goto exit;

		error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_dac_clksel, temp4);
		if (error) goto exit;

		error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_dac_ph_sel, 0);
		if (error) goto exit;

		error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_afe_mem17, 2, 1, 0);
		if (error) goto exit;
		

	}	
	
exit :
	if(error)
		modulator->bandwidth = 0;
	else
		modulator->bandwidth = bandwidth;
	return (error);
}


Dword IT9510_setFrequency (
	IN  IT9510INFO*    modulator,
	IN  Dword           frequency
) {
	Dword error = ModulatorError_NO_ERROR;
	
	unsigned int tmp;
	int i;
	int point = 0;
	Dword upper,lower;
	Byte freq_code_H,freq_code_L;
	if(modulator->isExtLo != True){
		/*----- set_lo_freq -----*/
		tmp = IT9510_getLoFreq(frequency);
		freq_code_L = (unsigned char) (tmp & 0xFF);
		freq_code_H = (unsigned char) ((tmp >> 8) & 0xFF);

		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_afe_mem6, freq_code_L);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator,  Processor_OFDM, p_IT9510_reg_afe_mem7, freq_code_H);
		if (error) goto exit;

		if(frequency>=600000)
			error = IT9510_writeRegister (modulator,  Processor_OFDM, 0xFBBC, 0x10);
		else if((frequency<600000) && (frequency>=300000))
			error = IT9510_writeRegister (modulator,  Processor_OFDM, 0xFBBC, 0x03);
		else if(frequency<300000)
			error = IT9510_writeRegister (modulator,  Processor_OFDM, 0xFBBC, 0x00);
		if (error) goto exit;


		if(frequency>950000)
			error = IT9510_writeRegisterBits (modulator, Processor_OFDM, 0xFB2C, 2, 1,1);
		else
			error = IT9510_writeRegisterBits (modulator, Processor_OFDM, 0xFB2C, 2, 1,0);
		if (error) goto exit;

		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_afe_mem9, 2);
		if (error) goto exit;

		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_afe_mem9, 1);
		if (error) goto exit;

		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_afe_mem9, 0);
		if (error) goto exit;
		

		
		error = IT9510_runTxCalibration(modulator, modulator->bandwidth, frequency);
		if (error) goto exit;
	}else{

		//use ext lo

	}
	modulator->frequency = frequency;

	//---
	if(modulator->dcInfo.tableGroups != 0){
		
		if(frequency < modulator->dcInfo.ptrDCtable[0].startFrequency || (modulator->dcInfo.tableGroups == 1)){
			point = 0;
		} else if(frequency > modulator->dcInfo.ptrDCtable[modulator->dcInfo.tableGroups-1].startFrequency){
			point = modulator->dcInfo.tableGroups-1;
		} else {
			for(i=0;i<modulator->dcInfo.tableGroups;i++){
				
				lower = modulator->dcInfo.ptrDCtable[i].startFrequency;
				upper = modulator->dcInfo.ptrDCtable[i+1].startFrequency;
				if((frequency>=lower) && (frequency<=upper)){
					
					if( (frequency-lower) > (upper - frequency) )
						point = i+1;
					else
						point = i;
					break;
				}
			}
			//point = modulator->dcInfo.tableGroups-1;
		}

		error = IT9510_setDCCalibrationValue(modulator, modulator->dcInfo.ptrDCtable[point].i, modulator->dcInfo.ptrDCtable[point].q);
		if (error) goto exit;

		error = IT9510_setOFSCalibrationValue(modulator, (Byte)modulator->dcInfo.ptrOFStable[point].i, (Byte)modulator->dcInfo.ptrOFStable[point].q);
		if (error) goto exit;		
	}



	//----
exit :
	return (error);
}


Dword IT9510_loadFirmware (
	IN  IT9510INFO*     modulator,
	IN  Byte*           firmwareCodes,
	IN  Segment*        firmwareSegments,
	IN  Word*           firmwarePartitions
) {
	Dword error = ModulatorError_NO_ERROR;
	Dword beginPartition = 0;
	Dword endPartition = 0;
	Dword version;
	Dword firmwareLength;
	Byte* firmwareCodesPointer;
	Dword i;
	Byte temp;
	
	/** Set I2C master clock speed. */
	temp = IT9510User_IIC_SPEED;
	error = IT9510_writeRegisters (modulator, Processor_LINK, p_IT9510_reg_lnk2ofdm_data_63_56, 1, &temp);
	if (error) goto exit;

	firmwareCodesPointer = firmwareCodes;

	beginPartition = 0;
	endPartition = firmwarePartitions[0];


	for (i = beginPartition; i < endPartition; i++) {
		firmwareLength = firmwareSegments[i].segmentLength;
		if (firmwareSegments[i].segmentType == 1) {
			/** Copy firmware */
			error = IT9517Cmd_sendCommand (modulator, Command_SCATTER_WRITE, Processor_LINK, firmwareLength, firmwareCodesPointer, 0, NULL);
			if (error) goto exit;
		}else{
			error = ModulatorError_INVALID_FW_TYPE;
			goto exit;
		}
		firmwareCodesPointer += firmwareLength;
	}

	/** Boot */
	error = IT9517Cmd_sendCommand (modulator, Command_BOOT, Processor_LINK, 0, NULL, 0, NULL);
	if (error) goto exit;

	IT9510User_delay (10);

	/** Check if firmware is running */
	version = 0;
	//error = IT9510_getFirmwareVersion (modulator, Processor_LINK, &version);
	//if (error) goto exit;

	for (i= 0; i < 10; i++)
    {
        version = 0;
        error = IT9510_getFirmwareVersion (modulator, Processor_LINK, &version);
        if (error)
		{                
        	IT9510User_delay (10);
			continue; // goto exit;
        }
        else
        {
			break;
        }
    }

    if ( i >= 10 )
	   goto exit;

	if (version == 0)
		error = ModulatorError_BOOT_FAIL;



exit :
	return (error);
}

Dword IT9510_loadScript (
	IN  IT9510INFO*    modulator,
	IN  Word*           scriptSets,
	IN  ValueSet*       scripts
) {
	Dword error = ModulatorError_NO_ERROR;
	Word beginScript;
	Word endScript;
	Byte i, supportRelay = 0, chipNumber = 0, bufferLens = 1;
	Word j;
	Byte temp;
	Byte buffer[20] = {0,};
	Dword tunerAddr, tunerAddrTemp;
	
	/** Querry SupportRelayCommandWrite **/
	error = IT9510_readRegisters (modulator, Processor_OFDM, 0x004D, 1, &supportRelay);
	if (error) goto exit;

	
	/** Enable RelayCommandWrite **/
	if (supportRelay) {
		temp = 1;
		error = IT9510_writeRegisters (modulator, Processor_OFDM, 0x004E, 1, &temp);
		if (error) goto exit;
	}

	if ((scriptSets[0] != 0) && (scripts != NULL)) {
		beginScript = 0;
		endScript = scriptSets[0];

		for (i = 0; i < chipNumber; i++) {
			/** Load OFSM init script */
			for (j = beginScript; j < endScript; j++) {
				tunerAddr = tunerAddrTemp = scripts[j].address;
				buffer[0] = scripts[j].value;

				while (j < endScript && bufferLens < 20) {
					tunerAddrTemp += 1;
					if (tunerAddrTemp != scripts[j+1].address)
						break;

					buffer[bufferLens] = scripts[j+1].value;
					bufferLens ++;
					j ++;
				}

				error = IT9510_writeRegisters (modulator, Processor_OFDM, tunerAddr, bufferLens, buffer);
				if (error) goto exit;
				bufferLens = 1;
			}
		}
	}

	/** Disable RelayCommandWrite **/
	if (supportRelay) {
		temp = 0;
		error = IT9510_writeRegisters (modulator, Processor_OFDM, 0x004E, 1, &temp);
		if (error) goto exit;
	}

exit :
	return (error);
}



Dword IT9510_writeRegister (
    IN  IT9510INFO*    modulator,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            value
) {
   	return (IT9510_writeRegisters(modulator, processor, registerAddress, 1, &value));
}


Dword IT9510_writeRegisters (
    IN  IT9510INFO*    modulator,
    IN  Processor     processor,
    IN  Dword         registerAddress,
    IN  Byte          writeBufferLength,
    IN  Byte*         writeBuffer
) {
  	Dword error = ModulatorError_NO_ERROR;

	Byte registerAddressLength;
	Word        command;
    Byte        buffer[255];
    Dword       bufferLength;
    Dword       remainLength;
    Dword       sendLength;
    Dword       i,cnt;
   
    Byte       maxFrameSize = IT9510User_MAXFRAMESIZE;

	if (processor == Processor_LINK) {
		if (registerAddress > 0x000000FF) {
			registerAddressLength = 2;
		} else {
			registerAddressLength = 1;
		}
	} else {
			registerAddressLength = 2;
	}
	
	if (writeBufferLength == 0) goto exit;
    if (registerAddressLength > 4) {
        error  = ModulatorError_PROTOCOL_FORMAT_INVALID;
        goto exit;
    }

      
    if ((writeBufferLength + 12) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }



    /** add frame header */
    command   = IT9517Cmd_buildCommand (Command_REG_DEMOD_WRITE, processor);
    buffer[1] = (Byte) (command >> 8);
    buffer[2] = (Byte) command;
    buffer[3] = (Byte) IT9517Cmd_sequence++;
    buffer[4] = (Byte) writeBufferLength;
    buffer[5] = (Byte) registerAddressLength;
    buffer[6] = (Byte) ((registerAddress) >> 24); /** Get first byte of reg. address  */
    buffer[7] = (Byte) ((registerAddress) >> 16); /** Get second byte of reg. address */
    buffer[8] = (Byte) ((registerAddress) >> 8);  /** Get third byte of reg. address  */
    buffer[9] = (Byte) (registerAddress );        /** Get fourth byte of reg. address */

    /** add frame data */
    for (i = 0; i < writeBufferLength; i++) {    
        buffer[10 + i] = writeBuffer[i];
    }

    /** add frame check-sum */
    bufferLength = 10 + writeBufferLength;
    error = IT9517Cmd_addChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;    

    /** send frame */
    i = 0;
    sendLength = 0;
    remainLength = bufferLength;
    while (remainLength > 0) {
        i     = (remainLength > IT9510User_MAX_PKT_SIZE) ? (IT9510User_MAX_PKT_SIZE) : (remainLength);
		for (cnt = 0; cnt < IT9510User_RETRY_MAX_LIMIT; cnt++) {
			error = IT9510User_busTx (modulator, i, &buffer[sendLength]);
			if (error == 0) break;
			IT9510User_delay (1);
		}
        if (error) goto exit;

        sendLength   += i;
        remainLength -= i;
    }

    /** get reply frame */
    bufferLength = 5;
    
	for (cnt = 0; cnt < IT9510User_RETRY_MAX_LIMIT; cnt++) {
		error = IT9510User_busRx (modulator, bufferLength, buffer);
		if (error == 0) break;
		IT9510User_delay (1);
	}
    if (error) goto exit;

    /** remove check-sum from reply frame */
    error = IT9517Cmd_removeChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

exit :
   
	
return (error);
}

Dword IT9510_writeGenericRegisters (
    IN  IT9510INFO*    modulator,
    IN  Byte            slaveAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    Byte writeBuffer[256];
	Byte i;
	
	writeBuffer[0] = bufferLength;
	writeBuffer[1] = 2;
	writeBuffer[2] = slaveAddress;

	for (i = 0; i < bufferLength; i++) {
		writeBuffer[3 + i] = buffer[i];
	}
	return (IT9517Cmd_sendCommand (modulator, Command_GENERIC_WRITE, Processor_LINK, bufferLength + 3, writeBuffer, 0, NULL));
}


Dword IT9510_writeEepromValues (
    IN  IT9510INFO*    modulator,
    IN  Word            registerAddress,
    IN  Byte            writeBufferLength,
    IN  Byte*           writeBuffer
) {
    Dword       error = ModulatorError_NO_ERROR;
    Word        command;
    Byte        buffer[255];
    Dword       bufferLength;
    Dword       remainLength;
    Dword       sendLength;
    Dword       i,cnt;
   
    Dword       maxFrameSize;
	Byte eepromAddress = 0x01;	
	Byte registerAddressLength = 0x01;
	Byte val = 0;
	error = IT9510_readRegister (modulator, Processor_LINK, 0x496D, &val);
    if((val & 0x0F) < 0x08)
        registerAddressLength = 0x01;
    else
        registerAddressLength = 0x02;



    IT9510User_enterCriticalSection ();

    if (writeBufferLength == 0) goto exit;

    maxFrameSize = IT9510User_MAXFRAMESIZE; 

    if ((Dword)(writeBufferLength + 11) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }

    /** add frame header */
    command   = IT9517Cmd_buildCommand (Command_REG_EEPROM_WRITE, Processor_LINK);
    buffer[1] = (Byte) (command >> 8);
    buffer[2] = (Byte) command;
    buffer[3] = (Byte) IT9517Cmd_sequence++;
    buffer[4] = (Byte) writeBufferLength;
    buffer[5] = (Byte) eepromAddress;
    buffer[6] = (Byte) registerAddressLength;
    buffer[7] = (Byte) (registerAddress >> 8);  /** Get high byte of reg. address */
    buffer[8] = (Byte) registerAddress;         /** Get low byte of reg. address  */

    /** add frame data */
    for (i = 0; i < writeBufferLength; i++) {
        buffer[9 + i] = writeBuffer[i];
    }

    /** add frame check-sum */
    bufferLength = 9 + writeBufferLength;
    error = IT9517Cmd_addChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

    /** send frame */
    i = 0;
    sendLength = 0;
    remainLength = bufferLength;
    while (remainLength > 0) {
        i     = (remainLength > IT9510User_MAX_PKT_SIZE) ? (IT9510User_MAX_PKT_SIZE) : (remainLength);        
       	for (cnt = 0; cnt < IT9510User_RETRY_MAX_LIMIT; cnt++) {
			error = IT9510User_busTx (modulator, i, &buffer[sendLength]);
			if (error == 0) break;
			IT9510User_delay (1);
		}
        if (error) goto exit;

        sendLength   += i;
        remainLength -= i;
    }
	
    /** get reply frame */
    bufferLength = 5;
    for (cnt = 0; cnt < IT9510User_RETRY_MAX_LIMIT; cnt++) {
		error = IT9510User_busRx (modulator, bufferLength, buffer);
		if (error == 0) break;
		IT9510User_delay (1);
	}
    if (error) goto exit;

    /** remove check-sum from reply frame */
    error = IT9517Cmd_removeChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

exit :
    IT9510User_leaveCriticalSection ();
    return (error);
}


Dword IT9510_writeRegisterBits (
    IN  IT9510INFO*    modulator,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            position,
    IN  Byte            length,
    IN  Byte            value
)
{
    Dword error = ModulatorError_NO_ERROR;

	Byte temp;

	if (length == 8) {
		error = IT9510_writeRegisters (modulator, processor, registerAddress, 1, &value);
		
	} else {
		error = IT9510_readRegisters (modulator, processor, registerAddress, 1, &temp);
		if (error) goto exit;
		

		temp = (Byte)REG_CREATE (value, temp, position, length);

		error = IT9510_writeRegisters (modulator, processor, registerAddress, 1, &temp);
		if (error) goto exit;
		
	}
exit:

	return (error);
}


Dword IT9510_readRegister (
    IN  IT9510INFO*    modulator,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    OUT Byte*           value
) {
    return (IT9510_readRegisters (modulator, processor, registerAddress, 1, value));
}


Dword IT9510_readRegisters (
    IN  IT9510INFO*    modulator,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            readBufferLength,
    OUT Byte*           readBuffer
) {
    Dword error = ModulatorError_NO_ERROR;
	
	Byte registerAddressLength;
	Word        command;
    Byte        buffer[255];
    Dword       bufferLength;
    Dword       sendLength;
    Dword       remainLength;
    Dword       i, k, cnt;
    
    Byte       maxFrameSize = IT9510User_MAXFRAMESIZE;
		
	if (processor == Processor_LINK) {
		if (registerAddress > 0x000000FF) {
			registerAddressLength = 2;
		} else {
			registerAddressLength = 1;
		}
	} else {
		registerAddressLength = 2;
	}

    if (readBufferLength == 0) goto exit;
    if (registerAddressLength > 4) {
        error  = ModulatorError_PROTOCOL_FORMAT_INVALID;
        goto exit;
    }

    if ((readBufferLength + 5) > IT9510User_MAX_PKT_SIZE) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }

    if ((readBufferLength + 5) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }



    /** add frame header */
    command   = IT9517Cmd_buildCommand (Command_REG_DEMOD_READ, processor);
    buffer[1] = (Byte) (command >> 8);
    buffer[2] = (Byte) command;
    buffer[3] = (Byte) IT9517Cmd_sequence++;
    buffer[4] = (Byte) readBufferLength;
    buffer[5] = (Byte) registerAddressLength;
    buffer[6] = (Byte) (registerAddress >> 24); /** Get first byte of reg. address  */
    buffer[7] = (Byte) (registerAddress >> 16); /** Get second byte of reg. address */
    buffer[8] = (Byte) (registerAddress >> 8);  /** Get third byte of reg. address  */
    buffer[9] = (Byte) (registerAddress);       /** Get fourth byte of reg. address */

    /** add frame check-sum */
    bufferLength = 10;
    error = IT9517Cmd_addChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;


    /** send frame */
    i = 0;
    sendLength   = 0;
    remainLength = bufferLength;
    while (remainLength > 0) {
        i     = (remainLength > IT9510User_MAX_PKT_SIZE) ? (IT9510User_MAX_PKT_SIZE) : (remainLength);        
      	for (cnt = 0; cnt < IT9510User_RETRY_MAX_LIMIT; cnt++) {
			error = IT9510User_busTx (modulator, i, &buffer[sendLength]);
			if (error == 0) break;
			IT9510User_delay (1);
		}
        if (error) goto exit;

        sendLength   += i;
        remainLength -= i;
    }

    /** get reply frame */
    bufferLength = 5 + readBufferLength;

	for (cnt = 0; cnt < IT9510User_RETRY_MAX_LIMIT; cnt++) {
		error = IT9510User_busRx (modulator, bufferLength, buffer);
		if (error == 0) break;
		IT9510User_delay (1);
	}
    if (error) goto exit;

    /** remove check-sum from reply frame */
    error = IT9517Cmd_removeChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

    for (k = 0; k < readBufferLength; k++) {
        readBuffer[k] = buffer[k + 3];
    }
	
exit:
	return (error);
}


Dword IT9510_readGenericRegisters (
    IN  IT9510INFO*    modulator,
    IN  Byte            slaveAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    Byte writeBuffer[3];

	writeBuffer[0] = bufferLength;
	writeBuffer[1] = 2;
	writeBuffer[2] = slaveAddress;

	return (IT9517Cmd_sendCommand (modulator, Command_GENERIC_READ, Processor_LINK, 3, writeBuffer, bufferLength, buffer));
}


Dword IT9510_readEepromValues (
    IN  IT9510INFO*    modulator,
    IN  Word            registerAddress,
    IN  Byte            readBufferLength,
    OUT Byte*           readBuffer
) {
    Dword       error = ModulatorError_NO_ERROR;
    Word        command;
    Byte        buffer[255];
    Dword       bufferLength;
    Dword       remainLength;
    Dword       sendLength;
    Dword       i, k, cnt;
    
    Dword   maxFrameSize;
	Byte	eepromAddress = 0x01;

	Byte	registerAddressLength = 0x01;

	Byte val = 0;
	error = IT9510_readRegister (modulator, Processor_LINK, 0x496D, &val);
    if((val & 0x0F) < 0x08)
        registerAddressLength = 0x01;
    else
        registerAddressLength = 0x02;

    IT9510User_enterCriticalSection ();

    if (readBufferLength == 0) goto exit;

    
    maxFrameSize = IT9510User_MAXFRAMESIZE; 

    if ((Dword)(readBufferLength + 5) > IT9510User_MAX_PKT_SIZE) {
        error  = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }
        
    if ((Dword)(readBufferLength + 5) > maxFrameSize) {
        error  = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }

    /** add command header */
    command   = IT9517Cmd_buildCommand (Command_REG_EEPROM_READ, Processor_LINK);
    buffer[1] = (Byte) (command >> 8);
    buffer[2] = (Byte) command;
    buffer[3] = (Byte) IT9517Cmd_sequence++;
    buffer[4] = (Byte) readBufferLength;
    buffer[5] = (Byte) eepromAddress;
    buffer[6] = (Byte) registerAddressLength;
    buffer[7] = (Byte) (registerAddress >> 8);  /** Get high byte of reg. address */
    buffer[8] = (Byte) registerAddress;         /** Get low byte of reg. address  */

    /** add frame check-sum */
    bufferLength = 9;
    error = IT9517Cmd_addChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

    /** send frame */
    i = 0;
    sendLength   = 0;
    remainLength = bufferLength;
    while (remainLength > 0) {
        i = (remainLength > IT9510User_MAX_PKT_SIZE) ? (IT9510User_MAX_PKT_SIZE) : (remainLength);        
        
		for (cnt = 0; cnt < IT9510User_RETRY_MAX_LIMIT; cnt++) {
			error = IT9510User_busTx (modulator, i, &buffer[sendLength]);
			if (error == 0) break;
			IT9510User_delay (1);
		}
        if (error) goto exit;

        sendLength   += i;
        remainLength -= i;
    }

	IT9510User_delay(5);

    /** get reply frame */
    bufferLength = 5 + readBufferLength;
    
	for (cnt = 0; cnt < IT9510User_RETRY_MAX_LIMIT; cnt++) {
		error = IT9510User_busRx (modulator, bufferLength, buffer);
		if (error == 0) break;
		IT9510User_delay (1);
	}
    if (error) goto exit;

    /** remove frame check-sum */
    error = IT9517Cmd_removeChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

    for (k = 0; k < readBufferLength; k++) {
        readBuffer[k] = buffer[k + 3];
    }

exit :
    IT9510User_leaveCriticalSection ();
    return (error);
}


Dword IT9510_readRegisterBits (
    IN  IT9510INFO*    modulator,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            position,
    IN  Byte            length,
    OUT Byte*           value
) {
        Dword error = ModulatorError_NO_ERROR;
	
	Byte temp = 0;
	error = IT9510_readRegisters (modulator, processor, registerAddress, 1, &temp);
	if (error) goto exit;

	if (length == 8) {
		*value = temp;
	} else {
		temp = REG_GET (temp, position, length);
		*value = temp;
	}

exit :
	return (error);
}


Dword IT9510_getFirmwareVersion (
    IN  IT9510INFO*    modulator,
    IN  Processor       processor,
    OUT Dword*          version
) {
    Dword error = ModulatorError_NO_ERROR;

	Byte writeBuffer[1] = {0,};
	Byte readBuffer[4] = {0,};

	/** Check chip version */
	writeBuffer[0] = 1;
	error = IT9517Cmd_sendCommand (modulator, Command_QUERYINFO, processor, 1, writeBuffer, 4, readBuffer);
	if (error) goto exit;
	
	*version = (Dword) (((Dword) readBuffer[0] << 24) + ((Dword) readBuffer[1] << 16) + ((Dword) readBuffer[2] << 8) + (Dword) readBuffer[3]);

exit :
	return (error);
}



Dword IT9510_loadIrTable (
    IN  IT9510INFO*    modulator,
    IN  Word            tableLength,
    IN  Byte*           table
) {
	Dword error = ModulatorError_NO_ERROR;
	Byte baseHigh;
	Byte baseLow;
	Word registerBase;
	Word i;

	error = IT9510_readRegisters (modulator, Processor_LINK, IT9510_ir_table_start_15_8, 1, &baseHigh);
	if (error) goto exit;
	error = IT9510_readRegisters (modulator, Processor_LINK, IT9510_ir_table_start_7_0, 1, &baseLow);
	if (error) goto exit;

	registerBase = (Word) (baseHigh << 8) + (Word) baseLow;

	if (registerBase) {
		for (i = 0; i < tableLength; i++) {
			error = IT9510_writeRegisters (modulator, Processor_LINK, registerBase + i, 1, &table[i]);
			if (error) goto exit;
		}
	}

exit :
	return (error);
}


Dword IT9510_initialize (
    IN  IT9510INFO*    modulator,
    IN  TsInterface   streamType,
	IN  Byte            busId,
	IN  Byte            i2cAddr
) {

	Dword error = ModulatorError_NO_ERROR;

	Dword version = 0;
	Byte c1_default_value[2],c2_default_value[2],c3_default_value[2],i;
	Byte tempbuf[10];

	modulator->frequency = 642000;	
	modulator->bandwidth = 8000;
	modulator->calibrationInfo.tableVersion =  0;
	modulator->calibrationInfo.ptrIQtableEx =  IQ_fixed_table0;
	modulator->calibrationInfo.tableGroups = IQ_TABLE_NROW;
	modulator->dcInfo.tableGroups = 0;
	modulator->dcInfo.ptrDCtable = NULL;
	modulator->dcInfo.ptrOFStable = NULL;
	modulator->busId = busId; 
	modulator->i2cAddr = i2cAddr;

	modulator->pcrMode = PcrModeDisable;// FPGA test
	modulator->nullPacketMode = NullPacketModeDisable;
	modulator->pcrCalInfo.positive = 0;
	modulator->pcrCalInfo.packetTimeJitter_ps = 0;

	modulator->rfGainInfo.tableIsValid = false;
	modulator->rfGainInfo.tableCount = 0;
	modulator->rfGainInfo.ptrGaintable = NULL;

	error = IT9510User_setBus(modulator);
    if (error) goto exit;

	if (modulator->busId == 0xFF) {
		goto exit;
	}

	error = IT9510_getFirmwareVersion (modulator, Processor_LINK, &version);
	if (error) goto exit;
	if (version != 0) {
		modulator->booted = True;
	} else {
		modulator->booted = False;	
	}

	modulator->firmwareCodes = IT9510Firmware_codes;
	modulator->firmwareSegments = IT9510Firmware_segments;
	modulator->firmwarePartitions = IT9510Firmware_partitions;
	modulator->scriptSets = IT9510Firmware_scriptSets;
	modulator->scripts = IT9510Firmware_scripts;
	
	/** Write secondary I2C address to device */
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_lnk2ofdm_data_63_56, IT9510User_IIC_SPEED);
	if (error) goto exit;	
	/** Load firmware */
	if (modulator->firmwareCodes != NULL) {
		if (modulator->booted == False) {
			error = IT9510_loadFirmware (modulator, modulator->firmwareCodes, modulator->firmwareSegments, modulator->firmwarePartitions);
			if (error) goto exit;
			modulator->booted = True;
		}
	}
	error = IT9510_writeRegister (modulator, Processor_LINK, 0xD924, 0);//set UART -> GPIOH4
	if (error) goto exit;


	/** Set I2C master clock 100k in order to support tuner I2C. */
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_lnk2ofdm_data_63_56, IT9510User_IIC_SPEED);//1a
	if (error) goto exit;

	/** Load script */
	if (modulator->scripts != NULL) {
		error = IT9510_loadScript (modulator, modulator->scriptSets, modulator->scripts);
		if (error) goto exit;
	}

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tsin_en, 1);
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_mpeg_stop_en, 0);
	if (error) goto exit;

	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_afe_mem2, 7, 1, 1); //CKO on
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_afe_mem22, 0xE0);
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_mp2if_ignore_sync_byte, 0);
	if (error) goto exit;

	for(i=0;i<10;i++)
		tempbuf[i] = 0;
	
	error = IT9510_writeRegisters (modulator, Processor_OFDM, IT9510_psi_table0_timer_H, 10, tempbuf);		//stop send FW psi table	
	if (error) goto exit;


	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_en, 0);//gpiox_en
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_on, 1);//gpiox_on
	if (error) goto exit;


	// patch 0xF424
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_repeat_start, 0);
	if (error) goto exit;
	
	/** Set the desired stream type */
	error = IT9510_setTsInterface (modulator, streamType);
	if (error) goto exit;

	error = IT9510User_Initialization(modulator);
	if (error) goto exit;

	/** Set H/W MPEG2 locked detection **/
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_lock3_out, 1);
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_padmiscdrsr, 1);
	if (error) goto exit;
	/** Set registers for driving power 0xD830 **/
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_padmiscdr2, 0);
	if (error) goto exit;
	

	/** Set registers for driving power 0xD831 **/
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_padmiscdr4, 0);
	if (error) goto exit;

	/** Set registers for driving power 0xD832 **/
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_padmiscdr8, 0);
	if (error) goto exit; 

	/** Set PLL **/
	error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBC2, 0x06);
	if (error) goto exit;   
	error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBC5, 0x33);
	if (error) goto exit;   
	error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBCE, 0x1B);
	if (error) goto exit;   
	error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBD7, 0x3B);
	if (error) goto exit;   

	error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFB2E, 0x11);
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBB3, 0x98);
	if (error) goto exit;
	
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_ts_fail_ignore, 1);
	if (error) goto exit;

	error = IT9510_readRegisters (modulator, Processor_OFDM, p_IT9510_reg_iqik_c1_7_0, 2, c1_default_value);
	if (error) goto exit;


	error = IT9510_readRegisters (modulator, Processor_OFDM, p_IT9510_reg_iqik_c2_7_0, 2, c2_default_value);
	if (error) goto exit;
	error = IT9510_readRegisters (modulator, Processor_OFDM, p_IT9510_reg_iqik_c3_7_0, 2, c3_default_value);
	if (error) goto exit;


	modulator->calibrationInfo.c1DefaultValue = c1_default_value[1]<<8 | c1_default_value[0];
	modulator->calibrationInfo.c2DefaultValue = c2_default_value[1]<<8 | c2_default_value[0];
	modulator->calibrationInfo.c3DefaultValue = c3_default_value[1]<<8 | c3_default_value[0];

exit:

	return (error);
}



Dword IT9510_finalize (
    IN  IT9510INFO*    modulator
) {
	Dword error = ModulatorError_NO_ERROR;

	error = IT9510User_Finalize(modulator);

	return (error);
}


Dword IT9510_reset (
    IN  IT9510INFO*    modulator
) {
	Dword error = ModulatorError_NO_ERROR;

	Byte value;
	Byte j;
	/** Enable OFDM reset */
	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, I2C_IT9510_reg_ofdm_rst_en, IT9510_reg_ofdm_rst_en_pos, IT9510_reg_ofdm_rst_en_len, 0x01);
	if (error) goto exit;

	/** Start reset mechanism */
	value = 0x00;
	
	/** Clear ofdm reset */
	for (j = 0; j < 150; j++) {
		error = IT9510_readRegisterBits (modulator, Processor_OFDM, I2C_IT9510_reg_ofdm_rst, IT9510_reg_ofdm_rst_pos, IT9510_reg_ofdm_rst_len, &value);
		if (error) goto exit;
		if (value) break;
		IT9510User_delay (10);
	}

	if (j == 150) {
		error = ModulatorError_RESET_TIMEOUT;
		goto exit;
	}

	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, I2C_IT9510_reg_ofdm_rst, IT9510_reg_ofdm_rst_pos, IT9510_reg_ofdm_rst_len, 0);
	if (error) goto exit;

	/** Disable OFDM reset */
	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, I2C_IT9510_reg_ofdm_rst_en, IT9510_reg_ofdm_rst_en_pos, IT9510_reg_ofdm_rst_en_len, 0x00);
	if (error) goto exit;
	

exit :

	return (error);
}



Dword IT9510_setTXChannelModulation (
    IN  IT9510INFO*            modulator,
    IN  ChannelModulation*      channelModulation
) {
	Dword error = ModulatorError_NO_ERROR;
	Byte temp;
	TPS  tps;
	modulator->outputMode=DVBT; //DVBT
	
	error = IT9510_setTxModeEnable(modulator,0);
	if (error) goto exit;
	/** Set constellation type */
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_phy_is_dvb, 1); //DVBT
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_inp_sel, 0); //DVBT
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_sync_byte_inv, 1); //DVBT ?????
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_intlv_sym_th_7_0, 0xE6); //DVBT
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_intlv_sym_th_10_8, 0x05); //DVBT
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, 0xF762, 0x01); //DVBT spectrum inverse = normal
	if (error) goto exit;


	modulator->channelModulation.constellation=channelModulation->constellation;
	temp=(Byte)channelModulation->constellation;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_const, temp);
	if (error) goto exit;

	modulator->channelModulation.highCodeRate=channelModulation->highCodeRate;
	temp=(Byte)channelModulation->highCodeRate;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tps_hpcr, temp);
	if (error) goto exit;
	/** Set low code rate */

	/** Set guard interval */
	modulator->channelModulation.interval=channelModulation->interval;
	temp=(Byte)channelModulation->interval;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tps_gi, temp);
	if (error) goto exit;
	/** Set FFT mode */
	modulator->channelModulation.transmissionMode=channelModulation->transmissionMode;
	temp=(Byte)channelModulation->transmissionMode;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tps_txmod, temp);
	if (error) goto exit;


	switch (channelModulation->interval){
		case Interval_1_OVER_32:              
			temp = 8;
			break;
		case Interval_1_OVER_16:            
			temp = 4;			
			break;
		case Interval_1_OVER_8:            
			temp = 2;			
			break;
		case Interval_1_OVER_4:             
			temp = 1;
			break;

		default:
			
			error = ModulatorError_INVALID_CONSTELLATION_MODE;
			
			break;
	}

	if(error)
		goto exit;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_dc_shift_tones, temp);
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_phase_shift_per_symbol, 1);
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_null_mode, (Byte)modulator->nullPacketMode);
	if (error) goto exit;	

	tps.constellation = (Byte)channelModulation->constellation;
	tps.highCodeRate = (Byte)channelModulation->highCodeRate;
	tps.lowCodeRate = (Byte)channelModulation->highCodeRate;
	tps.interval = (Byte)channelModulation->interval;
	tps.transmissionMode = (Byte)channelModulation->transmissionMode;
	error = IT9510_setTPS(modulator, tps, True);

exit :
	return (error);
}


Dword IT9510_setISDBTChannelModulation (
	IN  IT9510INFO*          modulator,
	IN  ISDBTModulation      isdbtModulation
) {
	Dword error = ModulatorError_NO_ERROR;

	Byte temp;
	TMCCINFO      TmccInfo;
	modulator->outputMode=ISDBT;//ISDBT
	
	error = IT9510_setTxModeEnable(modulator,0);
	if (error) goto exit;


	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_intlv_sym_th_7_0, 0xDC); //ISDBT
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_intlv_sym_th_10_8, 0x04); //ISDBT
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_sync_byte_inv, 0); //ISDBT 
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_phy_is_dvb, 0); //ISDBT
	if (error) goto exit;
	
	error = IT9510_writeRegister (modulator, Processor_OFDM, 0xF762, 0x01); //ISDBT spectrum inverse = normal
	if (error) goto exit;
	
	modulator->isdbtModulation.isPartialReception = isdbtModulation.isPartialReception;
	/** set layer A constellation */
	modulator->isdbtModulation.layerA.constellation=isdbtModulation.layerA.constellation;
	temp=(Byte)isdbtModulation.layerA.constellation + 1;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_const, temp);
	if (error) goto exit;

	/** Set guard interval */
	modulator->isdbtModulation.interval=isdbtModulation.interval;
	temp=(Byte)isdbtModulation.interval;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tps_gi, temp); 
	if (error) goto exit;

	/** Set FFT mode */
	modulator->isdbtModulation.transmissionMode=isdbtModulation.transmissionMode;
	temp=(Byte)isdbtModulation.transmissionMode;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tps_txmod, temp);
	if (error) goto exit;


	
	/** set layer A coderate */
	modulator->isdbtModulation.layerA.codeRate=isdbtModulation.layerA.codeRate;
	temp=(Byte)isdbtModulation.layerA.codeRate;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tps_hpcr, temp);
	if (error) goto exit;


	/** set Down Sample Rate */
	//modulator->isdbtModulation.ds=isdbtModulation->ds;
	//temp=isdbtModulation->ds;
	//if(temp)
	//	temp = 0x03; 
	//error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_intp_ds, temp);
	//if (error) goto exit;

		
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_inp_sel, 1); //ISDBT
	if (error) goto exit;


	/** set layer A interLeaving Length */ 
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_intlvlen_a, 0);
	if (error) goto exit;

	error = IT9510_writeRegister(modulator, Processor_OFDM, 0xF7D5, 7);
	if (error) goto exit;

	/** set Partial Reception */
	modulator->isdbtModulation.isPartialReception=isdbtModulation.isPartialReception;
	if(isdbtModulation.isPartialReception){
		temp = 1;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmcc_partial_recp, temp);
		if (error) goto exit;

		/** set layer B coderate */
		modulator->isdbtModulation.layerB.codeRate=isdbtModulation.layerB.codeRate;
		temp=(Byte)isdbtModulation.layerB.codeRate;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmcc_cr_b, temp);
		if (error) goto exit;

		/** set layer B constellation */
		modulator->isdbtModulation.layerB.constellation=isdbtModulation.layerB.constellation;
		temp=(Byte)isdbtModulation.layerB.constellation + 1;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmcc_carmod_b, temp);
		if (error) goto exit;

		/** set layer B interLeaving Length */
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_intlvlen_b, 0);
		if (error) goto exit;


	}else{
		temp = 0;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmcc_partial_recp, temp);
		if (error) goto exit;

		/** set layer B coderate to unused */
		modulator->isdbtModulation.layerB.codeRate=(CodeRate)0x07;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmcc_cr_b, 0x07);
		if (error) goto exit;

		/** set layer B constellation to unused*/
		modulator->isdbtModulation.layerB.constellation=(Constellation)0x07;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmcc_carmod_b, 0x07);
		if (error) goto exit;

		/** set layer B interLeaving Length to unused */
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_intlvlen_b, 0x07);
		if (error) goto exit;
	}

	switch (isdbtModulation.interval){
		case Interval_1_OVER_32:              
			temp = 8;
			break;
		case Interval_1_OVER_16:            
			temp = 4;			
			break;
		case Interval_1_OVER_8:            
			temp = 2;			
			break;
		case Interval_1_OVER_4:             
			temp = 1;
			break;

		default:
			
			error = ModulatorError_INVALID_CONSTELLATION_MODE;
			
			break;
	}

	if(error)
		goto exit;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_dc_shift_tones, temp);
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_phase_shift_per_symbol, 1);
	if (error) goto exit;
	

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_null_mode, (Byte)modulator->nullPacketMode);
	if (error) goto exit;


	TmccInfo.systemIdentification = ARIB_STD_B31;	
	TmccInfo.isPartialReception = isdbtModulation.isPartialReception;
	TmccInfo.layerA = isdbtModulation.layerA;
	TmccInfo.layerB = isdbtModulation.layerB;
	error = IT9510_setTMCCInfo(modulator, TmccInfo, True);

exit :
	return (error);
}

Dword IT9510_setTxModeEnable (
    IN  IT9510INFO*            modulator,
    IN  Byte                    enable
) {
	Dword error = ModulatorError_NO_ERROR;


	if(enable){
		error = IT9510User_rfPowerOn(modulator, True);
		if (error) goto exit;
		if((modulator->outputMode ==ISDBT)&&(modulator->bandwidth == 6000)){
			//temp5 = 3;
			error = IT9510_writeRegister (modulator,  Processor_OFDM, p_IT9510_reg_intp_ds, 3);
			if (error) goto exit;
			
		}else if((modulator->outputMode ==ISDBT)&&(modulator->bandwidth == 7000)){
			//temp5 = 4;
			error = IT9510_writeRegister (modulator,  Processor_OFDM, p_IT9510_reg_intp_ds, 4);
			if (error) goto exit;			
		}
		//afe Power up
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_afe_mem0, 0);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_afe_mem1, 0x0); //org FC
		if (error) goto exit;
					
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_fec_sw_rst, 0);
		if (error) goto exit;

		error = IT9510_writeRegister (modulator, Processor_LINK, 0xDDAB, 0);
		if (error) goto exit;
		
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tsin_en, 1);
		if (error) goto exit;


		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_null_mode, 0); //set null packet
		if (error) goto exit;

		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_null_mode, (Byte)modulator->nullPacketMode); //set null packet
		if (error) goto exit;

		error = IT9510_setPcrModeEnable(modulator, enable);
		if (error) goto exit;
		error = IT9510User_setTxModeEnable(modulator, enable);
		if (error) goto exit;	


		/* RFFC2072 issue. Add by JK.*/
#if RF_RELOCK_MONITOR
		if(modulator->deviceType == 0x9C) 
			modulator->start_detect_relock_status = 1;
#endif
	}else{
		
		//error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tsin_en, 0);
		//if (error) goto exit;

		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_null_mode, 0); // stop null packet
		if (error) goto exit;

		error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_ep6_addr_reset, 1);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_fec_sw_rst, 1);
		if (error) goto exit;

		//afe Power down
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_afe_mem0, 1);
		if (error) goto exit;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_afe_mem1, 0xFE);
		if (error) goto exit;

		error = IT9510_setPcrModeEnable(modulator, enable);
		if (error) goto exit;
		error = IT9510User_setTxModeEnable(modulator, enable);
		if (error) goto exit;
		error = IT9510User_rfPowerOn(modulator, False);
		if (error) goto exit;

		/* RFFC2072 issue. Add by JK.*/
#if RF_RELOCK_MONITOR
		if(modulator->deviceType == 0x9C) 
			modulator->start_detect_relock_status =0;
#endif
	}

	
exit :
	IT9510User_delay(100);
	return (error);
}

Dword IT9510_acquireTxChannel (
	IN  IT9510INFO*            modulator,
    IN  Word            bandwidth,
    IN  Dword           frequency
) {
	Dword error = ModulatorError_NO_ERROR;
	
	error = IT9510_selectBandwidth (modulator, bandwidth);
	if (error) goto exit;
	modulator->bandwidth = bandwidth;
	
	/** Set frequency */
	
	error = IT9510_setFrequency (modulator, frequency);
	if (error) goto exit;
	
	error = IT9510User_acquireChannel(modulator, bandwidth, frequency);
exit :
	return (error);
}

Dword IT9510_acquireTxChannelDual(
	IN  IT9510INFO*    modulator,
	IN  Word          bandwidth,
	IN  Dword         frequency1,
	IN  Dword         frequency2
	)
{
	/*
	*  ToDo:  Add code here
	*
	*  // If no error happened return 0, else return error code.
	*  return (0);
	*/
	Dword error = ModulatorError_NO_ERROR;
	Dword LoFrequency, EagleFreq;

	if (frequency2 > frequency1)
	{
		LoFrequency = frequency2 - frequency1;
		EagleFreq = frequency1;
	}
	else
	{
		LoFrequency = frequency1 - frequency2;
		EagleFreq = frequency2;
	}

	error = IT9510_selectBandwidth(modulator, bandwidth);
	if (error) goto exit;
	modulator->bandwidth = bandwidth;

	/** Set frequency */

	error = IT9510_setFrequency(modulator, EagleFreq);
	if (error) goto exit;

	error = IT9510User_acquireChannelDual(modulator, bandwidth, LoFrequency);

exit:

	return (error);
}

Dword IT9510_resetPSBBuffer (
	IN  IT9510INFO*    modulator
){
	Dword error = ModulatorError_NO_ERROR;
	Dword temp;

	if(modulator->tsInterfaceType == PARALLEL_TS_INPUT)
		temp = 0xF9CC;
	else
		temp = 0xF9CD;

	error = IT9510_writeRegister (modulator, Processor_OFDM, 0xF9A4, 1);
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, temp, 0);
	if (error) goto exit;



	error = IT9510_writeRegister (modulator, Processor_OFDM, 0xF9A4, 0);
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, temp, 1);

exit :


	return (error);
}


Dword IT9510_setTsInterface (
    IN  IT9510INFO*    modulator,
    IN  TsInterface   streamType
) {
    Dword error = ModulatorError_NO_ERROR;
	Word frameSize;
	Byte buffer[2];
	Byte packetSize_EP4;
	Byte packetSize_EP5;

	/** Enable DVB-T interrupt if next stream type is StreamType_DVBT_DATAGRAM */	
	error = IT9510_writeRegisterBits (modulator, Processor_LINK, p_IT9510_reg_dvbt_inten, IT9510_reg_dvbt_inten_pos, IT9510_reg_dvbt_inten_len, 1);
	if (error) goto exit;
	/** Enable DVB-T mode */
	error = IT9510_writeRegisterBits (modulator, Processor_LINK, p_IT9510_reg_dvbt_en, IT9510_reg_dvbt_en_pos, IT9510_reg_dvbt_en_len, 1);
	if (error) goto exit;
	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_mpeg_ser_mode, mp2if_mpeg_ser_mode_pos, mp2if_mpeg_ser_mode_len, 0);
	if (error) goto exit;
	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_mpeg_par_mode, mp2if_mpeg_par_mode_pos, mp2if_mpeg_par_mode_len, 0);
	if (error) goto exit;
	
	error = IT9510_writeRegister (modulator, Processor_OFDM, 0xF714, 0);
	if (error) goto exit;

	packetSize_EP4 = (Byte) (IT9510User_USB20_MAX_PACKET_SIZE_EP4 / 4);
	packetSize_EP5 = (Byte) (IT9510User_USB20_MAX_PACKET_SIZE_EP5 / 4);

	
	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_mp2_sw_rst, IT9510_reg_mp2_sw_rst_pos, IT9510_reg_mp2_sw_rst_len, 1);
	if (error) goto exit;

	/** Reset EP4 */
	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_mp2if2_sw_rst, IT9510_reg_mp2if2_sw_rst_pos, IT9510_reg_mp2if2_sw_rst_len, 1);
	if (error) goto exit;

	
	/** Disable EP4 */
	error = IT9510_writeRegisterBits (modulator, Processor_LINK, p_IT9510_reg_ep4_tx_en, IT9510_reg_ep4_tx_en_pos, IT9510_reg_ep4_tx_en_len, 0);
	if (error) goto exit;

	/** Disable ep4 NAK */
	error = IT9510_writeRegisterBits (modulator, Processor_LINK, p_IT9510_reg_ep4_tx_nak, IT9510_reg_ep4_tx_nak_pos, IT9510_reg_ep4_tx_nak_len, 0);
	if (error) goto exit;

	/** Reset EP5 */
	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_mp2if2_sw_rst, IT9510_reg_mp2if2_sw_rst_pos, IT9510_reg_mp2if2_sw_rst_len, 1);
	if (error) goto exit;

	
	/** Disable EP5 */
	error = IT9510_writeRegisterBits (modulator, Processor_LINK, p_IT9510_reg_ep5_tx_en, IT9510_reg_ep5_tx_en_pos, IT9510_reg_ep5_tx_en_len, 0);
	if (error) goto exit;

	/** Disable EP5 NAK */
	error = IT9510_writeRegisterBits (modulator, Processor_LINK, p_IT9510_reg_ep5_tx_nak, IT9510_reg_ep5_tx_nak_pos, IT9510_reg_ep5_tx_nak_len, 0);
	if (error) goto exit;

	// Enable ep4 /
	error = IT9510_writeRegisterBits (modulator, Processor_LINK, p_IT9510_reg_ep4_tx_en, IT9510_reg_ep4_tx_en_pos, IT9510_reg_ep4_tx_en_len, 1);
	if (error) goto exit;

	// Set ep4 transfer length /
	frameSize = IT9510User_USB20_FRAME_SIZE_EP4/4;
	buffer[p_IT9510_reg_ep4_tx_len_7_0 - p_IT9510_reg_ep4_tx_len_7_0] = (Byte) frameSize;
	buffer[p_IT9510_reg_ep4_tx_len_15_8 - p_IT9510_reg_ep4_tx_len_7_0] = (Byte) (frameSize >> 8);
	error = IT9510_writeRegisters (modulator, Processor_LINK, p_IT9510_reg_ep4_tx_len_7_0, 2, buffer);

	// Set ep4 packet size /
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_ep4_max_pkt, packetSize_EP4);
	if (error) goto exit;

	// Enable EP5
	error = IT9510_writeRegisterBits (modulator, Processor_LINK, p_IT9510_reg_ep5_tx_en, IT9510_reg_ep5_tx_en_pos, IT9510_reg_ep5_tx_en_len, 1);
	if (error) goto exit;

	// Set EP5 transfer length
	frameSize = IT9510User_USB20_FRAME_SIZE_EP5/4;
	buffer[p_IT9510_reg_ep5_tx_len_7_0 - p_IT9510_reg_ep5_tx_len_7_0] = (Byte) frameSize;
	buffer[p_IT9510_reg_ep5_tx_len_15_8 - p_IT9510_reg_ep5_tx_len_7_0] = (Byte) (frameSize >> 8);
	error = IT9510_writeRegisters (modulator, Processor_LINK, p_IT9510_reg_ep5_tx_len_7_0, 2, buffer);

	// Set EP5 packet size 
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_ep5_max_pkt, packetSize_EP5);
	if (error) goto exit;

	/** Disable 15 SER/PAR mode */
	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_mpeg_ser_mode, mp2if_mpeg_ser_mode_pos, mp2if_mpeg_ser_mode_len, 0);
	if (error) goto exit;
	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_mpeg_par_mode, mp2if_mpeg_par_mode_pos, mp2if_mpeg_par_mode_len, 0);
	if (error) goto exit;

	
	/** Enable mp2if2 */
	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_mp2if2_en, IT9510_reg_mp2if2_en_pos, IT9510_reg_mp2if2_en_len, 1);
	if (error) goto exit;

	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_tsin_en, IT9510_reg_tsin_en_pos, IT9510_reg_tsin_en_len, 1);
	if (error) goto exit;
	if(streamType == PARALLEL_TS_INPUT){
	/** Enable tsip */
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_ts_in_src, 1);
		if (error) goto exit;
	}else{
		/** Enable tsis */
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_ts_in_src, 0);
		if (error) goto exit;
	}

	/** Negate EP4 reset */
	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_mp2_sw_rst, IT9510_reg_mp2_sw_rst_pos, IT9510_reg_mp2_sw_rst_len, 0);
	if (error) goto exit;

	/** Negate ep4 reset */
	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_mp2if2_sw_rst, IT9510_reg_mp2if2_sw_rst_pos, IT9510_reg_mp2if2_sw_rst_len, 0);
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_host_reverse, 0);
	if (error) goto exit;

	error = IT9510_writeRegisterBits (modulator, Processor_LINK, p_IT9510_reg_ep6_rx_en, IT9510_reg_ep6_rx_en_pos, IT9510_reg_ep6_rx_en_len, 0);
	if (error) goto exit;

	error = IT9510_writeRegisterBits (modulator, Processor_LINK, p_IT9510_reg_ep6_rx_nak, IT9510_reg_ep6_rx_nak_pos, IT9510_reg_ep6_rx_nak_len, 0);
	if (error) goto exit;


	error = IT9510_writeRegisterBits (modulator, Processor_LINK, p_IT9510_reg_ep6_rx_en, IT9510_reg_ep6_rx_en_pos, IT9510_reg_ep6_rx_en_len, 1);
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_ep6_max_pkt, 0x80);
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_ep6_cnt_num_7_0, 0x16);
	if (error) goto exit;

	error = IT9510User_mpegConfig (modulator);

	modulator->tsInterfaceType = streamType;

exit :

	return (error);
}




Dword IT9510_getIrCode (
    IN  IT9510INFO*    modulator,
    OUT Dword*          code
)  {
    Dword error = ModulatorError_NO_ERROR;
	Byte readBuffer[4];

	error = IT9517Cmd_sendCommand (modulator, Command_IR_GET, Processor_LINK, 0, NULL, 4, readBuffer);	
	if (error) goto exit;

	*code = (Dword) ((readBuffer[0] << 24) + (readBuffer[1] << 16) + (readBuffer[2] << 8) + readBuffer[3]);

exit :
	return (error);
}


Dword IT9510_TXreboot (
    IN  IT9510INFO*    modulator
)  {
	Dword error = ModulatorError_NO_ERROR;
	Dword version;
	Byte i;
	
	error = IT9510_getFirmwareVersion (modulator, Processor_LINK, &version);
	if (error) goto exit;
	if (version == 0xFFFFFFFF) goto exit;       
	if (version != 0) {
		
		error = IT9517Cmd_reboot (modulator);
		IT9510User_delay (1);
		if (error) goto exit;		
		
		if (modulator->busId == Bus_USB) 
			goto exit;

		IT9510User_delay (10);

		version = 1;
		for (i = 0; i < 30; i++) {
			error = IT9510_getFirmwareVersion (modulator, Processor_LINK, &version);
			if (error == ModulatorError_NO_ERROR) break;
			IT9510User_delay (10);
		}
		if (error) 
			goto exit;
		
		if (version != 0)
			error = ModulatorError_REBOOT_FAIL;
	}

	modulator->booted = False;

exit :
	return (error);
}


Dword IT9510_controlPowerSaving (
    IN  IT9510INFO*    modulator,
    IN  Byte          control
) {
    Dword error = ModulatorError_NO_ERROR;
	Byte temp;
		
	if (control) {
		/** Power up case */
		if (modulator->busId == Bus_USB) {
			error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_afe_mem0, 3, 1, 0);
			if (error) goto exit;
			temp = 0;
			error = IT9510_writeRegisters (modulator, Processor_OFDM, p_IT9510_reg_dyn0_clk, 1, &temp);
			if (error) goto exit;
		} 
		
	} else {
		/** Power down case */
		if (modulator->busId == Bus_USB) {
			
			error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_afe_mem0, 3, 1, 1);
		
		} 

	}

exit :
	return (error);
}




Dword IT9510_controlPidFilter (
    IN  IT9510INFO*    modulator,
    IN  Byte            control,
	IN  Byte            enable
) {
	Dword error = ModulatorError_NO_ERROR;

	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_pid_complement, mp2if_pid_complement_pos, mp2if_pid_complement_len, control);
	if(error) goto exit;
	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_pid_en, mp2if_pid_en_pos, mp2if_pid_en_len, enable);

exit:
	return (error);
}


Dword IT9510_resetPidFilter (
    IN  IT9510INFO*    modulator
) {
	Dword error = ModulatorError_NO_ERROR;

	error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_pid_rst, mp2if_pid_rst_pos, mp2if_pid_rst_len, 1);
	if (error) goto exit;

exit :
	return (error);
}


Dword IT9510_addPidToFilter (
    IN  IT9510INFO*    modulator,
    IN  Byte            index,
    IN  Pid             pid
) {
   	Dword error = ModulatorError_NO_ERROR;

	Byte writeBuffer[2];
	
	/** Enable pid filter */
	if((index>0)&&(index<32)){
		writeBuffer[0] = (Byte) pid.value;
		writeBuffer[1] = (Byte) (pid.value >> 8);
	
		error = IT9510_writeRegisters (modulator, Processor_OFDM, p_mp2if_pid_dat_l, 2, writeBuffer);
		if (error) goto exit;
	
		error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_mp2if_pid_index_en, IT9510_mp2if_pid_index_en_pos, IT9510_mp2if_pid_index_en_len, 1);
		if (error) goto exit;
	
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_mp2if_pid_index, index);
		if (error) goto exit;
	}else{
		error = ModulatorError_INDEX_OUT_OF_RANGE;
	}
exit :

	return (error);
}

Dword IT9510_addPidToISDBTPidFilter (
    IN  IT9510INFO*    modulator,
    IN  Byte            index,
    IN  Pid             pid,
	IN	TransportLayer  layer
) {
    Dword error = ModulatorError_NO_ERROR;

	Byte writeBuffer[2];

	/** Enable pid filter */
	
	writeBuffer[0] = (Byte) pid.value;
	writeBuffer[1] = (Byte) (pid.value >> 8);

	error = IT9510_writeRegisters (modulator, Processor_OFDM, p_mp2if_pid_dat_l, 2, writeBuffer);
	if (error) goto exit;

	if(modulator->isdbtModulation.isPartialReception == True)
		error = IT9510_writeRegister(modulator, Processor_OFDM, p_IT9510_mp2if_pid_index_en, (Byte)layer);
	else
		error = IT9510_writeRegister(modulator, Processor_OFDM, p_IT9510_mp2if_pid_index_en, 1);

	if (error) goto exit;


	error = IT9510_writeRegister (modulator, Processor_OFDM, p_mp2if_pid_index, index);
	if (error) goto exit;

exit :

	return (error);
}

Dword IT9510_sendHwPSITable (
	IN  IT9510INFO*    modulator,
	IN  Byte*            pbuffer
) {
 	Dword error = ModulatorError_NO_ERROR;
	Byte temp_timer[10];
	Byte tempbuf[10] ;
	Byte i,temp, temp2;
	//Byte prePIStable[188] ;
	
	error = IT9510_readRegisters (modulator, Processor_OFDM, IT9510_psi_table0_timer_H, 10, temp_timer);		//save psi table timer	
	if (error) goto exit;
	
	for(i=0;i<10;i++)
		tempbuf[i] = 0;

	
	error = IT9510_writeRegisters (modulator, Processor_OFDM, IT9510_psi_table0_timer_H, 10, tempbuf);		//stop send FW psi table	
	if (error) goto exit;
	IT9510User_delay(1);
	//error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_psi_tbl_sel, 0); // select table 0 
	//if (error) goto exit;

	for(i=0 ; i<50 ;i++){
		error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_psi_access, &temp);		//wait per table send	
		if (error) goto exit;
		
		error = IT9510_readRegister (modulator, Processor_OFDM, IT9510_Packet_Insertion_Flag, &temp2);		//wait per table send	
		if (error) goto exit;
		if(temp == 0 && temp2 == 0) break;
		IT9510User_delay(1);
	}
	
//------------------------------------------------------------------------------------------------------	

	error = IT9510_writeRegister (modulator, Processor_OFDM, IT9510_Packet_Insertion_Flag, 0); // stop insertion 
	if (error) goto exit;

	for(i=0;i<4;i++){
		error = IT9510_writeRegisters (modulator, Processor_OFDM, Insertion_table0 + (47*i), 47, &pbuffer[i*47]); //write data to HW psi table buffer
		if (error) goto exit;
	}

	IT9510User_delay(1);
	error = IT9510_writeRegister (modulator, Processor_OFDM, IT9510_Packet_Insertion_Flag, 1); // stop insertion 
	if (error) goto exit;

	for(i=0 ; i<50 ;i++){
		error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_psi_access, &temp);		//wait per table send	
		if (error) goto exit;
		
		error = IT9510_readRegister (modulator, Processor_OFDM, IT9510_Packet_Insertion_Flag, &temp2);		//wait per table send	
		if (error) goto exit;
		if(temp == 0 && temp2 == 0) break;
		IT9510User_delay(1);
	}
	
//--------------------------------------------------------------------------------------------------------------	

	error = IT9510_writeRegisters (modulator, Processor_OFDM, IT9510_psi_table0_timer_H, 10, temp_timer);		//set org timer	
	if (error) goto exit;
	
	

exit :

	return (error);
}

Dword IT9510_accessFwPSITable (
	IN  IT9510INFO*    modulator,
	IN  Byte		  psiTableIndex,
	IN  Byte*         pbuffer
) {
	Dword error ;
	Byte i;
	Byte temp[2];
	Byte temp_timer[10];
	Byte tempbuf[10] ;
	error = ModulatorError_NO_ERROR;
	
	temp[0] = 0;
	temp[1] = 0;
	for(i=0;i<10;i++)
		tempbuf[i] = 0;
	
	if((psiTableIndex>=0)&&(psiTableIndex<5)){
		error = IT9510_writeRegisters (modulator, Processor_OFDM, IT9510_psi_table0_timer_H+(psiTableIndex)*2, 2, temp);		//set timer	= 0 & stop
		if (error) goto exit;

		error = IT9510_readRegisters (modulator, Processor_OFDM, IT9510_psi_table0_timer_H, 10, temp_timer);		//save psi table timer	
		if (error) goto exit;

		error = IT9510_writeRegisters (modulator, Processor_OFDM, IT9510_psi_table0_timer_H, 10, tempbuf);		//stop send FW psi table	
		if (error) goto exit;

		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_psi_tbl_sel, psiTableIndex); // select table  
		if (error) goto exit;

		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_psi_index, 0); 
		if (error) goto exit;

		if(psiTableIndex == 0){
			for(i=0;i<4;i++){
				error = IT9510_writeRegisters (modulator, Processor_OFDM, PSI_table0 + (47*i), 47, &pbuffer[i*47]); //write data to HW psi table buffer
				if (error) goto exit;
			}
		} else {
			for(i=0;i<188;i++){
				error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_psi_dat,  pbuffer[i]); //write data to HW psi table buffer
				if (error) goto exit;
			}
		}

		error = IT9510_writeRegisters (modulator, Processor_OFDM, IT9510_psi_table0_timer_H, 10, temp_timer);		//set org timer	
		if (error) goto exit;

	}else{
		error = ModulatorError_INVALID_INDEX;
	}
	
exit :

	return (error);
}

Dword IT9510_setFwPSITableTimer (
	IN  IT9510INFO*    modulator,
	IN  Byte		  psiTableIndex,
	IN  Word          timer_ms
) {
    Dword error ;
	Byte temp[2];
	error = ModulatorError_NO_ERROR;

	temp[0] = (Byte)(timer_ms>>8);
	temp[1] = (Byte)timer_ms;
	


	if((psiTableIndex>=0)&&(psiTableIndex<5)){	
		error = IT9510_writeRegisters (modulator, Processor_OFDM, IT9510_psi_table0_timer_H+(psiTableIndex)*2, 2,temp);				
	}else{
		error = ModulatorError_INVALID_INDEX;
	}	
	return (error);
}


Dword IT9510_setSlaveIICAddress (
    IN  IT9510INFO*    modulator,
	IN  Byte          SlaveAddress
){
	Dword error = ModulatorError_NO_ERROR;

	if(modulator != NULL)
		modulator->slaveIICAddr = SlaveAddress;
	else
		error  = ModulatorError_NULL_HANDLE_PTR;
    return (error);
}

Dword IT9510_runTxCalibration (
	IN  IT9510INFO*    modulator,
	IN  Word            bandwidth,
    IN  Dword           frequency
){
	Dword error = ModulatorError_NO_ERROR;
	Byte c1_default_value[2],c2_default_value[2],c3_default_value[2];

	if((bandwidth !=0) && (frequency !=0)){
		error = IT9510Tuner_setIQCalibration(modulator,frequency);		
	}else{
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}
	if (error) goto exit;
	error = IT9510_readRegisters (modulator, Processor_OFDM, p_IT9510_reg_iqik_c1_7_0, 2, c1_default_value);
	if (error) goto exit;
	error = IT9510_readRegisters (modulator, Processor_OFDM, p_IT9510_reg_iqik_c2_7_0, 2, c2_default_value);
	if (error) goto exit;
	error = IT9510_readRegisters (modulator, Processor_OFDM, p_IT9510_reg_iqik_c3_7_0, 2, c3_default_value);
	if (error) goto exit;
	
	modulator->calibrationInfo.c1DefaultValue = c1_default_value[1]<<8 | c1_default_value[0];
	modulator->calibrationInfo.c2DefaultValue = c2_default_value[1]<<8 | c2_default_value[0];
	modulator->calibrationInfo.c3DefaultValue = c3_default_value[1]<<8 | c3_default_value[0];
	modulator->calibrationInfo.outputGain = 0;

exit:
	return (error);
}

Dword IT9510_setIQValue(
	IN  IT9510INFO*    modulator,
    IN  int dAmp,
	IN  int dPhi
){

	return IT9510Tuner_setIQCalibrationEx(modulator, dAmp, dPhi);

}

Dword IT9510_adjustOutputGain (
	IN  IT9510INFO*    modulator,
	IN  int			  *gain	   
){
	Dword error = ModulatorError_NO_ERROR;
	int amp_mul;
	int c1value = 0;
	int c2value = 0;
	int c3value = 0;	
	int c1value_default;
	int c2value_default;
	int c3value_default;	
	
	Dword amp_mul_max1 = 0;
	Dword amp_mul_max2 = 0;
	Dword amp_mul_max3 = 0;
	int amp_mul_max = 0;
	int i = 0;
	
	int gain_X10 = *gain * 10;
	Bool overflow = False;

	c1value_default = modulator->calibrationInfo.c1DefaultValue;
	c2value_default = modulator->calibrationInfo.c2DefaultValue;
	c3value_default = modulator->calibrationInfo.c3DefaultValue;	
	
	if (c1value_default>1023) c1value_default = c1value_default-2048;
	if (c2value_default>1023) c2value_default = c2value_default-2048;
	if (c3value_default>1023) c3value_default = c3value_default-2048;

	if(c1value_default != 0)
		amp_mul_max1 = 10000*1023/abs(c1value_default);
	else
		amp_mul_max1 = 0xFFFFFFFF;

	if(c2value_default != 0)
		amp_mul_max2 = 10000*1023/abs(c2value_default);
	else
		amp_mul_max2 = 0xFFFFFFFF;

	if(c3value_default != 0)
		amp_mul_max3 = 10000*1023/abs(c3value_default);
	else
		amp_mul_max3 = 0xFFFFFFFF;


	if (amp_mul_max1<amp_mul_max3) {
		if (amp_mul_max1<amp_mul_max2) {
				amp_mul_max = (int)amp_mul_max1;
			} else {
				amp_mul_max = (int)amp_mul_max2;
			}
	  } else if (amp_mul_max3<amp_mul_max2) {
        	amp_mul_max =(int)amp_mul_max3;
   	  } else {
   	  	amp_mul_max =(int)amp_mul_max2;
   	  	}

	if(gain_X10>0){
		//d_amp_mul = 1;
		amp_mul = 10000;
		for(i = 0 ; i<gain_X10 ; i+=10){
			if (amp_mul_max>amp_mul) {
				amp_mul = (amp_mul * 11220 + 5000)/10000;
				c1value = (c1value_default * amp_mul + 5000)/10000;
				c2value = (c2value_default* amp_mul + 5000)/10000;
				c3value = (c3value_default * amp_mul + 5000)/10000;
			}
			if(c1value>0x03ff){
				c1value=0x03ff;
				overflow = True;				
			}
			
			if(c3value>0x03ff){
				c3value=0x03ff;
				overflow = True;				
			}

			if(overflow)
				break;
		}
			
		
	}else if(gain_X10<0){
		//d_amp_mul = 1;
		amp_mul = 10000;
		for(i = 0 ; i>gain_X10 ; i-=10){
			if (amp_mul_max>amp_mul) {
				//d_amp_mul *= 0.501;
			amp_mul = (amp_mul * 8910  + 5000)/10000;
			
			c1value = (c1value_default * amp_mul + 5000)/10000;
			c2value = (c2value_default * amp_mul + 5000)/10000;
			c3value = (c3value_default * amp_mul + 5000)/10000;
			}
			if(c1value==0){
				overflow = True;
			}
			
			if(c3value==0){
				overflow = True;
			}

			if(overflow)
				break;			
		}
		
	}else{
		c1value = c1value_default;
		c2value = c2value_default;
		c3value = c3value_default;

	}
	if (c1value<0) {c1value=c1value+2048;}
	if (c2value<0) {c2value=c2value+2048;}
	if (c3value<0) {c3value=c3value+2048;}
	c1value = (c1value%2048);
	c2value = (c2value%2048);
	c3value = (c3value%2048);
	*gain = i/10;
	modulator->calibrationInfo.outputGain = *gain;

	IT9510User_adjustOutputGain(modulator, gain);

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_c1_7_0, (Byte)(c1value&0x00ff));
	if (error) goto exit;		
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_c1_10_8, (Byte)(c1value>>8));
	if (error) goto exit;		
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_c2_7_0, (Byte)(c2value&0x00ff));
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_c2_10_8, (Byte)(c2value>>8));
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_c3_7_0, (Byte)(c3value&0x00ff));
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_c3_10_8, (Byte)(c3value>>8));
	if (error) goto exit;


exit:

	return (error);
}

Dword IT9510_getGainRange (
	IN  IT9510INFO*    modulator,
	IN  Dword           frequency,
	IN  Word            bandwidth,    
	OUT int*			maxGain,
	OUT int*			minGain
){
	Dword error = ModulatorError_NO_ERROR;
	Byte val[6], bIndex;

	if((bandwidth !=0) && (frequency !=0)){
		error = IT9510Tuner_calIQCalibrationValue(modulator,frequency,val);
	}else{
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}

	*maxGain = 100;
	IT9510_calOutputGain(modulator, val, maxGain);

	*minGain = -100;
	IT9510_calOutputGain(modulator, val, minGain);

	// get the digital output gain info according freq.
	bIndex = 0;

	if (IT9510User_getOutputGainInfo(modulator, frequency, &bIndex) == 0)
	{
		int gain_value;

		// setting output gain
		// digital gain
		gain_value = modulator->rfGainInfo.ptrGaintable[bIndex].digitalGainValue;

		modulator->calibrationInfo.outputGain = gain_value;
	}

exit:		
	return (error);
}

Dword IT9510_getOutputGain (
	IN  IT9510INFO*    modulator,
	OUT  int			  *gain	   
){
   
    *gain = modulator->calibrationInfo.outputGain;

    return(ModulatorError_NO_ERROR);
}

Dword IT9510_suspendMode (
    IN  IT9510INFO*    modulator,
    IN  Byte          enable
){
	Dword   error = ModulatorError_NO_ERROR;

	Byte temp;
	

	error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_afe_mem0, &temp);//get power setting

	if(error == ModulatorError_NO_ERROR){
		if(enable){
		// suspend mode	
			temp = temp | 0x2D; //set bit0/2/3/5 to 1	
			/** Set PLL **/
			error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBC2, 0x0F);
			if (error) goto exit;   
			error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBC5, 0x3B);
			if (error) goto exit;   
			error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBCE, 0x1B);
			if (error) goto exit;   
			error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBD7, 0x3B);
			if (error) goto exit;   

		}else{
		// resume mode	
			temp = temp & 0xD2; //set bit0/2/3/5 to 0	
			/** Set PLL **/
			error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBC2, 0x06);
			if (error) goto exit;   
			error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBC5, 0x33);
			if (error) goto exit;   
			error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBCE, 0x1B);
			if (error) goto exit;   
			error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBD7, 0x3B);
			if (error) goto exit;   
		}
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_afe_mem0, temp);
	}
exit:
	return (error);
}


Dword IT9510_setTPS (
    IN  IT9510INFO*    modulator,
    IN  TPS           tps,
	IN  Bool		  actualInfo
){
	Dword   error = ModulatorError_NO_ERROR;
	//---- set TPS Cell ID
	if (modulator->outputMode == DVBT){
	    if(actualInfo){
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tps_cell_id_15_8, (Byte)(tps.cellid>>8));
		    if (error) goto exit;
    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tps_cell_id_7_0, (Byte)(tps.cellid));
		    if (error) goto exit;
    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_carmod_a, (Byte)tps.constellation);//set constellation
		    if (error) goto exit;
    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_a, (Byte)tps.highCodeRate);//set highCodeRate
		    if (error) goto exit;
		    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_intlvlen_a, (Byte)tps.interval);//set interval
		    if (error) goto exit;
    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_b, (Byte)tps.lowCodeRate);//set lowCodeRate
		    if (error) goto exit;
		    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_a, (Byte)tps.transmissionMode);//set transmissionMode
		    if (error) goto exit;	  
    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, 0xF7D5, 0);
		    if (error) goto exit;
		    }
		    else{
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tps_cell_id_15_8, (Byte)(tps.cellid>>8));
		    if (error) goto exit;
    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tps_cell_id_7_0, (Byte)(tps.cellid));
		    if (error) goto exit;
    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_carmod_a, (Byte)tps.constellation);//set constellation
		    if (error) goto exit;
    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_a, (Byte)tps.highCodeRate+2);//set highCodeRate
		    if (error) goto exit;
		    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_intlvlen_a, (~tps.interval)&0x03);//set interval
		    if (error) goto exit;
    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_b, (Byte)tps.lowCodeRate+2);//set lowCodeRate
		    if (error) goto exit;
		    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_a, (Byte)tps.transmissionMode);//set transmissionMode
		    if (error) goto exit;
    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, 0xF7D5, 7);
		    if (error) goto exit;
	    }
	}else{
		error = ModulatorError_INVALID_OUTPUT_MODE;
	}
exit:	
	return (error);
}

Dword IT9510_getTPS (
    IN  IT9510INFO*    modulator,
    IN  pTPS           pTps
){
	Dword   error = ModulatorError_NO_ERROR;
	//---- get TPS Cell ID
	Byte temp;
	Word cellID = 0;
	if (modulator->outputMode == DVBT){
	    error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_tps_cell_id_15_8, &temp);//get cell id
	    if (error) goto exit;
	    cellID = temp<<8;
    
	    error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_tps_cell_id_7_0, &temp);//get cell id
	    if (error) goto exit;
	    cellID = cellID | temp;	
	    pTps->cellid = cellID;
    
	    
	    error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_carmod_a, &temp);//get constellation
	    if (error) goto exit;
	    pTps->constellation = temp & 0x03;
    
    
	    error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_a, &temp);//get highCodeRate
	    if (error) goto exit;
	    pTps->highCodeRate = temp & 0x07;
    
	    error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_intlvlen_a, &temp);//get interval
	    if (error) goto exit;
	    pTps->interval = temp & 0x07;
    
    
	    error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_b, &temp);//get lowCodeRate
	    if (error) goto exit;
	    pTps->lowCodeRate = temp & 0x07;
    
	    error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_a, &temp);//get transmissionMode
	    if (error) goto exit;
	    pTps->transmissionMode = temp & 0x0F;

	
	}else{
		error = ModulatorError_INVALID_OUTPUT_MODE;
	}
exit:	
	return (error);
}

Dword IT9510_setIQtable (
	IN  IT9510INFO*    modulator,
    IN  CalibrationInfo calibrationInfo
){
	Dword   error = ModulatorError_NO_ERROR;

	
	if(calibrationInfo.ptrIQtableEx == NULL){
		error = ModulatorError_NULL_PTR;
		modulator->calibrationInfo.ptrIQtableEx =  IQ_fixed_table0; // set to default table
		modulator->calibrationInfo.tableGroups = IQ_TABLE_NROW;
		modulator->calibrationInfo.tableVersion = 0;
	}else{
		modulator->calibrationInfo.ptrIQtableEx = calibrationInfo.ptrIQtableEx;
		modulator->calibrationInfo.tableGroups = calibrationInfo.tableGroups;
		modulator->calibrationInfo.tableVersion = calibrationInfo.tableVersion;
	}
	return (error);
}

Dword IT9510_setDCtable (
	IN  IT9510INFO*    modulator,
    IN  DCInfo dcInfo
){
	Dword   error = ModulatorError_NO_ERROR;

	if(dcInfo.ptrDCtable != NULL && dcInfo.ptrOFStable != NULL && dcInfo.tableGroups !=0) {
		modulator->dcInfo.ptrDCtable = dcInfo.ptrDCtable;
		modulator->dcInfo.ptrOFStable = dcInfo.ptrOFStable;
		modulator->dcInfo.tableGroups = dcInfo.tableGroups;
	} else{
		error = ModulatorError_NULL_PTR;
	}
	return (error);
}

Dword IT9510_setRFGaintable(
	IN  IT9510INFO*    modulator,
	IN  RFGainInfo rfGainInfo
	){
	Dword   error = ModulatorError_NO_ERROR;

	if (rfGainInfo.tableIsValid != false && rfGainInfo.tableCount != 0 && rfGainInfo.ptrGaintable != NULL) {
		modulator->rfGainInfo.tableIsValid = rfGainInfo.tableIsValid;
		modulator->rfGainInfo.tableCount = rfGainInfo.tableCount;
		modulator->rfGainInfo.ptrGaintable = rfGainInfo.ptrGaintable;
	}
	else{
		error = ModulatorError_NULL_PTR;
	}
	return (error);
}

Dword IT9510_setDCCalibrationValue (
	IN  IT9510INFO*	modulator,
    IN	int			dc_i,
	IN	int			dc_q
){
	Dword   error = ModulatorError_NO_ERROR;
	Word	dc_i_temp,dc_q_temp;
	if(dc_i<0)
		dc_i_temp = (Word)(512 + dc_i) & 0x01FF;
	else
		dc_i_temp = ((Word)dc_i) & 0x01FF;
	

	if(dc_q<0)
		dc_q_temp = (Word)(512 + dc_q) & 0x01FF;
	else
		dc_q_temp = ((Word)dc_q) & 0x01FF;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_dc_i_7_0, (Byte)(dc_i_temp));
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_dc_i_8, (Byte)(dc_i_temp>>8));
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_dc_q_7_0, (Byte)(dc_q_temp));
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_dc_q_8, (Byte)(dc_q_temp>>8));
	if (error) goto exit;
exit:
	return (error);
}

Dword IT9510_getDCCalibrationValue (
	IN  IT9510INFO*	modulator,
    IN	int*		dc_i,
	IN	int*		dc_q
){
	Dword   error = ModulatorError_NO_ERROR;
	Word	dc_i_temp = 0, dc_q_temp = 0;
	Byte	temp;

	error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_dc_i_8, &temp);
	if (error) goto exit;
	dc_i_temp = temp;
	
	error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_dc_i_7_0, &temp);
	if (error) goto exit;
	dc_i_temp = (dc_i_temp<<8) | temp;

	error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_dc_q_8, &temp);
	if (error) goto exit;
	dc_q_temp = temp;

	error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_iqik_dc_q_7_0, &temp);
	if (error) goto exit;
	dc_q_temp = (dc_q_temp<<8) | temp;

	*dc_i = (int)(dc_i_temp & 0x01FF);
	*dc_q = (int)(dc_q_temp & 0x01FF);


	if(dc_i_temp>255)
		*dc_i = *dc_i -512;

	if(dc_q_temp>255)
		*dc_q = *dc_q -512;

exit:
	return (error);
}

Dword IT9510_setTMCCInfo (
    IN  IT9510INFO*    modulator,
    IN  TMCCINFO      TmccInfo,
	IN  Bool		  actualInfo
){
		Dword   error = ModulatorError_NO_ERROR;
	if (modulator->outputMode == ISDBT){
		error = IT9510_writeRegisterBits(modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_system_id, 0, 2, (Byte)TmccInfo.systemIdentification);//set system Identification
		if (error) goto exit;
	
		error = IT9510_writeRegister(modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_c, 0x07);//set constellation ??
	    if (error) goto exit;
    
	    if(actualInfo){
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_carmod_a, (Byte)TmccInfo.layerA.constellation + 1);//set constellation
		    if (error) goto exit;
		    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_a, (Byte)TmccInfo.layerA.codeRate);//set CodeRate
		    if (error) goto exit;
		    
    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_partial_recp, (Byte)TmccInfo.isPartialReception);//set PartialReception ??
		    if (error) goto exit;
		    
    
		    if(TmccInfo.isPartialReception == 1){ // 1+12
    
			    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_carmod_b, (Byte)TmccInfo.layerB.constellation + 1 );//set constellation
			    if (error) goto exit;
			    
			    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_b, (Byte)TmccInfo.layerB.codeRate);//set constellation
			    if (error) goto exit;
    
			    error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_a, IT9510_reg_tmccinfo_numseg_a_pos, IT9510_reg_tmccinfo_numseg_a_len, 1);//set layer A seg
			    if (error) goto exit;
    
			    error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_b, IT9510_reg_tmccinfo_numseg_b_pos, IT9510_reg_tmccinfo_numseg_b_len, 12);//set layer B seg
			    if (error) goto exit;
    
			    error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_c, IT9510_reg_tmccinfo_numseg_c_pos, IT9510_reg_tmccinfo_numseg_c_len, 0xF);//set layer C seg
			    if (error) goto exit;
			}
			else{ //layer A 13seg

			    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_carmod_b, 0x07);//set constellation ??
			    if (error) goto exit;
			    
			    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_b, 0x07);//set constellation ??
			    if (error) goto exit;
    
			    error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_a, IT9510_reg_tmccinfo_numseg_a_pos, IT9510_reg_tmccinfo_numseg_a_len, 13);//set layer A seg
			    if (error) goto exit;
    
			    error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_b, IT9510_reg_tmccinfo_numseg_b_pos, IT9510_reg_tmccinfo_numseg_b_len, 0xF);//set layer B seg
			    if (error) goto exit;
    
			    error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_c, IT9510_reg_tmccinfo_numseg_c_pos, IT9510_reg_tmccinfo_numseg_c_len, 0xF);//set layer C seg
			    if (error) goto exit;
		    }
		}
		else{

		    //---- set TPS Cell ID
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_carmod_a, (Byte)TmccInfo.layerA.constellation );//set constellation
		    if (error) goto exit;
		    
		    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_a, (Byte)TmccInfo.layerA.codeRate+2);//set CodeRate
		    if (error) goto exit;
		    
    
		    if(TmccInfo.isPartialReception == 1){ // 1+12
			    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_partial_recp, 0);//set PartialReception ??
			    if (error) goto exit;
    
			    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_carmod_b, (Byte)TmccInfo.layerB.constellation );//set constellation
			    if (error) goto exit;
			    
			    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_b, (Byte)TmccInfo.layerB.codeRate+2);//set constellation
			    if (error) goto exit;
    
			    error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_a, IT9510_reg_tmccinfo_numseg_a_pos, IT9510_reg_tmccinfo_numseg_a_len, 1);//set layer A seg
			    if (error) goto exit;
    
			    error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_b, IT9510_reg_tmccinfo_numseg_b_pos, IT9510_reg_tmccinfo_numseg_b_len, 12);//set layer B seg
			    if (error) goto exit;
    
			    error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_c, IT9510_reg_tmccinfo_numseg_c_pos, IT9510_reg_tmccinfo_numseg_c_len, 0xF);//set layer C seg
			    if (error) goto exit;
			}
			else{ //layer A 13seg
			    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_partial_recp, 1);//set PartialReception ??
			    if (error) goto exit;
    
			    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_carmod_b, 0x07);//set constellation ??
			    if (error) goto exit;
			    
			    error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_b, 0x07);//set constellation ??
			    if (error) goto exit;
    
			    error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_a, IT9510_reg_tmccinfo_numseg_a_pos, IT9510_reg_tmccinfo_numseg_a_len, 13);//set layer A seg
			    if (error) goto exit;
    
			    error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_b, IT9510_reg_tmccinfo_numseg_b_pos, IT9510_reg_tmccinfo_numseg_b_len, 0xF);//set layer B seg
			    if (error) goto exit;
    
			    error = IT9510_writeRegisterBits (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_numseg_c, IT9510_reg_tmccinfo_numseg_c_pos, IT9510_reg_tmccinfo_numseg_c_len, 0xF);//set layer C seg
			    if (error) goto exit;
		    }
	    }
	}else{
		error = ModulatorError_INVALID_OUTPUT_MODE;
	}
exit:	
	return (error);
}

Dword IT9510_getTMCCInfo (
    IN  IT9510INFO*    modulator,
    IN  pTMCCINFO     pTmccInfo
){
	Dword   error = ModulatorError_NO_ERROR;
	//---- get TPS Cell ID
	Byte temp;
	if (modulator->outputMode == ISDBT){
	    error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_carmod_a, &temp);//get constellation
	    if (error) goto exit;
	    if(temp>0)
		    temp = temp - 1;
	    pTmccInfo->layerA.constellation = (Constellation)(temp & 0x03);
    
	    error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_carmod_b, &temp);//get constellation
	    if (error) goto exit;
	    if(temp>0)
		    temp = temp - 1;
	    pTmccInfo->layerB.constellation = (Constellation)(temp & 0x03);
    
    
	    error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_a, &temp);//get CodeRate
	    if (error) goto exit;
	    pTmccInfo->layerA.codeRate = (CodeRate)(temp & 0x07);
    
	    error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_cr_b, &temp);//get CodeRate
	    if (error) goto exit;
	    pTmccInfo->layerB.codeRate = (CodeRate)(temp & 0x07);
    
	    
	    error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_tmccinfo_partial_recp, &temp);//get PartialReception
	    if (error) goto exit;
	    pTmccInfo->isPartialReception = (Bool)(temp & 0x01);
	
	}else{
		error = ModulatorError_INVALID_OUTPUT_MODE;
	}
exit:	
	return (error);

}

Dword IT9510_getTSinputBitRate (
    IN  IT9510INFO*    modulator,
    IN  Word*     BitRate_Kbps
){
	Dword   error = ModulatorError_NO_ERROR;
	Word	time_count=10000;	//1 count = 4.6us for ASIC ;10000 count = 46ms
	
	Dword	BitRate;
	Byte temp;
	Word packet;
	
	error = IT9510_writeRegister (modulator, Processor_LINK, p_ir_up_num_7_0, (Byte)time_count);//LSB
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_LINK, p_ir_up_num_15_8, (Byte)(time_count>>8));//MSB
	if (error) goto exit;

	IT9510User_delay(150);


	error = IT9510_readRegister (modulator, Processor_LINK, r_ir_byte_count_15_8, &temp);//MSB
	if (error) goto exit;
	packet = temp;

	error = IT9510_readRegister (modulator, Processor_LINK, r_ir_byte_count_7_0, &temp);//MSB
	if (error) goto exit;
	packet = (packet<<8) | temp;
	if(packet == 0){
		error = ModulatorError_SLOW_CLK_FAIL;
		goto exit;
	}else{
		packet--;
	}
	BitRate = (188*8*packet)/46; //kbps

	*BitRate_Kbps = (Word)BitRate;
	if(BitRate>32000){	//MAX =31.6M
		error = ModulatorError_BITRATE_OUT_OF_RANGE;
		goto exit;
	}

exit:	
	return (error);
}


Dword IT9510_setPcrMode (
    IN  IT9510INFO*		modulator,
    IN  PcrMode			mode
){
	Dword   error = ModulatorError_NO_ERROR;

	if((modulator->isdbtModulation.isPartialReception == True) && (modulator->outputMode == ISDBT )){
		//1+12 mode not support PCR re-stamp
		modulator->pcrMode = PcrModeDisable;
	}else{
		modulator->pcrMode = mode;
		if(mode == PcrModeDisable) { 
			//modulator->nullPacketMode = NullPacketModeDisable;
			error = IT9510_setPcrModeEnable (modulator, 0);
		} else {
			if(modulator->nullPacketMode == NullPacketModeDisable)
				modulator->nullPacketMode =	NormalMode;
			error = IT9510_setPcrModeEnable (modulator, 1);
		}
	}

	return (error);
}

Dword IT9510_setNullPacketMode (
    IN  IT9510INFO*		modulator,
    IN  NullPacketMode	mode
){
	Dword   error = ModulatorError_NO_ERROR;

	modulator->nullPacketMode = mode;
	error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_null_mode, (Byte)mode);
		
	return (error);
}


Dword IT9510_isTsBufferOverflow (
	IN  IT9510INFO*	modulator,
    OUT	Bool		*overflow	
){
	Dword   error = ModulatorError_NO_ERROR;
	Byte	temp = 0;
	error = IT9510_readRegister (modulator, Processor_OFDM, p_IT9510_reg_tsip_overflow, &temp);
	if (error) goto exit;

	if(temp) {
		*overflow = True;
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_tsip_overflow, 1); //clear
		if (error) goto exit;
	} else {
		*overflow = False;
	}

exit:
	return (error);
}


Dword IT9510_getChipType (
	IN  IT9510INFO*	modulator,
    OUT	Word		*chipType	
){
	Dword   error = ModulatorError_NO_ERROR;
	Byte	var[2];
	Byte	type1,type2;

	error = IT9510_readRegisters(modulator, Processor_LINK, 0x1222+1, 2, var);
	if (error)	goto exit;

	error = IT9510_readRegister(modulator, Processor_LINK, 0xD805, &type1);
	if (error)	goto exit;

	error = IT9510_readRegister(modulator, Processor_LINK, 0xD917, &type2);
	if (error)	goto exit;

	*chipType = var[1]<<8 | var[0];

	if(*chipType == 0x9517){
		if(type1 == 1)
			*chipType = 0x9513;	
		else if(type2 == 1)
			*chipType = 0x9511;	
	}else{ 
		error = ModulatorError_INVALID_CHIP_TYPE;
	}
exit:
	return (error);
}

Dword IT9510_setOFSCalibrationValue (
	IN  IT9510INFO*	modulator,
    IN	Byte		ofs_i,
	IN	Byte		ofs_q
){
	Dword   error = ModulatorError_NO_ERROR;
	
	error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBBA, ofs_i);
	if (error) goto exit;

	error = IT9510_writeRegister (modulator, Processor_OFDM, 0xFBBB, ofs_q);
	
exit:
	return (error);
}

Dword IT9510_getOFSCalibrationValue (
	IN  IT9510INFO*	modulator,
    IN	Byte*		ofs_i,
	IN	Byte*		ofs_q
){
	Dword   error = ModulatorError_NO_ERROR;
	
	error = IT9510_readRegister (modulator, Processor_OFDM, 0xFBBA, ofs_i);
	if (error) goto exit;

	error = IT9510_readRegister (modulator, Processor_OFDM, 0xFBBB, ofs_q);
	
exit:
	return (error);
}

Dword IT9510_aesEncryptionEnable (
	IN  IT9510INFO*	modulator,
    IN	Bool		enable
	
){
	Dword   error = ModulatorError_NO_ERROR;

	if(enable)
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_aes_bypass, 0);
	else

		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_aes_bypass, 1);
	
	return (error);
}

Dword IT9510_setAesEncryptionKey (
	IN  IT9510INFO*	modulator,
    IN	Byte*		key
	
){
	Dword   error = ModulatorError_NO_ERROR;

	
	error = IT9510_writeRegisters (modulator, Processor_OFDM, p_IT9510_reg_aes_key0_7_0, 16, key);
	
	

	return (error);
}

Dword IT9510_setEncryptionStartAddress (
	IN  IT9510INFO*	modulator,
    IN	Byte		addr
	
){
	Dword   error = ModulatorError_NO_ERROR;

	if(addr<=43)
		error = IT9510_writeRegister (modulator, Processor_OFDM, p_IT9510_reg_aes_word_num, addr);
	else
		error = ModulatorError_INDEX_OUT_OF_RANGE;
	

	return (error);
}
Dword IT9510_setSineTone (
	IN  IT9510INFO*	modulator, 
    IN	Bool		on_off
	
) {
	Dword	error;
	Byte	val;

	// p_reg_iqik_sine (0xF751)
	if (on_off) 
	{
		val = 0x01;
		error = IT9510_writeRegisters (modulator, Processor_OFDM, 0xF751, 1, &val);
		if (error) goto exit;

		error = IT9510_readRegisters (modulator, Processor_OFDM, 0xFBB9, 1, &val);
		if (error) goto exit;
		val = (val & 0xCF) + 0x10;
		error = IT9510_writeRegisters (modulator, Processor_OFDM, 0xFBB9, 1, &val);
		if (error) goto exit;

	} 
	else 
	{
		val = 0x00;
		error = IT9510_writeRegisters (modulator, Processor_OFDM, 0xF751, 1, &val);
		if (error) goto exit;

		error = IT9510_readRegisters (modulator, Processor_OFDM, 0xFBB9, 1, &val);
		if (error) goto exit;
		val = (val & 0xCF);
		error = IT9510_writeRegisters (modulator, Processor_OFDM, 0xFBB9, 1, &val);
		if (error) goto exit;
	}

exit:
	return(error);
}

Dword IT9510_enableTpsEncryption (
	IN  IT9510INFO*	modulator,
    IN	Dword		key
	
){
	Dword   error = ModulatorError_NO_ERROR;
	Byte	temp[4];
	
	error = IT9510_writeRegister (modulator, Processor_OFDM, 0x14, 0); // disable Encryptio
	if (error) goto exit;

	temp [0] = (Byte)(key >> 24);
	temp [1] = (Byte)(key >> 16);
	temp [2] = (Byte)(key >> 8);
	temp [3] = (Byte)key;

	error = IT9510_writeRegisters (modulator, Processor_OFDM, 0x15, 4, temp);
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_OFDM, 0x14, 1); // enable Encryptio
	if (error) goto exit;
exit:	
	return (error);
}

Dword IT9510_disableTpsEncryption (
	IN  IT9510INFO*	modulator	
){
	Dword   error = ModulatorError_NO_ERROR;
	

	error = IT9510_writeRegister (modulator, Processor_OFDM, 0x14, 0);
	
	return (error);
}



Dword IT9510_readEepromValuesByGI2C(
    IN  IT9510INFO*	modulator,
	IN  Byte            slaveAddress,
    IN  Word			registerAddress,
    IN  Byte			bufferLength,
    OUT Byte*			buffer
) {
    Dword   error = ModulatorError_NO_ERROR;
    Byte	writeBuffer[3];
    Byte	rBuf[255];
    Byte	i;

    writeBuffer[0] = (registerAddress >> 8);
    writeBuffer[1] = (registerAddress & 0xFF);
    error = IT9510_writeGenericRegisters (modulator, slaveAddress, 2, writeBuffer);
    if(error) 
		return error;
    error = IT9510_readGenericRegisters (modulator, slaveAddress, bufferLength, rBuf);
    if(error) 
		return error;

    for(i = 0; i < bufferLength; i++)
		  buffer[i] = rBuf[i];
	IT9510User_delay(15);
    return error;
}

Dword IT9510_writeEepromValuesByGI2C(
    IN  IT9510INFO*	modulator,
	IN  Byte            slaveAddress,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    Dword	error = ModulatorError_NO_ERROR;
    Byte	wBuf[255];
    int		i;
    Word	len;

    if(bufferLength > 64 - (registerAddress % 64))
    {
      len = 64 - (registerAddress % 64);
      wBuf[0] = (registerAddress >> 8);
      wBuf[1] = (registerAddress & 0xFF);
      for(i = 0 ; i < len; i++)
        wBuf[i + 2] = buffer[i];
      error = IT9510_writeGenericRegisters(modulator, slaveAddress, len + 2, wBuf);
      if(error) 
		  return error;

		  IT9510User_delay(15);

		  wBuf[0] = ((registerAddress + len) >> 8);
		  wBuf[1] = ((registerAddress + len) & 0xFF);
		  for(i = 0 ; i < bufferLength - len; i++)
        wBuf[i + 2] = buffer[len + i];
		  error = IT9510_writeGenericRegisters(modulator, slaveAddress, bufferLength - len + 2, wBuf);
		  if(error) 
			  return error;
    }
    else
    {
      wBuf[0] = (registerAddress >> 8);
      wBuf[1] = (registerAddress & 0xFF);
      for(i = 0 ; i < bufferLength; i++)
        wBuf[i + 2] = buffer[i];
	
		  error = IT9510_writeGenericRegisters(modulator, slaveAddress, bufferLength + 2, wBuf);
		  if(error) 
			  return error;
    }
    IT9510User_delay(15);
    return error;
}





Dword IT9510_writeIT9560EEPROM(
	IN  IT9510INFO*	modulator,
	IN  Byte        slaveAddress,
	IN  Word        startAddress,
	IN  unsigned char* buffer, 
	IN  int writeSize
){
	Dword error;
	int fw_size;
	Word eeprom_offset;
	unsigned char eeprom_burst_size;
	unsigned char check_buffer[256];
	int i;
	int cnt = 0;

	//switch to BB eeprom
			
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_en, 1);//gpiox_en
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_on, 1);//gpiox_on
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_o, 0);//gpiox_o
	if (error) goto exit;		

	IT9510User_delay(10);

	fw_size = writeSize;
	eeprom_burst_size = 32;
	eeprom_offset = 0;			
	while(fw_size > 0)
	{
		if(fw_size > eeprom_burst_size)
		{
			cnt++;
			error = IT9510_writeEepromValuesByGI2C(modulator, slaveAddress, startAddress+eeprom_offset , eeprom_burst_size, buffer + eeprom_offset);
			if (error) 
				goto exit;	
			fw_size -= eeprom_burst_size;
			eeprom_offset += eeprom_burst_size;
		}
		else
		{
			cnt++;
			error = IT9510_writeEepromValuesByGI2C(modulator,  slaveAddress, startAddress+eeprom_offset , (unsigned char)(fw_size), buffer + eeprom_offset);
			if (error) 
				goto exit;	
			fw_size = 0;
		}
	}			
	
	//Checking
	fw_size = writeSize;		
	eeprom_offset = 0; 
	while(fw_size > 0)
	{
		if(fw_size > eeprom_burst_size)
		{
			error = IT9510_readEepromValuesByGI2C(modulator,  slaveAddress, startAddress+eeprom_offset, eeprom_burst_size, check_buffer); 
			if (error) goto exit;	

			for(i = 0; i < eeprom_burst_size; i++)
			{
				if(check_buffer[i] != buffer[ eeprom_offset + i]){
					error = ModulatorError_WRITE_EEPROM_FAIL;
					goto exit;
				}
			}
			fw_size -= eeprom_burst_size;
			eeprom_offset += eeprom_burst_size;
		}
		else
		{
			error = IT9510_readEepromValuesByGI2C(modulator,  slaveAddress, startAddress+eeprom_offset, (unsigned char)(fw_size), check_buffer);
			if (error) 
				goto exit;	

			for(i = 0; i < fw_size; i++)
			{
				if(check_buffer[i] != buffer[eeprom_offset + i]){
					error = ModulatorError_WRITE_EEPROM_FAIL;
					goto exit;
				}
			}
			fw_size = 0;
		}
	}		

exit:
	IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_o, 1);//gpiox_o
	return error;
}			



Dword IT9510_readIT9560EEPROM(
	IN  IT9510INFO*	modulator,
	IN  Byte        slaveAddress,
	IN  Word        startAddress,
	OUT unsigned char* buffer, 
	IN  int readSize
){
	Dword error;
	int fw_size;
	int eeprom_offset;
	unsigned char eeprom_burst_size;
	
	//switch to BB eeprom
			
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_en, 1);//gpiox_en
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_on, 1);//gpiox_on
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_o, 0);//gpiox_o
	if (error) goto exit;		

	IT9510User_delay(10);

	fw_size = readSize;
	eeprom_burst_size = 32;
	eeprom_offset = 0;			
	
	
	while(fw_size > 0)
	{
		if(fw_size > eeprom_burst_size)
		{
			error = IT9510_readEepromValuesByGI2C(modulator, slaveAddress, eeprom_offset + startAddress, eeprom_burst_size, buffer + eeprom_offset); 
			if(error) goto exit;

			fw_size -= eeprom_burst_size;
			eeprom_offset += eeprom_burst_size;
		}
		else
		{
			error = IT9510_readEepromValuesByGI2C(modulator, slaveAddress, eeprom_offset + startAddress, (unsigned char)(fw_size), buffer + eeprom_offset);
			if(error) goto exit;

			fw_size = 0;
		}
	}		
exit:
	IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_o, 1);//gpiox_o
	return error;
}			



Dword IT9510_getIT9560FwVersion(
	IN  IT9510INFO*	modulator,
	IN  Byte        slaveAddress,
	OUT Byte *version
){
	Dword error;
	unsigned short checksum;
	unsigned char buffer[256];
	int i;

	//switch to BB eeprom
			
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_en, 1);//gpiox_en
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_on, 1);//gpiox_on
	if (error) goto exit;
	error = IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_o, 0);//gpiox_o
	if (error) goto exit;		

	error = IT9510_readEepromValuesByGI2C(modulator, slaveAddress, 256 + 0, 32, buffer);
	if(error) goto exit;
	error = IT9510_readEepromValuesByGI2C(modulator, slaveAddress, 256 + 32, 32, buffer + 32);
	if(error) goto exit;
	error = IT9510_readEepromValuesByGI2C(modulator, slaveAddress, 256 + 64, 32, buffer + 64);
	if(error) goto exit;
	error = IT9510_readEepromValuesByGI2C(modulator, slaveAddress, 256 + 96, 32, buffer + 96);
	if(error) goto exit;
	error = IT9510_readEepromValuesByGI2C(modulator, slaveAddress, 256 + 128, 32, buffer + 128);
	if(error) goto exit;
	error = IT9510_readEepromValuesByGI2C(modulator, slaveAddress, 256 + 160, 32, buffer + 160);
	if(error) goto exit;
	error = IT9510_readEepromValuesByGI2C(modulator, slaveAddress, 256 + 192, 32, buffer + 192);
	if(error) goto exit;
	error = IT9510_readEepromValuesByGI2C(modulator, slaveAddress, 256 + 224, 32, buffer + 224);
	if(error) goto exit;

	checksum = 0;
	for(i = 0; i < 256; i += 2)
	  checksum += (buffer[i] * 256 + buffer[i + 1]);
	if(checksum != 0xFFFF)
	{
		error = ModulatorError_INVALID_FW_TYPE;	  
		goto exit;
	}

	for(i = 0; i < 8; i++)
		version[i] = buffer[24 + i];

	
exit:
	IT9510_writeRegister (modulator, Processor_LINK, p_IT9510_reg_top_gpioh6_o, 1);//gpiox_o
	return error;

}

Dword IT9510_setPcrInfo(
	IN  IT9510INFO*	modulator,
	IN  PCRCALINFO	pcrCalInfo
){

	modulator->pcrCalInfo = pcrCalInfo;
	return ModulatorError_NO_ERROR;
}


//----------------------------------------------
