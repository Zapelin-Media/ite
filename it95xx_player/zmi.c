/* Copyright (C) 2013-2019 by Ubaldo Porcheddu <ubaldo@zapelin.com> */

#include "api.h"
#include "crc.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>

int zmiDevice=0;


int zmiLog(const char * format, ...) {
 char buf[80];
 char txt[256];
 time_t raw;
 time(&raw);
 va_list args;
 
 va_start(args, format);
 vsnprintf(txt,sizeof(txt),format,args);
 va_end(args); 

 strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",localtime(&raw));

 fprintf(stderr,"%s zmi[%02d]: %s\n", buf, zmiDevice, txt);
 
 return 0;
}


int zmi(char *card, char *mode, char *frequency, char *bandwidth, char *constellation, char *codeRate, char *interval, char *transmissionMode, char *gain, char *timeout) { //card, mode, frequency, bandwidth, timeout
 int zmiFile=0;				//stdin
 int zmiFreq=0;
 int zmiBand=0;
 int zmiRate=0;
 int zmiMode=0; 			// 0 dvb eagleI, 1 dvb eagleII, 2 isdb eagleII
 int zmiEagle=EAGLEI;
 int zmiTimeout=10;
 int zmiGain=0;				//-25 +6
 int zmiConstellation=2;		//0:QPSK  1:16QAM  2:64QAM
 int zmiCodeRate=2;			//0:1/2  1:2/3  2:3/4  3:5/6  4:7/8
 int zmiInterval=0;			//0:1/32  1:1/16  2:1/8  3:1/4
 int zmiTransmissionMode=1;		//0:2K  1:8K  2:4K
 int i=0, r=0, w=0, zr=0, zw=0;
 unsigned long t=0, now=time(0);
 char buf[TRANSMITTER_BLOCK_SIZE];
 if (card != NULL && frequency != NULL && bandwidth != NULL) {
  zmiDevice = atoi(card);
  zmiFreq = strtof(frequency, NULL) * 1000;
  zmiBand = atoi(bandwidth)*1000; 
  zmiMode = atoi(mode);
  zmiGain = atoi(gain);
  zmiConstellation = atoi(constellation);
  zmiCodeRate = atoi(codeRate);
  zmiInterval = atoi(interval);
  zmiTransmissionMode = atoi(transmissionMode);
  if (timeout != NULL) { zmiTimeout=atoi(timeout); }
  if (zmiMode > 0) { zmiEagle=EAGLEII; }
  
  zmiLog("start");
  if (g_ITEAPI_TxDeviceInit(zmiEagle,zmiDevice) == ERR_NO_ERROR && g_ITEAPI_TxSetChannel(zmiFreq, zmiBand, zmiDevice) == ERR_NO_ERROR) {
   Dword dwStatus = 0;
   if (zmiMode < 2) {	//DVB-T
    zmiLog("DVB");
    MODULATION_PARAM ChannelModulation_Setting;  
    ChannelModulation_Setting.constellation = zmiConstellation;
    ChannelModulation_Setting.highCodeRate = zmiCodeRate;
    ChannelModulation_Setting.interval = zmiInterval;
    ChannelModulation_Setting.transmissionMode = zmiTransmissionMode;
    dwStatus=g_ITEAPI_TxSetChannelModulation(ChannelModulation_Setting, zmiDevice);
   }
   if (zmiMode == 2) { 	//ISDB-T
    zmiLog("ISDB");
    ISDBTModulation ChannelModulation_Setting;   
    ChannelModulation_Setting.isPartialReception = False;
    ChannelModulation_Setting.layerA.constellation = zmiConstellation;
    ChannelModulation_Setting.layerA.codeRate = zmiCodeRate;
    ChannelModulation_Setting.interval = zmiInterval;
    ChannelModulation_Setting.transmissionMode = zmiTransmissionMode;
    g_ITEAPI_TxControlPidFilter(0, 0, zmiDevice);
    dwStatus=g_ITEAPI_TxSetISDBTChannelModulation(ChannelModulation_Setting, zmiDevice);
   }

   if (!dwStatus) { 
    g_ITEAPI_TxAdjustOutputGain(zmiGain,&zmiGain,zmiDevice);
    g_ITEAPI_TxSetModeEnable(True, zmiDevice);              
    fcntl(zmiFile, F_SETFL, fcntl(zmiFile, F_GETFL) | O_NONBLOCK);
    
    for (i=1; i<=5; i++) { g_ITEAPI_TxSetPeridicCustomPacketTimer(i, 0, zmiDevice); }
    
    g_ITEAPI_StartTransfer(zmiDevice);
    zmiLog("begin loop");
    while (time(0)-now < zmiTimeout) {
     now=time(0);
     
     t=0;
     while ((t/1000) < zmiTimeout) {
      r=read(zmiFile,buf,sizeof(buf));
      if (r > 0) { break; }
      usleep(1000);
      t=t+1;
     }

     t=0;
     while ((t/1000) < zmiTimeout) {
      w=g_ITEAPI_TxSendTSData(buf, r, zmiDevice);
      if (w == 0) { break; }
      usleep(1000);
      t=t+1;
     }

    } 
    g_ITEAPI_StopTransfer(zmiDevice);
    g_ITEAPI_TxSetModeEnable(False, zmiDevice);   
   }
  }
  g_ITEAPI_TxDeviceExit(zmiDevice);
  zmiLog("end loop");
  return 1;
 } else {
  return -1;
 }
 zmiLog("stop"); 
}

int main(int argc, char **argv) { //card, mode, freq, bandwidth, timeout
 char pidFile[32];
 FILE *fd;
 if (argc == 11) {
  sprintf(pidFile,"/tmp/eja.pid.zmi.%d",atoi(argv[1]));
  fd=fopen(pidFile,"w");
  if (fd) {
   fprintf(fd,"%d",getpid());
   fclose(fd);
  }
  zmi(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10]); 
 } else {
  printf("usage: zmi card mode frequency bandwidth constellation codeRate interval transmissionMode gain timeout\n");
 }

 return 0;
}
