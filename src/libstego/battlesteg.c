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
 *
 * Author: Marko Krause <zeratul2099@googlemail.com>
 *
 *  For more info visit <http://parsys.informatik.uni-oldenburg.de/~stego/>
 */
 
 
 
 #include "libstego/battlesteg.h"



/**
* Embeds a message in a rasterimage using BattleSteg.
* The algorithm first filters the image, and sorts the filter results.
* Then the top ten percent of the results are used to generate "engines" -
* pixels to drive off to find clusters.  Currently, just a good neighbour
* is needed for this and the neighbour to become a "ship".  Once a list
* of ships is established, the algorithm begins to hide the data.  It
* randomly generates positions, until it hits a "ship".  Then the following
* hits are ranged close to the hit, until the algorithm decides to move away.
* @param *src_data a struct for pixeldata to provide the original image
* @param *stego_data a struct for pixeldata to return the steganogram
* @param *message the message to embed
* @param msglen the length of the message
* @param *para additional parameters for BattleSteg, including passphrase
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t battlesteg_embed(const rgb_data_t *src_data, rgb_data_t *stego_data, uint8_t *message, uint32_t msglen, battlesteg_parameter *para)
{
    filter_map_t filter_map, sorted_filter_map;
    ship_array_t ships;
    rgb_data_t been_shot;
    uint32_t width = src_data->size_x;
    uint32_t height = src_data->size_y;
    uint32_t size = width * height;
    uint32_t i, k;
    
    // initiate stego_data and been_shot
    stego_data->size_x = width;
    stego_data->size_y = height;
    been_shot.size_x = width;
    been_shot.size_y = height;
    stego_data->data = (rgb_pixel_t*)malloc(sizeof(rgb_pixel_t)*width*height);
    been_shot.data = (rgb_pixel_t*)malloc(sizeof(rgb_pixel_t)*width*height);
    if(stego_data->data == 0) {
        FAIL(LSTG_E_MALLOC);
    }
    if(been_shot.data == 0) {
        free(stego_data->data);
        FAIL(LSTG_E_MALLOC);
    }
    for (i = 0; i < size; i++) {
        stego_data->data[i].rgb[0] = src_data->data[i].rgb[0];
        stego_data->data[i].rgb[1] = src_data->data[i].rgb[1];
        stego_data->data[i].rgb[2] = src_data->data[i].rgb[2];
        been_shot.data[i].rgb[0] = 0;
        been_shot.data[i].rgb[1] = 0;
        been_shot.data[i].rgb[2] = 0;
    }
 
    // filter image
    battlesteg_use_laplace(src_data, &filter_map, para->startbit);
    // initiate sorted filter_map
    sorted_filter_map.size_x = width;
    sorted_filter_map.size_y = height;
    sorted_filter_map.filter_values = (filter_value_t*)malloc(sizeof(filter_value_t) * size);
    if(sorted_filter_map.filter_values == 0) {
        free(stego_data->data);
        free(been_shot.data);
        free(filter_map.filter_values);
        FAIL(LSTG_E_MALLOC);
    }
    for (i = 0; i < size; i++) {
        sorted_filter_map.filter_values[i].value = filter_map.filter_values[i].value;
        sorted_filter_map.filter_values[i].coord_x = filter_map.filter_values[i].coord_x;
        sorted_filter_map.filter_values[i].coord_y = filter_map.filter_values[i].coord_y;
    }
    
    // sort filter results
    battlesteg_quicksort(sorted_filter_map.filter_values, 0, size-1);
    battlesteg_shuffle_equal(&sorted_filter_map);

    // find ships
    uint32_t num_ships = battlesteg_find_ships(&filter_map, &sorted_filter_map, &ships);

    
    if (3*(8-para->startbit)*num_ships/8-4 < (msglen+(msglen/10)))
    {
        free(stego_data->data);
        free(been_shot.data);
        free(filter_map.filter_values);
        free(sorted_filter_map.filter_values);
        free(ships.ships);
        FAIL(LSTG_E_MSGTOOLONG);
    }

    // free filter_maps, because we don't need them anymore
    free(filter_map.filter_values);
    free(sorted_filter_map.filter_values);
    uint8_t move_away = 0;
    
    // initiate PRNG
    prng_state *state;
    random_init(&state, para->password, para->pwlen);
    
    // embed message length
    for (i = 0; i < 32; i++) {
        // normal shot
        if (move_away <= 0) {
            move_away = para->move_away;
            para->last_hit_x = -1;
            para->last_hit_y = -1;
               
        }
        // ranged shot
        else {
            move_away--;
        }
        uint32_t one = 1;
        uint8_t embed = (msglen & (one << (31-i))) >> (31 - i);
        battlesteg_make_shot(stego_data, &ships, &been_shot, &embed, para, 1, &state);
    }
    // embed message
    for (i = 0; i < msglen; i++) {
        uint8_t one = 1;
        for (k = 0; k < 8; k++) {
            // normal shot
            if (move_away <= 0) {
                move_away = para->move_away;
                para->last_hit_x = -1;
                para->last_hit_y = -1;
               
            }
            // ranged shot
            else {
                move_away--;
            }
            uint8_t embed = (message[i] & (one << (7-k))) >> (7 - k);
            battlesteg_make_shot(stego_data, &ships, &been_shot, &embed, para, 1, &state);
        }
    }
    // free ships and been_shot
    free(ships.ships);
    free(been_shot.data);
    random_cleanup(state);
    return LSTG_OK;


}

/**
* Extracts a BattleSteg-embedded-message from a stegonogram.
* The process of extraction is similar to the embedding. The algorithm
* filters the image, sorts the results, finds engines and ship an
* randomly "shots" on the image. If a shit is the, the algorithm reads
* the specified LSB and goes on.
* @param *stego_data a struct for pixeldata containing the stegonagram
* @param **message returnpointer for the extracted message
* @param *msglen the length of the message
* @param *para additional parameters for BattleSteg including passphrase
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t battlesteg_extract(rgb_data_t *stego_data, uint8_t **message, uint32_t *msglen, battlesteg_parameter *para)
{
    

    filter_map_t filter_map, sorted_filter_map;
    ship_array_t ships;
    rgb_data_t been_shot;
    uint32_t width = stego_data->size_x;
    uint32_t height = stego_data->size_y;
    uint32_t size = width * height;
    uint32_t i, k;
    
    // initiate been_shot
    been_shot.size_x = width;
    been_shot.size_y = height;
    been_shot.data = (rgb_pixel_t*)malloc(sizeof(rgb_pixel_t)*width*height);
    if(been_shot.data == 0) {
        FAIL(LSTG_E_MALLOC);
    }
    for (i = 0; i < size; i++) {
        been_shot.data[i].rgb[0] = 0;
        been_shot.data[i].rgb[1] = 0;
        been_shot.data[i].rgb[2] = 0;
    }
    // filter image
    battlesteg_use_laplace(stego_data, &filter_map, para->startbit);
    // initiate sorted filter_map
    sorted_filter_map.size_x = width;
    sorted_filter_map.size_y = height;
    sorted_filter_map.filter_values = (filter_value_t*)malloc(sizeof(filter_value_t) * size);
    if (sorted_filter_map.filter_values == 0) {
        free(been_shot.data);
        free(filter_map.filter_values);
        FAIL(LSTG_E_MALLOC);
    }
    for (i = 0; i < size; i++) {
        sorted_filter_map.filter_values[i].value = filter_map.filter_values[i].value;
        sorted_filter_map.filter_values[i].coord_x = filter_map.filter_values[i].coord_x;
        sorted_filter_map.filter_values[i].coord_y = filter_map.filter_values[i].coord_y;
    }

    // sort filter results
    battlesteg_quicksort(sorted_filter_map.filter_values, 0, size-1);
    battlesteg_shuffle_equal(&sorted_filter_map);

    // find ships
    battlesteg_find_ships(&filter_map, &sorted_filter_map, &ships);

    // free filter_maps, because we don't need them anymore
    free(filter_map.filter_values);
    free(sorted_filter_map.filter_values);

    uint8_t move_away = 0;
    
    // initiate PRNG
    prng_state *state;
    random_init(&state, para->password, para->pwlen);

    // extract message length
    for (i = 0; i < 32; i++) {
        // normal shot
        if (move_away <= 0) {
            move_away = para->move_away;
            para->last_hit_x = -1;
            para->last_hit_y = -1;
               
        }
        // ranged shot
        else {
            move_away--;
        }
        uint8_t embed;
        
        battlesteg_make_shot(stego_data, &ships, &been_shot, &embed, para, 0, &state);
        *msglen = (*msglen << 1) + embed;

    }

    // Initiate message
    *message = (uint8_t*)malloc(sizeof(uint8_t) * *msglen);
    if (*message == NULL) {
        FAIL(LSTG_E_MALLOC);
    }
    (*message)[1] = 12;

    // extract message
    for (i = 0; i < *msglen; i++) {
        (*message)[i] = 0;
        for (k = 0; k < 8; k++) {

            // normal shot
            if (move_away <= 0) {
                move_away = para->move_away;
                para->last_hit_x = -1;
                para->last_hit_y = -1;              
            }
            // ranged shot
            else {
                move_away--;
            }
            uint8_t embed;
            battlesteg_make_shot(stego_data, &ships, &been_shot, &embed, para, 0, &state);
            if (k != 7) {            
                (*message)[i] = ((*message)[i] + embed) << 1;
            }
            else {
                
                (*message)[i] = (*message)[i] + embed;
            }
        }
        
    }

    // free ships and been_shot
    free(ships.ships);
    free(been_shot.data);
    random_cleanup(state);
    return LSTG_OK;
}
/**
* Will always return 0, because message length can only
* identified during extraction.
* @param *stego_data a struct for pixeldata containing the stegonagram
* @param *para additional parameters for BattleSteg including passphrase
* @return 0
*/
uint32_t battlesteg_get_message_length(const rgb_data_t *stego_data, const battlesteg_parameter *para)
{
    return 0;
}

