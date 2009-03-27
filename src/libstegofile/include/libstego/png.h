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
 
 
 
#ifndef __PNG_H__
#define __PNG_H__

#include <libstego/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <libstego/errors.h>
#include <png.h>

typedef struct {
    png_structp png_struct;
    png_infop info_struct;
} png_internal_data_t;

uint32_t io_png_read(const uint8_t *filename, rgb_data_t *rgb_data, png_internal_data_t *png_internal);

uint32_t io_png_integrate(png_internal_data_t *png_internal, const rgb_data_t *rgb_data);

uint32_t io_png_write(const uint8_t *filename, png_internal_data_t *png_internal);

uint32_t io_png_cleanup_data(rgb_data_t *rgb_data);

uint32_t io_png_cleanup_internal(png_internal_data_t *png_internal);




#endif // __PNG_H__
