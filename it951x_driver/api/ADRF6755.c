#include "ADRF6755.h"
//#include "stdio.h"
//----- BUS implement
//#include "i2cimpl.h"
#include "IT9510.h"

//------------------


Dword ADRF6755_enterCriticalSection (ADRF6755INFO* ADRF6755	
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
	
	//IT9510_writeRegister ((IT9510INFO*)ADRF6755->userData, Processor_LINK, ((IT9510INFO*)(ADRF6755->userData))->systemConfig.rfEnable+1, 1);
	
    return (ModulatorError_NO_ERROR);
}

Dword ADRF6755_leaveCriticalSection (ADRF6755INFO* ADRF6755	
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
	//IT9510_writeRegister ((IT9510INFO*)ADRF6755->userData, Processor_LINK, ((IT9510INFO*)(ADRF6755->userData))->systemConfig.rfEnable+1, 0);
    return (ModulatorError_NO_ERROR);
}

Dword ADRF6755_writeRegister(ADRF6755INFO* ADRF6755, Byte addr, Byte value) {
	Dword error = 0;
	Byte buf[2];

	buf[0] = addr;
	buf[1] = value;

//------- Edit by user
	//error = I2c_writeControlBus(&modulator, 2, buf);
	error = IT9510_writeGenericRegisters((IT9510INFO*)ADRF6755->userData, IIC_Addr, 2,buf);
	if(error) goto exit;	

//-----------------------
	ADRF6755->Reg[addr] = value;
exit:
	return (error);
}

Dword ADRF6755_readRegister(ADRF6755INFO* ADRF6755, Byte addr, Byte* value) {

	Dword error = 0;
	Byte buf[2];
	buf[0] = addr;
	//buf[1] = value;

//------- Edit by user
	error = IT9510_writeRegister((IT9510INFO*)ADRF6755->userData, Processor_LINK,p_IT9510_reg_repeat_start, 1);
    if(error) return error;
    
    error = IT9510_writeGenericRegisters((IT9510INFO*)ADRF6755->userData, IIC_Addr, 1,buf);
    if(error) return error;

    error = IT9510_writeRegister((IT9510INFO*)ADRF6755->userData, Processor_LINK, p_IT9510_reg_repeat_start, 0);
    if(error) return error;

     //Read part
     error = IT9510_readGenericRegisters((IT9510INFO*)ADRF6755->userData, IIC_Addr, 1,value);
     if(error) return error;

//-----------------------
	ADRF6755->Reg[addr] = *value;

	return (error);
}