/**
* Returns the capacity of a cover image.
* To get the capacity of a cover image, the algorithm simply counts the
* ships and multiplies it with the number of LSBs, which are used for
* embedding.
* @param *src_data a struct for pixeldata to provide the original image
* @param capacity capacity of cover image in Bytes
* @param *para additional parameters for BattleSteg including passphrase
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t battlesteg_check_capacity(const rgb_data_t *src_data, uint32_t *capacity, const battlesteg_parameter *para)
{
    filter_map_t filter_map, sorted_filter_map;
    ship_array_t ships;
    uint32_t width = src_data->size_x;
    uint32_t height = src_data->size_y;
    uint32_t size = width * height;
    uint32_t num_lsb = 8 - para->startbit;
    uint32_t i;
    // filter image
    battlesteg_use_laplace(src_data, &filter_map, para->startbit);
    // initiate sorted filter_map
    sorted_filter_map.size_x = width;
    sorted_filter_map.size_y = height;
    sorted_filter_map.filter_values = (filter_value_t*)malloc(sizeof(filter_value_t) * size);
    if (sorted_filter_map.filter_values == 0) {
        FAIL(LSTG_E_MALLOC);
    }
    for (i = 0; i < size; i++) {
        sorted_filter_map.filter_values[i].value = filter_map.filter_values[i].value;
        sorted_filter_map.filter_values[i].coord_x = filter_map.filter_values[i].coord_x;
        sorted_filter_map.filter_values[i].coord_y = filter_map.filter_values[i].coord_y;
    }
    // sort filter results
    battlesteg_quicksort(sorted_filter_map.filter_values, 0, size-1);

    // find ships
    uint32_t num_ships = battlesteg_find_ships(&filter_map, &sorted_filter_map, &ships);

    *capacity = ((num_ships * num_lsb * 3)-32)/8;

    // free filter_maps and ships
    free(filter_map.filter_values);
    free(sorted_filter_map.filter_values);
    free(ships.ships);
   
    return LSTG_OK;

}
/**
* Generates a Laplace filter map of an image
* @param *src_data the pixeldata of the image
* @param *filter_map data-struct for the filter map
* @param filterbits the bits of each color which are used by the filter
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t battlesteg_use_laplace(const rgb_data_t *src_data, filter_map_t *filter_map, uint8_t filterbits)
{
    uint32_t row, column, pixcount, width, height;
    uint8_t shift = 8 - filterbits;
    width = src_data->size_x;
    height = src_data->size_y;
    // empty pixel as default value
    rgb_pixel_t empty_pixel;
    empty_pixel.rgb[0] = 0;
    empty_pixel.rgb[1] = 0;
    empty_pixel.rgb[2] = 0;
    rgb_pixel_t left_pixel = empty_pixel;
    rgb_pixel_t right_pixel = empty_pixel;
    rgb_pixel_t upper_pixel = empty_pixel;
    rgb_pixel_t lower_pixel = empty_pixel;
    int32_t red_diff = 0, green_diff = 0, blue_diff = 0;
    // initiate filter_map
    filter_map->size_x = width;
    filter_map->size_y = height;
    filter_map->filter_values = (filter_value_t*)malloc(sizeof(filter_value_t)*width*height);
    if (filter_map->filter_values == 0) {
        FAIL(LSTG_E_MALLOC);
    }
    // iterate all pixels
    for(row = 0; row < height; row++) {
        for(column = 0; column < width; column++) {
            pixcount = 4;
            if (row == 0) {
                upper_pixel = empty_pixel;
                pixcount--;
            } else {
                // get upper pixel
                upper_pixel = src_data->data[(row-1)*width+column];
                upper_pixel.rgb[0] = upper_pixel.rgb[0] >> shift;
                upper_pixel.rgb[1] = upper_pixel.rgb[1] >> shift;
                upper_pixel.rgb[2] = upper_pixel.rgb[2] >> shift;
            }
            if (row == height-1) {
                lower_pixel = empty_pixel;
                pixcount--;
            } else {
                // get lower pixel
                lower_pixel = src_data->data[(row+1)*width+column];
                lower_pixel.rgb[0] = lower_pixel.rgb[0] >> shift;
                lower_pixel.rgb[1] = lower_pixel.rgb[1] >> shift;
                lower_pixel.rgb[2] = lower_pixel.rgb[2] >> shift;
            }
            if (column == 0) {
                left_pixel = empty_pixel;
                pixcount--;
            } else {
                // get left pixel
                left_pixel = src_data->data[row*width+column-1];
                left_pixel.rgb[0] = left_pixel.rgb[0] >> shift;
                left_pixel.rgb[1] = left_pixel.rgb[1] >> shift;
                left_pixel.rgb[2] = left_pixel.rgb[2] >> shift;
            }
            if (column == width-1) {
                right_pixel = empty_pixel;
                pixcount--;
            } else {
                // get right pixel
                right_pixel = src_data->data[row*width+column+1];
                right_pixel.rgb[0] = right_pixel.rgb[0] >> shift;
                right_pixel.rgb[1] = right_pixel.rgb[1] >> shift;
                right_pixel.rgb[2] = right_pixel.rgb[2] >> shift;
            }
            // sum of redvalues of the sorounding pixels
            int32_t red_sum = left_pixel.rgb[0]
                            + right_pixel.rgb[0]
                            + upper_pixel.rgb[0]
                            + lower_pixel.rgb[0];
            // sum of greenvalues of the sorounding pixels
            int32_t green_sum = left_pixel.rgb[1]
                            + right_pixel.rgb[1]
                            + upper_pixel.rgb[1]
                            + lower_pixel.rgb[1];
            // sum of bluevalues of the sorounding pixels
            int32_t blue_sum = left_pixel.rgb[2]
                            + right_pixel.rgb[2]
                            + upper_pixel.rgb[2]
                            + lower_pixel.rgb[2];

            // calculate color differences
            red_diff = (src_data->data[row*width+column].rgb[0] >> shift) * pixcount - red_sum;
            green_diff = (src_data->data[row*width+column].rgb[1] >> shift) * pixcount - green_sum;
            blue_diff = (src_data->data[row*width+column].rgb[2] >> shift) * pixcount - blue_sum;
            filter_map->filter_values[row*width+column].value = abs(red_diff) + abs(green_diff) + abs(blue_diff);
            filter_map->filter_values[row*width+column].coord_x = column;
            filter_map->filter_values[row*width+column].coord_y = row;
            // set diff to zero again
            red_diff = 0;
            green_diff = 0;
            blue_diff = 0;
        }
    }
    return LSTG_OK;
}

/**
* Sorts the filter map by the filter value with quicksort
* @param *array the array of filter_value_t to sort
* @param left first element in array to sort
* @param right last element in array to sort
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t battlesteg_quicksort(filter_value_t *array, int left, int right)
{
    int32_t pivot;
    int32_t l, r;
    filter_value_t temp;

    if (left < right)
    {
        l = left;
        r = right;
        pivot = array[(left+right) >> 1].value;
        do
        {
            while (array[l].value < pivot) l++;
            while (array[r].value > pivot) r--;

            if (l <= r)
            {
                temp = array[r];
                array[r] = array[l];
                array[l] = temp;

                l++;
                r--;
            }
        }
        while (l <= r);

        battlesteg_quicksort(array, left, r);
        battlesteg_quicksort(array, l, right);
    }
    return LSTG_OK;
}


/**
* Shuffles equal filter values on the filter map
* @param *sorted_filter_map data-struct for the sorted filter map
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t battlesteg_shuffle_equal(filter_map_t *sorted_filter_map) 
{
    uint32_t i, j, k;
    uint32_t size = sorted_filter_map->size_x * sorted_filter_map->size_y;
    prng_state *state;
    uint8_t seed = 0;
    random_init(&state, &seed, 1);
    for (i = 0; i < size; i++) {
        j = i;
        while (j < size && sorted_filter_map->filter_values[i].value == sorted_filter_map->filter_values[j].value) {
            j++;
        }

        if (i == j) {
            continue;
        }
        for (k = i; k < j; k++) {
            uint32_t rand_pos = random_next(state) % (j-i);
            if (i+rand_pos == k) {
                continue;
            }

            filter_value_t temp = sorted_filter_map->filter_values[k];
            sorted_filter_map->filter_values[k] = sorted_filter_map->filter_values[i+rand_pos];
            sorted_filter_map->filter_values[i+rand_pos] = temp;
        }
        i = j;
    }
    return LSTG_OK;
}

/**
* Finds ships (pixels to embed)
* @param *filter_map data-struct for the filter map
* @param *sorted_filter_map data-struct for the sorted filter map
* @param *ship_array pointer to save and return the location of the ships
* @return the number of ships
*/
uint32_t battlesteg_find_ships(filter_map_t *filter_map, filter_map_t *sorted_filter_map, ship_array_t *ship_array) {

    uint32_t i, median, count, x, y, num_ships = 0;
    uint32_t width = filter_map->size_x;
    uint32_t height = filter_map->size_y;
    // Define the number of filter values
    uint32_t num_pixel = width * height;
    // Define the top ten filter values
    uint32_t topten = num_pixel / 10 * 9;
    // Initiate ship array
    ship_array->size_x = width;
    ship_array->size_y = height;
    ship_array->ships = (uint8_t*)malloc(sizeof(uint8_t) * width * height);
    if (ship_array->ships == 0) {
        FAIL(LSTG_E_MALLOC);
    }
    for (i = 0; i < num_pixel; i++) {
        ship_array->ships[i] = 0;
    }
    // Define Median
    median = sorted_filter_map->filter_values[num_pixel / 2].value;
    // Find ships
    
    for (i = topten; i < num_pixel; i++) {
        x = sorted_filter_map->filter_values[i].coord_x;
        y = sorted_filter_map->filter_values[i].coord_y;

        count = 0;
        // check upper pixel
        if (y != 0) {
            if (filter_map->filter_values[(y-1) * width + x].value > median) {
                // ship found!
                ship_array->ships[(y-1) * width + x] = 1;
                count++;
            }
        }
        // check lower pixel
        if (y != height-1) {
            if (filter_map->filter_values[(y+1) * width + x].value > median) {
                // ship found!
                ship_array->ships[(y+1) * width + x] = 1;
                count++;
            }
        }
        // check left pixel
        if (x != 0) {
            if (filter_map->filter_values[y * width + x - 1].value > median) {
                // ship found!
                ship_array->ships[y * width + x - 1] = 1;
                count++;
            }
        }
        // check right pixel
        if (x != width-1) {
            if (filter_map->filter_values[y * width + x + 1].value > median) {
                // ship found!
                ship_array->ships[y * width + x + 1] = 1;
                count++;
            }
        }
        if (count > 0) {
            ship_array->ships[y * width + x] = 1;
           
        }
    }
    for (i=0; i < num_pixel;i++) {
        if(ship_array->ships[i]) {
            num_ships++;
        }
    
    }
    
    return num_ships;

}

