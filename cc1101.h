/*
 * cc1101.h
 *
 *  Created on: Aug 2, 2015
 *      Author: sean
 */

#ifndef CC1101_H_
#define CC1101_H_

#include "cc1101_defs.h"

#include <stdint.h>

// # CC1101 HAL
//
// ## Hardware resources used
//
// ### CC1101 SPI interface
// P2.5 - EUSCI_A1 CLK
// P2.6 - EUSCI_A1 SIMO
// P2.7 - EUSCI_A1 SOMI (doubles as GDO1 (defaults to Hi-Z) when CSn is deasserted)
// P5.6 - GPIO - CC1101 CSn
//
// ### Miscellaneous inputs
// P5.7 - GPIO - CC1101 GDO2 (defaults to CHIP_RDYn)
// P6.6 - GPIO - CC1101 GDO0 (defaults to CLK_XOSC/192)

// Initialize the CC1101 HAL.
void cc1101_init(void);

// Shift a byte through the SPI interface.
// This interface is blocking.
//
// b: Byte to shift out.
// return: Byte shifted in while `b` was shifted out (may be garbage).
uint8_t cc1101_shift_byte(uint8_t b);

// TODO: document
void cc1101_strobe(uint8_t strobe);

// TODO: document
void cc1101_write_reg(uint8_t addr, uint8_t value);

#endif /* CC1101_H_ */
