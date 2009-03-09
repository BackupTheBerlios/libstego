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
 
 
 
#ifndef __PW_H__
#define __PW_H__

#include "libstego/types.h"
#include "libstego/utils.h"

typedef struct {
    float gain;
    float epsilon;
    uint8_t jillion;
    uint16_t t;
 } pw_parameter;

uint8_t pw_embed(const wav_data_t *src_data, wav_data_t *stego_data, uint8_t *message, int32_t msglen, const pw_parameter *para);

uint8_t pw_extract(const wav_data_t *stego_data, uint8_t *message, int32_t msglen, const pw_parameter *para);

int32_t pw_get_message_length(const wav_data_t *stego_data, const pw_parameter *para);

int32_t pw_check_capacity(const wav_data_t *src_data, const pw_parameter *para);

#endif // __PW_H__
