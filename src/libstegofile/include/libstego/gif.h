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
 
 
 
#ifndef __GIF_H__
#define __GIF_H__

#include <libstego/types.h>
#include <libstego/errors.h>
#include <libstego/utils.h>
#include <stdlib.h>
#include <gif_lib.h>

typedef struct {
    GifWord SWidth, SHeight,        /* Screen dimensions. */
	SColorResolution,         /* How many colors can we generate? */
	SBackGroundColor;         /* I hope you understand this one... */
    ColorMapObject *SColorMap;  /* NULL if not exists. */
    int ImageCount;             /* Number of current image */
    GifImageDesc Image;         /* Block describing current image */
    struct SavedImage *SavedImages; /* Use this to accumulate file state 
				       -- Always 0 for us */
    VoidPtr UserData;           /* hook to attach user data (TVT) */
    VoidPtr Private;            /* Don't mess with this! */
    char * SrcFilename;         /* Name of the Source file */
} gif_internal_data_t;

uint32_t io_gif_read(const uint8_t *filename, palette_data_t *palette_data, gif_internal_data_t *gif_struct);

uint32_t io_gif_integrate(gif_internal_data_t *gif_struct, const palette_data_t *palette_data);

uint32_t io_gif_write(const uint8_t *filename, gif_internal_data_t *gif_struct);

uint32_t io_gif_cleanup_internal(gif_internal_data_t *gif_file);

uint32_t io_gif_cleanup_data(palette_data_t* palette_data);

#endif // __GIF_H__
