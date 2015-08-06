#include "cc1101.h"

#include "msp.h"

// # Useful documents
//
// MSP432P401x datasheet <http://www.ti.com/lit/gpn/msp432p401r>
// MSP432P4xx Family TRM <http://www.ti.com/lit/pdf/slau356>
// MSP‑EXP432P401R UG <http://www.ti.com/lit/pdf/slau597>

// EUSCI_A0 - 115200 baud UART for console
void init_uart_115200(void);

void systick_wait(uint32_t nticks);


void main(void)
{
    WDT_A->rCTL.r = WDTPW | WDTHOLD;    // Stop watchdog timer

    init_uart_115200();

    cc1101_init();

    cc1101_begin_transaction();
    cc1101_shift_byte(0x30); // SRES command strobe. Resets the chip.
    cc1101_end_transaction();

    uint8_t uart_to_spi_buf;
    uint8_t spi_to_uart_buf;
    for (;;) {
        while (EUSCI_A0->rIFG.b.bRXIFG != 1)
            continue;
        uart_to_spi_buf = EUSCI_A0->rRXBUF.b.bRXBUF; // This clears RXIFG automatically.

        cc1101_begin_transaction();
        spi_to_uart_buf = cc1101_shift_byte(uart_to_spi_buf);
        cc1101_end_transaction();

        while (EUSCI_A0->rIFG.b.bTXIFG != 1)
            continue;
        EUSCI_A0->rTXBUF.b.bTXBUF = spi_to_uart_buf; // This clears TXIFG automatically.
    }
}

void init_uart_115200(void)
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
