#ifndef UART_H
#define UART_H

#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include "uart.h"

int uart_open( USART_TypeDef* USARTx, uint32_t baud, uint32_t flags );
int uart_close( USART_TypeDef* USARTx );
int uart_putc( int c, USART_TypeDef* USARTx );
int uart_getc( USART_TypeDef* USARTx );

#endif
