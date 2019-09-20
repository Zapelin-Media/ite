#ifndef _IT9510Tuner_H_
#define _IT9510Tuner_H_


Dword IT9510Tuner_setIQCalibration(
	IN  IT9510INFO*    modulator,
    IN  Dword         frequency	
);

Dword IT9510Tuner_setIQCalibrationEx(
	IN  IT9510INFO*    modulator,
    IN  int dAmp,
	IN  int dPhi
); 

Dword IT9510Tuner_calIQCalibrationValue(
	IN  IT9510INFO*    modulator,
    IN  Dword         frequency,
	IN  Byte*		  val
);


#endif
