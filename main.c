#include "cc1101.h"
#include "uart.h"

#include "msp.h"

// # Useful documents
//
// [DSD] MSP432P401x Device-Specific Datasheet        - <http://www.ti.com/lit/gpn/msp432p401r>
// [TRM] MSP432P4xx Family Technical Reference Manual - <http://www.ti.com/lit/pdf/slau356>
// [UG]  MSP‑EXP432P401R User's Guide                 - <http://www.ti.com/lit/pdf/slau597>

void systick_wait(uint32_t nticks);


void main(void)
{
    WDT_A->rCTL.r = WDTPW | WDTHOLD;    // Stop watchdog timer

    uart_init();
    cc1101_init();

    cc1101_strobe(CC1101_STROBE_SRES); // Reset the CC1101 (TODO: do this in cc1101_init?).

    uint8_t uart_to_spi_buf;
    uint8_t spi_to_uart_buf;
    for (;;) {
        while (EUSCI_A0->rIFG.b.bRXIFG != 1)
            continue;
        uart_to_spi_buf = EUSCI_A0->rRXBUF.b.bRXBUF; // This clears RXIFG automatically.

        spi_to_uart_buf = cc1101_shift_byte(uart_to_spi_buf);

        while (EUSCI_A0->rIFG.b.bTXIFG != 1)
            continue;
        EUSCI_A0->rTXBUF.b.bTXBUF = spi_to_uart_buf; // This clears TXIFG automatically.
    }
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
