/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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
#include "py/runtime.h"
#include "__init__.h"
#include "SPI_595.h"

#include "shared-bindings/digitalio/Pull.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/util.h"
#include "samd/timers.h"
#include "supervisor/shared/translate.h"
#include "timer_handler.h"


static uint8_t spi_595_tc_index = 0xff;


void spi_595_interrupt_handler(uint8_t index) {
    if (index != spi_595_tc_index) {
        return;
    }
    Tc* tc = tc_insts[index];
    if (!tc->COUNT16.INTFLAG.bit.MC0) {
        return;
    }

    spi_595_tick();

    // Clear the interrupt bit.
    tc->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;
}

void spi_595_init() {
    spi_595_obj_t* spi_595 = MP_STATE_VM(spi_595_singleton);

    common_hal_digitalio_digitalinout_switch_to_input(spi_595->buttons, PULL_UP);

    for (size_t i = 0; i < spi_595->rows_size; ++i) {
        digitalio_digitalinout_obj_t *pin = MP_OBJ_TO_PTR(spi_595->rows[i]);
        common_hal_digitalio_digitalinout_switch_to_output(pin, false,
            DRIVE_MODE_PUSH_PULL);
    }
    for (size_t i = 0; i < spi_595->cols_size; ++i) {
        digitalio_digitalinout_obj_t *pin = MP_OBJ_TO_PTR(spi_595->cols[i]);
        common_hal_digitalio_digitalinout_switch_to_output(pin, true,
            DRIVE_MODE_PUSH_PULL); // DRIVE_MODE_OPEN_DRAIN
    }
    if (spi_595_tc_index == 0xff) {
        // Find a spare timer.
        uint8_t index = find_free_timer();
        if (index == 0xff) {
            mp_raise_RuntimeError(translate("All timers in use"));
        }
        Tc *tc = tc_insts[index];

        spi_595_tc_index = index;
        set_timer_handler(true, index, TC_HANDLER_SPI_595);

        // We use GCLK0 for SAMD21 and GCLK1 for SAMD51 because they both run
        // at 48mhz making our math the same across the boards.
        #ifdef SAMD21
        turn_on_clocks(true, index, 0);
        #endif
        #ifdef SAMD51
        turn_on_clocks(true, index, 1);
        #endif


        #ifdef SAMD21
        tc->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 |
                                TC_CTRLA_PRESCALER_DIV64 |
                                TC_CTRLA_WAVEGEN_MFRQ;
        #endif
        #ifdef SAMD51
        tc_reset(tc);
        tc_set_enable(tc, false);
        tc->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16
            | TC_CTRLA_PRESCALER_DIV256;
//            | TC_CTRLA_PRESCALER_DIV64;
        tc->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;
        #endif

        tc_set_enable(tc, true);
//        tc->COUNT16.CC[0].reg = 64;
        tc->COUNT16.CC[0].reg = 188;

        // Clear our interrupt in case it was set earlier
        tc->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;
        tc->COUNT16.INTENSET.reg = TC_INTENSET_MC0;
        tc_enable_interrupts(spi_595_tc_index);
    }
}

void spi_595_reset(void) {
    if (spi_595_tc_index != 0xff) {
        tc_reset(tc_insts[spi_595_tc_index]);
        spi_595_tc_index = 0xff;
    }
    MP_STATE_VM(spi_595_singleton) = NULL;
}
