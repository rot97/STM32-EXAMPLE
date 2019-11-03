#include "stm32f103xb.h"

/*
B0 - светодиод
B1 - кнопка
*/

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
	//Установим MODE в 00 - вход
	GPIOB->CRL &= ~GPIO_CRL_MODE1_Msk;
	/*Настроим тип входа
	00 - аналоговый 
	01 - без подтяжки
	10 - с подтяжкой
	*/
	//Сброс
	GPIOB->CRL &= ~GPIO_CRL_CNF1_Msk;
	//Настроим как вход без подтяжки
	GPIOB->CRL |= GPIO_CRL_CNF1_0;
	
	GPIOC->BSRR |= GPIO_BSRR_BR13;
	GPIOB->BSRR |= GPIO_BSRR_BR0;
	
	//Введём 2 переменные 
	//Разрешение на мигание светодиодом
	int LED_Blink = 1;
	
	//Прошлое состояние кнопки
	int past_button = 0;
	
	while(1){
		for(int i =0; i < 200000; i++){
			//Считаем состояние кнопки
			//Если кпопка нажата, а до этого не нажата
			if((GPIOB->IDR & GPIO_IDR_IDR1) && (past_button == 0)){
				//Запомним факт нажатия 
				past_button = 1;
				//Мы зафиксировали нажатие кнопки - изменим флаг, разрещающий мигание
				LED_Blink = ~LED_Blink;
				} //Если кнопка не нажата - просто запоминаем состояние кпопки
				else if((GPIOB->IDR & GPIO_IDR_IDR1) == 0){
					past_button = 0;
				}
		}
		
		//Если мигание разрещено - меняем состояние светодиодов
		if(LED_Blink == 1){
			GPIOC->ODR ^= GPIO_ODR_ODR13;
			GPIOB->ODR ^= GPIO_ODR_ODR0;
		}
	}
}
