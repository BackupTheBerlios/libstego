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
 
 
 
#include "libstego/types.h"
#include <stdio.h>
#include <stdlib.h>
/*
 * Extended-precision integers.
 *
 * Written by Matthew Kwan - January 1998
 */

#ifndef _EPI_H
#define _EPI_H

#define EPI_MAX_BITS	2040

typedef struct {
	uint32_t	epi_high_bit;
	uint8_t	epi_bits[EPI_MAX_BITS];
} EPI;


/*
 * Define external functions.
 */

extern void	epi_init (EPI *epi);
extern int32_t	epi_as_int (const EPI *epi);
extern void	epi_copy (EPI *epi1, const EPI *epi2);
extern void	epi_set (EPI *epi, int32_t n);
extern int32_t	epi_cmp (const EPI *epi1, const EPI *epi2);
extern void	epi_add (EPI *epi1, const EPI *epi2);
extern void	epi_multiply (EPI *epi, int32_t n);
extern int32_t	epi_divide (EPI *epi, int32_t n);
extern void	epi_decrement (EPI *epi);
extern void	epi_shift (EPI *epi, int32_t n);

#endif
