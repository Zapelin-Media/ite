#include "RFFC2072.h"
//#include "stdio.h"
//#include "math.h"
#include "IT9510.h"

#define HI 1
#define LO 0
#define GPIO_SPI 0


#ifdef USE_FLOAT
double log2_log(double x)
{
    return log(x)/log(2.0);
}
#else
Word log2(Dword n) {//but truncated
     
     int logValue = -1;
	 if (n>0){
		 while (n) {//
			 logValue++;
			 n >>= 1;
		 }
	 }
     return (Word)logValue;
 }
#endif
void ENX_OUT(RFFC2072INFO *RFFC2072, Byte state){
#if GPIO_SPI
	IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh4_o, state);
	IT9510User_delay(0);
#endif	
}

void CLK_OUT(RFFC2072INFO *RFFC2072, Byte state){
#if GPIO_SPI
	IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh5_o, state);
	IT9510User_delay(0);
#endif	
}

void DATA_OUT(RFFC2072INFO *RFFC2072, Byte state ){
#if GPIO_SPI
	IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh6_o, state);
	IT9510User_delay(0);
#endif	
}

void DATA_IN(RFFC2072INFO *RFFC2072, Byte *state ){
#if GPIO_SPI
	Byte temp;
	//IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh6_en, 0);//SDA
	
	//IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh6_on, 1); 
	

	IT9510User_delay(0);
	IT9510_readRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, r_IT9510_reg_top_gpioh6_i, &temp);


	//IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh6_en, 1);//SDA
	
	//IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh6_on, 1); 
	 *state = temp;
#endif
}


Dword RFFC2072_readRegister (
	RFFC2072INFO *RFFC2072,
	Byte regAddr,
	Word *value
) {

#if GPIO_SPI
	Byte i,tempAddr, temp;
	Word tempValue = 0;
    tempAddr = 0x80 | regAddr;

	CLK_OUT(RFFC2072,HI);
	ENX_OUT(RFFC2072,LO);
	CLK_OUT(RFFC2072,LO);
	CLK_OUT(RFFC2072,HI);

	for(i=0;i<8;i++){

		CLK_OUT(RFFC2072,LO);

		DATA_OUT(RFFC2072,tempAddr>>(7-i));
		
		CLK_OUT(RFFC2072,HI);	
	}
	
	IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh6_en, 0);//SDA
	
	IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh6_on, 1); 

	CLK_OUT(RFFC2072,LO);
	CLK_OUT(RFFC2072,HI);	
	CLK_OUT(RFFC2072,LO);
	CLK_OUT(RFFC2072,HI);

	for(i=0;i<16;i++){

		CLK_OUT(RFFC2072,LO);

		DATA_IN(RFFC2072,&temp);
		tempValue = (tempValue<<1) | (temp & 0x01);
		CLK_OUT(RFFC2072,HI);	
	}

	*value = tempValue;
	ENX_OUT(RFFC2072,HI);
	CLK_OUT(RFFC2072,LO);
	IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh6_en, 1);//SDA
	
	IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh6_on, 1); 
#else

	Byte Buff[3];
	Byte CMD = 0x71; // read

	Buff[0] = CMD;
	Buff[1] = regAddr;
	Buff[2] = 2;

	IT9510_writeRegisters((IT9510INFO*)RFFC2072->userData, Processor_LINK, 0x4900, 3, Buff);

	//IT9510User_delay(1000);

	IT9510_readRegisters((IT9510INFO*)RFFC2072->userData, Processor_LINK, 0x7E00, 2, Buff);
	*value = (Buff[0]<<8) | Buff[1];
	//IT9510User_delay(1000);
#endif
    return (ModulatorError_NO_ERROR);
}


