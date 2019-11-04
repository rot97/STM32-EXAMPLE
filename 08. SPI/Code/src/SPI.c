#include "SPI.h"

//Настройка SPI1
void SPI1_Init(){
	//Подаём тактирование на SPI1
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	//Настраиваем порты
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
	//SCK
	GPIOA->CRL |= GPIO_CRL_MODE5_Msk;
	GPIOA->CRL &= ~GPIO_CRL_CNF5_Msk;
	GPIOA->CRL |= GPIO_CRL_CNF5_1;
	
	//MOSI
	GPIOA->CRL |= GPIO_CRL_MODE7_Msk;
	GPIOA->CRL &= ~GPIO_CRL_CNF7_Msk;
	GPIOA->CRL |= GPIO_CRL_CNF7_1;
	
	//MISO
	GPIOA->CRL &= ~GPIO_CRL_MODE6_Msk;
	GPIOA->CRL &= ~GPIO_CRL_CNF6_Msk;
	GPIOA->CRL |= GPIO_CRL_CNF6_0;
	
	//CS
	GPIOA->CRH |= GPIO_CRH_MODE11_Msk;
	GPIOA->CRH &= ~GPIO_CRH_CNF11_Msk;
	
	//Если устройство не используется CS = 1
	GPIOA->BSRR |= GPIO_BSRR_BS10;
	
	//Настройка SPI непостердсвенно
	//Настройка размера пакета
	/*
	Данное устройство использует размер 8 бит,
	однако посколько между приёмом и предачей устройстову не трубется сброс CS, для удобства будем использовать 16бит
	*/
	SPI1->CR1 |= SPI_CR1_DFF;
	
	//Настройка порядка отправки битов
	/*
	Для данного устройсва первый бит должен быть старшим
	*/
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;	
	
	//Настройка типа управления CS. В 99.9% случаев это делается програмно
	SPI1->CR1 |= SPI_CR1_SSM;
	SPI1->CR1 |= SPI_CR1_SSI;
	
	//Настроййка делителя тактовой частоты
	/*
	На шине тактирования сейчас 72МГц, устройсво поддерживает максимум 5МГц.
	Ближайший делитель - 16
	Получим 4.5Мгц
	*/
	SPI1->CR1 &= ~SPI_CR1_BR_Msk;
	SPI1->CR1 |= 0b011 << SPI_CR1_BR_Pos;
	
	//Настройка типа устройства
	/*
	В даном случае у нас Master (ведущие). В большинстве задач происходит связь с датчиками,
	а самим датчиком МК выступает достаточно редко. 
	*/
	SPI1->CR1 |= SPI_CR1_MSTR;
	
	//Настройка типа SPI
	/*
	В нашем случае испльзуется MODE 3 (CPOL = 1, CPHA = 1)
	*/
	SPI1->CR1 |= SPI_CR1_CPOL;
	SPI1->CR1 |= SPI_CR1_CPHA;
	
	//Включаем SPI
	SPI1->CR1 |= SPI_CR1_SPE;	
}


//Чтение данных
uint8_t SPI1_Read(uint8_t reg){
	//Очистим буфер
	(void) SPI1->DR;
	
	/*
	На вход фунции поступает адресс регистра, который мы хотим считать.
	Мы должны преобразовать этот адресс в форму, отвечающую за чтение (для каждого датчика она индивидуальна)
	Затем мы сформируем блок передаваемых данных - 16 бит
	Старшие 8 бит будут содержать наш адресс
	Младшие 8 бит будут предаваться для того, чтобы датчик отправил нам ответ (это может быть следующий адресс, либо заполнение)
	В документации на датчик написано, что при чтении старший дит адресса равен 1
	*/
	uint16_t data = (((1<<7) | reg) << 8) | 0x80;

	//Устанавливаем CS = 0 (выбираем устройво)
	GPIOA->BSRR |= GPIO_BSRR_BR11;
	
	//Отправляем данные 
	SPI1->DR = data;
	
	//Ждём когда отправим данные и получим результат
	while(!(SPI1->SR & SPI_SR_TXE));
	while((SPI1->SR & SPI_SR_BSY));
	
	//Устанавливаем CS = 1 (устройство больше не используется)
	GPIOA->BSRR |= GPIO_BSRR_BS11;
	
	//Забираем данные из буфера
	data = (SPI1->DR);
	
	//Возращаем данные, предворительно переведя из в 8 бит
	return (uint8_t) data;
}

//Запись данных
void SPI1_Write(uint8_t reg, uint8_t data){
	//Очистим буфер
	(void) SPI1->DR;
	
	/*
	Запись данных очень похожа на чтение:
	Передаём 16 бит
	Старшие 8 бит - адресс регистра в который пишем
	По документации страший бит адресса должен быть равен 0
	Младшие 8 бит - записываемое значение
	*/
	uint16_t d = (reg << 8) | data;

	//Устанавливаем CS = 0 (выбираем устройво)
	GPIOA->BSRR |= GPIO_BSRR_BR11;
	
	//Отправляем данные 
	SPI1->DR = d;
	
	//Ждём когда отправим данные и получим результат
	while(!(SPI1->SR & SPI_SR_TXE));
	while((SPI1->SR & SPI_SR_BSY));
	
	//Устанавливаем CS = 1 (устройство больше не используется)
	GPIOA->BSRR |= GPIO_BSRR_BS11;
}