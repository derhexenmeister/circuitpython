/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Keith Evans
 * Based on PewPew
 * Copyright (c) 2019 Radomir Dopieralski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdbool.h>

#include "py/mpstate.h"
#include "__init__.h"
#include "SPI_595.h"

#include "shared-bindings/digitalio/DigitalInOut.h"

// Array
#define ROW_SIZE        8
#define COL_SIZE        8

// 2-bits per pixel
#define RED_MASK        0x30
#define GREEN_MASK      0x0C
#define BLUE_MASK       0x03

// SPI data packet ordering
#define RED_OFFSET      0   // Active-low
#define BLUE_OFFSET     1   // Active-low
#define GREEN_OFFSET    2   // Active-low
#define COL_OFFSET      3   // Active-high

void spi_595_tick(void) {
    busio_spi_obj_t *spi;
    digitalio_digitalinout_obj_t *spi_cs;
    uint8_t spi_packet[4];

    spi_595_obj_t* spi_595 = MP_STATE_VM(spi_595_singleton);
    if (!spi_595) { return; }

    spi = MP_OBJ_TO_PTR(spi_595->spi);
    spi_cs = MP_OBJ_TO_PTR(spi_595->chip_select);

    for (uint8_t col = 0 ; col < COL_SIZE ; col++) {
        uint8_t red_value   = 0xff;
        uint8_t green_value = 0xff;
        uint8_t blue_value  = 0xff;
        for (uint8_t row = 0 ; row < ROW_SIZE ; row++) {
            uint8_t color = spi_595->buffer[row * ROW_SIZE + col];
            if (color & RED_MASK) {
                red_value &= ~(1 << row);
            }
            if (color & GREEN_MASK) {
                green_value &= ~(1 << row);
            }
            if (color & BLUE_MASK) {
                blue_value &= ~(1 << row);
            }
        }
        spi_packet[COL_OFFSET] = (1 << col);

        // Red
        spi_packet[RED_OFFSET]   = red_value;
        spi_packet[GREEN_OFFSET] = 0xff;
        spi_packet[BLUE_OFFSET]  = 0xff;

        common_hal_digitalio_digitalinout_set_value(spi_cs, false);
        common_hal_busio_spi_write(spi, spi_packet, 4);
        common_hal_digitalio_digitalinout_set_value(spi_cs, true);

        // Green
        spi_packet[RED_OFFSET]   = 0xff;
        spi_packet[GREEN_OFFSET] = green_value;
        spi_packet[BLUE_OFFSET]  = 0xff;

        common_hal_digitalio_digitalinout_set_value(spi_cs, false);
        common_hal_busio_spi_write(spi, spi_packet, 4);
        common_hal_digitalio_digitalinout_set_value(spi_cs, true);

        // Blue
        spi_packet[RED_OFFSET]   = 0xff;
        spi_packet[GREEN_OFFSET] = 0xff;
        spi_packet[BLUE_OFFSET]  = blue_value;

        common_hal_digitalio_digitalinout_set_value(spi_cs, false);
        common_hal_busio_spi_write(spi, spi_packet, 4);
        common_hal_digitalio_digitalinout_set_value(spi_cs, true);
    }

    // Turn off (otherwise last column will be brighter (for one color)
    spi_packet[RED_OFFSET]   = 0xff;
    spi_packet[GREEN_OFFSET] = 0xff;
    spi_packet[BLUE_OFFSET]  = 0xff;
    spi_packet[COL_OFFSET]   = 0x00;
    common_hal_digitalio_digitalinout_set_value(spi_cs, false);
    common_hal_busio_spi_write(spi, spi_packet, 4);
    common_hal_digitalio_digitalinout_set_value(spi_cs, true);
}
