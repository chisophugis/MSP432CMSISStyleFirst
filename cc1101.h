/*
 * cc1101.h
 *
 *  Created on: Aug 2, 2015
 *      Author: sean
 */

#ifndef CC1101_H_
#define CC1101_H_

#include "cc1101_defs.h"

#include "msp.h"
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
//
// ## Requirements
// GDO0 must be configured as if by the following statement:
//     // 0x06 = Asserts when sync word sent/received, de-asserts at end of packet.
//     cc1101_write_reg(CC1101_REG_IOCFG0, 0x06);
// TODO: does it make more sense to simply make this call in cc1101_init, and then
// document "don't touch CC1101_REG_IOCFG0"?

// Initialize the CC1101 HAL.
void cc1101_init(void);

// Shift a byte through the SPI interface.
// This interface is blocking.
//
// b: Byte to shift out.
// return: Byte shifted in while `b` was shifted out (may be garbage).
uint8_t cc1101_shift_byte(uint8_t b);

// Send command strobe `strobe`.
// This interface is blocking.
void cc1101_strobe(uint8_t strobe);

// Write `value` to CC1101 register `addr`.
// This interface is blocking.
void cc1101_write_reg(uint8_t addr, uint8_t value);

// Read CC1101 register `addr`.
// This interface is blocking.
uint8_t cc1101_read_reg(uint8_t addr);

// Send packet containing `data` with length `len`.
// This interface is blocking.
// The packet length is automatically prepended; it should not be included in `data`.
// Note: No complex buffering is done, so TXFIFO will overflow
// if the packet is >63 bytes (64 bytes - 1 for the length).
void cc1101_send_simple_packet(uint8_t *data, uint32_t len);

#endif /* CC1101_H_ */
