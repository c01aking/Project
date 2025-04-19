#ifndef __ESP32C3_H
#define __ESP32C3_H

#include <stdbool.h>
#include <stdint.h>

#define RX_BUFFER_SIZE	4096

/*
我在这里定义了枚举和结构体，用于在另一个轮询文件中引入这个c文件中的中断回调函数的变量
*/
typedef enum{
	RX_RESULT_OK,
	RX_RESULT_ERROR,
	RX_RESULT_FAIL,
}RESULT_STATE;

typedef struct{
	uint8_t rxdata[RX_BUFFER_SIZE];
	uint32_t rxlen;
	bool rxready;
	RESULT_STATE rxresult;
}rxdata_parameter_t;

typedef void(*Timer3_IRQ_Handler_callback_t)(void);

bool esp_at_init(void);
bool esp_at_send_command(const char *cmd, const char **rsp, uint32_t *length, uint32_t timeout);
bool esp_at_send_data(const char *data, uint32_t length);

bool esp_at_reset(void);

bool esp_at_wifi_init(void);
bool esp_at_wifi_connect(const char *ssid, const char *pwd);

bool esp_at_http_get(const char *url, const char **rsp, uint32_t *length, uint32_t timeout);
bool esp_at_time_get(uint32_t *timestamp);
bool esp_at_sntp_init(void);

void Timer3_IRQ_Handler_callback_register(Timer3_IRQ_Handler_callback_t callback);
void Timer_for_ESP_Init(void);

#endif
