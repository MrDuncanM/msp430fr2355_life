/**
 * main.c
 */

#include <msp430.h> 
#include <stdint.h>

#include "clock.h"
#include "gpio.h"
#include "i2c.h"
#include "rng.h"
#include "ssd1315/ssd1315_img.h"


#define LIFE_HIGH_LOW_CUTOFF 0x0200

#define LIFE_COL_WIDTH 128
#define LIFE_PAGE_HIEGHT 6

int main(void)
{
    SSD1315_IMG_ARRAY(life, 2, LIFE_COL_WIDTH, LIFE_PAGE_HIEGHT);

	WDTCTL = WDTPW | WDTHOLD;	// stop watch dog timer
	
	clock_init();
    gpio_init();
    i2c_init();
    ssd1315_init();

    ssd1315_clear();
    ssd1315_print(25, 0, "Conway's Life");
    ssd1315_print(43, 1, "[Start]");

    while (1) {
        switch(__even_in_range(gpio_get_button(), BTN_MASK)) {
        case BTN_NONE: break;
        case BTN_LEFT:
            break;
        case BTN_RIGHT:
            break;
        case BTN_A:
        {
            uint8_t curBuffer = 0;
            uint8_t newBuffer;
            uint16_t livePixels = 0;
            uint32_t generationCount = 0;
            gpio_toggle_led2();

            ssd1315_print(37, 1, "[Running]");

            gpio_toggle_led1();
            uint8_t x, y;
            for (x = 0; x < LIFE_COL_WIDTH; ++x) {
                for (y = 0; y < LIFE_PAGE_HIEGHT * 8; ++y) {
                    uint8_t val = rng_get_val(16) < LIFE_HIGH_LOW_CUTOFF;
                    if (val)
                        ++livePixels;
                    ssd1315_img_set(&life[curBuffer], x, y, val);
                }
            }
            gpio_toggle_led1();

            ssd1315_img_draw(&life[curBuffer], 0, 2);

            ssd1315_print(0, 1, "Gen:            ");
            while (livePixels > 0) {

                ssd1315_uint32(25, 1, generationCount);
                gpio_toggle_led1();

                ++generationCount;
                livePixels = 0;
                newBuffer = (curBuffer == 0);

                uint8_t x, y, neighborCount, pixelLives;
                for (x = 0; x < LIFE_COL_WIDTH; ++x) {
                    for (y = 0; y < LIFE_PAGE_HIEGHT * 8; ++y) {
                        neighborCount = 0;
                        // UpperLeft
                        if (ssd1315_img_get(&life[curBuffer], x-1, y-1) == 1)
                            ++neighborCount;
                        // Left
                        if (ssd1315_img_get(&life[curBuffer], x-1, y) == 1)
                            ++neighborCount;
                        // LowerLeft
                        if (ssd1315_img_get(&life[curBuffer], x-1, y+1) == 1)
                            ++neighborCount;
                        // Lower
                        if (ssd1315_img_get(&life[curBuffer], x, y+1) == 1)
                            ++neighborCount;
                        // LowerRight
                        if (ssd1315_img_get(&life[curBuffer], x+1, y+1) == 1)
                            ++neighborCount;
                        // Right
                        if (ssd1315_img_get(&life[curBuffer], x+1, y) == 1)
                            ++neighborCount;
                        // UpperRight
                        if (ssd1315_img_get(&life[curBuffer], x+1, y-1) == 1)
                            ++neighborCount;
                        // Upper
                        if (ssd1315_img_get(&life[curBuffer], x, y-1) == 1)
                            ++neighborCount;

                        // Determine the state of the center
                        if (ssd1315_img_get(&life[curBuffer], x, y) == 1) {
                            // Live cells die with less than 2 or more than 3 neighbors.
                            pixelLives = (neighborCount > 1 && neighborCount < 4);
                            ssd1315_img_set(&life[newBuffer], x, y, pixelLives);
                        }
                        else {
                            // Dead cells spring to life with exactly 3 neighbors.
                            pixelLives = (neighborCount == 3);
                            ssd1315_img_set(&life[newBuffer], x, y, pixelLives);
                        }
                        livePixels += pixelLives;
                    }
                }
                curBuffer = newBuffer;

                gpio_toggle_led1();
                ssd1315_img_draw(&life[curBuffer], 0, 2);
            }
            // Show the generation count
            ssd1315_print(41, 1, "[Restart]");
            gpio_toggle_led2();
            ssd1315_print(0, 4, "                     ");
            ssd1315_print(13, 4, "---Generations---");
            ssd1315_print(0, 5, "                     ");
            ssd1315_uint32(0, 5, generationCount);
            ssd1315_print(0, 6, "                     ");

            break;
        }
        default: break;
        }
    }
}
