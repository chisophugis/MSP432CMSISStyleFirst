/*
 * uart.h
 *
 *  Created on: Aug 6, 2015
 *      Author: sean
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

// # UART HAL
//
// 115200 baud UART for talking to PC.
//
// ## Hardware resources used
//
// P3.5 - EUSCI_A0 TXD
// P3.6 - EUSCI_A0 RXD

// Initialize the UART HAL.
void uart_init(void);

// Receive a byte over the UART.
// Note: this function blocks.
uint8_t uart_recv(void);

// Send a byte over the UART.
// Note: this function blocks.
void uart_send(uint8_t value);

#endif /* UART_H_ */
