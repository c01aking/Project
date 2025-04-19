#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Serial.h"
#include "ESP32C3.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#define RX_BUFFER_SIZE	4096			//用于定义缓冲区大小的宏					

//#define RX_RESULT_OK	0
//#define RX_RESULT_ERROR	1
//#define RX_RESULT_FAIL	2

//static uint8_t rxdata[RX_BUFFER_SIZE];
//static uint32_t rxlen;
//static bool rxready;
//static uint8_t rxresult;

static rxdata_parameter_t rxdata_parameter;
Timer3_IRQ_Handler_callback_t Timer3_IRQ_Handler_callback;

rxdata_parameter_t *rxdata_ptr(void)
{
	return &rxdata_parameter;							//将结构体指针传出，用于在另一份文件中操作中断回调服务函数的变量
}

/*
这一版是没有把中断用到的参数改成结构体的版本，因为在异步轮询代码中需要引用同样的结构体，所以我把它改成了
用结构体和成员定义变量的方式，然后用一个指针传走
不用全局变量是为了实现模块内封装与模块外访问，static关键字用于避免全局变量的命名污染
*/
//static void on_usart_received(uint8_t data)				//每当串口收到一个数据，就传入回调函数，存入接收缓冲区，收到换行符时检查缓冲区
//{
//	if(!rxready)
//	{
//		return;
//	}
//	
//	if(rxlen < RX_BUFFER_SIZE)			//程序是顺序执行下来的，不需要再次判断rxready = true，判断接收缓冲区是否溢出
//	{
//		rxdata[rxlen++] = data;			//rxdata[rxlen] = data;	rxlen++;
//	}
//	
//	else
//	{
//		rxresult = RX_RESULT_FAIL;		//错误码机制
//		rxready = false;
//		return;
//	}
//	
//	if(data == '\n')					//此时rxdata[rxlen - 1]应也为'\n'，因为排入缓冲区后，索引(或是说长度加了一)
//	{
//		if(rxlen >= 2 && rxdata[rxlen - 2] == '\r')			//所以这里查的是倒数第二个，rxlen减了二
//		{
//			if(rxlen >= 4 && rxdata[rxlen - 4] == 'O' && rxdata[rxlen - 3] == 'K')		//ESP32会回复“OK”或“ERROR”,这两段就在判断是OK还是ERROR
//			{
//				rxresult = RX_RESULT_OK;
//				rxready = false;
//			}
//			else if(rxlen >= 7 && rxdata[rxlen -7] == 'E' && rxdata[rxlen - 6] == 'R' && rxdata[rxlen - 5] == 'R'
//				&& rxdata[rxlen - 4] == 'O' && rxdata[rxlen - 3] == 'R')
//			{
//				rxresult = RX_RESULT_ERROR;
//				rxready = false;
//			}
//		}
//	}
//}

static void on_usart_received(uint8_t data)				//每当串口收到一个数据，就传入回调函数，存入接收缓冲区，收到换行符时检查缓冲区
{
	if(!rxdata_parameter.rxready)
	{
		return;
	}
	
	if(rxdata_parameter.rxlen < RX_BUFFER_SIZE)			//程序是顺序执行下来的，不需要再次判断rxready = true，判断接收缓冲区是否溢出
	{
		rxdata_parameter.rxdata[rxdata_parameter.rxlen++] = data;			//rxdata[rxlen] = data;	rxlen++;
	}
	
	else
	{
		rxdata_parameter.rxresult = RX_RESULT_FAIL;		//错误码机制
		rxdata_parameter.rxready = false;
		return;
	}
	
	if(data == '\n')					//此时rxdata[rxlen - 1]应也为'\n'，因为排入缓冲区后，索引(或是说长度加了一)
	{
		if(rxdata_parameter.rxlen >= 2 && rxdata_parameter.rxdata[rxdata_parameter.rxlen - 2] == '\r')			//所以这里查的是倒数第二个，rxlen减了二
		{
			if(rxdata_parameter.rxlen >= 4 && rxdata_parameter.rxdata[rxdata_parameter.rxlen - 4] == 'O' && rxdata_parameter.rxdata[rxdata_parameter.rxlen - 3] == 'K')		//ESP32会回复“OK”或“ERROR”,这两段就在判断是OK还是ERROR
			{
				rxdata_parameter.rxresult = RX_RESULT_OK;
				rxdata_parameter.rxready = false;
			}
			else if(rxdata_parameter.rxlen >= 7 
				&& rxdata_parameter.rxdata[rxdata_parameter.rxlen -7] == 'E' 
				&& rxdata_parameter.rxdata[rxdata_parameter.rxlen - 6] == 'R' 
				&& rxdata_parameter.rxdata[rxdata_parameter.rxlen - 5] == 'R'
				&& rxdata_parameter.rxdata[rxdata_parameter.rxlen - 4] == 'O' 
				&& rxdata_parameter.rxdata[rxdata_parameter.rxlen - 3] == 'R')
			{
				rxdata_parameter.rxresult = RX_RESULT_ERROR;
				rxdata_parameter.rxready = false;
			}
		}
	}
}

