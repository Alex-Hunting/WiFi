#ifndef _USART_H_
#define _USART_H_

#include "stm32f10x.h"

void USART1_Configuration(int Baudrate);
void Sendware(uint8_t *wareaddr, uint32_t waresize);
void USART2_Configuration(int Baudrate);
void USART3_Configuration(int Baudrate);

#endif
