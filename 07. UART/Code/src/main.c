#include "stm32f103xb.h"

/*
A8 - MCO

USART1
A9 - TX
A10 - RX
*/

/*
Программа передаёт Hello world в теминал, а всё ответы возращает назад.
*/

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


void USART1_Init(){
	//Настройка портов
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	
	//A9 - Tx
	GPIOA->CRH |= GPIO_CRH_MODE9_Msk;
	GPIOA->CRH &= ~GPIO_CRH_CNF9_Msk;
	GPIOA->CRH |= GPIO_CRH_CNF9_1;
	
	//A10 - Rx
	/*
	Внимание! В серии STM32F1 входы альтернативных функций необходимо настроить как плавающий вход!
	В более старших сериях всё логичнее: для входов и выходов используюется режим альтернативной функции
	*/
	GPIOA->CRH &= ~GPIO_CRH_MODE10_Msk;
	GPIOA->CRH &= ~GPIO_CRH_CNF10_Msk;
	GPIOA->CRH |= GPIO_CRH_CNF10_0;
	
	//Настройка USART1 непосредсвенно
	//Подаём тактирование
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	/*
	Самый сложный момент: интерфейс ассинхронный, поэтому необходимо синсхронизировать частоты приёмника и передатчика
	Именно поэтому кварц обезателен!
	Сейчас на шине 72МГц. Для примера будем использовать скорость 19200.
	Для жтого нужно настроить преобзазователь частоты.
	Из даташита берём формулу DIV = FCK/(16*FUART)
	В нашем случае FCK = 72 000 000, FUART = 19 200
	Следовательно DIV = 234.375
	Это число записывается в регистр BRR. Биты [3:0] Отвечают за дробную часть числа, [15:0] за целую 
	Целая часть числа 234 = 128 + 64 + 32 + 8 + 2 = 0b1101010 = 0xEA
	С учетом того, что под дробную часть  числа отводиться 4 бита
	0.375 = 0.25 + 0.125 = 0b0110 = 0x6
	Склеив получаем 0xEA6
	Подробнее об этом можно почитать разделе 27.3.4 Reference manual
	*/
	//USART1->BRR = 0xEA6;
	USART1->BRR = 0xEA6;
	
	//Включаем передатчик
	USART1->CR1 |= USART_CR1_TE;
	//Включаем преёмник
	USART1->CR1 |= USART_CR1_RE;
	//Включаем UART
	USART1->CR1 |= USART_CR1_UE;
	
	//Поскольку у нас нет задачи, связанных с большим объёмом вычислительных ресурсов включим прерывание только на приём
	USART1->CR1 |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);
}


//Передача одного символа
void USART1_sent(char c){
	//Ждём пока отправатся предыдущий символ
	while(!(USART1->SR & USART_SR_TC));
	
	//Как только передача завершена - передаём следующий сивол, загрузив его в буфер
	USART1->DR = c;
}

//Передача строки
void USART1_sentstr(char* str){
	/*
	Принимаем строку, индикатор конца - '\n'. Если его нет, то всё очень плохо
	*/
	//Текущий символ
	uint8_t i = 0;
	do{
		//Передаём текущий сисвол
		USART1_sent(str[i]);
		//Передали символ новой строки? Передача закончена
	}	while(str[i++] != '\n');
}

//Буфер символа
char data;

//Флаг приёма
uint8_t RX_Flag = 0;

int main(){
	RCC_Init();
	MCO_Init();
	
	USART1_Init();
	
	char  str[] = "Hello world!\r\n";
	
	//Передадим строку Hello world!
	USART1_sentstr(str);
	
	while(1){
		USART1_sentstr(str);
		for(int i =0; i <10000; i++);
		//Приняли символ?
		if(RX_Flag){
			//Сбросили флаг
			RX_Flag = 0;
			//Передадим символ
			USART1_sent(data);
		}
	}
}

//Как только приняли символ - срабатывает прерывание, а мы забираем приняты символ в буфер
void USART1_IRQHandler(){
	//Напишем в общем случае: на одном прерывании сидит несколько событий. Проверим тот факт, что мы приняли сивол.
	if(USART1->SR & USART_SR_RXNE){
		//Забираем символ
		data = USART1->DR;
		//Очищаем флаг прерывания
		USART1->SR &= ~USART_SR_RXNE;
		//Устанавливаем флаг приёма символа
		RX_Flag = 1;
	}
}

