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
    // Use the "backchannel UART" on the LaunchPad.
    // P1.2 - EUSCI_A0 RXD (`RXD <<` jumper on MSP‑EXP432P401R)
    // P1.3 - EUSCI_A0 TXD (`TXD >>` jumper on MSP‑EXP432P401R)
    // See the schematics in MSP‑EXP432P401R UG.
    // See the DSD for the default assignment.
    //
    // Note the issue described here: https://e2e.ti.com/support/microcontrollers/msp430/f/166/p/417262/1494026#1494026
    // Basically the XDS100-ET's USB-UART bridge is optimized for throughput, so short messages
    // (like short commands) get +150ms latency tacked on as they wait to be flushed by a timeout in the USB-UART bridge.
    // This delay also seems to block the sender on the host PC, which is a big issue in its own right.
    // To avoid this, I'm removing the RXD/TXD jumpers on the MSP‑EXP432P401R and using my own USB-UART bridge.
    DIO->rPASEL0.b.bP1SEL0 |= (BIT2 | BIT3);

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
    EUSCI_A0->rTXBUF.b.bTXBUF = value;
    while (EUSCI_A0->rIFG.b.bTXIFG != 1)
        continue;
}
