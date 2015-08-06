/*
 * uart.h
 *
 *  Created on: Aug 6, 2015
 *      Author: sean
 */

#ifndef UART_H_
#define UART_H_

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

#endif /* UART_H_ */