Dword RFFC2072_writeRegister (
	RFFC2072INFO *RFFC2072,
	Byte regAddr,
	Word value
) {


    
#if GPIO_SPI
    Byte i;
	Dword tempValue;
	tempValue = (regAddr <<16) | value;
	if(regAddr<0x1F){
		RFFC2072->reg[regAddr] = value;

		CLK_OUT(RFFC2072,HI);
		ENX_OUT(RFFC2072,LO);

		CLK_OUT(RFFC2072,LO);
		CLK_OUT(RFFC2072,HI);

		for(i=0;i<24;i++){

			CLK_OUT(RFFC2072,LO);

			DATA_OUT(RFFC2072,(Byte)(tempValue>>(23-i)));
			
			CLK_OUT(RFFC2072,HI);	
		}

	
		ENX_OUT(RFFC2072,HI);
		CLK_OUT(RFFC2072,LO);
	}
#else
	Byte Buff[3];
	Byte CMD = 0x70; // write

	Buff[0] = (Byte)(value >> 8);
	Buff[1] = (Byte) value ;

	IT9510_writeRegisters((IT9510INFO*)RFFC2072->userData, Processor_LINK, 0x7E00, 2, Buff);
	//IT9510User_delay(10);

	Buff[0] = CMD;
	Buff[1] = regAddr;
	Buff[2] = 2;
	//IT9510User_delay(1000);
	IT9510_writeRegisters((IT9510INFO*)RFFC2072->userData, Processor_LINK, 0x4900, 3, Buff);
	//IT9510User_delay(10);
#endif
    return (ModulatorError_NO_ERROR);
}


Dword RFFC2072_setDeviceOperation(RFFC2072INFO *RFFC2072){



	return (ModulatorError_NO_ERROR);
}

Dword RFFC2072_setCalibrationMode(RFFC2072INFO *RFFC2072){



	return (ModulatorError_NO_ERROR);
}

Dword RFFC2072_setOperatingFrequency(RFFC2072INFO *RFFC2072, Dword f_lo_KHz){


	Word writeValue, P2_FREQ1,P2_FREQ2,P2_FREQ3;

	int i;
	Dword error;
#ifdef USE_FLOAT
	double temp, temp2, ndiv,fvco_KHz;
	
	Byte n_lo, lodiv,fbkdiv, numlsb,p2presc,p2lodiv;
	
	Word n ,nummsb ;
	
	RFFC2072_enableDevice(RFFC2072, False);

	temp = 5400000;
	 
	temp2 = temp/f_lo_KHz;
	temp = log2_log(temp2);
	n_lo = Byte(temp);
	p2lodiv = n_lo;
	lodiv =pow (2.0, n_lo);// 2^n_lo;
	fvco_KHz = lodiv * f_lo_KHz;

	if(fvco_KHz>3200000){
		fbkdiv = 4;
		
		p2presc = 2;
	}else{
		fbkdiv = 2;
		
		p2presc = 1;

	}
	ndiv = fvco_KHz/fbkdiv/26000;

	n =  Word(ndiv);
	temp = double(65536*(ndiv-n));
	nummsb = Word(double(65536*(ndiv-n)));
	numlsb = Byte(double(256*(temp-nummsb)));

	
	P2_FREQ1 = n<<7 | p2lodiv<<4 | p2presc<<2 | 0x02;
	P2_FREQ2 = nummsb;
	P2_FREQ3 = numlsb<<8;
#else
	
//test log2 function
	Dword dw_temp, dw_temp1000, dw_temp2, dw_ndiv1000,dw_fvco_KHz;
	Byte n_lo2, lodiv2,fbkdiv2, numlsb2,p2presc2,p2lodiv2;
	Word n2 ,nummsb2 ;

	RFFC2072_enableDevice(RFFC2072, False);

	dw_temp = 5400000;	 
	dw_temp2 = dw_temp/(f_lo_KHz);
	dw_temp = log2(dw_temp2);
	n_lo2 = (Byte)(dw_temp);
	p2lodiv2 = n_lo2;
	//lodiv =pow (2.0, n_lo);// 2^n_lo;
	lodiv2 = 1;
	for(i=0;i<n_lo2;i++)
		lodiv2 = lodiv2 * 2;


	dw_fvco_KHz = lodiv2 * f_lo_KHz;

	if(dw_fvco_KHz>3200000){
		fbkdiv2 = 4;
		
		p2presc2 = 2;
	}else{
		fbkdiv2 = 2;
		
		p2presc2 = 1;

	}
	dw_ndiv1000 = (dw_fvco_KHz*10)/fbkdiv2/26;

	n2 =  (Word) (dw_fvco_KHz/fbkdiv2/26000);
	dw_temp1000 = (65536*(dw_ndiv1000-n2*10000));
	nummsb2 = (Word)((65536*(dw_ndiv1000-n2*10000))/10000);
	numlsb2 = (Byte)((256*(dw_temp1000-nummsb2*10000))/10000);
//-------------------------------------------


	P2_FREQ1 = n2<<7 | p2lodiv2<<4 | p2presc2<<2 | 0x02;
	P2_FREQ2 = nummsb2;
	P2_FREQ3 = numlsb2<<8;
#endif
	writeValue = (RFFC2072->reg[reg_VCO_AUTO]& 0x7FFE) | 0x8000;	//ct_min=0 ct_max=127
	error = RFFC2072_writeRegister(RFFC2072, reg_VCO_AUTO, writeValue);
	if (error) goto exit;

	error = RFFC2072_writeRegister(RFFC2072, reg_P2_FREQ1, P2_FREQ1);
	if (error) goto exit;
	error = RFFC2072_writeRegister(RFFC2072, reg_P2_FREQ2, P2_FREQ2);
	if (error) goto exit;
	error = RFFC2072_writeRegister(RFFC2072, reg_P2_FREQ3, P2_FREQ3);
	if (error) goto exit;


exit:	
	
	return (ModulatorError_NO_ERROR);
}

