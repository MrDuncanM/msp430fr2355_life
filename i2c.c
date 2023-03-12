/*
 * i2c.c
 *
 *  Created on: Jun 6, 2022
 *      Author: jenkins
 */


#include <msp430.h>
#include "i2c.h"

#define SDA BIT2                                                        // i2c sda pin (P1.2)
#define SCL BIT3                                                        // i2c scl pin (P1.3

unsigned char *PTxData;
unsigned int TXByteCtr;

void i2c_init(void) {
    // Configure USCI_B0 for I2C mode
    UCB0CTLW1 |= UCSWRST;                             // put eUSCI_B in reset state
    UCB0CTLW0 |= UCMODE_3 | UCMST;                    // I2C mode & I2C master
    UCB0BRW = 8;                                   // baud rate = SMCLK / 10
    UCB0CTLW0 &=~ UCSWRST;                            // clear reset register
    UCB0IE |= UCTXIE0 | UCNACKIE;                     // transmit and NACK interrupt enable
}

void i2c_write(unsigned char slave_address, unsigned char *DataBuffer, unsigned int ByteCtr) {
    while (UCB0CTLW0 & UCTXSTP);                      // Ensure stop condition got sent


    PTxData = DataBuffer;
    TXByteCtr = ByteCtr;                                    // Load TX byte counter

    UCB0I2CSA = slave_address;           // configure slave address
    UCB0CTLW0 |= UCTR | UCTXSTT;                      // I2C TX, start condition

    __bis_SR_register(LPM0_bits | GIE);               // Enter LPM0 w/ interrupts
                                                      // Remain in LPM0 until all data
                                                      // is TX'd
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCIB0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(EUSCI_B0_VECTOR))) EUSCIB0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCB0IV,USCI_I2C_UCBIT9IFG))
  {
        case USCI_NONE: break;                        // Vector 0: No interrupts break;
        case USCI_I2C_UCALIFG: break;
        case USCI_I2C_UCNACKIFG:
            UCB0CTL1 |= UCTXSTT;                      // resend start if NACK
          break;                                      // Vector 4: NACKIFG break;
        case USCI_I2C_UCSTTIFG: break;                // Vector 6: STTIFG break;
        case USCI_I2C_UCSTPIFG: break;                // Vector 8: STPIFG break;
        case USCI_I2C_UCRXIFG3: break;                // Vector 10: RXIFG3 break;
        case USCI_I2C_UCTXIFG3: break;                // Vector 14: TXIFG3 break;
        case USCI_I2C_UCRXIFG2: break;                // Vector 16: RXIFG2 break;
        case USCI_I2C_UCTXIFG2: break;                // Vector 18: TXIFG2 break;
        case USCI_I2C_UCRXIFG1: break;                // Vector 20: RXIFG1 break;
        case USCI_I2C_UCTXIFG1: break;                // Vector 22: TXIFG1 break;
        case USCI_I2C_UCRXIFG0: break;                // Vector 24: RXIFG0 break;
        case USCI_I2C_UCTXIFG0:
        if (TXByteCtr) {                              // Check TX byte counter
            UCB0TXBUF = *PTxData++;                   // Load TX buffer
            TXByteCtr--;                              // Decrement TX byte counter
        }
        else {                                        //Done with transmission
            UCB0CTLW0 |= UCTXSTP;                     // I2C stop condition
            UCB0IFG &= ~UCTXIFG;                      // Clear USCI_B0 TX int flag
            __bic_SR_register_on_exit(LPM0_bits);     // Exit LPM0
        }
          break;                                      // Vector 26: TXIFG0 break;
        case USCI_I2C_UCBCNTIFG: break;               // Vector 28: BCNTIFG
        case USCI_I2C_UCCLTOIFG: break;               // Vector 30: clock low timeout
        case USCI_I2C_UCBIT9IFG: break;               // Vector 32: 9th bit
        default: break;
  }
}
