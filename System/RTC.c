#include "stm32f10x.h"                  // Device header
#include <time.h>

#include "stdbool.h"

static uint16_t RTC_Time[6];

void RTC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);
	
	RCC_LSEConfig(RCC_LSE_ON);
	while (!RCC_GetFlagStatus(RCC_FLAG_LSERDY));
	
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
	
	RTC_WaitForSynchro();
	RTC_WaitForLastTask();
	
	RTC_SetPrescaler(32768 - 1);
	RTC_WaitForLastTask();
	
//	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
//	{
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
//		
//		PWR_BackupAccessCmd(ENABLE);
//		
//		RCC_LSEConfig(RCC_LSE_ON);
//		while (!RCC_GetFlagStatus(RCC_FLAG_LSERDY));
//		
//		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
//		RCC_RTCCLKCmd(ENABLE);
//		
//		RTC_WaitForSynchro();
//		RTC_WaitForLastTask();
//		
//		RTC_SetPrescaler(32768 - 1);
//		RTC_WaitForLastTask();
//		
//		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
//	}
//	else
//	{
//		RTC_WaitForSynchro();
//		RTC_WaitForLastTask();
//	}
}

//void RTC_Record_Time(uint32_t *timestamp, uint16_t **p_time)				//这个不能一个函数完成所有事情，联网上传时间戳应该是每十分钟一次，还是应该用本地的时间戳来进行时间显示，使用mktime来得到时间，这样RTC工作
//{
//	time_t time_cnt;
//	struct tm time_date;
//	
//	RTC_SetCounter(*timestamp);
//	RTC_WaitForLastTask();
//	
//	time_cnt = RTC_GetCounter() + 60 * 60 * 8;
//	time_date = *localtime(&time_cnt);
//	
//	RTC_Time[0] = time_date.tm_year + 1900;
//	RTC_Time[1] = time_date.tm_mon + 1;
//	RTC_Time[2] = time_date.tm_mday;
//	RTC_Time[3] = time_date.tm_hour;
//	RTC_Time[4] = time_date.tm_min;
//	RTC_Time[5] = time_date.tm_sec;
//	
//	*p_time = RTC_Time;
//}

void RTC_Record_Time(uint32_t *timestamp)
{
//	time_t time_cnt;
//	struct tm time_date;
	
	RTC_SetCounter(*timestamp);
	RTC_WaitForLastTask();
}

void RTC_ReadTime(uint16_t **p_time)
{
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter() + 60 * 60 * 8;
	time_date = *localtime(&time_cnt);
	
	RTC_Time[0] = time_date.tm_year + 1900;
	RTC_Time[1] = time_date.tm_mon + 1;
	RTC_Time[2] = time_date.tm_mday;
	RTC_Time[3] = time_date.tm_hour;
	RTC_Time[4] = time_date.tm_min;
	RTC_Time[5] = time_date.tm_sec;
	
	*p_time = RTC_Time;
}
