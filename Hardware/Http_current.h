#ifndef __HTTP_CURRENT_H
#define __HTTP_CURRENT_H

#include <stdbool.h>
#include <stdio.h>

#include "ESP32C3.h"

typedef enum{
	//ESP_HTTP_NEED_UPDATE = 1,
	ESP_HTTP_SENDING,
	ESP_HTTP_WAITING_RESPONSE,
	ESP_HTTP_DONE,
	ESP_HTTP_ERROR,
}EspHttpState;

typedef struct{
	char cmd_temp[128];
	const char *cmd;
	const char *rsp;
	uint16_t timeout;
//	uint32_t start_time;
	EspHttpState state;
	bool finished;
}EspHttpRequset;

void esp_http_request_start(const char *cmd, const char **rsp, uint16_t timeout);
void esp_http_poll(void);
bool esp_http_request_is_done(void);
bool esp_http_request_is_success(void);

#endif 
