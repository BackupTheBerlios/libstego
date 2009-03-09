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
 
 
 
#include "libstego/gifshuffle.h"
#include <stdio.h>
#include <stdlib.h>

/**
* Embeds a message in a paletteimage using GifShuffle.
* The given palette is sorted in RGB order (256^2 * R + 256^1 * G + B).
* Then, a 1 gets added in front of the message's bytesequence.
* After that the algorithm iterates through the unique colours of the palette
* and defines a new index for each unique colour n-i by computing 
* new_index = m % i ; if the index is taken, each already copied colour gets 
* shifted up one place in the new palette. After that m gets the new value
* m / i .
* When iteration has finished the non unique colours from the original
* palette are copied to the end of the new palette.
* @param *src_data a struct for palettedata to provide the original image
* @param *stego_data a struct for palettedata to return the steganogram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for GifShuffle, including passphrase
* @return an errorcode or 0 if success
*/
uint32_t gifshuffle_embed(const palette_data_t *src_data, palette_data_t *stego_data, uint8_t *message, int32_t msglen, const gifshuffle_parameter *para)
{
    uint32_t err = 0;
    if ((err = copy_palette_data_t(src_data, stego_data)) != LSTG_OK) {
        FAIL(LSTG_E_MALLOC);
    }
    uint32_t n = src_data->unique_length;
    // create a copy of src_data->unique_colors. The original may not be changed!
    rgb_pixel_t *unique_copy = (rgb_pixel_t*)malloc(sizeof(rgb_pixel_t) * n);
    for (uint32_t i = 0; i < n; i++) {
        unique_copy[i].comp.r = src_data->unique_colors[i].comp.r;
        unique_copy[i].comp.g = src_data->unique_colors[i].comp.g;
        unique_copy[i].comp.b = src_data->unique_colors[i].comp.b;
    }
    if (para->method == 0) {
        // We use normal gifshuffle. If != 1 we leave this out for adv gifshuffle when the first shuffle is already done.
        if ((err = sort_palette_rgb(unique_copy, n)) != LSTG_OK){
            free(unique_copy);
            FAIL(LSTG_E_MALLOC);
        }
        if (para->pwlen > 0){
            // We want to use a random sort with password as random seed
            prng_state *state = 0;
            random_init(&state, para->password, para->pwlen);
            if ((err = sort_palette_random(unique_copy,stego_data->unique_colors, 
                        n, state) != LSTG_OK)){
                free(unique_copy);
                random_cleanup(state);
                FAIL(LSTG_E_MALLOC);
            }
            random_cleanup(state);
            for (uint32_t i = 0 ;i < n; i++){
                  unique_copy[i].comp.r = stego_data->unique_colors[i].comp.r;
                  unique_copy[i].comp.g = stego_data->unique_colors[i].comp.g;
                  unique_copy[i].comp.b = stego_data->unique_colors[i].comp.b;
            }
        }
    }
    err = 0;
    uint8_t *msg = (uint8_t*) malloc(sizeof(uint8_t) * (msglen));
    if (msg == NULL) {
        free(unique_copy);
        FAIL(LSTG_E_MALLOC);
    }
    // add the message length to the first four bytes of the message
    uint32_t len = msglen;
    printf("Message length = %i\n", len);
    for (uint32_t i = 0; i < msglen; i++) {
        msg[i] = message[i];
    }

    // Create EPI that holds the message as a huge integer
    EPI msg_as_number;
    epi_init(&msg_as_number);
    EPI one_byte;
    epi_init(&one_byte);
    // add leading 1 to msg_as_number
    epi_set(&msg_as_number, 1);
    for (uint32_t i = 0; i < msglen; i++) {
        epi_shift(&msg_as_number, 8);
        epi_set(&one_byte,msg[i]);
        epi_add(&msg_as_number, &one_byte);
    }
    uint32_t used_counter = 0;
    uint32_t* used = (uint32_t*)malloc(sizeof(uint32_t) * n);
    uint32_t index_used;
    //compute new index for color
    for (uint32_t i = 0; i < n; i++) {
        uint32_t msg_as_int = epi_as_int(&msg_as_number);
        uint32_t new_index = epi_divide(&msg_as_number,i+1);
        msg_as_int = epi_as_int(&msg_as_number);
        // new index is already in use:
        for(uint32_t j = 0; j < used_counter; j++) {
            if (used[j] == new_index) {
                index_used = 1;
                break;
            }
        }
        // get space for new color by putting all higher colors up by 1
        if (index_used == 1) {
            for(uint32_t ii = n - 1; ii > new_index; ii--) {
                stego_data->unique_colors[ii].comp.r = stego_data->unique_colors[ii-1].comp.r;
                stego_data->unique_colors[ii].comp.g = stego_data->unique_colors[ii-1].comp.g;
                stego_data->unique_colors[ii].comp.b = stego_data->unique_colors[ii-1].comp.b;
            }
            // also change the used indexes
            for (uint32_t ii = 0; i < used_counter; ii++) {
                if (used[ii] >= new_index) {
                    used[ii]++;
                }
            }
        }
        stego_data->unique_colors[new_index].comp.r = unique_copy[n-i-1].comp.r;
        stego_data->unique_colors[new_index].comp.g = unique_copy[n-i-1].comp.g;
        stego_data->unique_colors[new_index].comp.b = unique_copy[n-i-1].comp.b;
        used[used_counter] = new_index;
        used_counter++;
    }
    epi_set(&msg_as_number,0);
    epi_set(&one_byte,0);
    free(used);
    used = 0;
    // fill shuffled unique colors into palette
    for(uint32_t i = 0; i < n; i++) {
        stego_data->table[i].comp.r = stego_data->unique_colors[i].comp.r;
        stego_data->table[i].comp.g = stego_data->unique_colors[i].comp.g;
        stego_data->table[i].comp.b = stego_data->unique_colors[i].comp.b;
    }
    // add non unique colors
    for(uint32_t i = 0; i < src_data->nonunique_length; i++) {
        stego_data->table[n+i].comp.r = stego_data->nonunique_colors[i].comp.r;
        stego_data->table[n+i].comp.g = stego_data->nonunique_colors[i].comp.g;
        stego_data->table[n+i].comp.b = stego_data->nonunique_colors[i].comp.b;
    }
    // change the pixel values, fitting the shuffled palette
    uint32_t use_it = 0;
    uint32_t use_switch = 0;
    for (uint32_t i = 0; i < src_data->size_x * src_data->size_y; i++) {
        // check if the color is nonunique. If so, toggle the use_it switch.
        uint32_t is_unique = 1;
        uint32_t new_index;
        for (uint32_t ii = 0; ii < src_data->nonunique_length; ii++) {
            if (src_data->table[src_data->img_data[i]].comp.r
                    == src_data->nonunique_colors[ii].comp.r
                    && src_data->table[src_data->img_data[i]].comp.g
                    == src_data->nonunique_colors[ii].comp.g
                    && src_data->table[src_data->img_data[i]].comp.b
                    == src_data->nonunique_colors[ii].comp.b) {
                is_unique = 0;
                if(use_switch == 0) {
                    use_it = 1;
                }
                else {
                    use_it = 0;
                }
                break;
            }
        }
        for (new_index = 0; new_index < src_data->tbl_size; new_index++) {
            if (stego_data->table[new_index].comp.r == 
                    src_data->table[src_data->img_data[i]].comp.r
                    && stego_data->table[new_index].comp.g == 
                    src_data->table[src_data->img_data[i]].comp.g
                    && stego_data->table[new_index].comp.b == 
                    src_data->table[src_data->img_data[i]].comp.b) {
                if (is_unique == 0 && use_it == 0) {
                    use_it = 1;
                }
                else {
                    break;
                }
            }
        }
        if (use_switch == 0) {
            use_switch = 1;
        }
        else {
            use_switch = 0;
        }
        // set the new index in the new pixel data
        stego_data->img_data[i] = new_index;
    }
    free(msg);
    free(unique_copy);
    msg = 0;
    unique_copy = 0;
    return 0;
}

