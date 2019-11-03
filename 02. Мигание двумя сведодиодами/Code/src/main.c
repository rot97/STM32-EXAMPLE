#include "stm32f103xb.h"

/*
B0 - светодиод
*/

int main(){
	
	//Настрока сведодиода на плате аналогична
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk;
	GPIOC->CRH |= GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1;
	GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk;

	//Настроим пин B0 для сведодиода
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	GPIOB->CRL &= ~GPIO_CRL_MODE0_Msk;
	GPIOB->CRL |= GPIO_CRL_MODE0_0 | GPIO_CRL_MODE0_1;
	GPIOB->CRL &= ~GPIO_CRL_CNF0_Msk;
	
	//Пусть в первоначальный момент вермени сведодиод C13 горит (он включён инверсно!), а B0 не горит
	GPIOC->BSRR |= GPIO_BSRR_BR13;
	GPIOB->BSRR |= GPIO_BSRR_BR0;
	
	while(1){
		//Подождём около 1 с
		for(int i =0; i < 1000000; i++); //По хорошему мы сейчас работаем на 8МГц, такой задержкой получить ровно 1с очень трудно
		
		//Ещё один из способов изменить состояние выхода - обратиться к регистру напрямую
		GPIOC->ODR ^= GPIO_ODR_ODR13;
		GPIOB->ODR ^= GPIO_ODR_ODR0;
	}
}
