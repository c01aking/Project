#include "stm32f10x.h"                  // Device header
#include "ST7735S.h"
#include "ESP32C3.h"
#include "Delay.h"
#include "weather.h"
#include "RTC.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

static const char *wifi_ssid = "test";
static const char *wifi_pwd = "12345678";
static const char *weather_url = "https://api.seniverse.com/v3/weather/now.json?key=SxDZB8t7wK96XHWuR&location=xian&language=en&unit=c";
static char *weather;
static char *temperature;
static uint16_t *time_array;
static bool weather_ok = false;
static bool sntp_ok = false;
static bool time2update = true;




void get_weather_time_callback(void)
{
	time2update = true;											//中断中不要搞太长时间的代码，否则肯定会出问题，不要用下面的中断，把屏幕显示都搞到中断里了，最后就屏幕显示混乱
}

int main(void)
{
	SysTick_Config(SystemCoreClock / 1000);						//设置SysTick每1ms触发一次系统定时器中断
	ST7735S_Init();
	ST7735S_clear(0x0000);
	RTC_Init();
	while (!esp_at_init());											//串口通信需要约定好波特率,要不然肯定会卡住
	while (!esp_at_wifi_init());
	while (!esp_at_wifi_connect(wifi_ssid, wifi_pwd));
	

//	uint16_t t = 0;
	
	ST7735S_ShowString(1, 1, "Xi'an", 0xFFFF, 0x0000);
	ST7735S_ShowString(2, 1, "Weather:", 0xFFFF, 0x0000);
	ST7735S_ShowString(3, 1, "Temperature:", 0xFFFF, 0x0000);
	ST7735S_ShowWord(6, 5, Year, 0xFFFF);
	ST7735S_ShowWord(6, 9, Month, 0xFFFF);
	ST7735S_ShowWord(6, 13, Day, 0xFFFF);
	ST7735S_ShowString(7, 1, "  :  :  ", 0xFFFF, 0x0000);
	
	Timer3_IRQ_Handler_callback_register(get_weather_time_callback);
	Timer_for_ESP_Init();
	
	while (1)
	{
		if (time2update || !weather_ok || !sntp_ok)
		{
			time2update = false;
			const char *rsp;
			esp_at_http_get(weather_url, &rsp, NULL, 10000);
			if (weather_parse(rsp, &weather, &temperature))
			{
				weather_ok = true;
			}
			else
			{
				weather_ok = false;
			}
			
			ST7735S_ShowString(2, 9, "        ", 0xFFFF, 0x0000);
			ST7735S_ShowString(2, 9, weather, 0xFFFF, 0x0000);
			ST7735S_ShowString(3, 13, temperature, 0xFFFF, 0x0000);
			
			uint32_t ts;
			sntp_ok = esp_at_sntp_init();
			esp_at_time_get(&ts);
			RTC_Record_Time(&ts, &time_array);
		}
		
		RTC_ReadTime(&time_array);
		ST7735S_ShowNum(6, 1, time_array[0], 0xFFFF, 0x0000);
		if (time_array[1] < 10)
		{
			ST7735S_ShowChar(6, 7, '0', 0xFFFF, 0x0000);
			ST7735S_ShowNum(6, 8, time_array[1], 0xFFFF, 0x0000);
		}
		else
		{
			ST7735S_ShowNum(6, 7, time_array[1], 0xFFFF, 0x0000);
		}
		
		if (time_array[2] < 10)
		{
			ST7735S_ShowChar(6, 11, '0', 0xFFFF, 0x0000);
			ST7735S_ShowNum(6, 12, time_array[2], 0xFFFF, 0x0000);
		}
		else
		{
			ST7735S_ShowNum(6, 11, time_array[2], 0xFFFF, 0x0000);
		}
		
		if (time_array[3] < 10)
		{
			ST7735S_ShowChar(7, 1, '0', 0xFFFF, 0x0000);
			ST7735S_ShowNum(7, 2, time_array[3], 0xFFFF, 0x0000);
		}
		else
		{
			ST7735S_ShowNum(7, 1, time_array[3], 0xFFFF, 0x0000);
		}
		
		if (time_array[4] < 10)
		{
			ST7735S_ShowChar(7, 4, '0', 0xFFFF, 0x0000);
			ST7735S_ShowNum(7, 5, time_array[4], 0xFFFF, 0x0000);
		}
		else
		{
			ST7735S_ShowNum(7, 4, time_array[4], 0xFFFF, 0x0000);
		}
		
		if (time_array[5] < 10)
		{
			ST7735S_ShowChar(7, 7, '0', 0xFFFF, 0x0000);
			ST7735S_ShowNum(7, 8, time_array[5], 0xFFFF, 0x0000);
		}
		else
		{
			ST7735S_ShowNum(7, 7, time_array[5], 0xFFFF, 0x0000);
		}
	}
}