Dword ADRF6755_initialize (ADRF6755INFO* ADRF6755,  Dword clk_KHz) {
		
	Dword error = 0;
	
	ADRF6755_enterCriticalSection(ADRF6755);
	error = ADRF6755_writeRegister(ADRF6755, CR30, 0x00);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR29, 0x80);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR28, 0x08); //power on; 1155<LO<2400 MHz
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR27, 0x10); //1155<LO<2400 MHz others: default
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR26, 0x00);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR25, 0x64); //new
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR24, 0x38); //0x18
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR23, 0x70);//72
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR22, 0x80);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR21, 0x00);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR20, 0x00);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR19, 0x80);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR18, 0x00);//0x60
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR17, 0x00);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR16, 0x00);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR15, 0x00);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR14, 0x00);//0x80	
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR13, 0xE8);//0xd8
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR12, 0x18);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR11, 0x0C); //new
	if(error) goto exit;
	if (Fpdf_KHz == 40000){
		if(clk_KHz == 20000)  //0x00 default //for 40M set to 0x01;for 80M set to 0x41 ;for 20M set to 0x21
			error = ADRF6755_writeRegister(ADRF6755, CR10, 0x21); 
		else if(clk_KHz == 80000)
			error = ADRF6755_writeRegister(ADRF6755, CR10, 0x41);
		else if(clk_KHz == 40000)
			error = ADRF6755_writeRegister(ADRF6755, CR10, 0x01);
		else 
			error = ModulatorError_FREQ_OUT_OF_RANGE;
	} else if(Fpdf_KHz == 80000){
		if(clk_KHz == 20000)
			error = ADRF6755_writeRegister(ADRF6755, CR10, 0x04);
		else if(clk_KHz == 40000)
			error = ADRF6755_writeRegister(ADRF6755, CR10, 0x21);
		else if(clk_KHz == 80000)
			error = ADRF6755_writeRegister(ADRF6755, CR10, 0x01);
		else 
			error = ModulatorError_FREQ_OUT_OF_RANGE;

	}else {
		error = ModulatorError_FREQ_OUT_OF_RANGE;
	}
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR9, 0xF0); //org 0xF0
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR8, 0x00);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR7, 0x00);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR6, 0x2E); //set to 1875MHz, INT = 46
	if(error) goto exit;
	//error = ADRF6755_writeRegister(ADRF6755, CR5, 0x00); //0x01 
	//if(error) goto exit;

	if (Fpdf_KHz == 40000){
		if(clk_KHz == 20000)  //0x00 default //for 40M set to 0x01;for 80M set to 0x41 ;for 20M set to 0x21
			error = ADRF6755_writeRegister(ADRF6755, CR5, 0x00); 
		else if(clk_KHz == 80000)
			error = ADRF6755_writeRegister(ADRF6755, CR5, 0x00);
		else if(clk_KHz == 40000)
			error = ADRF6755_writeRegister(ADRF6755, CR5, 0x00);
		else 
			error = ModulatorError_FREQ_OUT_OF_RANGE;
	} else if(Fpdf_KHz == 80000){
		if(clk_KHz == 20000)
			error = ADRF6755_writeRegister(ADRF6755, CR5, 0x10);
		else if(clk_KHz == 40000)
			error = ADRF6755_writeRegister(ADRF6755, CR5, 0x00);
		else if(clk_KHz == 80000)
			error = ADRF6755_writeRegister(ADRF6755, CR5, 0x00);
		else 
			error = ModulatorError_FREQ_OUT_OF_RANGE;

	}else {
		error = ModulatorError_FREQ_OUT_OF_RANGE;
	}
	if(error) goto exit;



	error = ADRF6755_writeRegister(ADRF6755, CR4, 0x01);
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR3, 0x09); //0x05 set to 1875MHz, FRAC = 29360128(0x01C00000)
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR2, 0xC0); //set to 1875MHz, FRAC = 29360128(0x01C00000)
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR1, 0x00); //set to 1875MHz, FRAC = 29360128(0x01C00000)
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR0, 0x00); //set to 1875MHz, FRAC = 29360128(0x01C00000)
	if(error) goto exit;
	error = ADRF6755_writeRegister(ADRF6755, CR27, 0x10); //1155<LO<2400 MHz others: default
	if(error) goto exit;
	IT9510User_delay(1);
	//error = ADRF6755_writeRegister(ADRF6755, CR29, 0x81); //modulator power on
	if(error) goto exit;

exit:
	ADRF6755_leaveCriticalSection(ADRF6755);
	return (error);
}

