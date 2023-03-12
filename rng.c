/*
 * rng.c
 *
 *  Created on: Jun 17, 2022
 *      Author: jenkins
 */

#include <msp430.h>
#include "rng.h"

volatile unsigned int RNG_val = 0;
volatile unsigned char RNG_shift;

// This is for majority voting across multiple captures.
//volatile unsigned int RNG_captures[RNG_CAPTURE_COUNT];
//unsigned char RNG_captureIndex = 0;

unsigned int rng_get_val(unsigned char bitWidth) {
    //RNG_captureIndex = 0;
    RNG_val = 0;
    RNG_shift = bitWidth - 1;

    // - Set ACLK as source for timer in capture mode.
    TB0CCTL0 |=                                     // Timer0_B3 Setup
            CM_1 |                                  // Capture rising edge
            CCIS_1 |                                // Use CCI0B=ACLK
            CCIE |                                  // Enable capture interrupt
            CAP;                                    // Enable capture mode

    // - Capture DCO / MCLK cycles.
    TB0CTL |=
            TBSSEL_2 |                              // Use SMCLK as clock source
            MC_2 |                                  // Start timer in continuous mode
            TBCLR;                                 // clear TB0R

    P1OUT &= ~BIT0; // LED2 Off

    // - Start timer and go to LPM.
    __bis_SR_register(LPM0_bits | GIE);
    __no_operation();

    return RNG_val;
}

// Timer0_B3 CCR0, TB Interrupt Handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) TIMER0_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    TB0CCTL0 &= ~CCIFG;         // Turn off Capture/Compare Interrupt Flag.
    P1OUT ^= BIT0;              // Toggle LED2

    // - Check if captured cycles is even/odd every ACLK cycle (in the interrupt)
    RNG_val |= (TB0CCR0 & 0x1);
    RNG_val <<= 1;

    // - Once enough bits have been accumulated return.
    if (--RNG_shift == 0) {
        TB0CCTL0 &= ~CCIE;
        P1OUT &= ~BIT0;
        __bic_SR_register_on_exit(LPM0_bits);     // Exit LPM0
    }

    // TODO: Possibly try to modulate the clock div via CSCTL5

    // TODO: Re-implement this multiple capture as part of the logic above.
    /*TB0CCTL0 &= ~CCIFG;
    timerBcaptureValues[timerBcapturePointer++] = TB0CCR0;
    P1OUT ^= BIT0;                  // Toggle P1.0 (LED)
    if (timerBcapturePointer >= NUMBER_TIMER_CAPTURES)
    {
        TB0CCTL0 &= ~CCIE;
        P1OUT &= ~BIT0;
        __bic_SR_register_on_exit(LPM0_bits);     // Exit LPM0
    }*/
}
