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
 
 
 
#include "libstego/sort_unsort.h"

/**
* Embeds a message in a paletteimage using Sort/Unsort
* First a new, empty palette is created. Then the original palette gets
* sorted by a Pseudo Random Number Generator, using the password as a seed.
* After that the unique colors are copied to the palette:
*  - If message bit is 1 the next colour in the original palette gets copied
*  - If message bit is 0 another color but the next one is picked random 
*  and copied to the actual index of the new palette.
* @param *src_data a struct for palettedata to provide the original image
* @param *stego_data a struct for palettedata to return the steganogram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for Sort/Unsort, including passphrase
* @return an errorcode or 0 if success
*/
uint32_t sortunsort_embed(const palette_data_t *src_data, palette_data_t *stego_data, uint8_t *message, uint32_t msglen, const sortunsort_parameter *para)
{
    uint32_t len = src_data->unique_length;
    if (copy_palette_data_t(src_data, stego_data) != LSTG_OK) {
        FAIL(LSTG_E_MALLOC);
    }
    /* Get new copy of src_data->unique_colors. */
    rgb_pixel_t* cpunique = (rgb_pixel_t*)malloc(sizeof(
                rgb_pixel_t) * len);
    for (uint32_t i = 0; i < stego_data->unique_length; i++) {
        cpunique[i].comp.r = src_data->unique_colors[i].comp.r;
        cpunique[i].comp.g = src_data->unique_colors[i].comp.g;
        cpunique[i].comp.b = src_data->unique_colors[i].comp.b;
    }
    prng_state *state = 0;
    random_init(&state, para->password, para->pwlen);
    if (sort_palette_rgb(cpunique,
                len) != LSTG_OK) {
        free(cpunique);
        FAIL(LSTG_E_MALLOC);
    }
    if (sort_palette_random(cpunique, 
                stego_data->unique_colors,
                len,
                state) != LSTG_OK) {
        free(cpunique);
        FAIL(LSTG_E_MALLOC);
    }
    // copy sorted colors back to old table 
    uint32_t i;
    for ( i = 0 ;i < len; i++){
        cpunique[i].comp.r = stego_data->unique_colors[i].comp.r;
        cpunique[i].comp.g = stego_data->unique_colors[i].comp.g;
        cpunique[i].comp.b = stego_data->unique_colors[i].comp.b;
    }
    // EMBED 
    // Add msg length to message 
    uint32_t *used = (uint32_t*)malloc(sizeof(uint32_t) * len);
    uint8_t *msg = (uint8_t*)malloc(sizeof(uint8_t) * (msglen+1));
    msg[0] = (uint8_t) msglen & 255;
    for (i = 1; i <= msglen; i++) {
        msg[i] = message[i-1];
    }
    for (i = 0 ;i < len; i++){
        used[i] = 256;
    }
    uint32_t old_pal_pointer = 0;
    uint32_t new_pal_pointer = 0;
    uint32_t used_counter = 0;
    for (i = 0; i < msglen + 1; i++){
       int32_t z;
       //printf("Embed byte: %i\n",msg[i]);
        for (z = 7; z >= 0; z--) {
           uint8_t bit = (msg[i] >> z) & 1;
            // Copy directly, message bit is 1 
            if (bit == 1){
                stego_data->unique_colors[new_pal_pointer].comp.r = 
                    cpunique[old_pal_pointer].comp.r;
                stego_data->unique_colors[new_pal_pointer].comp.g = 
                    cpunique[old_pal_pointer].comp.g;
                stego_data->unique_colors[new_pal_pointer].comp.b = 
                    cpunique[old_pal_pointer].comp.b;
                used[used_counter] = old_pal_pointer;
                used_counter++;
                uint32_t index_in_use = 1;
                while (index_in_use){
                    index_in_use    = 0;
                    for(uint32_t ii = 0; ii < used_counter; ii++) {
                    if (used[ii] == old_pal_pointer){
                        index_in_use = 1;
                        break;
                        }
                    }
                    if (index_in_use == 1) {
                        old_pal_pointer++;
                    }
                }
                new_pal_pointer += 1;
            }
            // Pick random from old palette between old_pal_pointer and len and put into new_palette at new_pal_pointer 
            else{
               uint32_t new_index = random_next(state);
                if (new_index >= len) {
                    new_index = new_index % len;
                }
                while (new_index <= old_pal_pointer) {
                    new_index++;
                }
                uint32_t index_in_use = 1;
                while (index_in_use == 1) {
                    index_in_use = 0;
                    for(uint32_t ii = 0; ii < used_counter; ii++) {
                        if(new_index == used[ii]){
                            index_in_use = 1;
                            break;
                        }
                    }
                    if (new_index < len - 1 && index_in_use == 1) {
                        new_index++;
                    }
                    else if (new_index == len - 1 && index_in_use == 1) {
                        new_index = old_pal_pointer + 1;
                    }
                    else {
                        stego_data->unique_colors[new_pal_pointer].comp.r =
                            cpunique[new_index].comp.r;
                        stego_data->unique_colors[new_pal_pointer].comp.g =
                            cpunique[new_index].comp.g;
                        stego_data->unique_colors[new_pal_pointer].comp.b =
                            cpunique[new_index].comp.b;
                        new_pal_pointer++;
                        used[used_counter] = new_index;
                        used_counter++;
                    }
                }
            }
        }
    }
    
    // put remaining unique colors also to new unique colors.
    for(i = 0; i < len; i++) {
        uint32_t is_in = 0;
        for(uint32_t j = 0; j < new_pal_pointer; j++) {
            if(stego_data->unique_colors[j].comp.r == 
                    src_data->unique_colors[i].comp.r
                    && stego_data->unique_colors[j].comp.g ==
                    src_data->unique_colors[i].comp.g
                    && stego_data->unique_colors[j].comp.b ==
                    src_data->unique_colors[i].comp.b){
                is_in = 1;
                break;
            }
        }
        if (is_in == 0) {
            stego_data->unique_colors[new_pal_pointer].comp.r = 
                src_data->unique_colors[i].comp.r;
            stego_data->unique_colors[new_pal_pointer].comp.g = 
                src_data->unique_colors[i].comp.g;
            stego_data->unique_colors[new_pal_pointer].comp.b = 
                src_data->unique_colors[i].comp.b;
            new_pal_pointer += 1;
        }
    }
    free(used);
    free(msg);
    free(cpunique);
    cpunique = 0;
    msg  = 0;
    used = 0;
    // Put rearranged unique colours back to palette struct
    for (i = 0; i < len; i++) {
        stego_data->table[i].comp.r = stego_data->unique_colors[i].comp.r;
        stego_data->table[i].comp.g = stego_data->unique_colors[i].comp.g;
        stego_data->table[i].comp.b = stego_data->unique_colors[i].comp.b;
    }
    // Fill with nonunique colours
    for (i = 0; i < src_data->nonunique_length; i++) {
        stego_data->table[len + i].comp.r = src_data->nonunique_colors[i].comp.r;
        stego_data->table[len + i].comp.g = src_data->nonunique_colors[i].comp.g;
        stego_data->table[len + i].comp.b = src_data->nonunique_colors[i].comp.b;
    }
    // change the pixel values
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
    random_cleanup(state);
    return 0;
}


