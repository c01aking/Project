#ifndef __ST7735S_H
#define __ST7735S_H

#define ST7735S_DC_Port		GPIOB
#define ST7735S_DC_Pin		GPIO_Pin_1

#define ST7735S_RES_Port	GPIOB
#define ST7735S_RES_Pin		GPIO_Pin_0

#define ST7735S_BLK_Port	GPIOB
#define ST7735S_BLK_Pin		GPIO_Pin_10

#define RCC_ST7735S_Port	RCC_APB2Periph_GPIOB

typedef enum
{
	Year = 0,
	Month = 1,
	Day,
	Hour,
	minute,
	second
}ST7735S_Time_Unit;

void ST7735S_Init(void);
void ST7735S_clear(uint16_t color);
void ST7735S_fill_color(uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye, uint32_t color);
void ST7735S_Point(uint8_t x, uint8_t y, uint16_t color);
void ST7735S_ShowChar(uint8_t Line, uint8_t Column, char Char, uint16_t color_char, uint16_t color_background);
void ST7735S_ShowString(uint8_t Line, uint8_t Column, char *String, uint16_t color_char, uint16_t color_background);
void ST7735S_ShowWord(uint8_t Line, uint8_t Column, ST7735S_Time_Unit Time, uint16_t color);
void ST7735S_ShowNum(uint8_t Line, uint8_t Column, uint16_t Num, uint16_t color_char, uint16_t color_background);
	
#endif
