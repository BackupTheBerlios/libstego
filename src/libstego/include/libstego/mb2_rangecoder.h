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
 
 
 
#ifndef __MB2_RANGECODER_H__
#define __MB2_RANGECODER_H__

#include "libstego/types.h"

typedef uint8_t freq;

typedef struct {
    uint8_t low;
    uint8_t range;
    uint8_t help;
    unsigned char buffer;
    uint8_t bytecount;

} rangecoder;

void encode_frequency(rangecoder *rangecoder, freq symbol_frequency, freq lower, freq total);
freq decode_frequency(rangecoder *rangecoder, freq total);

void decode_update(rangecoder *rangecoder, freq symbol_frequency, freq lower, freq total);


uint8_t start_encoding(rangecoder *rangecoder);
uint8_t stop_encoding(rangecoder *rangecoder);




#endif // __MB2_RANGECODER_H__

