#include "stm32f103xb.h"

void RCC_Init(){
	//Настраиваем задержку флеш памяти. Без этого тупо повиснем
	FLASH->ACR &= ~FLASH_ACR_LATENCY; 
	FLASH->ACR |= (2 << FLASH_ACR_LATENCY_Pos);
	
	
	//Настройка кварца (8МГц)
	RCC->CR |= RCC_CR_HSEON; // Включаем кварц
	while(!(RCC->CR & RCC_CR_HSERDY));//Ждём включения кварца
	
	RCC->CFGR &= ~RCC_CFGR_PLLXTPRE;//Устанавливаем делитель HSE = 1
	
	
	//Настраиваем множитель
	RCC->CFGR |= RCC_CFGR_PLLSRC; //Источник PLL - HSE
	
	RCC->CFGR &= ~RCC_CFGR_PLLMULL;
	RCC->CFGR |= RCC_CFGR_PLLMULL9; //Настраиваем умножение частоты на 9
	
	RCC->CR |= RCC_CR_PLLON; //Включаем множитель
	while(!(RCC->CR & RCC_CR_PLLRDY));//Ждём включения множителя
	
	
	//Настройка системных шин
	RCC->CFGR &= ~RCC_CFGR_HPRE; // Устанавливаем делитель шины AHB = 1
	
	RCC->CFGR &= ~RCC_CFGR_PPRE2_Msk; //Устанавливаем делитель шины APB2 = 1
	
	RCC->CFGR &= ~RCC_CFGR_PPRE1_Msk;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;//Устанавливаем делитель шины APB1 = 2
	
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL; //Подключаем множитель к шине AHB
}

void MCO_Init(){
	//Настраиваем PA8
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	GPIOA->CRH |= GPIO_CRH_MODE8_Msk;
	GPIOA->CRH &= ~GPIO_CRH_CNF8_Msk;
	GPIOA->CRH |= GPIO_CRH_CNF8_1;
	
	RCC->CFGR &= ~RCC_CFGR_MCO_Msk; // Очистка источника MCO
	RCC->CFGR |= RCC_CFGR_MCO_SYSCLK; //Задаём источник MCO (есть готовые маски для HSE, HSI, PLL, SYSCLK)
	
}

int main(){
	RCC_Init();
	MCO_Init();
	
	while(1);
}
