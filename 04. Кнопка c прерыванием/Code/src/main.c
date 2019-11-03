#include "stm32f103xb.h"

/*
B0 - светодиод
B1 - кнопка
*/

int LED_Blink = 1; //Разрещение мигания должно быть глобальным флагом!

int main(){
	
	//Светодиод на плате
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk;
	GPIOC->CRH |= GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1;
	GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk;

	//Светодиод на B0
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	GPIOB->CRL &= ~GPIO_CRL_MODE0_Msk;
	GPIOB->CRL |= GPIO_CRL_MODE0_0 | GPIO_CRL_MODE0_1;
	GPIOB->CRL &= ~GPIO_CRL_CNF0_Msk;
	
	//Настройка кнопки на B1
	GPIOB->CRL &= ~GPIO_CRL_MODE1_Msk;
	GPIOB->CRL &= ~GPIO_CRL_CNF1_Msk;
	GPIOB->CRL |= GPIO_CRL_CNF1_0;
	
	//Настройка прерывания
	//Включаем модуль прерываний
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	
	
	//Зададим порт для прерывания на пине 2
	//Очистка регистра
	AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI1_Msk;
	//Прерывание для порта B
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PB;
	
	//Настрока события
	/*
	RTSR - по фронту
	FTSR - по срезу
	*/
	//По фронту
	EXTI->RTSR |= EXTI_RTSR_RT1;
	
	//Включаем регистрацию прерывания
	EXTI->IMR |= EXTI_IMR_IM1;
	
	//Включаем прерывание и назначаем вектор
	NVIC_EnableIRQ (EXTI1_IRQn);
	
	GPIOC->BSRR |= GPIO_BSRR_BR13;
	GPIOB->BSRR |= GPIO_BSRR_BR0;
	
	while(1){
		for(int i =0; i < 200000; i++);

		if(LED_Blink == 1){
			GPIOC->ODR ^= GPIO_ODR_ODR13;
			GPIOB->ODR ^= GPIO_ODR_ODR0;
		}
	}
}

//Вектор прерывания
void EXTI1_IRQHandler(){
	//Очистка флага
	EXTI->PR |= EXTI_PR_PR1;
	
	LED_Blink = ~LED_Blink;
}
