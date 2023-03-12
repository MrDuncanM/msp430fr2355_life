/*
 * gpio.h
 *
 *  Created on: Jun 7, 2022
 *      Author: jenkins
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <msp430.h>
#include <stdint.h>

/*
 * Pin Configuration
 */
#define SDA BIT2                                                        // i2c SDA pin P1.2
#define SCL BIT3                                                        // i2c SCL pin P1.3

#define LED1 BIT0                                                       // Red LED on P1.0
#define LED2 BIT6                                                       // Green LED on P6.6

#define BUTTON1 BIT1                                                    // BTN_LEFT on P4.1
#define BUTTON2 BIT3                                                    // BTN_RIGHT on P2.3
#define BUTTON3 BIT2                                                    // BTN_A from P2.2 to GND

/*
 * Custom Types
 */

typedef enum {
    BTN_NONE = 0x0,
    BTN_LEFT = 0x1,
    BTN_RIGHT = 0x2,
    BTN_A = 0x4
} Button;

#define BTN_MASK (BTN_LEFT | BTN_RIGHT | BTN_A)

/*
 * Interface
 */

void gpio_init(void);

void gpio_toggle_led1(void);
void gpio_toggle_led2(void);

Button gpio_get_button(void);

#endif /* GPIO_H_ */
