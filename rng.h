/*
 * rng.h
 *
 *  Created on: Jun 17, 2022
 *      Author: jenkins
 */

#ifndef RNG_H_
#define RNG_H_

// This is for multi-capture majority voting support.
//#define RNG_CAPTURE_COUNT 5

// This is based on slaa338a.pdf "Random Number Generation Using MSP430 MCUs"
// ACLK is based on REFO @ ~32.768 khz instead of VLO @ 10 khz (although we can use VLO)
// TimerB is in use instead of the TimerA system.
unsigned int rng_get_val(unsigned char bitWidth);

#endif /* RNG_H_ */
