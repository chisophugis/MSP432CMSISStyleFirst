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
// P1.2 - EUSCI_A0 RXD (`RXD <<` jumper on MSP‑EXP432P401R)
// P1.3 - EUSCI_A0 TXD (`TXD >>` jumper on MSP‑EXP432P401R)

// Initialize the UART HAL.
void uart_init(void);

// Receive a byte over the UART.
// Note: this function blocks until a byte arrives.
uint8_t uart_recv(void);

// Send a byte over the UART.
// Note: this function blocks until the byte is sent.
void uart_send(uint8_t value);

#endif /* UART_H_ */
