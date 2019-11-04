#ifndef SPI_HEADER
#define SPI_HEADER

#include "stm32f103xb.h"

void SPI1_Init(void);

uint8_t SPI1_Read(uint8_t reg);

void SPI1_Write(uint8_t reg, uint8_t data);

#endif
