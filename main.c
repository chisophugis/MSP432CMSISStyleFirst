#include "cc1101.h"
#include "uart.h"

#include "msp.h"

// # Useful documents
//
// [DSD]    MSP432P401x Device-Specific Datasheet        - <http://www.ti.com/lit/gpn/msp432p401r>
// [TRM]    MSP432P4xx Family Technical Reference Manual - <http://www.ti.com/lit/pdf/slau356>
// [UG]     MSP‑EXP432P401R User's Guide                 - <http://www.ti.com/lit/pdf/slau597>
// [CC1101] CC1101 Datasheet                             - <http://www.ti.com/lit/gpn/cc1101>

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
    {CC1101_REG_IOCFG0,      0x06}, // Required by CC1101 HAL.
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

    uint8_t packet_index = 0;
    for (;;) {
        // TODO: Switch to line oriented format. Extract commands to separate file(s?).
        uint8_t cmd = uart_recv();
        switch (cmd) {
        case 'e': { // _e_cho a byte back
            uint8_t b = uart_recv();
            uart_send(b);
            break;
        }
        case 'r': { // _r_elay byte to CC1101
            uint8_t b = uart_recv();
            uint8_t b2 = cc1101_shift_byte(b);
            uart_send(b2);
            break;
        }
        case 'p': { // send _p_acket
            enum { kPacketLen = 10 };
            ++packet_index;
            packet_index &= ((1 << 4) - 1);
            uint8_t buf[kPacketLen];
            for (i = 0; i < kPacketLen; i++)
                buf[i] = ~packet_index;
            cc1101_send_simple_packet(buf, kPacketLen);
            uart_send(packet_index); // Simple ACK for testing purposes. Is there a better thing to send?
            break;
        }
        case 'f': { // set _f_requency (and calibrate)
            uint8_t freq2 = uart_recv();
            uint8_t freq1 = uart_recv();
            uint8_t freq0 = uart_recv();
            cc1101_write_reg(CC1101_REG_FREQ2, freq2);
            cc1101_write_reg(CC1101_REG_FREQ1, freq1);
            cc1101_write_reg(CC1101_REG_FREQ0, freq0);
            cc1101_strobe(CC1101_STROBE_SCAL);
            // See last row in Table 34 of CC1101 datasheet for timing info.
            systick_wait_us(1000);
            // TODO: Return more calibration registers? That will help understand the behavior better.
            uint8_t fscal1 = cc1101_read_reg(CC1101_REG_FSCAL1);
            uart_send(fscal1);
        }
        }
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

