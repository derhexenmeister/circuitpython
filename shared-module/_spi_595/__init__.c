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

void spi_595_tick(void) {
    busio_spi_obj_t *spi;
    digitalio_digitalinout_obj_t *spi_cs;
    uint8_t spi_packet[4];

    spi_595_obj_t* spi_595 = MP_STATE_VM(spi_595_singleton);
    if (!spi_595) { return; }

    // Reds
    spi_packet[0] = 0x00; // R (active low)
    spi_packet[1] = 0xff; // B (active low)
    spi_packet[2] = 0xff; // G (active low)
    spi = MP_OBJ_TO_PTR(spi_595->spi);
    spi_cs = MP_OBJ_TO_PTR(spi_595->chip_select);
    for (uint8_t col = 0 ; col < 8 ; col++) {
        spi_packet[3] = (1 << col); // COL (active high)
        common_hal_digitalio_digitalinout_set_value(spi_cs, false);
        common_hal_busio_spi_write(spi, spi_packet, 4);
        common_hal_digitalio_digitalinout_set_value(spi_cs, true);
    }

    // Greens
    spi_packet[0] = 0xff; // R (active low)
    spi_packet[1] = 0xff; // B (active low)
    spi_packet[2] = 0x00; // G (active low)
    spi = MP_OBJ_TO_PTR(spi_595->spi);
    spi_cs = MP_OBJ_TO_PTR(spi_595->chip_select);
    for (uint8_t col = 0 ; col < 8 ; col++) {
        spi_packet[3] = (1 << col); // COL (active high)
        common_hal_digitalio_digitalinout_set_value(spi_cs, false);
        common_hal_busio_spi_write(spi, spi_packet, 4);
        common_hal_digitalio_digitalinout_set_value(spi_cs, true);
    }

    // Blues
    spi_packet[0] = 0xff; // R (active low)
    spi_packet[1] = 0x00; // B (active low)
    spi_packet[2] = 0xff; // G (active low)
    spi = MP_OBJ_TO_PTR(spi_595->spi);
    spi_cs = MP_OBJ_TO_PTR(spi_595->chip_select);
    for (uint8_t col = 0 ; col < 8 ; col++) {
        spi_packet[3] = (1 << col); // COL (active high)
        common_hal_digitalio_digitalinout_set_value(spi_cs, false);
        common_hal_busio_spi_write(spi, spi_packet, 4);
        common_hal_digitalio_digitalinout_set_value(spi_cs, true);
    }

    // Turn off (otherwise last column will be brighter (for one color)
    spi_packet[0] = 0xff; // R (active low)
    spi_packet[1] = 0xff; // B (active low)
    spi_packet[2] = 0xff; // G (active low)
    spi_packet[3] = 0x00; // COL (active high)
    common_hal_digitalio_digitalinout_set_value(spi_cs, false);
    common_hal_busio_spi_write(spi, spi_packet, 4);
    common_hal_digitalio_digitalinout_set_value(spi_cs, true);

#ifdef NEVER_DEF
    pin = MP_OBJ_TO_PTR(spi_595->cols[col]);
    ++col;
    if (col >= spi_595->cols_size) {
        spi_595->pressed |= last_pressed & pressed;
        last_pressed = pressed;
        pressed = 0;
        col = 0;
        ++turn;
        if (turn > 11) {
            turn = 0;
        }
    }
    if (!common_hal_digitalio_digitalinout_get_value(spi_595->buttons)) {
        pressed |= 1 << col;
    }
    common_hal_digitalio_digitalinout_set_value(pin, true);
    for (size_t x = 0; x < spi_595->rows_size; ++x) {
        pin = MP_OBJ_TO_PTR(spi_595->rows[x]);
        uint8_t color = spi_595->buffer[col * (spi_595->rows_size) + x];
        bool value = false;
        switch (color & 0x03) {
            case 3:
                value = true;
                break;
            case 2:
                if (turn == 2 || turn == 5 || turn == 8 || turn == 11) {
                        value = true;
                }
                break;
            case 1:
                if (turn == 0) {
                    value = true;
                }
                break;
            case 0:
                break;
        }
        common_hal_digitalio_digitalinout_set_value(pin, value);
    }
    pin = MP_OBJ_TO_PTR(spi_595->cols[col]);
    common_hal_digitalio_digitalinout_set_value(pin, false);
#endif // NEVER_DEF
}
