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
 
 
 
#ifndef __SVG_H__
#define __SVG_H__

#include <libstego/types.h>
#include <libstego/utils.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xmlsave.h>

#include <sndfile.h>
#include <stdlib.h>

/**
 * Data structure for internal use.
 */
typedef struct {
	/** Pointer to the parsed XML document in memory. */
	xmlDocPtr doc;
} svg_internal_data_t;

uint32_t io_svg_read(
        const uint8_t *filename,
        svg_data_t *data,
        svg_internal_data_t *int_dat);

uint32_t io_svg_integrate(
        svg_internal_data_t *int_dat,
        const svg_data_t *data);

uint32_t io_svg_write(
        const uint8_t *filename,
        svg_internal_data_t *int_dat);

uint32_t io_svg_cleanup_data(svg_data_t *data);

uint32_t io_svg_cleanup_internal(svg_internal_data_t *int_dat);

#endif // __SVG_H__
