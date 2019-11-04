#include "stm32f103xb.h"
#include "SPI.h"
#include "RCC.h"
#include "UART.h"

/*
A8 - MCO

USART1
A9 - TX
A10 - RX

SPI1
A5 - SCK
A6 - MISO
A7 - MOSI
A11 - CS
*/

/*
ADXL345
FCLK <= 5MHz
MODE = 3
MSB FIRST
*/

int main(){
	RCC_Init();
	MCO_Init();
	
	SPI1_Init();
	
	//Проверим соединение. Обычно это осуществляется запросом ID. В данном случае оно 0xE0, а располагается в регистре 0x00
	if(SPI1_Read(0x00) == 0xE5){
		//Если всё ОК, попадём сюда
		SPI1_Write(0x2C, 0x02);
		while(1);
	}
	
	while(1){
		//Если не всё ОК, попадём сюда
	}
}


