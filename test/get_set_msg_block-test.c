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
#include "libstego/utils.h"

#include <stdlib.h>
#include <stdio.h>

uint8_t *teststring = "ABCD";

uint8_t test_strcpy(uint8_t blocklen)
{
    printf("testing stringcopy with %d bit blocklen...", blocklen);
    uint32_t msgblock = 0;
    uint32_t msglen = strlen(teststring)*8;
    if (msglen % blocklen) {
        printf("not supported.\n");
        return 0;
    }
    uint8_t *copy = (uint8_t*)malloc(strlen(teststring));
    uint32_t nblocks = msglen/blocklen;
    for(int i = 0; i<nblocks; ++i) {
        msgblock = get_msg_block(teststring, i, blocklen);
        set_msg_block(copy, msgblock, i, blocklen);
    }
    if(strcmp(teststring, copy)) {
        printf("failed.\n");
        printf("%s",copy);
        return 1;
    }
    free(copy);
    printf("ok.\n");
    return 0;
}

uint8_t test_intcpy(uint8_t blocklen)
{
    uint32_t testint = 0xabcd1234;
    printf("testing intcopy with %d bit blocklen...", blocklen);
    uint32_t msgblock = 0;
    uint32_t msglen = 32;
    if (msglen % blocklen) {
        printf("not supported.\n");
        return 0;
    }
    uint32_t *copy = ALLOC(uint32_t);
    uint32_t nblocks = msglen/blocklen;
    for(int i = 0; i<nblocks; ++i) {
        msgblock = get_msg_block((uint8_t*)&testint, i, blocklen);
        set_msg_block((uint8_t*)copy, msgblock, i, blocklen);
    }
    if(testint!=*copy) {
        printf("failed.\n");
        printf("%x",*copy);
        return 1;
    }
    free(copy);
    printf("ok.\n");
    return 0;

    
    return 0;
}

int32_t main(void)
{
    uint8_t *teststring_copy = (char*) malloc(strlen(teststring));
    
    uint8_t fail = 0;
    for(uint8_t blocklen = 1; blocklen <= 32; blocklen++) {
        fail |= test_strcpy(blocklen);
        fail |= test_intcpy(blocklen);
    }
    return 0;
}
