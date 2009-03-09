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
 
 
 
#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <stdint.h>
#include <gsl/gsl_rng.h>

typedef struct {
    gsl_rng *gsl_state;
} prng_state;

uint8_t random_init(prng_state **newstate, const uint8_t *seed, const uint32_t length);

uint8_t random_set_fast(prng_state *state);

uint32_t random_next(prng_state *state);

uint32_t random_cleanup(prng_state *state);

#endif /* _RANDOM_H_ */
