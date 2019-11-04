#include "RCC.h"

void RCC_Init(){
	FLASH->ACR &= ~FLASH_ACR_LATENCY; 
	FLASH->ACR |= (2 << FLASH_ACR_LATENCY_Pos);
	
	RCC->CR |= RCC_CR_HSEON; 
	while(!(RCC->CR & RCC_CR_HSERDY));
	RCC->CFGR &= ~RCC_CFGR_PLLXTPRE;

	RCC->CFGR |= RCC_CFGR_PLLSRC;
	
	RCC->CFGR &= ~RCC_CFGR_PLLMULL;
	RCC->CFGR |= RCC_CFGR_PLLMULL9;
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	
	RCC->CFGR &= ~RCC_CFGR_HPRE;
	RCC->CFGR &= ~RCC_CFGR_PPRE2_Msk;
	RCC->CFGR &= ~RCC_CFGR_PPRE1_Msk;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
}


void MCO_Init(){
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	GPIOA->CRH |= GPIO_CRH_MODE8_Msk;
	GPIOA->CRH &= ~GPIO_CRH_CNF8_Msk;
	GPIOA->CRH |= GPIO_CRH_CNF8_1;
	
	RCC->CFGR &= ~RCC_CFGR_MCO_Msk;
	RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;
	
}