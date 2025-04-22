#include "stm32f10x.h"                  // Device header
#include "ST7735S.h"
#include "ESP32C3.h"
#include "Delay.h"
#include "weather.h"
#include "RTC.h"
#include "Http_current.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

static const char *wifi_ssid = "test";
static const char *wifi_pwd = "12345678";
static const char *weather_url = "https://api.seniverse.com/v3/weather/now.json?key=SxDZB8t7wK96XHWuR&location=xian&language=en&unit=c";
static char *weather;
static char *temperature;
static uint16_t *time_array;
//static bool weather_ok = false;
//static bool sntp_ok = false;
static bool weather_request_in_progress = false;
extern EspHttpRequset current_request;
static bool need_update = true;
static bool time_ready = false;
static bool weather_done = false;


void get_weather_time_callback(void)							//中断服务函数中更新需要的标志位
{
	need_update = true;
	//current_request.state = ESP_HTTP_NEED_UPDATE;				//中断中不要搞太长时间的代码，否则肯定会出问题，不要用下面的中断，把屏幕显示都搞到中断里了，最后就屏幕显示混乱
}

/*
这个程序实现了异步不阻塞的天气温度和时间请求，由于能力有限，我没把它做成状态机与任务队列的形式(同时也是因为只有两个任务)
下一版准备添加陀螺仪来实现屏幕的旋转，如果有能力有可能就加入RTOS实现多任务


*/
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
		const char *rsp;
		if (need_update && !weather_request_in_progress)
		{
			esp_http_request_start(weather_url, &rsp, 10000);					//每次都会重置state的值为SENDING，实际上每次循环都在发送而没有解析开正确的值	
																				//这个rsp必须要有的，它存放ESP返回的天气信息
			weather_request_in_progress = true;
			need_update = false;
		}
		
		esp_http_poll();						
		
		if ((current_request.state == ESP_HTTP_DONE) && weather_request_in_progress && esp_http_request_is_done())			//这个代码应该是只有在需要进行网络请求更新时才进入
		{
			weather_request_in_progress = false;
			if (esp_http_request_is_success())
			{
				if (!weather_parse(rsp, &weather, &temperature))				//这就是上面的rsp，程序从这里面解析数据
				{
					need_update = true;											//两任务还是需要两个接收缓冲区的，否则很容易就覆盖了，像这里的问题就是逻辑上走不通，容易产生数据覆盖
				}
				else
				{
					ST7735S_ShowString(2, 9, "        ", 0xFFFF, 0x0000);		//2025.4.21现在解决了写入本地时间戳的问题，但是代码运行不到这里了
					ST7735S_ShowString(2, 9, weather, 0xFFFF, 0x0000);
					ST7735S_ShowString(3, 13, temperature, 0xFFFF, 0x0000);
					weather_done = true;
				}
			}
			else
			{
				need_update = true;
			}
		}
		
		uint32_t ts;
//		if(!esp_at_sntp_init())											//问题出在这里，这个代码运行太快了，主循环走三轮的时间这个时间请求代码才会走完
//		{
//			need_update = true;											//这个Init也有命令发送，可能也要等缓冲区接收完成
//		}
//		esp_at_time_get(&ts);											//在这发送时间请求来写入本地时间戳
//		RTC_Record_Time(&ts, &time_array);

		if (weather_done)
		{
			if (!time_ready)
			{
				if (esp_at_sntp_init())
				{
					time_ready = esp_at_time_get(&ts);
				}
			}
			
			if (time_ready)
			{
				RTC_Record_Time(&ts, &time_array);
				time_ready = false;
				need_update = false;
				weather_done = false;
			}
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
