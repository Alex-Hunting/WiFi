#ifndef _RTC_H_
#define _RTC_H_

#include "stm32f10x.h"

void U2Putchar(u8 data);
void U2Putstr(char *sp);

void RTC_Configuartion(void);
void NVIC_Configuartion(void);
void RTC_Init(void);
void Get_Time(void);
void Set_Time(int second);

#endif
