#include "stm32f10x.h"                  // Device header
#include "MySPI.h"

void MySPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(Soft_SPI_SS_Port, Soft_SPI_SS_Pin, (BitAction)BitValue);
}

void MySPI_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(Soft_SPI_SCK_Port, Soft_SPI_SCK_Pin, (BitAction)BitValue);
}

void MySPI_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(Soft_SPI_MOSI_Port, Soft_SPI_MOSI_Pin, (BitAction)BitValue);
}

int8_t MySPI_R_MISO(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
}

void MySPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_Soft_SPI_Port, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = Soft_SPI_SS_Pin | Soft_SPI_SCK_Pin | Soft_SPI_MOSI_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Soft_SPI_SS_Port, &GPIO_InitStructure);
	
	MySPI_W_SS(1);
	MySPI_W_SCK(0);
}

void MySPI_Start(void)
{
	MySPI_W_SS(0);
}

void MySPI_Stop(void)
{
	MySPI_W_SS(1);
}

uint8_t MySPI_SwapByte(uint8_t ByteSend)			//这是模式0的时序
{
	uint8_t ByteReceive = 0x00;
	uint8_t i;
	
	for (i = 0; i < 8; i ++)
	{
		MySPI_W_MOSI(ByteSend & (0x80 >> i));
		MySPI_W_SCK(1);
		if (MySPI_R_MISO() == 1)
		{
			ByteReceive |= (0x80 >> i);
		}
		MySPI_W_SCK(0);
	}

	return ByteReceive;
}
