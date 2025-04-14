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

//void get_weather_time_callback(void)							//这个中断还没进行调试，先修改两个定时器的值实现在OLED上显示每秒自增以确定定时器的正确性
//{
//	const char *rsp;
//	esp_at_http_get(weather_url, &rsp, NULL, 10000);
//	if (weather_parse(rsp, &weather, &temperature))
//	{
//		weather_ok = true;
//	}
//	else
//	{
//		weather_ok = false;
//	}
//	
//	uint32_t ts;
//	sntp_ok = esp_at_sntp_init();
//	esp_at_time_get(&ts);
//	RTC_Record_Time(&ts, &time_array);
//}

int main(void)
{
//	Timer3_IRQ_Handler_callback_register(get_weather_time_callback);
	ST7735S_Init();
	ST7735S_clear(0x0000);
	RTC_Init();
	while (!esp_at_init());											//串口通信需要约定好波特率,要不然肯定会卡住
	while (!esp_at_wifi_init());
	while (!esp_at_wifi_connect(wifi_ssid, wifi_pwd));
	

	uint16_t t = 0;
	
	ST7735S_ShowString(1, 1, "Xi'an", 0xFFFF, 0x0000);
	ST7735S_ShowString(2, 1, "Weather:", 0xFFFF, 0x0000);
	ST7735S_ShowString(3, 1, "Temperature:", 0xFFFF, 0x0000);
	ST7735S_ShowWord(6, 5, Year, 0xFFFF);
	ST7735S_ShowWord(6, 9, Month, 0xFFFF);
	ST7735S_ShowWord(6, 13, Day, 0xFFFF);
	ST7735S_ShowString(7, 1, "  :  :  ", 0xFFFF, 0x0000);
	
	while (1)
	{
		t ++;
//		Delay_ms(500);
		
		if(!weather_ok || t % 120 == 0)									//有时会卡住是因为这个时候满足了t值的条件，它去联网更新了天气和时间，但是由于阻塞式的代码导致了执行效率低，看起来像是卡住
			{																//做成软件触发中断和定时器中断的形式，可能能够改善这个情况(使用一个定时器，当标志位不满足时手动修改计数器产生溢出，当标志正确时，定时器自动定时，到时产生中断，在中断中处理天气和时间数据的获取)
			const char *rsp;
			esp_at_http_get(weather_url, &rsp, NULL, 10000);			//问题出现在这里，rsp已经正确的接收了待接收的值，但是跳出这个函数时，rsp指向的内存会乱码(不知道为什么解决了)
			if (weather_parse(rsp, &weather, &temperature))	
			/*
			如果你想接一个数据回来，那么定义的时候就不要只定义指针，而是定义数组，数组才是真正能接数据的，指针更像是传话的，它不能完整的带回整件事情
			但是参数是传值进去，如果想把它带出来就得用指针，传一级指针只是传进去指向了一个数组，但是没法带出来，于是就类似传一级指针带值出来的思路传二级指针进去带一个一级指针(即字符数组)出来
			*/
			{
				weather_ok = true;
			}
			else
			{
				weather_ok = false;
			}
			
			ST7735S_ShowString(2, 9, weather, 0xFFFF, 0x0000);
			ST7735S_ShowString(3, 13, temperature, 0xFFFF, 0x0000);
			//显示在屏幕上
			}
		
		if (!sntp_ok || t == 1200)
		{
			uint32_t ts;
			sntp_ok = esp_at_sntp_init();
			esp_at_time_get(&ts);
			RTC_Record_Time(&ts, &time_array);
			
			t = 0;
			//显示在屏幕上
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
		
		
		
						//进位的时候会出现问题，原因是屏幕显示的代码实现时，个位显示在左边，而不会占全
	}
}
