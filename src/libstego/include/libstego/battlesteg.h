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
 
 
 
#ifndef __BATTLESTEG_H__
#define __BATTLESTEG_H__

#include <libstego/types.h>
#include <libstego/errors.h>
#include <libstego/random.h>
#include <libstego/utils.h>
#include <stdlib.h>
#include <stdio.h>


/*
 * Struct to associate a filter value to a coordinate 
 */
typedef struct {

    uint16_t value;
    uint16_t coord_x;
    uint16_t coord_y;

} filter_value_t;

/*
 * Struct to hold the filter map 
 */
typedef struct {

    filter_value_t *filter_values;
    uint32_t size_x;
    uint32_t size_y;

} filter_map_t;


/*
 * Struct to hold the ships 
 */
typedef struct {

    uint8_t *ships;
    uint32_t size_x;
    uint32_t size_y;

} ship_array_t;


uint32_t battlesteg_embed(const rgb_data_t *src_data, rgb_data_t *stego_data,
			 uint8_t *message, uint32_t msglen, battlesteg_parameter *para);

uint32_t battlesteg_extract(rgb_data_t *stego_data,
			   uint8_t **message, uint32_t *msglen,
			   battlesteg_parameter *para);

uint32_t battlesteg_get_message_length(const rgb_data_t *stego_data,
				      const battlesteg_parameter *para);

uint32_t battlesteg_check_capacity(const rgb_data_t *src_data, uint32_t *capacity, const battlesteg_parameter *para);

uint32_t battlesteg_use_laplace(const rgb_data_t *src_data, filter_map_t *filter_map, uint8_t filterbits);

uint32_t battlesteg_quicksort(filter_value_t *array, int left, int right);

uint32_t battlesteg_shuffle_equal(filter_map_t *sorted_filter_map);

uint32_t battlesteg_find_ships(filter_map_t *filter_map, filter_map_t *sorted_filter_map, ship_array_t *ship_array);

uint32_t battlesteg_make_shot(rgb_data_t *stego_data, ship_array_t *ships, rgb_data_t *been_shot,
                                uint8_t *msg_bit, battlesteg_parameter *para, uint8_t set_get, prng_state **state);

#endif // __BATTLESTEG_H__