/**
* Extracts a Sort/Unsort-embedded-message from a stegonogram
* A copy of the stego-palette is created and that palette again sorted by
* the PRNG with the password-seed as described above.
* Then iterate through the new palette and check whether the next color in that
* palette is in the same index as in the stego-palette:
*  - If it is, add a 1-Bit to the message
*  - It it is not, add a 0-Bit to the message.
* @param *stego_data a struct for palettedata containing the stegonagram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for Sort/Unsort including passphrase
* @return an errorcode or 0 if success
*/
uint32_t sortunsort_extract(const palette_data_t *stego_data, uint8_t **message, uint32_t *msglen, const sortunsort_parameter *para)
{
    uint32_t len = stego_data->unique_length;
    uint8_t err =  0;
    rgb_pixel_t *new_pal = (rgb_pixel_t*)malloc(sizeof(rgb_pixel_t) * len);
    rgb_pixel_t *rgb_pal = (rgb_pixel_t*)malloc(sizeof(rgb_pixel_t) * len);
    uint8_t* messagebuf = (uint8_t*) malloc(sizeof(uint8_t) * 256);
    prng_state *state = 0;
    random_init(&state, para->password, para->pwlen);
    for (uint32_t i = 0; i < len; i++) {
        rgb_pal[i].comp.r = stego_data->unique_colors[i].comp.r;
        rgb_pal[i].comp.g = stego_data->unique_colors[i].comp.g;
        rgb_pal[i].comp.b = stego_data->unique_colors[i].comp.b;
    }

    if ((err = sort_palette_rgb(rgb_pal,len) != LSTG_OK)) {
        free(new_pal);
        free(rgb_pal);
        FAIL(LSTG_E_MALLOC);
    }
    for (uint32_t i = 0; i < len; i++) {
        new_pal[i].comp.r = rgb_pal[i].comp.r;
        new_pal[i].comp.g = rgb_pal[i].comp.g;
        new_pal[i].comp.b = rgb_pal[i].comp.b;
    }
    if ((err = sort_palette_random(rgb_pal, new_pal, 
                len, state) != LSTG_OK)) {
        free(new_pal);
        free(rgb_pal);
        FAIL(LSTG_E_MALLOC);
    }
    free(rgb_pal);
    rgb_pal = 0;
    uint32_t used_counter = 0;
    uint32_t *used = (uint32_t*) malloc(sizeof(uint32_t) * len);
    uint8_t byte = 0;
    uint32_t message_index = 0;
    uint32_t pal_pointer = 0;
    for (uint32_t i = 1; i <= len; i++) {
        uint32_t bit;
        if (stego_data->unique_colors[i-1].comp.r == new_pal[pal_pointer].comp.r
                && stego_data->unique_colors[i-1].comp.g == new_pal[pal_pointer].comp.g
                && stego_data->unique_colors[i-1].comp.b == new_pal[pal_pointer].comp.b){
            bit = 1;
            used[used_counter] = pal_pointer;
            used_counter++;
        }
        else {
            bit = 0;
            for(uint32_t ii = 0; ii < len; ii++) {
                if (new_pal[ii].comp.r == stego_data->unique_colors[i-1].comp.r
                        && new_pal[ii].comp.g == stego_data->unique_colors[i-1].comp.g
                        && new_pal[ii].comp.b == stego_data->unique_colors[i-1].comp.b) {
                    used[used_counter] = ii;
                    used_counter++;
                    break;
                }
            }

        }

        if (bit == 1) {
            byte = byte << 1;
            byte = byte + bit;
        }
        else {
            byte  = byte << 1;
        }
        if (i % 8 == 0 && i != 0) {
            // It's the message length hidden in the first byte:
            if (i == 8) {
                (*msglen) = (uint32_t)byte;
                byte   = 0;
            }
            else {
                messagebuf[message_index] = byte;
                message_index ++;
                byte = 0;
            }
        }
        // find next unused index
        uint32_t index_used = 1;
        while(index_used == 1) {
            index_used = 0;
            for (uint32_t ii = 0; ii < used_counter; ii++) {
                if (pal_pointer == used[ii]) {
                    index_used = 1;
                    break;
                }
            }
            if (index_used == 1) {
                pal_pointer++;
            }
        }
    }
    *message = (uint8_t*)malloc(sizeof(uint8_t) * (*msglen + 1));
    for(uint32_t ii = 0; ii < *msglen; ii++) {
        (*message)[ii] = messagebuf[ii];
    }
    (*message)[*msglen] = '\0';
    printf("Message in extract: %s\n",(*message));
    free(new_pal);
    free(used);
    free(messagebuf);
    messagebuf = 0;
    used = 0;
    new_pal = 0;
    random_cleanup(state);
    return 0;
}

/**
* Returns the capacity of a cover image in Bit
* Count the amount of unique colours in the given palette and return amount -9.
* 1 Bit because of permutation, 8 because of the messages length.
* @param *src_data a struct for palettedata to provide the original image
* @param *para additional parameters for Sort/Unsort including passphrase
* @return capacity of cover image
*/
uint32_t sortunsort_check_capacity(const palette_data_t *src_data, uint32_t *capacity, const sortunsort_parameter *para)
{
    *capacity = src_data->unique_length - 1 -8;
    return 0;

}
