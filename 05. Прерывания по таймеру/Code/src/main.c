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
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI1_Msk;
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PB;
	EXTI->RTSR |= EXTI_RTSR_RT1;
	EXTI->IMR |= EXTI_IMR_IM1;
	
	NVIC_EnableIRQ (EXTI1_IRQn);
	
	//Настройка таймера
	//Включаем таймер
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	//Настроим делитель
	//На входе таймера 8МГц, поделив на 8000 получим 1кГц
	TIM2->PSC = 8000-1;
	
	//Зададим значение после которого будет сброс
	TIM2->ARR = 1000-1;
	
	//Прерывание по обновлению
	TIM2->DIER |= TIM_DIER_UIE;
	
	//Включаем счёт
	TIM2->CR1 |= TIM_CR1_CEN;
	
	//Включаем прерывание
	NVIC_EnableIRQ (TIM2_IRQn);
	
	GPIOC->BSRR |= GPIO_BSRR_BR13;
	GPIOB->BSRR |= GPIO_BSRR_BR0;
	
	while(1);
}

//Вектор прерывания для таймера
void TIM2_IRQHandler(){
	TIM2->SR &= ~TIM_SR_UIF;
	
	if(LED_Blink == 1){
			GPIOC->ODR ^= GPIO_ODR_ODR13;
			GPIOB->ODR ^= GPIO_ODR_ODR0;
	}
}

void EXTI1_IRQHandler(){
	EXTI->PR |= EXTI_PR_PR1;
	
	LED_Blink = ~LED_Blink;
}
