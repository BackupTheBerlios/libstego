/*
 *  This file is part of libstego.
 *
 *  libstego is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  libstego is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libstego.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *  Copyright 2008 2009 by Jan C. Busch, René Frerichs, Lars Herrmann,
 *  Matthias Kloppenborg, Marko Krause, Christian Kuka, Sebastian Schnell,
 *  Ralf Treu.
 *
 *  For more info visit <http://parsys.informatik.uni-oldenburg.de/~stego/>
 */
 
 
 
#ifndef __T_H__
#define __T_H__

#include <libstego/types.h>
#include <libstego/errors.h>
#include <libstego/random.h>
#include <stdlib.h>
#include <stdio.h>

uint8_t t_embed(const jpeg_data_t *jpeg, const palette_data_t *palette, palette_data_t *stego_palette, uint8_t *message, int32_t msglen, const t_parameter *para);

uint8_t t_extract(const jpeg_data_t *jpeg, const palette_data_t *stego_palette, uint8_t *message, int32_t msglen, const t_parameter *para);

uint8_t t_get_message_length(const jpeg_data_t *jpeg, const t_parameter *para);

uint8_t t_check_capacity(const jpeg_data_t *jpeg, const t_parameter *param, uint32_t *capacity);

uint8_t _t_calc_weightmatrix(uint8_t *message, int32_t msglen, uint8_t *key, int32_t keylen, int32_t patternlen, uint8_t *matrix);

uint8_t _t_embed_coordinates(uint8_t *matrix, const rgb_data_t *gif);

#define LO            -1
#define HI            +1

#define BINARY(x)     ((x)==LO ? false : true)
#define BIPOLAR(x)    ((x)==false ? LO : HI)

#endif // __T_H__
