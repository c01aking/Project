#include "stm32f10x.h"                  // Device header

#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "weather.h"

static char weather_t[32];
static char temperature_t[16];


/*输入为{"results":[{"location":{"id":"WXH13WQT7XKN","name":"Tangshan","country":"CN","path":"Tangshan,Tangshan,Hebei,China","timezone":"Asia/Shanghai","timezone_offset":"+08:00"},"now":{"text":"Cloudy","code":"4","temperature":"9"},"last_update":"2025-03-29T17:00:20+08:00"}]}*/
bool weather_parse(const char *data, char **weather, char **temperature)				//command函数会使rsp指向接收缓冲区
{
	char *p;																		//定义字符串指针，用于确定位置
	p = strstr(data, "\"text\":\"");												//检索天气前的字符
	if (p == NULL)																	//避免空指针
	{
		return false;
	}
	p += strlen("\"text\":\"");														//将指针指向天气的第一个字母
	char *q;
	q = strchr(p, '\"');															//找到最后的双引号，这里的目的是为了确定天气变量的长度
	if (q == NULL)
	{
		return false;
	}
	int len;																		//定义长度
	len = q - p;
	if (len >= sizeof(weather_t))													//防止溢出的保护处理，防止内存越界
	{
		len = sizeof(weather_t) - 1;
	}
	strncpy(weather_t, p, len);
	weather_t[len] = '\0';															//将字符串最后一位定位结束字符
	*weather = weather_t;
	
	p = strstr(data, "\"temperature\":\"");
	if (p == NULL)
	{
		return false;
	}
	p += strlen("\"temperature\":\"");
	q = strchr(p, '\"');
	if (q == NULL)
	{
		return false;
	}
	len = q - p;
	if (len >= sizeof(temperature_t))
	{
		len = sizeof(temperature_t) - 1;
	}
	strncpy(temperature_t, p, len);
	temperature_t[len] = '\0';
	*temperature = temperature_t;
	
	return true;
}
