//#include "stm32f10x.h"                  // Device header
//#include "Http_current.h"
//#include "Serial.h"
//#include "ESP32C3.h"

//extern rxdata_parameter_t *rxdata_ptr(void);
//static rxdata_parameter_t *rxdata;

//static EspHttpRequset current_request;

//volatile uint32_t sys_ms = 0;

//void SysTick_Handler(void)										//这个函数已经在stm32f10x_it.c文件中定义了，于是我把库文件中的函数加入了__weak前缀使其可以在main中定义，感觉上在it文件中重写函数然后extern应该也可以
//{
//	sys_ms++;
//}

//uint32_t millis(void)
//{
//	return sys_ms;												//加入系统时钟函数
//}

//bool esp_at_http_get(const char *url, const char **rsp, uint32_t *length, uint32_t timeout)
//{
//	char cmd[128];
//	
//	snprintf(cmd, sizeof(cmd), "AT+HTTPCGET=\"%s\"", url);
//	if(!esp_at_send_command(cmd, rsp, NULL, 10000))
//	{
//		return false;
//	}
//	
//	return true;
//}

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

//void esp_http_request_start(const char *url, const char **rsp, uint16_t timeout)	//用于初始化调度任务，确定网络请求命令，缓冲区指针，超时时间，开始时间以及状态
//{
//	char cmd[128];
//	snprintf(cmd, sizeof(cmd), "AT+HTTPCGET=\"%s\"", url);
//	
//	rxdata = rxdata_ptr();					//链接结构体指针，使这里定义的结构体指针指向链接的结构体
//	current_request.cmd 		= cmd;
//	*current_request.rsp 		= (const char *)rxdata->rxdata;						//这里我想让rsp指向rxdata结构体中的rxdata数组地址
//	current_request.timeout		= timeout;
//	current_request.start_time	= millis();
//	current_request.state		= ESP_HTTP_SENDING;
//	current_request.finished	= false;
//}

///*
//将完整的网络请求分成几步，以达成异步轮询的效果
//1、初始化
//2、发送命令
//3、检查接收完成
//将接收到的放在rxdata中，因为串口接收中断由rxdata的数据来判断接收结果
//*/
//void esp_http_poll(void)
//{
//	switch(current_request.state)								//如果一个枚举类型作为了switch的对象但是case没有覆盖全部情况时，就会汇报警告
//	{
//		case ESP_HTTP_SENDING:
//			rxdata->rxlen = 0;
//			rxdata->rxready = true;
//			rxdata->rxresult = RX_RESULT_FAIL;
//			Serial_SendString(current_request.cmd);
//			Serial_SendString("\r\n");
//			current_request.state = ESP_HTTP_WAITING_RESPONSE;
//			break;
//		
//		case ESP_HTTP_WAITING_RESPONSE:							//等待回应
//			if (rxdata->rxready && millis() - current_request.start_time > current_request.timeout)			//如果millis函数用不了也可以使用timeout--的形式吧
//			{
//				current_request.state = ESP_HTTP_WAITING_RESPONSE;
//			}
//			
//		default:
//			break;
//			
//	}
//}

//bool esp_http_request_is_done(void)
//{
//	
//}

//bool esp_http_request_is_success(void)
//{
//	
//}

//void esp_http_request_reset(void)
//{
//	
//}
