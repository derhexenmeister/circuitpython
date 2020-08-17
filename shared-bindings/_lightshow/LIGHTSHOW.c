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
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/gc.h"
#include "py/mpstate.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/util.h"
#include "LIGHTSHOW.h"
#include "shared-module/_lightshow/LIGHTSHOW.h"
#include "supervisor/shared/translate.h"

//| class LIGHTSHOW: TODO update this test TODO
//|     """This is an internal module to be used by the ``lightshow.py`` library from
//|     https://github.com/derhexenmeister/lightshow to handle the
//|     RGB LED matrix display for an ItsyBitsy M4 Express board (or similar)
//|
//|     Usage::
//|
//|         This singleton class is instantiated by the ``lightshow`` library, and
//|         used internally by it. All user-visible interactions are done through
//|         that library."""
//|
//|     def __init__(self, spi: Any, chip_select: Any, buffer: Any):
//|         """Initializes matrix scanning routines.
//|
//|         spi is an configured ``busio.SPI`` object used to drive the clock and
//|         data lines connected to the matrix. chip_select is a configured
//|         ``DigitalInputOutput`` object used to control the SPI chip select connected
//|         to the matrix.
//|         The ``buffer`` is a 64 byte long ``bytearray`` that stores what should
//|         be displayed on the matrix."""
//|         ...
//|
STATIC mp_obj_t lightshow_make_new(const mp_obj_type_t *type, size_t n_args,
        const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 3, 3, true);
    enum { ARG_spi, ARG_chip_select, ARG_buffer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_chip_select, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_buffer, MP_ARG_OBJ | MP_ARG_REQUIRED },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
                     allowed_args, args);

    if (!MP_OBJ_IS_TYPE(args[ARG_spi].u_obj,
                        &busio_spi_type)) {
        mp_raise_TypeError(translate("spi must be busio.SPI"));
    }
    busio_spi_obj_t *spi = MP_OBJ_TO_PTR(
            args[ARG_spi].u_obj);
    if (common_hal_busio_spi_deinited(spi)) {
        raise_deinited_error();
    }

    if (!MP_OBJ_IS_TYPE(args[ARG_chip_select].u_obj,
                        &digitalio_digitalinout_type)) {
        mp_raise_TypeError(translate("chip_Select must be digitalio.DigitalInOut"));
    }
    digitalio_digitalinout_obj_t *chip_select = MP_OBJ_TO_PTR(
            args[ARG_chip_select].u_obj);
    if (common_hal_digitalio_digitalinout_deinited(chip_select)) {
        raise_deinited_error();
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);

    if (bufinfo.len != 64) {
        mp_raise_ValueError(translate("Incorrect buffer size"));
    }

    lightshow_obj_t *lightshow = MP_STATE_VM(lightshow_singleton);
    if (!lightshow) {
        lightshow = m_new_obj(lightshow_obj_t);
        lightshow->base.type = &lightshow_type;
        lightshow = gc_make_long_lived(lightshow);
        MP_STATE_VM(lightshow_singleton) = lightshow;
    }

    lightshow->spi = spi;
    lightshow->chip_select = chip_select;
    lightshow->buffer = bufinfo.buf;
    lightshow_init();

    return MP_OBJ_FROM_PTR(lightshow);
}

STATIC const mp_rom_map_elem_t lightshow_locals_dict_table[] = {
};
STATIC MP_DEFINE_CONST_DICT(lightshow_locals_dict, lightshow_locals_dict_table);
const mp_obj_type_t lightshow_type = {
    { &mp_type_type },
    .name = MP_QSTR_LIGHTSHOW,
    .make_new = lightshow_make_new,
    .locals_dict = (mp_obj_dict_t*)&lightshow_locals_dict,
};