/*
未使用结构体版本
*/
//bool esp_at_send_command(const char *cmd, const char **rsp, uint32_t *length, uint32_t timeout)
//{
//	rxlen = 0;						//重置缓冲区数组索引
//	rxready = true;					//准备接收数据确定
//	rxresult = RX_RESULT_FAIL;		//设置为错误状态是为了保证只有真正正确时才置正确的标志，防止某些逻辑上的错误，所有的标志变量在定义时初始状态一般都为错误
//	
//	Serial_SendString(cmd);			//字符指针和字符串是相同的
//	Serial_SendString("\r\n");			//字符串实际上是预编译确定的指针类型，在代码中确定的字符串会被编译为指针，当需要字符串时实际上是传入了指针类型，这使得传字符串的形参需要是字符指针类型
//	
//	while (rxready && timeout --)			//取一个，接收完成或是超时都中止
//	{
//		Delay_ms(1);
//	}
////	while(rxready);
//	
//	rxready = false;						//正确的流程会在中断服务函数里置rxready为false，如果超时，这一条语句置为false，将状态覆盖重置
//	
//	if(rsp)									//在C语言中，指针在条件判断中被视为布尔值，NULL为false，非NULL为true，实际上这里是判断rap是否为空指针
//	{										//这样说来，字符串也可以作为if判断的条件，但是字符串在预编译时已经确定了地址，所以永远是true
//		*rsp = (const char *)rxdata;		//数组变量在C语言中大多会衰变为指向其第一个元素的指针，即rxdata指的实际是&rxdata[0]，仅需要将类型对齐即可
//	}										//为什么一定要定义rsp是char类型的二级指针，这是为了实现逐字节访问内存，如果rsp是uint32_t **,那么每次解引用这个指针都会得到4个字节,而不能自由的访问单个字节
//											/*
//											直接对二级指针进行类型转换和先解一次引用再对一级指针强制类型转换是相同的，但是不推荐，
//											这样写能明确的通知编译器，我需要这个uint32_t的指针，但是只需要它指向的内存，当解析它时，就按char类型解而不是uint32_t
//											*/
//	if(length)
//	{
//		*length = rxlen;
//	}
//	
//	return rxresult == RX_RESULT_OK;		//其实是在中断服务函数中由不同的状态定义rxresult的结果，它是那个RX的标志变量，如果正确的接收了ESP的回复，则置true，否则为false
//}

bool esp_at_send_command(const char *cmd, const char **rsp, uint32_t *length, uint32_t timeout)
{
	rxdata_parameter.rxlen = 0;						//重置缓冲区数组索引
	rxdata_parameter.rxready = true;					//准备接收数据确定
	rxdata_parameter.rxresult = RX_RESULT_FAIL;		//设置为错误状态是为了保证只有真正正确时才置正确的标志，防止某些逻辑上的错误，所有的标志变量在定义时初始状态一般都为错误
	
	Serial_SendString(cmd);			//字符指针和字符串是相同的
	Serial_SendString("\r\n");			//字符串实际上是预编译确定的指针类型，在代码中确定的字符串会被编译为指针，当需要字符串时实际上是传入了指针类型，这使得传字符串的形参需要是字符指针类型
	
	while (rxdata_parameter.rxready && timeout --)			//取一个，接收完成或是超时都中止
	{
		Delay_ms(1);
	}
//	while(rxready);
	
	rxdata_parameter.rxready = false;						//正确的流程会在中断服务函数里置rxready为false，如果超时，这一条语句置为false，将状态覆盖重置
	
	if(rsp)									//在C语言中，指针在条件判断中被视为布尔值，NULL为false，非NULL为true，实际上这里是判断rap是否为空指针
	{										//这样说来，字符串也可以作为if判断的条件，但是字符串在预编译时已经确定了地址，所以永远是true
		*rsp = (const char *)rxdata_parameter.rxdata;		//数组变量在C语言中大多会衰变为指向其第一个元素的指针，即rxdata指的实际是&rxdata[0]，仅需要将类型对齐即可
	}										//为什么一定要定义rsp是char类型的二级指针，这是为了实现逐字节访问内存，如果rsp是uint32_t **,那么每次解引用这个指针都会得到4个字节,而不能自由的访问单个字节
											/*
											直接对二级指针进行类型转换和先解一次引用再对一级指针强制类型转换是相同的，但是不推荐，
											这样写能明确的通知编译器，我需要这个uint32_t的指针，但是只需要它指向的内存，当解析它时，就按char类型解而不是uint32_t
											*/
	if(length)
	{
		*length = rxdata_parameter.rxlen;
	}
	
	return rxdata_parameter.rxresult == RX_RESULT_OK;		//其实是在中断服务函数中由不同的状态定义rxresult的结果，它是那个RX的标志变量，如果正确的接收了ESP的回复，则置true，否则为false
}