/**
* Embeds or extracts a single pixel into a ship
* @param *stego_data the data where to embed or to extract from
* @param *ship_array array to mark locations of ships
* @param *been_shot stucture to save positions of embedded or extracted bits
* @param *para the battlesteg_parameter structure
* @param *msg_bit bit to embed or extract
* @param set_get 0 for embed, 1 for extract
* @param **state the state for the prng
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t battlesteg_make_shot(rgb_data_t *stego_data, ship_array_t *ship_array, rgb_data_t *been_shot,
                                uint8_t *msg_bit, battlesteg_parameter *para, uint8_t set_get, prng_state **state) {
      
    uint32_t x, y, width, height, bit, color;
    width = stego_data->size_x;
    height = stego_data->size_y;

   
    while(1) {
        // ranged shot
        if (para->last_hit_x > -1 && para->last_hit_y > -1) {
            uint8_t range_left, range_right, range_up, range_down;
            if (para->last_hit_x == 0) {
                range_left = 0;
            }
            else {
                range_left = para->range % para->last_hit_x;
            }
            if (para->last_hit_x == width-1) {
                range_right = 0;
            }
            else {
                range_right = para->range % (width - para->last_hit_x);
            }
            if (para->last_hit_y == 0) {
                range_up = 0;
            }
            else {
                range_up = para->range % para->last_hit_y;
            }
            if (para->last_hit_y == height-1) {
                range_down = 0;
            }
            else {
                range_down = para->range % (height - para->last_hit_y);
            }
            x = para->last_hit_x - range_left + (random_next(*state) % (range_left + range_right));
            y = para->last_hit_y - range_up + (random_next(*state) % (range_up + range_down));
        }
        else { // normal shot
            x = random_next(*state) % width;
            y = random_next(*state) % height;
        }
        color = random_next(*state) % 3;
        bit = random_next(*state) % (8-para->startbit);

        if(ship_array->ships[x + y * width] == 1 && (been_shot->data[x + y * width].rgb[color] & (1<<bit)) == 0) {
  
            if (set_get == 1) {
                *msg_bit = *msg_bit << bit;
                stego_data->data[x + y * width].rgb[color] 
                    = (stego_data->data[x + y * width].rgb[color] & ~(1 << bit))| *msg_bit;
            }
            else {
                
                *msg_bit = (stego_data->data[x + y * width].rgb[color] & (1<<bit)) >> bit;
                
            }
            been_shot->data[x + y * width].rgb[color] = been_shot->data[x + y * width].rgb[color] | (1 << bit);
            // save last shot            
            if (para->last_hit_x == -1 && para->last_hit_y == -1) {
                para->last_hit_x = x;
                para->last_hit_y = y;
            }
            return LSTG_OK;
        } 
        
    }

    // no empty ship found
    return LSTG_ERROR;
    
}


