//*****************************************************************************
//
// MSP432 main.c template - P1.0 port toggle
//
//****************************************************************************

#include "msp.h"

// EUSCI_A0 - UART for console
// EUSCI_A1 - SPI for talking to CC1101

void main(void)
{
    volatile uint32_t i;

    WDT_A->rCTL.r = WDTPW | WDTHOLD;    // Stop watchdog timer

    // Map USCI_A0 to outputs.
    PMAP->rKEYID = 0x2D52; // Enable writing.
    // 11.2.2
    // PxSEL.y
    PMAP->rP3MAP45 = (PMAP->rP3MAP45 & 0xFF) | (PM_UCA0TXD << 8); // Ugly. Should use an array of bytes instead.
    DIO->rPBSEL0.b.bP3SEL0 = BIT5;

    EUSCI_A0->rCTLW0.b.bSWRST = 1; // Reset USCI_A0.
    // Configure ports.
    //EUSCI_A0->rCTLW0.b.b

    // Use SMCLK, 3MHz out of the box according to http://www.ti.com/lit/ug/slau597/slau597.pdf sec. 2.6
    // 22.3.10
    // 3,000,000/115200 = 26.0417
    EUSCI_A0->rMCTLW.b.bOS16 = 1;
    EUSCI_A0->rBRW = 1;
    EUSCI_A0->rMCTLW.b.bBRF = 10;
    EUSCI_A0->rMCTLW.b.bBRS = 0x00; // Table 22-4

    EUSCI_A0->rCTLW0.b.bSSEL = 2;

    // ultimately: "Enable interrupts (optional) via UCRXIE or UCTXIE." - but busy loop for now.
    EUSCI_A0->rCTLW0.b.bSWRST = 0; // Bring USCI_A0 out of reset.

    static uint8_t msg[] = {0x55, 0xAA, 0x55, 0xAA};
    for (;;) {
    	++i;
    	i &= 3;
    	// This also clears TXIFG automatically.
    	EUSCI_A0->rTXBUF.b.bTXBUF = msg[i];
    	volatile int delay;
    	for (delay = 0; delay < 1000; delay++) // How on earth is this delaying 6.6ms?
    		continue;
    	while (EUSCI_A0->rIFG.b.bTXIFG != 1)
    		continue;
    }
}
