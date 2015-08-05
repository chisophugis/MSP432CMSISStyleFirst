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

	// TODO:
	// P5.6 - GPIO - CC1101 CSn
	// P5.7 - GPIO - CC1101 GDO2 (defaults to CHIP_RDYn)
	// P6.6 - GPIO - CC1101 GDO1 (defaults to CLK_XOSC/192)
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
