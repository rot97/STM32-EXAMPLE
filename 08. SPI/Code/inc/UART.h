#ifndef UART_HEADER
#define UART_HEADER

#include "stm32f103xb.h"

extern char data;
extern uint8_t RX_Flag;

void USART1_Init(void);

void USART1_sent(char c);

void USART1_sentstr(char* str);

#endif
