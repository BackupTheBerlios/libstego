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
 
 
 
/*
 * Extended-precision integers.
 *
 * Written by Matthew Kwan - January 1998
 */

#include "libstego/epi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Initialize an EPI.
 */

void
epi_init (
	EPI	*epi
) {
	epi->epi_high_bit = 0;
	memset (epi->epi_bits, 0, EPI_MAX_BITS * sizeof (char));
}


/*
 * Convert an integer to an EPI.
 */

void
epi_set (
	EPI		*epi,
	int32_t		n
) {
	int32_t		i = 0;

	epi_init (epi);

	while (n != 0) {
	    epi->epi_bits[i++] = n & 1;
	    n >>= 1;
	}

	epi->epi_high_bit = i;
}


/*
 * Return the integer equivalent of an EPI.
 */

int32_t
epi_as_int (
	const EPI	*epi
) {
	int32_t	i, n = 0;

	for (i = epi->epi_high_bit - 1; i >= 0; i--) {
	    n <<= 1;
	    n += epi->epi_bits[i];
	}

	return (n);
}


/*
 * Assign an EPI to another.
 *
 * epi1 = epi2;
 */

void
epi_copy (
	EPI		*epi1,
	const EPI	*epi2
) {
	int32_t		i;

	epi_init (epi1);

	epi1->epi_high_bit = epi2->epi_high_bit;
	for (i=0; i<epi2->epi_high_bit; i++)
	    epi1->epi_bits[i] = epi2->epi_bits[i];
}


/*
 * Compare two EPIs.
 */

int32_t
epi_cmp (
	const EPI	*epi1,
	const EPI	*epi2
) {
	int32_t		i, hd = epi1->epi_high_bit - epi2->epi_high_bit;

	if (hd != 0)
	    return (hd);

	for (i = epi1->epi_high_bit - 1; i >= 0; i--)
	    if ((hd = epi1->epi_bits[i] - epi2->epi_bits[i]) != 0)
		return (hd);

	return (0);
}


/*
 * Add two EPIs together.
 *
 * epi1 += epi2;
 */

void
epi_add (
	EPI		*epi1,
	const EPI	*epi2
) {
	int32_t		i, hb = epi1->epi_high_bit;
	int32_t		v = 0;

	if (epi2->epi_high_bit > hb)
	    hb = epi2->epi_high_bit;

	for (i=0; i<hb; i++) {
	    v += epi1->epi_bits[i] + epi2->epi_bits[i];
	    epi1->epi_bits[i] = v & 1;
	    v >>= 1;
	}

	if (v != 0)
	    epi1->epi_bits[i++] = v;

	epi1->epi_high_bit = i;
}


/*
 * Shift an EPI n bits to the left.
 * Negative n values are OK.
 *
 * epi <<= n;
 */

void
epi_shift (
	EPI		*epi,
	int32_t		n
) {
	int32_t		hb = epi->epi_high_bit;

	if (n == 0 || hb == 0)
	    return;

	if (hb + n <= 0) {
	    epi_init (epi);
	    return;
	}

	if (n < 0) {
	    int32_t		i;

	    epi->epi_high_bit += n;
	    for (i=0; i<epi->epi_high_bit; i++)
		epi->epi_bits[i] = epi->epi_bits[i - n];
	    for (i = 0; i < -n; i++)
		epi->epi_bits[epi->epi_high_bit + i] = 0;
	} else {
	    int32_t		i;

	    for (i = epi->epi_high_bit - 1; i >= 0; i--)
		epi->epi_bits[i + n] = epi->epi_bits[i];
	    for (i=0; i<n; i++)
		epi->epi_bits[i] = 0;
	    epi->epi_high_bit += n;
	}
}


/*
 * Multiply an EPI by an integer.
 *
 * epi *= n;
 */

void
epi_multiply (
	EPI		*epi,
	int32_t		n
) {
	EPI		epiret;

	epi_init (&epiret);

	while (n != 0) {
	    if ((n & 1) != 0)
		epi_add (&epiret, epi);

	    n >>= 1;
	    epi_shift (epi, 1);
	}

	epi_copy (epi, &epiret);
}


/*
 * Subtract one EPI from another.
 * Assumes that epi1 > epi2.
 *
 * epi1 -= epi2;
 */

static void
epi_subtract (
	EPI		*epi1,
	const EPI	*epi2
) {
	int32_t		i;
	int32_t		v = 0;

	for (i=0; i<epi1->epi_high_bit; i++) {
	    v = epi1->epi_bits[i] - epi2->epi_bits[i] - v;
	    epi1->epi_bits[i] = v & 1;
	    v >>= 1;
	    v &= 1;
	}

	i = epi1->epi_high_bit - 1;
	epi1->epi_high_bit = 0;
	while (i >= 0) {
	    if (epi1->epi_bits[i] == 1) {
		epi1->epi_high_bit = i + 1;
		break;
	    }
	    i--;
	}
}


/*
 * Decrement an EPI.
 *
 * epi--;
 */

void
epi_decrement (
	EPI		*epi
) {
	int32_t		i;
	int32_t		v = 1;

	for (i=0; i<epi->epi_high_bit; i++) {
	    v = epi->epi_bits[i] - v;
	    epi->epi_bits[i] = v & 1;
	    v >>= 1;
	    v &= 1;

	    if (v == 0)
		break;
	}

	i = epi->epi_high_bit - 1;
	epi->epi_high_bit = 0;
	while (i >= 0) {
	    if (epi->epi_bits[i] == 1) {
		epi->epi_high_bit = i + 1;
		break;
	    }
	    i--;
	}
}


/*
 * Divide an EPI by an integer.
 * Return the remainder.
 *
 * x = epi % n;
 * epi /= n;
 * return x;
 */

int32_t
epi_divide (
	EPI		*epi,
	int32_t		n
) {
	EPI		epires, epidiv;
	int32_t		rem;

	epi_set (&epidiv, n);
	epi_init (&epires);

	if (epi_cmp (epi, &epidiv) >= 0) {
	    int32_t		i, hb = 1;

	    epi_shift (&epidiv, 1);
	    while (epi_cmp (&epidiv, epi) <= 0) {
		hb++;
		epi_shift (&epidiv, 1);
	    }

	    for (i=0; i<hb; i++) {
		epi_shift (&epidiv, -1);
		if (epi_cmp (&epidiv, epi) <= 0) {
		    epires.epi_bits[hb - i - 1] = 1;
		    epi_subtract (epi, &epidiv);
		}
	    }

	    epires.epi_high_bit = hb;
	}

	rem = epi_as_int (epi);
	epi_copy (epi, &epires);

	return (rem);
}
