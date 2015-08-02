//*****************************************************************************
//
// MSP432 main.c template - P1.0 port toggle
//
//****************************************************************************

#include "msp.h"


// EUSCI_A0 - 115200 baud UART for console
void init_uart_115200(void);
// EUSCI_A1 - SPI for talking to CC1101
void init_cc1101_spi(void);

void systick_wait(uint32_t nticks);


void main(void)
{
    WDT_A->rCTL.r = WDTPW | WDTHOLD;    // Stop watchdog timer

    init_uart_115200();

    init_cc1101_spi();

    uint8_t uart_to_spi_buf;
    uint8_t spi_to_uart_buf;
    for (;;) {
    	while (EUSCI_A0->rIFG.b.bRXIFG != 1)
    		continue;
    	uart_to_spi_buf = EUSCI_A0->rRXBUF.b.bRXBUF; // This clears RXIFG automatically.

    	while (EUSCI_A1->rIFG.a.bTXIFG != 1)
    		continue;
    	EUSCI_A1->rTXBUF.a.bTXBUF = uart_to_spi_buf; // This clears TXIFG automatically.

    	while (EUSCI_A1->rIFG.a.bRXIFG != 1)
    		continue;
    	spi_to_uart_buf = EUSCI_A1->rRXBUF.a.bRXBUF; // This clears RXIFG automatically.

    	while (EUSCI_A0->rIFG.b.bTXIFG != 1)
    		continue;
    	EUSCI_A0->rTXBUF.b.bTXBUF = spi_to_uart_buf; // This clears TXIFG automatically.

    	systick_wait(3 * 1000);
    }
}

void init_uart_115200(void)
{
	// Map ports.
	// P3.5 - EUSCI_A0 TXD
	// P3.6 - EUSCI_A0 RXD
    PMAP->rKEYID = 0x2D52; // Enable writing.
    // 11.2.2
    // PxSEL.y
    PMAP->rP3MAP45 = (PMAP->rP3MAP45 & 0xFF) | (PM_UCA0TXD << 8); // Ugly. Should use an array of bytes instead.
    DIO->rPBSEL0.b.bP3SEL0 |= BIT5;
    PMAP->rP3MAP67 = (PMAP->rP3MAP67 & 0xFF00) | PM_UCA0RXD;
    DIO->rPBSEL0.b.bP3SEL0 |= BIT6;


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

void init_cc1101_spi(void)
{
	// Map ports.
	PMAP->rKEYID = 0x2D52; // Enable writing.
	// P2.5 - EUSCI_A1 CLK
	// P2.6 - EUSCI_A1 SIMO
	// P2.7 - EUSCI_A1 MISO
	volatile uint8_t *p2 = (uint8_t *)&PMAP->rP2MAP01;
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

}

void systick_wait(uint32_t nticks)
{
	SysTick->LOAD = nticks - 1; // It counts [loaded value,...,1,0].
	SysTick->VAL = 0;
	// Use bus clock. Enable.
	SysTick->CTRL = (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk);
	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
		continue;
	SysTick->CTRL = 0; // Disable.
}
