#include "cc1101.h"
#include "uart.h"

#include "msp.h"

// # Useful documents
//
// [DSD] MSP432P401x Device-Specific Datasheet        - <http://www.ti.com/lit/gpn/msp432p401r>
// [TRM] MSP432P4xx Family Technical Reference Manual - <http://www.ti.com/lit/pdf/slau356>
// [UG]  MSP‑EXP432P401R User's Guide                 - <http://www.ti.com/lit/pdf/slau597>

void systick_wait(uint32_t nticks);
void systick_wait_us(uint32_t us);


// Just the registers that differ from the defaults.
// Generated by SmartRF Studio.
// TODO: in the future, make sure to also have SmartRF Studio
// generate "what these settings actually are" description.
// E.g. 433MHz, no manchester, etc.
struct register_setting {
    uint8_t addr;
    uint8_t value;
};
static const struct register_setting preferred_settings[] =
{
    {CC1101_REG_IOCFG0,      0x06},
    {CC1101_REG_FIFOTHR,     0x47},
    {CC1101_REG_PKTCTRL0,    0x05},
    {CC1101_REG_FSCTRL1,     0x06},
    {CC1101_REG_FREQ2,       0x10},
    {CC1101_REG_FREQ1,       0xA7},
    {CC1101_REG_FREQ0,       0x62},
    {CC1101_REG_MDMCFG4,     0xF8},
    {CC1101_REG_MDMCFG3,     0x93},
    {CC1101_REG_MDMCFG2,     0x33},
    {CC1101_REG_DEVIATN,     0x15},
    {CC1101_REG_MCSM0,       0x18},
    {CC1101_REG_FOCCFG,      0x16},
    {CC1101_REG_WORCTRL,     0xFB},
    {CC1101_REG_FREND0,      0x11},
    {CC1101_REG_FSCAL3,      0xE9},
    {CC1101_REG_FSCAL2,      0x2A},
    {CC1101_REG_FSCAL1,      0x00},
    {CC1101_REG_FSCAL0,      0x1F},
    {CC1101_REG_TEST2,       0x81},
    {CC1101_REG_TEST1,       0x35},
    {CC1101_REG_TEST0,       0x09}
};

void main(void)
{
    WDT_A->rCTL.r = WDTPW | WDTHOLD;    // Stop watchdog timer

    uart_init();
    cc1101_init();

    cc1101_strobe(CC1101_STROBE_SRES); // Reset the CC1101 (TODO: do this in cc1101_init?).

    // Configure CC1101 registers.
    int i;
    int len = sizeof(preferred_settings)/sizeof(preferred_settings[0]);
    for (i = 0; i < len; i++) {
        const struct register_setting *p = &preferred_settings[i];
        cc1101_write_reg(p->addr, p->value);
    }

    uint8_t uart_to_spi_buf;
    uint8_t spi_to_uart_buf;
    uint8_t iters = 0;
    for (;;) {
        while (EUSCI_A0->rIFG.b.bRXIFG != 1)
            continue;
        uart_to_spi_buf = EUSCI_A0->rRXBUF.b.bRXBUF; // This clears RXIFG automatically.

        const int kPacketLen = 10;
        cc1101_write_reg(CC1101_REG_TXFIFO, kPacketLen);
        ++iters;
        iters &= ((1 << 4) - 1);
        for (i = 0; i < kPacketLen; i++)
            cc1101_write_reg(CC1101_REG_TXFIFO, ~iters);
        cc1101_strobe(CC1101_STROBE_STX);
        // XXX: detect end of transmission. Configure GDO0_CFG to 0x06? (non-default)
        systick_wait_us(100 * 1000);

        spi_to_uart_buf = cc1101_shift_byte(uart_to_spi_buf);

        while (EUSCI_A0->rIFG.b.bTXIFG != 1)
            continue;
        EUSCI_A0->rTXBUF.b.bTXBUF = spi_to_uart_buf; // This clears TXIFG automatically.
    }
}

void systick_wait_us(uint32_t us)
{
    // 3 MHz clock rate.
    systick_wait(us * 3);
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

