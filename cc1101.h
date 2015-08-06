/*
 * cc1101.h
 *
 *  Created on: Aug 2, 2015
 *      Author: sean
 */

#ifndef CC1101_H_
#define CC1101_H_

#include <stdint.h>

// # CC1101 HAL
//
// ## Hardware resources used
//
// ### CC1101 SPI interface
// P2.5 - EUSCI_A1 CLK
// P2.6 - EUSCI_A1 SIMO
// P2.7 - EUSCI_A1 SOMI
// P5.6 - GPIO - CC1101 CSn
//
// ### Miscellaneous inputs
// P5.7 - GPIO - CC1101 GDO2 (defaults to CHIP_RDYn)
// P6.6 - GPIO - CC1101 GDO1 (defaults to CLK_XOSC/192)

// Initialize the CC1101 HAL.
void cc1101_init(void);

// Shift a byte through the SPI interface.
// This interface is blocking.
//
// b: Byte to shift out.
// return: Byte shifted in while `b` was shifted out (may be garbage).
uint8_t cc1101_shift_byte(uint8_t b);

// Asserts CSn and waits for CHIP_RDYn from the CC1101.
void cc1101_begin_transaction(void);

// Deasserts CSn.
void cc1101_end_transaction(void);

#endif /* CC1101_H_ */