Dword ADRF6755_setFrequency (ADRF6755INFO* ADRF6755, Dword frequency_KHz) {

	Dword	error = 0;
	Dword	INT_value = 0;
	Dword	FRAC = 0, temp;
	Byte	RFDIV = 0;
	Byte	f_rang = 0;
	Byte	RegValue;
	int i,N=1;
	ADRF6755_enterCriticalSection(ADRF6755);
	if((frequency_KHz>1155000) && (frequency_KHz<=3000000)) {
		RFDIV = 0;
		f_rang = 1;
	}else if((frequency_KHz>577500) && (frequency_KHz<=1155000)) {
		RFDIV = 1;
		f_rang = 0;
	}else if((frequency_KHz>288750) && (frequency_KHz<=577500)) {
		RFDIV = 2;
		f_rang = 0;
	}else if((frequency_KHz>144375) && (frequency_KHz<=288750)){
		RFDIV = 3;
		f_rang = 0;
	}else if((frequency_KHz>=100000) && (frequency_KHz<=144375)) {
		RFDIV = 4;
		f_rang = 0;
	}else{
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}

	if(RFDIV>0){

		for (i=0;i<RFDIV;i++)
			N = N*2;
	}else{
		N = 1;	
	}


	//N = frequency_KHz/Fpdf_KHz; N = INT + FRAC/2^25
	INT_value = ((N*frequency_KHz)/Fpdf_KHz);
	temp = ((N*frequency_KHz*1000)/Fpdf_KHz - INT_value*1000);
	FRAC = (temp * 3355443)/100; //   org:(temp * 33554432)/1000;
	//printf("RFDIV=%d f_rang=%d\n",RFDIV,f_rang);
	//printf("INT=%d FRAC=%d\n",INT_value,FRAC);

	//error = ADRF6755_writeRegister(ADRF6755, CR29, 0x80);
	//if(error) goto exit;
	IT9510User_delay(1);
	//--- write to ADRF6755 register
	RegValue = ((ADRF6755->Reg[CR28] & 0xF8) | RFDIV);
	error = ADRF6755_writeRegister(ADRF6755, CR28, RegValue); //RFDIV
	if(error) goto exit;
	RegValue = ((ADRF6755->Reg[CR7] & 0xF0) | (Byte)(INT_value>>8));
	error = ADRF6755_writeRegister(ADRF6755, CR7, RegValue); //INT_value[11:8]
	if(error) goto exit;
	RegValue = (Byte)INT_value;
	error = ADRF6755_writeRegister(ADRF6755, CR6, RegValue); //INT_value[7:0]
	if(error) goto exit;

	RegValue = ((ADRF6755->Reg[CR3] & 0xFE) | (Byte)(FRAC>>24));
	error = ADRF6755_writeRegister(ADRF6755, CR3, RegValue); //FRAC[24]
	if(error) goto exit;

	RegValue = (Byte)(FRAC>>16);
	error = ADRF6755_writeRegister(ADRF6755, CR2, RegValue); //FRAC[23:16]
	if(error) goto exit;

	RegValue = (Byte)(FRAC>>8);
	error = ADRF6755_writeRegister(ADRF6755, CR1, RegValue); //FRAC[15:8]
	if(error) goto exit;

	RegValue = (Byte)(FRAC);
	error = ADRF6755_writeRegister(ADRF6755, CR0, RegValue); //FRAC[7:0]
	if(error) goto exit;

	RegValue = ((ADRF6755->Reg[CR27] & 0xEF) | (f_rang<<4));
	error = ADRF6755_writeRegister(ADRF6755, CR27, RegValue); //RFDIV
	if(error) goto exit;

	IT9510User_delay(1);
	//error = ADRF6755_writeRegister(ADRF6755, CR29, 0x81);
	//if(error) goto exit;
exit:
	ADRF6755_leaveCriticalSection(ADRF6755);
	return (error);
}

Dword ADRF6755_modulatorPowerOn (ADRF6755INFO* ADRF6755, Bool enable) {

	Dword	error = 0;
	ADRF6755_enterCriticalSection(ADRF6755);
	if(enable)
		error = ADRF6755_writeRegister(ADRF6755, CR29, 0x81);
	else
		error = ADRF6755_writeRegister(ADRF6755, CR29, 0x80);

	ADRF6755_leaveCriticalSection(ADRF6755);
	return (error);
}


Dword ADRF6755_adjustOutputGain (ADRF6755INFO* ADRF6755, Byte gain) {

	Dword	error = 0;
	Byte temp = 0;
	ADRF6755_enterCriticalSection(ADRF6755);
	ADRF6755->gain = gain;
	if(gain<32){
		error = ADRF6755_writeRegister(ADRF6755, CR30, gain);
	}else if(gain>=32 && gain<48){
		temp = 0x30 + (gain - 32);
		error = ADRF6755_writeRegister(ADRF6755, CR30, temp);
	}else{
		error = 0xFF;
	}
	ADRF6755_leaveCriticalSection(ADRF6755);
	return (error);
}
