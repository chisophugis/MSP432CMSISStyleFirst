/*
 * uart.c
 *
 *  Created on: Aug 6, 2015
 *      Author: sean
 */

#include "uart.h"

#include "msp.h"

void uart_init(void)
{
    // Map ports.
    // P3.5 - EUSCI_A0 TXD
    // P3.6 - EUSCI_A0 RXD
    PMAP->rKEYID = 0x2D52; // Enable writing.
    volatile uint8_t *p3 = (volatile uint8_t *)&PMAP->rP3MAP01;
    p3[5] = PM_UCA0TXD;
    p3[6] = PM_UCA0RXD;
    DIO->rPBSEL0.b.bP3SEL0 |= (BIT5 | BIT6);

    // Configure.
    EUSCI_A0->rCTLW0.b.bSWRST = 1; // Bring into reset.

    // Use SMCLK, 3MHz out of the box for MSP‑EXP432P401R.
    // See Family TRM Section 22.3.10 for the details of the baud rate setup here.
    // 3,000,000/115200 = 26.0417
    EUSCI_A0->rMCTLW.b.bOS16 = 1;
    EUSCI_A0->rBRW = 1;
    EUSCI_A0->rMCTLW.b.bBRF = 10;
    EUSCI_A0->rMCTLW.b.bBRS = 0x00; // Table 22-4
    EUSCI_A0->rCTLW0.b.bSSEL = 2; // SMCLK clock source.

    EUSCI_A0->rCTLW0.b.bSWRST = 0; // Bring out of reset.
}
