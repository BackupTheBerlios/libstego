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

#include "libstego/random.h"


int main(void)
{
    printf("Testing PRNG with \"test\" as seed\n");
    prng_state *prng = 0;
    random_init(&prng, "test", 4);
    for (int i = 0; i < 10; ++i)
    {
        printf("%u\n", random_next(prng));
    }
    random_cleanup(prng);
    printf("Reinitializing the PRNG with the same seed (numbers should be the same)\n");
    random_init(&prng, "test", 4);
    for (int i = 0; i < 10; ++i)
    {
        printf("%u\n", random_next(prng));
    }
    random_cleanup(prng);
    printf("Reinitializing the PRNG with a different seed (numbers should be different)\n");
    random_init(&prng, "test2", 5);
    for (int i = 0; i < 10; ++i)
    {
        printf("%u\n", random_next(prng));
    }
    random_cleanup(prng);
    random_init(&prng, "test", 4);
    printf("Starting benchmark with 100000 random numbers...");
    for (int i = 0; i < 100000; ++i) {
      random_next(prng);
    }
    printf("finished\n");
    random_cleanup(prng);
    return 0;
}
