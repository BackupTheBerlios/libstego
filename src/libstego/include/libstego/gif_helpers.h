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
 
 
 
#ifndef __GIF_HELPERS_H__
#define __GIF_HELPERS_H__

#include <stdlib.h>
#include <stdio.h>
#include "libstego/random.h"
#include "libstego/types.h"
#include "libstego/random.h"

uint8_t copy_palette_data_t(const palette_data_t *src_data, palette_data_t  *copy);

uint8_t sort_palette_random(const rgb_pixel_t *src_data, rgb_pixel_t *new_pal, uint32_t length, prng_state *state);

uint8_t sort_palette_rgb(rgb_pixel_t *unique_colors, uint32_t unique_length);

#endif  // __GIF_HELPERS_H__
