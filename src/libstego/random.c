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
 
 
 
#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_rng.h>

#include "libstego/random.h"
#include "libstego/errors.h"
#include "libstego/utils.h"
#include "libstego/types.h"
#include "libstego/whirlpool.h"



#define LSTG_RAND_FILTER     0xffffffff


uint8_t random_init(prng_state **state, const uint8_t *seed, const uint32_t length)
{
    prng_state *newstate = ALLOC(prng_state);
    uint8_t hash[64];
    NESSIEstruct whirlpool_state;
    NESSIEinit(&whirlpool_state);
    NESSIEadd(seed, length * 8, &whirlpool_state);
    NESSIEfinalize(&whirlpool_state, hash);
    *state = newstate;
    newstate->gsl_state = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(newstate->gsl_state, *((uint64_t*)hash));
    return LSTG_OK;
}

uint8_t random_set_fast(prng_state *state)
{
    return 0;
}

uint32_t random_next(prng_state *state)
{
    return gsl_rng_get(state->gsl_state) & LSTG_RAND_FILTER;
}

uint32_t random_cleanup(prng_state *state)
{
    gsl_rng_free(state->gsl_state);
    free(state);
    return LSTG_OK;
}
