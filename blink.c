//*****************************************************************************
//
// MSP432 main.c template - P1.0 port toggle
//
//****************************************************************************

#include "msp.h"

void main(void)
{
    volatile uint32_t i;

    WDT_A->rCTL.r = WDTPW | WDTHOLD;    // Stop watchdog timer

    // The following code toggles P1.0 port
    DIO->rPADIR.b.bP1DIR |= BIT0;       // Configure P1.0 as output
    DIO->rPBDIR.b.bP3DIR |= BIT6;

    // TODO:
    // Assert CSn and wait for SO to go low (see section 19.1.1 of the cc1101 datasheet).
    // Set FREND0.PA_POWER=0 and put the desired output power in index 0 of PATABLE


    while(1)
    {
        DIO->rPAOUT.b.bP1OUT ^= BIT0;   // Toggle P1.0
        DIO->rPBOUT.b.bP3OUT ^= BIT6;
        for(i=1000; i>0; i--);         // Delay
    }
}
