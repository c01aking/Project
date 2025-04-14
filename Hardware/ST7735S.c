#include "stm32f10x.h"                  // Device header
#include "MySPI.h"
#include "ST7735S.h"
#include "Delay.h"
#include "ST7735_Font.h"

#include <stdio.h>
#include <string.h>

static char time_buffer[16];

void ST7735S_rdy_W_Data(void)
{
	GPIO_SetBits(ST7735S_DC_Port, ST7735S_DC_Pin);
}

void ST7735S_rdy_W_Command(void)
{
	GPIO_ResetBits(ST7735S_DC_Port, ST7735S_DC_Pin);
}

void ST7735S_W_Data(uint8_t Data)
{
	MySPI_Start();
	ST7735S_rdy_W_Data();
	MySPI_SwapByte(Data);
	MySPI_Stop();
}

void ST7735S_W_Data_u16(uint16_t Data)
{
	ST7735S_W_Data(Data >> 8);
	ST7735S_W_Data(Data);
}

void ST7735S_W_Command(uint8_t Command)
{
	MySPI_Start();
	ST7735S_rdy_W_Command();
	MySPI_SwapByte(Command);
	MySPI_Stop();
}

void ST77335S_parameter_Init(void)
{
	ST7735S_W_Command(0x11);
	ST7735S_W_Command(0xB1); 
	ST7735S_W_Data(0x05); 
	ST7735S_W_Data(0x3C); 
	ST7735S_W_Data(0x3C); 
	ST7735S_W_Command(0xB2); 
	ST7735S_W_Data(0x05);
	ST7735S_W_Data(0x3C); 
	ST7735S_W_Data(0x3C); 
	ST7735S_W_Command(0xB3); 
	ST7735S_W_Data(0x05); 
	ST7735S_W_Data(0x3C); 
	ST7735S_W_Data(0x3C); 
	ST7735S_W_Data(0x05); 
	ST7735S_W_Data(0x3C); 
	ST7735S_W_Data(0x3C); 
	//------------------------------------End ST7735S Frame Rate---------------------------------// 
	ST7735S_W_Command(0xB4); //Dot inversion 
	ST7735S_W_Data(0x03); 
	//------------------------------------ST7735S Power Sequence---------------------------------// 
	ST7735S_W_Command(0xC0); 
	ST7735S_W_Data(0x28); 
	ST7735S_W_Data(0x08); 
	ST7735S_W_Data(0x04); 
	ST7735S_W_Command(0xC1); 
	ST7735S_W_Data(0XC0); 
	ST7735S_W_Command(0xC2); 
	ST7735S_W_Data(0x0D); 
	ST7735S_W_Data(0x00); 
	ST7735S_W_Command(0xC3); 
	ST7735S_W_Data(0x8D); 
	ST7735S_W_Data(0x2A); 
	ST7735S_W_Command(0xC4); 
	ST7735S_W_Data(0x8D); 
	ST7735S_W_Data(0xEE); 
	//---------------------------------End ST7735S Power Sequence-------------------------------------// 
	ST7735S_W_Command(0xC5); //VCOM 
	ST7735S_W_Data(0x1A); 
	ST7735S_W_Command(0x36); //MX, MY, RGB mode 
	ST7735S_W_Data(0xC0); 
//	ST7735S_W_Data(0x00); 
	//------------------------------------ST7735S Gamma Sequence---------------------------------// 
	ST7735S_W_Command(0xE0); 
	ST7735S_W_Data(0x04); 
	ST7735S_W_Data(0x22); 
	ST7735S_W_Data(0x07); 
	ST7735S_W_Data(0x0A); 
	ST7735S_W_Data(0x2E); 
	ST7735S_W_Data(0x30); 
	ST7735S_W_Data(0x25); 
	ST7735S_W_Data(0x2A); 
	ST7735S_W_Data(0x28); 
	ST7735S_W_Data(0x26); 
	ST7735S_W_Data(0x2E); 
	ST7735S_W_Data(0x3A); 
	ST7735S_W_Data(0x00); 
	ST7735S_W_Data(0x01); 
	ST7735S_W_Data(0x03); 
	ST7735S_W_Data(0x13); 
	ST7735S_W_Command(0xE1); 
	ST7735S_W_Data(0x04); 
	ST7735S_W_Data(0x16); 
	ST7735S_W_Data(0x06); 
	ST7735S_W_Data(0x0D); 
	ST7735S_W_Data(0x2D); 
	ST7735S_W_Data(0x26); 
	ST7735S_W_Data(0x23); 
	ST7735S_W_Data(0x27); 
	ST7735S_W_Data(0x27); 
	ST7735S_W_Data(0x25); 
	ST7735S_W_Data(0x2D); 
	ST7735S_W_Data(0x3B); 
	ST7735S_W_Data(0x00); 
	ST7735S_W_Data(0x01); 
	ST7735S_W_Data(0x04); 
	ST7735S_W_Data(0x13); 
	//------------------------------------End ST7735S Gamma Sequence-----------------------------// 
	ST7735S_W_Command(0x3A); //65k mode 
	ST7735S_W_Data(0x05); 
	ST7735S_W_Command(0x29); //Display on 
}

