#ifndef _ESP8285_H_
#define _ESP8285_H_

#include "stm32f10x.h"

void ESP8285_Log(char *str, u8 state);
void U3Putchar(u8 data);
void U3Putstr(char *sp);
u8 ESP8285_CmdSend(char *str, char *result, int timeout);
void ESP8285_Init(void);
void ESP8285_SetMode(void);
void ESP8255_QueryIP(void);
void ESP8285_QueryMode(void);
void ESP8285_Connect_Network(void);
void Connect_NTP(void);
void Time_Calibration(void);
void test(void);
void WIFI_Data_Analysis(void);
int Time_Analysis(void);

#endif