Dword RFFC2072_setLoopFilter(RFFC2072INFO *RFFC2072){
	
	return (ModulatorError_NO_ERROR);

}

Dword RFFC2072_enableDevice(RFFC2072INFO *RFFC2072, Bool enable){


	Dword error;
	Word writeValue;
	
	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh2_o, (Byte)enable);
//	Sleep(10);
	msleep(10);
	writeValue = (RFFC2072->reg[reg_PLL_CTRL]& 0xFFF7) | 0x0008;	//set relock
	error = RFFC2072_writeRegister(RFFC2072, reg_PLL_CTRL, writeValue);
	


	//error = RFFC2072_writeRegister(RFFC2072, reg_PLL_CTRL, 0x822C);
	/*
	if(enable){
		int i=0;
		printf("RFFC2072 after setting\n");
		for(i=reg_LF; i<reg_READBACK0;i++){
			RFFC2072_readRegister(RFFC2072, i, &RFFC2072->reg[i]);
			printf("reg[%x] = 0x%x\n",i,RFFC2072->reg[i]);
		}

		for(i=0; i<8;i++){
			writeValue = (RFFC2072->reg[reg_DEV_CTRL]& 0x0FFF) | (i<<12);	//ct_min=0 ct_max=127
			RFFC2072_writeRegister(RFFC2072, reg_DEV_CTRL, writeValue);


			RFFC2072_readRegister(RFFC2072, reg_READBACK0, &(RFFC2072->reg[reg_READBACK0+i]));
			printf("readback[%x] = 0x%x\n",i,RFFC2072->reg[reg_READBACK0+i]);
		}
	}
	*/
	return (error);
}

