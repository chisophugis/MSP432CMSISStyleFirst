/*
 * cc1101.c
 *
 *  Created on: Aug 2, 2015
 *      Author: sean
 */

#include "cc1101.h"

#include "msp.h"

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
    // TODO: Use GDO0?
}

uint8_t cc1101_shift_byte(uint8_t b)
{
    while (EUSCI_A1->rIFG.a.bTXIFG != 1)
        continue;
    EUSCI_A1->rTXBUF.a.bTXBUF = b;
    while (EUSCI_A1->rIFG.a.bRXIFG != 1)
        continue;
    uint8_t ret = EUSCI_A1->rRXBUF.a.bRXBUF;
    return ret;
}

void cc1101_begin_transaction(void)
{
    DIO->rPCOUT.b.bP5OUT &= ~BIT6; // Assert CSn.
    while ((DIO->rPCIN.b.bP5IN & BIT6) == BIT6) // Wait for CHIP_RDYn to be asserted.
        continue;
}

void cc1101_end_transaction(void)
{
    DIO->rPCOUT.b.bP5OUT |= BIT6; // Deassert CSn.
}