bool esp_at_reset(void)
{
	if(!esp_at_send_command("AT+RESTORE", NULL, NULL, 10000))
	{
		return false;
	}
	
	Delay_ms(2000);
	
	if(!esp_at_send_command("ATE0", NULL, NULL, 1000))
	{
		return false;
	}
	
	if(!esp_at_send_command("AT+SYSSTORE=0", NULL, NULL, 1000))
	{
		return false;
	}
	
	return true;
}

/*
未使用结构体版本
*/
//bool esp_at_init(void)
//{
//    rxready = false;
//	
//	Serial_Init();
//	esp_usart_receive_register(on_usart_received);					//注册回调函数后，reset函数可以使用中断回调服务函数，将回传信息存放在缓冲区里
//	
//	return esp_at_reset();
//}

bool esp_at_init(void)
{
    rxdata_parameter.rxready = false;
	
	Serial_Init();
	esp_usart_receive_register(on_usart_received);					//注册回调函数后，reset函数可以使用中断回调服务函数，将回传信息存放在缓冲区里
	
	return esp_at_reset();
}

bool esp_at_send_data(const char *data, uint32_t length)
{
    Serial_SendArray((uint8_t *)data, length);
	
	return true;
}

bool esp_at_wifi_init(void)
{	
	if(!esp_at_send_command("AT+CWMODE=1", NULL, NULL, 1000))
	{
		return false;
	}
	
	return true;
}
bool esp_at_wifi_connect(const char *ssid, const char *pwd)
{
	char cmd[64];
	
	snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
	if(!esp_at_send_command(cmd, NULL, NULL, 10000))
	{
		return false;
	}
	return true;
}

bool esp_at_http_get(const char *url, const char **rsp, uint32_t *length, uint32_t timeout)
{
	char cmd[128];
	
	snprintf(cmd, sizeof(cmd), "AT+HTTPCGET=\"%s\"", url);
	if(!esp_at_send_command(cmd, rsp, NULL, 10000))
	{
		return false;
	}
	
	return true;
}

bool esp_at_sntp_init(void)
{
//	if(!esp_at_send_command("AT+SNTP=1,\"cn.ntp.org.cn\"", NULL, NULL, 1000))
//	{
//		return false;
//	}
	
	if(!esp_at_send_command("AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"", NULL, NULL, 1000))
	{
		return false;
	}
	
	if(!esp_at_send_command("AT+CIPSNTPTIME?", NULL, NULL, 1000))
	{
		return false;
	}
	
	return true;
}

bool esp_at_time_get(uint32_t *timestamp)
{
	const char *rsp;
	uint32_t length;
	
	if(!esp_at_send_command("AT+SYSTIMESTAMP?", &rsp, &length, 1000))
	{
		return false;
	}
	
	char *sts = strstr(rsp, "+SYSTIMESTAMP:") + strlen("+SYSTIMESTAMP:");
	
	*timestamp = atoi(sts);
	
	return true;
}

void Timer_for_ESP_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);						//TIM2定时1s用于为TIM3提供时钟输入产生级联
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_ClockDivision		= TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode		= TIM_CounterMode_Down;
	TIM_TimeBaseStructure.TIM_Period			= 10000 - 1;					//自动重装载
	TIM_TimeBaseStructure.TIM_Prescaler			= 7200 - 1;						//72MHz预分频
	TIM_TimeBaseStructure.TIM_RepetitionCounter	= 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);						//选择TIM2为主模式，送出更新事件为TRGO输出信号
	
//	TIM_Cmd(TIM2, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);						//TIM3用于产生中断信号，实现每10min更新天气与时间数据
	TIM_ITRxExternalClockConfig(TIM3, TIM_TS_ITR1);
	
	TIM_TimeBaseStructure.TIM_ClockDivision		= TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode		= TIM_CounterMode_Down;
	TIM_TimeBaseStructure.TIM_Period			= 600 - 1;						//计数到600即10min，目前问题可能出现在这里，TIM2定时器溢出时没有级联TIM3，这导致TIM不能正确的计数
	TIM_TimeBaseStructure.TIM_Prescaler			= 1 - 1;						//1Hz不分频
	TIM_TimeBaseStructure.TIM_RepetitionCounter	= 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_External1);							//配置TIM3为从模式，接收来自TIM2的时钟信号
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);								//这个优先级分组一个程序只能分一次，它决定了中断是如何划分的
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel						= TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 1;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}

void Timer3_IRQ_Handler_callback_register(Timer3_IRQ_Handler_callback_t callback)
{
	Timer3_IRQ_Handler_callback = callback;
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		Timer3_IRQ_Handler_callback();
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