Dword RFFC2072_initialize(
	RFFC2072INFO *RFFC2072
){
	Byte i;
	Word writeValue;
	Dword error;
	//set GPIO pin

	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh1_en, 1);//reset
	if (error) goto exit;
	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh1_on, 1);
	if (error) goto exit;

	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh2_en, 1);//RF EN
	if (error) goto exit;
	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh2_on, 1);
	if (error) goto exit;


	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh4_en, 1);//SEN
	if (error) goto exit;
	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh4_on, 1);
	if (error) goto exit;

	
	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh5_en, 1);//SCLK
	if (error) goto exit;
	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh5_on, 1); 
	if (error) goto exit;

	
	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh6_en, 1);//SDA
	if (error) goto exit;
	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh6_on, 1); 
	if (error) goto exit;


	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh4_o, 1);
	if (error) goto exit;
	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh5_o, 0);
	if (error) goto exit;
	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh6_o, 0);
	if (error) goto exit;

	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh1_o, 0);
	if (error) goto exit;
	IT9510User_delay(1);
	error = IT9510_writeRegister ((IT9510INFO*)RFFC2072->userData, Processor_LINK, p_IT9510_reg_top_gpioh1_o, 1);
	if (error) goto exit;

	RFFC2072_enableDevice(RFFC2072, False);

	//DEFAULT REGISTER SETTINGS
	RFFC2072->reg[reg_LF]		 = 0xFFFA;	//addr = 0x00
	RFFC2072->reg[reg_XO]		 = 0x4064;	//addr = 0x01
	RFFC2072->reg[reg_CAL_TIME]	 = 0x9055;	//addr = 0x02
	RFFC2072->reg[reg_VCO_CTRL]	 = 0x2D02;	//addr = 0x03
	RFFC2072->reg[reg_CT_CAL1]	 = 0xB0BF;	//addr = 0x04
	RFFC2072->reg[reg_CT_CAL2]	 = 0xB0BF;	//addr = 0x05
	RFFC2072->reg[reg_PLL_CAL1]	 = 0x0028;	//addr = 0x06
	RFFC2072->reg[reg_PLL_CAL2]  = 0x0028;	//addr = 0x07
	RFFC2072->reg[reg_VCO_AUTO]  = 0xFC06;	//addr = 0x08
	RFFC2072->reg[reg_PLL_CTRL]  = 0x8224;//0x8224;	//addr = 0x09
	RFFC2072->reg[reg_PLL_BIAS]  = 0x0207;	//addr = 0x0A
	RFFC2072->reg[reg_MIX_CONT]  = 0x4F00;	//addr = 0x0B
	RFFC2072->reg[reg_P1_FREQ1]  = 0x2324;	//addr = 0x0C
	RFFC2072->reg[reg_P1_FREQ2]  = 0x6276;	//addr = 0x0D
	RFFC2072->reg[reg_P1_FREQ3]  = 0x2700;	//addr = 0x0E
	RFFC2072->reg[reg_P2_FREQ1]  = 0x1B96;	//addr = 0x0F
	RFFC2072->reg[reg_P2_FREQ2]  = 0xC4E4;	//addr = 0x10
	RFFC2072->reg[reg_P2_FREQ3]  = 0x4E00;	//addr = 0x11
	RFFC2072->reg[reg_FN_CTRL]	 = 0x2A80;//0x2A80;	//addr = 0x12
	RFFC2072->reg[reg_EXT_MOD]   = 0x0000;	//addr = 0x13
	RFFC2072->reg[reg_FMOD]      = 0x0000;	//addr = 0x14
	RFFC2072->reg[reg_SDI_CTRL]  = 0x0000;	//addr = 0x15///////////////////
	RFFC2072->reg[reg_GPO]		 = 0x70C1;//0x0001;	//addr = 0x16////////////////////
	RFFC2072->reg[reg_T_VCO]	 = 0x0000;	//addr = 0x17
	RFFC2072->reg[reg_IQMOD1]	 = 0x0283;	//addr = 0x18
	RFFC2072->reg[reg_IQMOD2]	 = 0xF00F;	//addr = 0x19
	RFFC2072->reg[reg_IQMOD3]	 = 0x0000;	//addr = 0x1A
	RFFC2072->reg[reg_IQMOD4]	 = 0x000F;	//addr = 0x1B
	RFFC2072->reg[reg_T_CTRL]	 = 0xC002;	//addr = 0x1C
	RFFC2072->reg[reg_DEV_CTRL]  = 0x0001;	//addr = 0x1D////////////////
	RFFC2072->reg[reg_TEST]		 = 0x0001;	//addr = 0x1E

	
	for(i=reg_LF; i<reg_READBACK0;i++){
		error = RFFC2072_writeRegister(RFFC2072, i, RFFC2072->reg[i]);
		if (error) goto exit;
	}
	
	RFFC2072->reg[reg_READBACK0] = 0x00;
	RFFC2072->reg[reg_READBACK1] = 0x00;
	RFFC2072->reg[reg_READBACK2] = 0x00;
	RFFC2072->reg[reg_READBACK3] = 0x00;
	RFFC2072->reg[reg_READBACK4] = 0x00;
	RFFC2072->reg[reg_READBACK5] = 0x00;
	RFFC2072->reg[reg_READBACK6] = 0x00;
	RFFC2072->reg[reg_READBACK7] = 0x00;
	

	//printf("RFFC2072 init setting\n");
	for(i=reg_LF; i<reg_READBACK0;i++){
		error = RFFC2072_readRegister(RFFC2072, i, &RFFC2072->reg[i]);
		if (error) goto exit;
		//printf("reg[%x] = 0x%x\n",i,RFFC2072->reg[i]);
	}

	for(i=0; i<8;i++){
		writeValue = (RFFC2072->reg[reg_DEV_CTRL]& 0x0FFF) | (i<<12);	//ct_min=0 ct_max=127
		error = RFFC2072_writeRegister(RFFC2072, reg_DEV_CTRL, writeValue);
		if (error) goto exit;

		error = RFFC2072_readRegister(RFFC2072, reg_READBACK0, &(RFFC2072->reg[reg_READBACK0+i]));
		if (error) goto exit;
		//printf("readback[%x] = 0x%x\n",i,RFFC2072->reg[reg_READBACK0+i]);
	}


