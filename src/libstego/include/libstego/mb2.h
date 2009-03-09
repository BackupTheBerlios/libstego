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
 
 
 
#ifndef __MB2_H__
#define __MB2_H__

#include "libstego/types.h"

uint8_t mb2_embed(const jpeg_data_t *src_data, jpeg_data_t *stego_data, uint8_t *message, int32_t msglen, const mb2_parameter *para);

uint8_t mb2_extract(const jpeg_data_t *stego_data, uint8_t *message, int32_t msglen, const mb2_parameter *para);

int32_t mb2_get_message_length(const jpeg_data_t *stego_data, const mb2_parameter *para);

int32_t mb2_check_capacity(const jpeg_data_t *src_data, const mb2_parameter *para);

#endif // __MB2_H__
