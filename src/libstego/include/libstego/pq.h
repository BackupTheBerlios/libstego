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
 
 
 
/******************************************************************************
 * 2008 Christian Kuka <ckuka@madkooky.de>
 * Alle Rechte vorbehalten
 ******************************************************************************/

#ifndef __PQ_H__
#define __PQ_H__

#include <libstego/types.h>
#include <libstego/utils.h>
#include <libstego/errors.h>

static const uint16_t _pq_luminance_quant_tbl[64] = {
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99
};
static const uint16_t _pq_chrominance_quant_tbl[64] = {
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};

#ifndef _PQ_HEADER_SIZE
    #define _PQ_HEADER_SIZE 64;
#endif

uint32_t pq_embed(const jpeg_data_t *src_data, jpeg_data_t *stego_data, uint8_t *message, uint32_t msglen, const pq_parameter *param);

uint32_t pq_extract(const jpeg_data_t *stego_data, uint8_t **message, uint32_t *msglen, const pq_parameter *param);

int32_t pq_get_message_length(const jpeg_data_t *stego_data, const pq_parameter *para);

int32_t pq_check_capacity(const jpeg_data_t *src_data, const pq_parameter *param, uint32_t *capacity);


#endif // __PQ_H__
