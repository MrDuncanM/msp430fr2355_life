/*
 * gpio.c
 *
 *  Created on: Jun 7, 2022
 *      Author: jenkins
 */

#include <msp430.h>
#include "gpio.h"

Button BUTTON_FLAGS = BTN_NONE;

void gpio_init(void) {
    P1DIR = 0xFF;
    P2DIR = 0xFF;
    P3DIR = 0xFF;
    P4DIR = 0xFF ^ BUTTON1;
    P5DIR = 0xFF;
    P6DIR = 0xFF;

    // Configure Pins for I2C
    P1SEL0 |= BIT2 | BIT3;
    P1SEL1 &= ~(BIT2 | BIT3);

    // Turn off LEDs
    P1OUT &= ~LED1;
    P6OUT &= ~LED2;

    // Configure Button Left
    P4DIR &= ~BUTTON1;
    P4REN |= BUTTON1;
    P4OUT |= BUTTON1;
    P4IES |= BUTTON1;

    // Configure Button Right & A
    P2DIR &= ~(BUTTON2 | BUTTON3);
    P2REN |= BUTTON2 | BUTTON3;
    P2OUT |= BUTTON2 | BUTTON3;
    P2IES |= BUTTON2 | BUTTON3;

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
}

void gpio_toggle_led1(void) {
    P1OUT ^= LED1;
}

void gpio_toggle_led2(void){
    P6OUT ^= LED2;
}

Button gpio_get_button(void) {
    // Wait for previous de-bounce
    if (SFRIE1 &= WDTIE)
        __bis_SR_register(LPM0_bits | GIE);               // Enter LPM0 w/ interrupts

    // Show the de-bounce (green LED on)
    //P6OUT |= LED2;

    BUTTON_FLAGS = BTN_NONE;
    P4IE |= BUTTON1;                 // Btn 1 interrupt enable.
    P4IFG = 0;
    P2IE |= BUTTON2 | BUTTON3;
    P2IFG = 0;

    __bis_SR_register(LPM0_bits | GIE);               // Enter LPM0 w/ interrupts

    return BUTTON_FLAGS;
}

/*
 * ISRs
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = PORT4_VECTOR
__interrupt void PORT4_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT4_VECTOR))) PORT4_ISR (void)
#else
#error Compiler not supported!
#endif
{
    BUTTON_FLAGS |= BTN_LEFT;

    // Show the de-bounce (red on / green off)
    //P1OUT |= LED1;
    //P6OUT &= ~LED2;

    // Setup WDT interval
    WDTCTL = WDT_ADLY_250;
    SFRIFG1 &= ~WDTIFG;
    SFRIE1 |= WDTIE;

    // Turn  off buttons
    P2IFG &= ~(BUTTON2 | BUTTON3);
    P2IE &= ~(BUTTON2 | BUTTON3);
    P4IFG &= ~BUTTON1;
    P4IE &= ~BUTTON1;

    __bic_SR_register_on_exit(LPM0_bits);     // Exit LPM0
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) PORT2_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(P2IFG, BUTTON2 | BUTTON3)) {
    case BUTTON2:
        BUTTON_FLAGS |= BTN_RIGHT;
        break;
    case BUTTON3:
        BUTTON_FLAGS |= BTN_A;
        break;
    default: break;
    }

    // Show the de-bounce (red on / green off)
    //P1OUT |= LED1;
    //P6OUT &= ~LED2;

    // Setup WDT interval
    WDTCTL = WDT_ADLY_250;
    SFRIFG1 &= ~WDTIFG;
    SFRIE1 |= WDTIE;

    // Turn  off buttons
    P2IFG &= ~(BUTTON2 | BUTTON3);
    P2IE &= ~(BUTTON2 | BUTTON3);
    P4IFG &= ~BUTTON1;
    P4IE &= ~BUTTON1;

    __bic_SR_register_on_exit(LPM0_bits);     // Exit LPM0
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) WDT_ISR (void)
#else
#error Compiler not supported!
#endif
{
    // This has flaws. (hold the button down)
    // The whole input system would need to be written to track if the button was still down after the WDT
    //   and not trigger again until it has settled to low. (using a few of the CCR registers in Timer_B7 would make this very efficient with multiple buttons)

    // Demonstrate de-bounce (turn red LED off)
    //P1OUT &= ~LED1;

    WDTCTL = (WDTPW | WDTHOLD);
    SFRIFG1 &= ~WDTIFG;                 /* clear interrupt flag */
    SFRIE1 &= ~WDTIE;

    __bic_SR_register_on_exit(LPM0_bits);     // Exit LPM0
}
