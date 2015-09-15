/*
 * cc1101.c
 *
 *  Created on: Aug 2, 2015
 *      Author: sean
 */

#include "cc1101.h"

#include "msp.h"

// TODO: This `GDO0_PIN` thing is awkward.
#define GDO0_PIN BITBAND_PERI(P6IN, 6)

// Asserts CSn and waits for CHIP_RDYn from the CC1101.
static void cc1101_begin_transaction(void);
// Deasserts CSn.
static void cc1101_end_transaction(void);
// FIXME: Once cc1101_shift_byte is no longer public, migrate the docs to here.
// Precondition: Must be inside a transaction.
static uint8_t cc1101_raw_shift_byte(uint8_t b);

void cc1101_init(void)
{
    // Map ports.
    PMAP->rKEYID = 0x2D52; // Enable writing.
    volatile uint8_t *p2 = (volatile uint8_t *)&PMAP->rP2MAP01;
    p2[5] = PM_UCA1CLK;
    p2[6] = PM_UCA1SIMO;
    p2[7] = PM_UCA1SOMI;
    DIO->rPASEL0.b.bP2SEL0 |= (BIT5 | BIT6 | BIT7);

    // Configure.
    EUSCI_A1->rCTLW0.a.bSWRST = 1; // Bring into reset.

    EUSCI_A1->rCTLW0.a.bSYNC = 1; // SPI mode (synchronous).
    EUSCI_A1->rCTLW0.a.bMST = 1; // Master mode.
    EUSCI_A1->rCTLW0.a.bCKPH = 1; // Data latches on positive edge.
    EUSCI_A1->rCTLW0.a.bCKPL = 0; // Inactive state is low.
    EUSCI_A1->rCTLW0.a.bMSB = 1; // MSB first.
    EUSCI_A1->rCTLW0.a.bSSEL = 2; // SMCLK
    EUSCI_A1->rBRW = 100; // SMCLK divider.

    EUSCI_A1->rCTLW0.a.bSWRST = 0; // Bring out of reset.

    // P5.6 - GPIO - CC1101 CSn
    DIO->rPCDIR.b.bP5DIR |= BIT6; // Configure as output.
    DIO->rPCOUT.b.bP5OUT |= BIT6; // Deassert CSn.

    // P5.7 - GPIO - CC1101 GDO2 (defaults to CHIP_RDYn)
    DIO->rPCDIR.b.bP5DIR &= ~BIT7; // Configure as input.

    // P6.6 - GPIO - CC1101 GDO0 (defaults to CLK_XOSC/192)
    DIO->rPCDIR.b.bP6DIR &= ~BIT6; // Configure as input.
}

uint8_t cc1101_shift_byte(uint8_t b)
{
    cc1101_begin_transaction();
    uint8_t ret = cc1101_raw_shift_byte(b);
    cc1101_end_transaction();
    return ret;
}

void cc1101_strobe(uint8_t strobe)
{
    cc1101_begin_transaction();
    (void)cc1101_raw_shift_byte(strobe);
    cc1101_end_transaction();
}

void cc1101_write_reg(uint8_t addr, uint8_t value)
{
    cc1101_begin_transaction();
    (void)cc1101_raw_shift_byte(addr);
    (void)cc1101_raw_shift_byte(value);
    cc1101_end_transaction();
}

uint8_t cc1101_read_reg(uint8_t addr)
{
    enum { kReadBit = 0x80 };
    cc1101_begin_transaction();
    (void)cc1101_raw_shift_byte(addr | kReadBit);
    uint8_t ret = cc1101_raw_shift_byte(0);
    cc1101_end_transaction();
    return ret;
}

void cc1101_send_simple_packet(uint8_t *data, uint32_t len)
{
    cc1101_write_reg(CC1101_REG_TXFIFO, len);
    int i;
    for (i = 0; i < len; i++)
        cc1101_write_reg(CC1101_REG_TXFIFO, data[i]);
    cc1101_strobe(CC1101_STROBE_STX);
    while (!GDO0_PIN)
        continue;
    while (GDO0_PIN)
        continue;
}

static void cc1101_begin_transaction(void)
{
    DIO->rPCOUT.b.bP5OUT &= ~BIT6; // Assert CSn.
    while ((DIO->rPCIN.b.bP5IN & BIT7) == BIT7) // Wait for CHIP_RDYn to be asserted.
        continue;
}

static void cc1101_end_transaction(void)
{
    DIO->rPCOUT.b.bP5OUT |= BIT6; // Deassert CSn.
}

static uint8_t cc1101_raw_shift_byte(uint8_t b)
{
    // Reset RXIFG.
    // Otherwise, we will end up never waiting for RXIFG below,
    // causing the RXBUF read below to read a stale value.
    // To boot, we are relying on the read from RXBUF
    // to clear RXIFG but since since we are plowing ahead
    // without waiting for the byte to actually be received,
    // RXIFG will end up set sometime later once the byte is
    // shifted in/out, perpetuating the cycle!
    EUSCI_A1->rIFG.a.bRXIFG = 0;

    while (EUSCI_A1->rIFG.a.bTXIFG != 1)
        continue;
    EUSCI_A1->rTXBUF.a.bTXBUF = b;
    while (EUSCI_A1->rIFG.a.bRXIFG != 1)
        continue;
    return EUSCI_A1->rRXBUF.a.bRXBUF;
}
