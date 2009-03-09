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
 
 
 
#ifndef __BMP_H__
#define __BMP_H__

#include <libstego/types.h>

uint8_t io_bmp_read(const uint8_t *filename, rgb_data_t *rgb_data, void *bmp_struct);

uint8_t io_bmp_integrate(void *bmp_struct, const rgb_data_t *rgb_data);

uint8_t io_bmp_write(const uint8_t *filename, void *bmp_struct);

uint8_t io_bmp_get_info(const uint8_t *filename, void *bmp_info);

uint8_t io_bmp_copy_info(const void *src_struct, void *copy);

#endif // __BMP_H__
