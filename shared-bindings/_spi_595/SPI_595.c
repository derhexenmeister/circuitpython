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
#include "SPI_595.h"
#include "shared-module/_spi_595/SPI_595.h"
#include "supervisor/shared/translate.h"

//| class SPI_595: TODO update this test TODO
//|     """This is an internal module to be used by the ``pew.py`` library from
//|     https://github.com/pewpew-game/pew-pewpew-standalone-10.x to handle the
//|     LED matrix display and buttons on the ``pewpew10`` board.
//|
//|     Usage::
//|
//|         This singleton class is instantiated by the ``pew`` library, and
//|         used internally by it. All user-visible interactions are done through
//|         that library."""
//|

//|     def __init__(self, buffer: Any, rows: Any, cols: Any, buttons: Any):
//|         """Initializes matrix scanning routines.
//|
//|         The ``buffer`` is a 64 byte long ``bytearray`` that stores what should
//|         be displayed on the matrix. ``rows`` and ``cols`` are both lists of
//|         eight ``DigitalInputOutput`` objects that are connected to the matrix
//|         rows and columns. ``buttons`` is a ``DigitalInputOutput`` object that
//|         is connected to the common side of all buttons (the other sides of the
//|         buttons are connected to rows of the matrix)."""
//|         ...
//|
STATIC mp_obj_t spi_595_make_new(const mp_obj_type_t *type, size_t n_args,
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

    spi_595_obj_t *spi_595 = MP_STATE_VM(spi_595_singleton);
    if (!spi_595) {
        spi_595 = m_new_obj(spi_595_obj_t);
        spi_595->base.type = &spi_595_type;
        spi_595 = gc_make_long_lived(spi_595);
        MP_STATE_VM(spi_595_singleton) = spi_595;
    }

    spi_595->spi = spi;
    spi_595->chip_select = chip_select;
    spi_595->buffer = bufinfo.buf;
    spi_595_init();

    return MP_OBJ_FROM_PTR(spi_595);
}


STATIC const mp_rom_map_elem_t spi_595_locals_dict_table[] = {
};
STATIC MP_DEFINE_CONST_DICT(spi_595_locals_dict, spi_595_locals_dict_table);
const mp_obj_type_t spi_595_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI_595,
    .make_new = spi_595_make_new,
    .locals_dict = (mp_obj_dict_t*)&spi_595_locals_dict,
};
