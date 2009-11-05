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
 
 
 
#ifndef __F5_H__
#define __F5_H__

#include <stdint.h>
#include <stdio.h>
#include "libstego/types.h"
#include "libstego/errors.h"
#include <libstego/random.h>
#include <libstego/utils.h>


typedef struct {
    int16_t **coeff_list;
    uint32_t list_len;
    uint8_t k;                 //number of bits to embed in a given block
    uint8_t n;                 //length of a single block
    prng_state *random;
    jpeg_data_t *stego_data;
    uint8_t *message;
    uint32_t msg_len;
    
} f5_context;


uint32_t f5_embed(const jpeg_data_t *src_data, jpeg_data_t *stego_data, uint8_t *message, int32_t msglen, const f5_parameter *para);

uint32_t f5_extract(const jpeg_data_t *stego_data, uint8_t **message, uint32_t *msglen, const f5_parameter *para);

uint32_t f5_extract_message(f5_context *context);

uint32_t f5_get_message_length(f5_context *context);

uint32_t f5_check_capacity(const jpeg_data_t *src_data, uint32_t *capacity);

uint32_t f5_create_coeff_list(f5_context *context);

uint32_t f5_choose_code(f5_context *context);

uint32_t jpeg_data_copy(const jpeg_data_t *src_data, jpeg_data_t *dst_data);

uint32_t jpeg_data_cleanup(jpeg_data_t *jpeg_data);

uint32_t f5_embed_message_length(f5_context *context);

uint32_t f5_matrix_encode(f5_context *context);

#endif // __F5_H__
