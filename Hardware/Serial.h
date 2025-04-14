#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

typedef void(*on_usart_received_t)(uint8_t data);

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(const char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

void esp_usart_receive_register(on_usart_received_t on_usart_received);

uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);

#endif