/**
* Extracts a GifShuffle-embedded-message from a stegonogram.
* The stego-palette gets copied to another palette which then gets sorted
* again by RGB (see above).
* Set m := 0.
* Iterate through the unique colours of the ordered palette and compute 
* m := m * (n - i) + p,
* where m is the message, n the amount of unique colours, i the iterating 
* variable and p the index of the i-th colour in the stego-palette. Iterate
* until the first 32 bits containing the message can be read to tell
* how long the message actually is. Then iterate till the number of bits
* given in the first 32 bits is reached.
* When the iteration has finished the highest 1-Bit of m gets deleted.
* m then contains the bytestream of the message.
* @param *stego_data a struct for palettedata containing the stegonagram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for GifShuffle including passphrase
* @return an errorcode or 0 if success
*/
uint32_t gifshuffle_extract(const palette_data_t *stego_data, uint8_t **message, uint32_t *msglen, const gifshuffle_parameter *para)
{
    uint32_t err = 0;
    uint32_t n = stego_data->unique_length;
    // Create a copy of the unique colors
    rgb_pixel_t* unique_copy = (rgb_pixel_t*)malloc(sizeof(rgb_pixel_t) * n);
    for (uint32_t i = 0; i < n; i++) {
        unique_copy[i].comp.r = stego_data->unique_colors[i].comp.r;
        unique_copy[i].comp.g = stego_data->unique_colors[i].comp.g;
        unique_copy[i].comp.b = stego_data->unique_colors[i].comp.b;
    }
    // Use normal gifshuffle, not adv gifshuffle
    if (para->method == 0) {
        // sort the copy in RGB order
        if((err = sort_palette_rgb(unique_copy, n) != LSTG_OK)) {
            free(unique_copy);
            FAIL(LSTG_E_MALLOC);
        }
        // Use gifshuffle with password order
        if (para->pwlen != 0) {
            prng_state *state = 0;
            random_init(&state, para->password, para->pwlen);
            // for random, create a copy and destroy it directly afterwards
            rgb_pixel_t* cp_pal = (rgb_pixel_t*)malloc(sizeof(rgb_pixel_t)*n);
            for (uint32_t jj = 0; jj < n; jj++) {
                cp_pal[jj].comp.r = unique_copy[jj].comp.r;
                cp_pal[jj].comp.g = unique_copy[jj].comp.g;
                cp_pal[jj].comp.b = unique_copy[jj].comp.b;
            }
            if ((err = sort_palette_random(cp_pal,
                        unique_copy, n, state) != LSTG_OK)) {
                free(unique_copy);
                FAIL(LSTG_E_MALLOC);
            }
            free(cp_pal);
            cp_pal = 0;
            random_cleanup(state);
        }
    }
    else {
        unique_copy = para->adv_pal->unique_colors;
    }
    // Create EPIs
    EPI m;
    EPI pos;
    epi_init(&m);
    epi_init(&pos);
    uint32_t variable_len = n;
    // Everything prepared, extract the message
    for (uint32_t i = 0; i < n; i++) {
        uint32_t oldpos;
        for (uint32_t j = 0; j < variable_len; j++) {
            if(stego_data->unique_colors[j].comp.r == unique_copy[i].comp.r
                    && stego_data->unique_colors[j].comp.g == unique_copy[i].comp.g
                    && stego_data->unique_colors[j].comp.b == unique_copy[i].comp.b) {
                 oldpos = j;
                 break;
            }
        }
        // Put every color in stego palette that has a higher index then the chosen color
        // down by 1. (Just like we put it up when while embedding)
        for(uint32_t j = oldpos; j < variable_len - 1; j++) {
            stego_data->unique_colors[j].comp.r = stego_data->unique_colors[j+1].comp.r;
            stego_data->unique_colors[j].comp.g = stego_data->unique_colors[j+1].comp.g;
            stego_data->unique_colors[j].comp.b = stego_data->unique_colors[j+1].comp.b;
        }
        variable_len --;
        epi_multiply(&m, n - i);
        epi_set(&pos, oldpos);
        epi_add(&m, &pos);
    }
    (*msglen) = m.epi_high_bit / 8;
    printf("Message length = %i\n",*msglen);
    *message = (uint8_t*)malloc(sizeof(uint8_t) * (*msglen + 1));
    uint8_t byte = 0;
    uint32_t msg_counter = 0;
    uint32_t bit_counter = 0;
    for (uint32_t i = m.epi_high_bit - 1; i > 0; i--) {
        bit_counter++;
        byte = byte << 1;
        byte += m.epi_bits[i-1];
        if (bit_counter % 8 == 0) {
            (*message)[msg_counter] = byte;
            byte = 0;
            msg_counter++;
        }
    }
    (*message)[*msglen] = '\0';
    free(unique_copy);
    unique_copy = 0;
    epi_set(&pos,0);
    epi_set(&m,0);
    return 0;
}

/**
* Returns the capacity of a cover image.
* GifShuffle has a capacity of log_2(n!) Bits.
* Count the amount of unique colours in the palette (n) and return the
* computed value.
* @param *src_data a struct for palettedata to provide the original image
* @param *para additional parameters for GifShuffle including passphrase
* @return capacity of cover image
*/
int32_t gifshuffle_check_capacity(const palette_data_t *src_data,uint32_t* capacity, const gifshuffle_parameter *para)
{
    if (src_data->unique_length < 1 || src_data->unique_length > 256){
        FAIL(LSTG_E_SIZEMISMATCH);
    }
    *capacity = capacity_lookup[src_data->unique_length - 1];
    return 0;
}
