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
 
 
 
#ifndef __SVG_STEG_H__
#define __SVG_STEG_H__

#include <libstego/types.h>
#include <libstego/utils.h>
#include <libstego/errors.h>
#include <libstego/random.h>

#include <libstego/lsb.h>

#include <stdlib.h>
#include <string.h>

uint32_t svg_get_message_length(
        const svg_data_t *data,
        uint32_t *msglen,
        const svg_parameter_t *param);

uint32_t svg_embed(
        const svg_data_t *src,
        svg_data_t *stego,
        const uint8_t *message,
        const uint32_t msglen,
        const svg_parameter_t *param);

uint32_t svg_extract(
        const svg_data_t *data,
        uint8_t **message,
        uint32_t *msglen,
        const svg_parameter_t *param);

uint32_t svg_check_capacity(
        const svg_data_t *data,
        uint32_t *capacity,
        const svg_parameter_t *param);

#endif // __SVG_STEG_H__