exit:
	return (error);
}

Dword RFFC2072_setMixerCurrent(
	RFFC2072INFO *RFFC2072,
	Byte			value
){

	Dword error = 0xff;
	Word writeValue;
	if(value<=7){
		writeValue = (RFFC2072->reg[reg_MIX_CONT]& 0xF1FF) | (value<<9);	//ct_min=0 ct_max=127
		error = RFFC2072_writeRegister(RFFC2072, reg_DEV_CTRL, writeValue);
	}
	return(error);
}


Dword RFFC2072_setChargePumpCurrent(
	RFFC2072INFO *RFFC2072,
	Byte			value
){

	Dword error = 0xff;
	Word writeValue;
	if(value<=63){
		writeValue = (RFFC2072->reg[reg_LF]& 0x81FF) | (value<<9);	//ct_min=0 ct_max=127
		error = RFFC2072_writeRegister(RFFC2072, reg_LF, writeValue);
	}
	return(error);
}


Dword RFFC2072_setChargePumpLeakage(
	RFFC2072INFO *RFFC2072,
	Byte			value
){

	Dword error = 0xff;
	Word writeValue;
	if(value<=7){
		writeValue = (RFFC2072->reg[reg_LF]& 0xFFF8) | value;	//ct_min=0 ct_max=127
		error = RFFC2072_writeRegister(RFFC2072, reg_LF, writeValue);
	}
	return(error);
}




Dword RFFC2072_setReLock(
	RFFC2072INFO *RFFC2072
){

	Dword error = 0xff;
	Word writeValue;
		
	writeValue = (RFFC2072->reg[reg_DEV_CTRL]& 0x0FFF) | (1<<12);	
	error = RFFC2072_writeRegister(RFFC2072, reg_DEV_CTRL, writeValue);
	if (error) goto exit;

	error = RFFC2072_readRegister(RFFC2072, reg_READBACK0, &(RFFC2072->reg[reg_READBACK0+1]));
	if (error) goto exit;
	//printf("readback[%x] = 0x%x\n",i,RFFC2072->reg[reg_READBACK0+i]);
	
	if((RFFC2072->reg[reg_READBACK0+1] & 0x8000) == 0){
		//printf("readback[1] = 0x%x\n",RFFC2072->reg[reg_READBACK0+1]);
		//printf("Unlock & set relock\n");	
		error = RFFC2072_writeRegister(RFFC2072, reg_PLL_CTRL, 0x822C);
	}else{
		//printf("lock !!!!\n");
	}
exit:
	return(error);
}
