//*****************************************************************************
//
// MSP432 main.c template - P1.0 port toggle
//
//****************************************************************************

#include "msp.h"

// P3.5 - EUSCI_A0 TXD
// P3.6 - EUSCI_A0 RXD
void init_port_mappings(void);

// EUSCI_A0 - 115200 baud UART for console
void init_uart_115200(void);
// EUSCI_A1 - SPI for talking to CC1101

void main(void)
{
    WDT_A->rCTL.r = WDTPW | WDTHOLD;    // Stop watchdog timer

    init_port_mappings();

    init_uart_115200();

    uint8_t buf;
    for (;;) {
    	while (EUSCI_A0->rIFG.b.bRXIFG != 1)
    		continue;
    	buf = EUSCI_A0->rRXBUF.b.bRXBUF; // This clears RXIFG automatically.

    	while (EUSCI_A0->rIFG.b.bTXIFG != 1)
    		continue;
    	EUSCI_A0->rTXBUF.b.bTXBUF = buf; // This clears TXIFG automatically.
    }
}

void init_port_mappings(void)
{
    // Map USCI_A0 to ports.
    PMAP->rKEYID = 0x2D52; // Enable writing.
    // 11.2.2
    // PxSEL.y
    PMAP->rP3MAP45 = (PMAP->rP3MAP45 & 0xFF) | (PM_UCA0TXD << 8); // Ugly. Should use an array of bytes instead.
    DIO->rPBSEL0.b.bP3SEL0 |= BIT5;
    PMAP->rP3MAP67 = (PMAP->rP3MAP67 & 0xFF00) | PM_UCA0RXD;
    DIO->rPBSEL0.b.bP3SEL0 |= BIT6;
}

void init_uart_115200(void)
{
    EUSCI_A0->rCTLW0.b.bSWRST = 1; // Reset USCI_A0.

    // Use SMCLK, 3MHz out of the box according to http://www.ti.com/lit/ug/slau597/slau597.pdf sec. 2.6
    // 22.3.10
    // 3,000,000/115200 = 26.0417
    EUSCI_A0->rMCTLW.b.bOS16 = 1;
    EUSCI_A0->rBRW = 1;
    EUSCI_A0->rMCTLW.b.bBRF = 10;
    EUSCI_A0->rMCTLW.b.bBRS = 0x00; // Table 22-4

    // Clock source selection.
    EUSCI_A0->rCTLW0.b.bSSEL = 2;

    // ultimately: "Enable interrupts (optional) via UCRXIE or UCTXIE." - but busy loop for now.

    EUSCI_A0->rCTLW0.b.bSWRST = 0; // Bring USCI_A0 out of reset.
}
