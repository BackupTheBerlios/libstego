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
 *  Author: Ralf Treu
 *
 *  For more info visit <http://parsys.informatik.uni-oldenburg.de/~stego/>
 */
 
 
 
#include "libstego/gif_helpers.h"

/**
* Makes a copy from palette_data_t struct
* @param *src_data original structure
* @param *copy pointer to not yet initialized target structure
* @return an errorcode or 0 if success
*/
uint8_t copy_palette_data_t(const palette_data_t *src_data, 
        palette_data_t  *copy)
{
    // allocate space for new structures
    copy->table  = (rgb_pixel_t*)malloc(sizeof(rgb_pixel_t) 
            * src_data->tbl_size);
    copy->unique_colors  = (rgb_pixel_t*)malloc(sizeof(rgb_pixel_t) 
            * src_data->unique_length);
    copy->nonunique_colors  = (rgb_pixel_t*) malloc(sizeof(rgb_pixel_t) 
            * src_data->nonunique_length);
    copy->img_data = (uint8_t*)malloc(sizeof(uint8_t) * src_data->size_x 
            * src_data->size_y);
    // copy simple data
    copy->tbl_size         = src_data->tbl_size;
    copy->unique_length    = src_data->unique_length;
    copy->nonunique_length = src_data->nonunique_length;
    copy->size_x           = src_data->size_x;
    copy->size_y           = src_data->size_y;
    // copy structures
    uint32_t i;
    
    for (i = 0; i < src_data->tbl_size; i++) {
        copy->table[i].comp.r = src_data->table[i].comp.r;
        copy->table[i].comp.g = src_data->table[i].comp.g;
        copy->table[i].comp.b = src_data->table[i].comp.b;
    }
    for (i = 0; i < src_data->unique_length; i++) {
        copy->unique_colors[i].comp.r = src_data->unique_colors[i].comp.r;
        copy->unique_colors[i].comp.g = src_data->unique_colors[i].comp.g;
        copy->unique_colors[i].comp.b = src_data->unique_colors[i].comp.b;
    }
    for (i = 0; i < src_data->nonunique_length; i++) {
        copy->nonunique_colors[i].comp.r = 
            src_data->nonunique_colors[i].comp.r;
        copy->nonunique_colors[i].comp.g = 
            src_data->nonunique_colors[i].comp.g;
        copy->nonunique_colors[i].comp.b = 
            src_data->nonunique_colors[i].comp.b;
    }
    for (i = 0; i < src_data->size_x * src_data->size_y; i++) {
        copy->img_data[i] = src_data->img_data[i];
    }
    return 0;
}

/**
* Writes colours from one colour table to another using random order
* @param *src_data original structure
* @param *new_pal pointer to target structure
* @param length the amount of colours in src_data
* @param *state a pointer to the randomizer
* @return an errorcode or 0 if success
*/
uint8_t sort_palette_random(const rgb_pixel_t *src_data, rgb_pixel_t *new_pal, uint32_t length, prng_state *state)
{
    /* Rearrange order of new_pal with prng */
    uint32_t *used = (uint32_t*) malloc(sizeof(uint32_t) * length);
    uint32_t new_index;
    uint32_t is_empty;
    for (uint32_t i = 0; i < length; i++) {
        new_index = random_next(state) % length;
        is_empty = 0;
        while (is_empty == 0) {
            is_empty = 1;
            for(uint32_t j = 0; j < i; j++) {
                if (new_index == used[j]) {
                    is_empty = 0;
                    break;
                }
            }
            if (is_empty == 0 && new_index < length - 1) {
                new_index += 1;
            }
            else if (is_empty == 0 && new_index == length -1) {
                new_index = 0;
            }
        }
        used[i] = new_index;
        new_pal[new_index].comp.r = src_data[i].comp.r;
        new_pal[new_index].comp.g = src_data[i].comp.g;
        new_pal[new_index].comp.b = src_data[i].comp.b;
    }
    free(used);
    used = 0;
    return 0;
}

/**
* Sorts a given colour table (unique colours only) to RGB
* @param *unique_colors the unique colours of a colour table
* @param unique_length the amount of colours in unique_colors
* @return an errorcode or 0 if success
*/
uint8_t sort_palette_rgb(rgb_pixel_t *unique_colors, uint32_t unique_length)
{
    rgb_pixel_t *unique_copy = (rgb_pixel_t*) malloc(sizeof(rgb_pixel_t) * unique_length);
    int64_t last_val = -1;
    uint32_t new_index;
    int64_t new_val;
    for (uint32_t i = 0; i < unique_length; i++) {
        // find the smallest entry in color table
        int64_t comp_val = 65536 * 256;
        for (uint32_t j = 0; j < unique_length; j++) {
            new_val = unique_colors[j].comp.r * 65536
                + unique_colors[j].comp.g * 256
                + unique_colors[j].comp.b;
            if (new_val > last_val && new_val < comp_val) {
                comp_val  = new_val;
                new_index = j;
            }
        }
        // add it to the copy at index i
        unique_copy[i].comp.r = unique_colors[new_index].comp.r;
        unique_copy[i].comp.g = unique_colors[new_index].comp.g;
        unique_copy[i].comp.b = unique_colors[new_index].comp.b;
        // set last_val to value just entered
        last_val = comp_val;
    }
    for (uint32_t i = 0; i < unique_length; i++) {
        unique_colors[i].comp.r = unique_copy[i].comp.r;
        unique_colors[i].comp.g = unique_copy[i].comp.g;
        unique_colors[i].comp.b = unique_copy[i].comp.b;
    }
    free(unique_copy);
    unique_copy = 0;
    return 0;
}
