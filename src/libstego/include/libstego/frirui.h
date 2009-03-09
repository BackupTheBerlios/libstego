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
 
 
 
#ifndef __FRIRUI_H__
#define __FRIRUI_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libstego/types.h>
#include <libstego/utils.h>
#include <libstego/errors.h>
#include <libstego/random.h>


typedef struct{
    uint8_t color1;
    uint8_t color2;
    uint32_t distance;
}DISTANCE;

typedef struct{
    uint32_t threshold;
    uint32_t parity;
    uint8_t pixels[9];
    uint32_t position_x[9];
    uint32_t position_y[9];
}BLOCK3X3; 

typedef struct{
    uint8_t pixels[4];
    uint32_t position_x[4];
    uint32_t position_y[4];
    uint32_t parity;
}BLOCK2X2;


typedef struct{
    uint32_t parity;
    uint8_t *pixels;
    uint32_t *id_in_stream;
}CHAIN;

uint32_t frirui_embed(const palette_data_t *src_data, palette_data_t *stego_data, uint8_t *message, uint32_t msglen, const frirui_parameter *para);

uint32_t frirui_extract(const palette_data_t *stego_data, uint8_t **message, uint32_t *msglen, const frirui_parameter *para);

uint32_t frirui_get_message_length(const palette_data_t *stego_data, const frirui_parameter *para);

uint32_t frirui_check_capacity(const palette_data_t *src_data, uint32_t *capacity, const frirui_parameter *para);

uint32_t frirui_calc_parity(uint8_t *parity, rgb_pixel_t *table, 
			    uint32_t tbl_size);

uint32_t frirui_calc_distances(rgb_pixel_t *table, uint32_t tbl_size, 
	DISTANCE *distances, uint32_t dist_length);

uint32_t frirui_build_colortable(DISTANCE *distances, uint32_t dist_length,
				uint8_t *new_colortable,
				//rgb_pixel_t *table,
				uint32_t tbl_size,
				uint8_t *parity);

uint32_t frirui_embed_message_to_chains(uint8_t *message, uint32_t msglen,
	CHAIN *chains, uint32_t ch_length, uint32_t num_of_chains,
	uint8_t* password, uint32_t pwlen, uint32_t size_x, uint32_t size_y, uint8_t *parity,
	uint8_t* img_data,uint8_t *new_colortable, uint32_t tbl_size, 
	rgb_pixel_t *table);

uint32_t embed_bit_to_chain(uint8_t bit,CHAIN *chain, uint32_t ch_length, 
	uint8_t *parity,rgb_pixel_t *table, uint8_t *new_colortable, 
	uint32_t tbl_size);

uint32_t frirui_embed_message_to_3X3BLOCKS(uint8_t **matrix, uint32_t rows, 
				    uint32_t columns,
				    BLOCK3X3 *blocks, uint32_t block_count, 
				    uint32_t threshold, uint8_t *password,
				    uint32_t pwlen, uint8_t *parity, 
				    uint8_t *message, uint32_t msglen, 
				    uint8_t *new_colortable,
				    uint32_t tbl_size, rgb_pixel_t *table);

uint32_t embed_bit_to_3X3BLOCK(uint8_t bit, BLOCK3X3 *block, uint32_t tbl_size,
			    uint8_t *new_colortable, rgb_pixel_t *table,
			    uint8_t *parity, uint32_t threshold);

uint32_t frirui_embed_message_to_2X2BLOCKS(uint8_t **matrix, uint32_t rows, 
				    uint32_t columns, BLOCK2X2 *blocks, 
				    uint32_t block_count, uint8_t *password,
				    uint32_t pwlen, uint8_t *parity, 
				    uint8_t *message,
				    uint32_t msglen, uint8_t *new_colortable,
				    uint32_t tbl_size, rgb_pixel_t *table);

uint32_t embed_bit_to_2X2BLOCK(uint8_t bit, BLOCK2X2 *block, uint32_t tbl_size, 
	uint32_t *usable_blocks, uint8_t *new_colortable,
	uint32_t *usable_colors, rgb_pixel_t *table, 
	uint32_t usable_length, uint8_t *parity);

#endif // __FRIRUI_H__
