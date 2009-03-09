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
 
 
 
#ifndef __WAV_H__
#define __WAV_H__

#include <libstego/types.h>
#include <libstego/utils.h>
#include <libstego/errors.h>
#include <sndfile.h>
#include <stdlib.h>
#include <string.h>

/**
 * Data structure for internal use.
 */
typedef struct {
    int32_t *data;
    SF_INFO info;
} wav_internal_data_t;

uint8_t io_wav_read(const uint8_t *filename, wav_data_t *wav_data, wav_internal_data_t *int_dat);

uint8_t io_wav_integrate(wav_internal_data_t *wav_struct, const wav_data_t *wav_data);

uint8_t io_wav_write(const uint8_t *filename, wav_internal_data_t *wav_struct);

uint8_t io_wav_copy_internal(const wav_internal_data_t *src_struct, wav_internal_data_t *copy);

uint8_t io_wav_cleanup_data(wav_data_t *wav_data);

uint8_t io_wav_cleanup_internal(wav_internal_data_t *wav_struct);

#endif // __WAV_H__
