#ifndef __MYSPI_H
#define __MYSPI_H

#define Soft_SPI_SS_Port	GPIOA
#define Soft_SPI_SS_Pin		GPIO_Pin_4

#define Soft_SPI_SCK_Port	GPIOA
#define Soft_SPI_SCK_Pin	GPIO_Pin_5

#define Soft_SPI_MOSI_Port	GPIOA
#define Soft_SPI_MOSI_Pin	GPIO_Pin_7

#define RCC_Soft_SPI_Port	RCC_APB2Periph_GPIOA

void MySPI_W_SS(uint8_t BitValue);
void MySPI_W_SCK(uint8_t BitValue);
void MySPI_W_MOSI(uint8_t BitValue);
int8_t MySPI_R_MISO(void);
void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_SwapByte(uint8_t ByteSend);			//这是模式0的时序

#endif