void ST7735S_Init(void)
{
	MySPI_Init();
	
	RCC_APB2PeriphClockCmd(RCC_ST7735S_Port, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = ST7735S_DC_Pin | ST7735S_RES_Pin | ST7735S_BLK_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ST7735S_DC_Port, &GPIO_InitStructure);
	
	GPIO_ResetBits(ST7735S_DC_Port, ST7735S_DC_Pin);
	GPIO_ResetBits(ST7735S_RES_Port, ST7735S_RES_Pin);
	GPIO_ResetBits(ST7735S_BLK_Port, ST7735S_BLK_Pin);
	
	Delay_ms(10);
	GPIO_SetBits(ST7735S_RES_Port, ST7735S_RES_Pin);
	Delay_ms(120);
	//
	
	ST77335S_parameter_Init();
	GPIO_SetBits(ST7735S_BLK_Port, ST7735S_BLK_Pin);
}

void ST7735S_Set_Address(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye)
{
	ST7735S_W_Command(0x2A);
	ST7735S_W_Data(xs >> 8);			//形参是8位的，如果给16位的就左移8位放高8位在这
	ST7735S_W_Data(xs & 0xFF);
	ST7735S_W_Data(xe >> 8);
	ST7735S_W_Data(xe & 0xFF);
	
	ST7735S_W_Command(0x2B);
	ST7735S_W_Data(ys >> 8);
	ST7735S_W_Data(ys & 0xFF);
	ST7735S_W_Data(ye >> 8);
	ST7735S_W_Data(ye & 0xFF);
}

void ST7735S_clear(uint16_t color)
{
	ST7735S_Set_Address(0, 0, 131, 161);		//配置像素起始终止点
	ST7735S_W_Command(0x2C);					//准备写显存命令
	uint8_t i, j;
	for (i = 0; i < 132; i ++)
	{
		for (j = 0; j < 162; j ++)
		{
			ST7735S_W_Data_u16(color);
		}
	}
}

void ST7735S_fill_color(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye, uint32_t color)
{
	ST7735S_Set_Address(xs, ys, xe, ye);
	ST7735S_W_Command(0x2C);
	uint8_t i, j;
	for (i = xs; i < xe + 1; i ++)
	{
		for (j = ys; j < ye + 1; j ++)
		{
			ST7735S_W_Data_u16(color);
		}
	}
	
}

void ST7735S_Point(uint8_t x, uint8_t y, uint16_t color)
{
	ST7735S_Set_Address(x, y, x, y);
	ST7735S_W_Command(0x2C);
	ST7735S_W_Data_u16(color);
}

void ST7735S_ShowChar(uint8_t Line, uint8_t Column, char Char, uint16_t color_char, uint16_t color_background)		//这个思路应该是正确的，一个一个遍历的去点像素点(体感上很浪费软件资源)
{
	ST7735S_fill_color((Column - 1) * 8 + 1, (Line - 1) * 16 + 1, Column * 8, Line * 16, color_background);
	uint8_t i, j;
	uint8_t index = Char - ' ';
	uint8_t line;
	for (i = 0; i < 16; i ++)
	{
		line = ST7735S_F8x16[index][i];
		for (j = 0; j < 8; j ++)				//1行1列应该是像素[1, 8][1, 16], 2行2列是[9, 16][17, 32]
		{
			if (line & (0x80 >> j))
			{
				ST7735S_Point((Column - 1) * 8 + 1 + j, (Line - 1) * 16 + 1 + i, color_char);
			}
		}
	}
}

void ST7735S_ShowString(uint8_t Line, uint8_t Column, char *String, uint16_t color_char, uint16_t color_background)
{
//	uint8_t len = strlen(String);
//	ST7735S_fill_color((Column - 1) * 8 + 1, (Line - 1) * 16 + 1, (Column + len - 1) * 8, Line * 16, color_background);
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		ST7735S_ShowChar(Line, Column + i, String[i], color_char, color_background);
	}
}

/*
一个字共32个字节对应32*8个像素点
1	2				[1, 8][9, 16]
3	4
5	6
7	8
...
*/
void ST7735S_ShowWord(uint8_t Line, uint8_t Column, ST7735S_Time_Unit Time, uint16_t color)
{
	uint8_t i, j, k, m;
	uint8_t line;
	m = 0;
	for (i = 0; i < 32; i ++)
	{
		line = ST7735S_F16x16[Time][i];
		for (j = 0; j < 8; j ++)
		{
			if (line & (0x80 >> j))
			{
				ST7735S_Point((Column - 1) * 8 + 1 + j, (Line - 1) * 16 + 1 + m, color);
			}
		}
		i ++;
		line = ST7735S_F16x16[Time][i];
		for (k = 0; k < 8; k ++)
		{
			if (line & (0x80 >> k))
			{
				ST7735S_Point(Column * 8 + 1 + k, (Line - 1) * 16 + 1 + m, color);
			}
		}
		m ++;
	}
}

void ST7735S_ShowNum(uint8_t Line, uint8_t Column, uint16_t Num, uint16_t color_char, uint16_t color_background)
{
	snprintf(time_buffer, sizeof(time_buffer), "%hu", Num);
	ST7735S_ShowString(Line, Column, time_buffer, color_char, color_background);
}
