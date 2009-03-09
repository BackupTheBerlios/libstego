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
 
 
 
#ifndef __JPEG_H__
#define __JPEG_H__

#include <libstego/types.h>

typedef struct {
  struct jpeg_compress_struct *cinfo;
  struct jpeg_decompress_struct *dinfo;
} jpeg_internal_data_t;


uint32_t io_jpeg_read(const uint8_t *filename, jpeg_data_t *jpeg_data,
		     jpeg_internal_data_t *jpeg_struct);

uint32_t io_jpeg_integrate(jpeg_internal_data_t *internal_data, const jpeg_data_t *jpeg_data);

uint32_t io_jpeg_write(const uint8_t *filename,
		      jpeg_internal_data_t *jpeg_struct);

uint32_t io_jpeg_copy_internal(jpeg_internal_data_t *target,
			      const jpeg_internal_data_t *source);

uint32_t io_jpeg_cleanup_data(jpeg_data_t *data);

uint32_t io_jpeg_cleanup_internal_data(jpeg_internal_data_t *data);

#endif // __JPEG_H__
