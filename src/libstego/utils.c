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
 
 
 

#include <libstego/types.h>
#include <libstego/errors.h>
#include <stdint.h>
#include <stdlib.h>


typedef int8_t (*cmpfunc)(void*, void*);

void gensort(void *array, size_t el_size, uint32_t num, cmpfunc fnc)
{
  
}

int8_t cmp_rgb(void *a, void *b)
{
  return 0;
}


int8_t cmp_brightness(void *a, void *b)
{
  return 0;
}

uint32_t get_msg_block(uint8_t *message, uint32_t block, uint32_t blength) {
    uint32_t streampos = block * blength; //Position of block in message
				          //stream
    uint32_t bytepos = streampos / 8;     //byte-aligned position in
				          //message stream
    uint8_t  bitpos = streampos % 8;      //position inside message
					  //stream byte
    
    uint32_t bitsleft = blength;
    uint32_t result = 0;
    while(bitsleft) {
	result = result << 1;
	result = result | ((message[bytepos] & (1 << bitpos)) ? 1 : 0);
	bitpos++;
	if(bitpos > 7) {
	    bitpos = 0;
	    bytepos++;
	}
	bitsleft--;
    }
    return result;
}    

uint32_t set_msg_block(uint8_t *buf, uint32_t msgbits, uint32_t block, uint32_t blength)
{
    uint32_t bitpos = block * blength;
    uint32_t bufbyte = bitpos/8;
    uint32_t bufbit = (bitpos % 8);
    for(uint32_t i=0; i< blength; i++) {
	uint8_t bit = (msgbits & (1 << (blength-1-i))) ? 1 : 0;
	buf[bufbyte] = buf[bufbyte] | (bit << (bufbit));
	bufbit++;
	if(bufbit == 8) {
	    bufbit = 0;
	    bufbyte++;
	}
    }
    return LSTG_OK;	
}



uint32_t jpeg_data_copy(const jpeg_data_t *src_data, jpeg_data_t *dst_data)
{
    dst_data->comp_num = src_data->comp_num;
    dst_data->comp = (jpeg_comp_t*) malloc(sizeof(jpeg_comp_t) * dst_data->comp_num);
    dst_data->size_x = src_data->size_x;
    dst_data->size_y = src_data->size_y;
    for (uint8_t comp = 0; comp < src_data->comp_num; comp++) {
	dst_data->comp[comp].nblocks = src_data->comp[comp].nblocks;
	dst_data->comp[comp].blocks = (jpeg_block_t*)malloc(sizeof(jpeg_block_t) *
							  dst_data->comp[comp].nblocks);
	dst_data->comp[comp].quant = (jpeg_quant_t*)malloc(sizeof(jpeg_quant_t));
	memcpy(dst_data->comp[comp].quant, src_data->comp[comp].quant, sizeof(jpeg_quant_t));
	memcpy(dst_data->comp[comp].blocks, src_data->comp[comp].blocks, sizeof(jpeg_block_t) * 
	       dst_data->comp[comp].nblocks);
    }
    return LSTG_OK;
}

uint32_t jpeg_data_cleanup(jpeg_data_t *jpeg_data) {
    for (uint8_t comp = 0; comp < jpeg_data->comp_num; comp++) {
            free(jpeg_data->comp[comp].blocks);
    }
    free(jpeg_data->comp);
    return LSTG_OK;
}
