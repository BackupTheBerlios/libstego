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
 
 
 
#ifndef __CPT_H__
#define __CPT_H__

#include "libstego/types.h"


typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t *data;
} cpt_matrix_t;

uint32_t cpt_embed(const rgb_data_t *src_data, rgb_data_t *stego_data,
                  const uint8_t *message, const int32_t msglen, const cpt_parameter *para);

uint32_t cpt_extract(const rgb_data_t *stego_data, uint8_t **message,
                    uint32_t *msglen, const cpt_parameter *para);

uint32_t cpt_get_message_length(const rgb_data_t *stego_data,
                               const cpt_parameter *para,
			       uint32_t *msglen);

uint32_t cpt_check_capacity(const rgb_data_t *src_data,
                           const cpt_parameter *para,
                           uint32_t *capacity);

#endif // __CPT_H__
