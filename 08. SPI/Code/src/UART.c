#include "UART.h"

char data;
uint8_t RX_Flag = 0;

void USART1_Init(){
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	
	GPIOA->CRH |= GPIO_CRH_MODE9_Msk;
	GPIOA->CRH &= ~GPIO_CRH_CNF9_Msk;
	GPIOA->CRH |= GPIO_CRH_CNF9_1;
	
	GPIOA->CRH &= ~GPIO_CRH_MODE10_Msk;
	GPIOA->CRH &= ~GPIO_CRH_CNF10_Msk;
	GPIOA->CRH |= GPIO_CRH_CNF10_0;
	
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	USART1->BRR = 0xEA6;
	
	USART1->CR1 |= USART_CR1_TE;
	USART1->CR1 |= USART_CR1_RE;
	USART1->CR1 |= USART_CR1_UE;
	
	USART1->CR1 |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_sent(char c){
	while(!(USART1->SR & USART_SR_TC));
	USART1->DR = c;
}

void USART1_sentstr(char* str){
	uint8_t i = 0;
	do{
		USART1_sent(str[i]);
	}	while(str[i++] != '\n');
}

void USART1_IRQHandler(){
	if(USART1->SR & USART_SR_RXNE){
		data = USART1->DR;
		USART1->SR &= ~USART_SR_RXNE;
		RX_Flag = 1;
	}
}
