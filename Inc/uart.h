/*
 * uart.h
 *
 *  Created on: Jan 10, 2026
 *      Author: parth
 */

#ifndef UART_H_
#define UART_H_
#include <stdint.h>
#include "stm32f4xx.h"

void uart2_rxtx_init(void);
char uart2_read(void);
void uart2_write(int ch);
void uart2_print(const char* str);
void uart2_write_floatx();
void uart2_write_floaty();
void dma1_stream6_init(uint32_t src,uint32_t dst,uint32_t len);

#endif /* UART_H_ */
