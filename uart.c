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
    // Or can use the "backchannel UART" on the LaunchPad.
    // P1.2 - EUSCI_A0 RXD
    // P1.3 - EUSCI_A0 TXD
    // See the schematics in the UG and the device specific datasheet for the default assignment.
    // The port mapping stuff above overrides this, so remove/disable it if using the backchannel UART.
    //DIO->rPASEL0.b.bP1SEL0 |= (BIT2 | BIT3);

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

uint8_t uart_recv(void)
{
    while (EUSCI_A0->rIFG.b.bRXIFG != 1)
        continue;
    return EUSCI_A0->rRXBUF.b.bRXBUF; // This clears RXIFG automatically.
}

void uart_send(uint8_t value)
{
    while (EUSCI_A0->rIFG.b.bTXIFG != 1)
        continue;
    EUSCI_A0->rTXBUF.b.bTXBUF = value; // This clears TXIFG automatically.
    // TODO: do the waiting *afterward*?
}
