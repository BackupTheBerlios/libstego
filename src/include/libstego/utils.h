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
 
 
 
#ifndef __UTILS_H_
#define __UTILS_H_

#define ALLOC(t) (t*) malloc(sizeof(t))
#define STRALLOC(len) (char*)malloc(sizeof(char)*len)
#define SALLOC(t) (struct t*) malloc(sizeof(struct t))
#define ALLOCN(t, n) (t*) malloc(sizeof(t) * n)
#define SALLOCN(t, n) (struct t*) malloc(sizeof(struct t) * n)

#define MOD(n,m) ((n<0)? -n % m: n % m)
#define CEIL(val, div) ((val/div) + ((val % div) ? 1 : 0))


#define SAFE_DELETE(p) \
    if (p) { \
        free(p); \
        (p) = NULL; \
    }


uint32_t get_msg_block(uint8_t *message, uint32_t block, uint32_t blength);

uint32_t set_msg_block(uint8_t *buf, uint32_t msgbits, uint32_t block, uint32_t blength);

uint32_t jpeg_data_copy(const jpeg_data_t *src_data, 
			jpeg_data_t *dst_data);

#endif /*__UTILS_H_*/
