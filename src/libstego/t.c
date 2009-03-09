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
 
 
 
#include "libstego/t.h"

/**
 * Embeds a message in a paletteimage using T
 * ... 
 * @param *jpeg a struct for jpegdata to provide the jpeg-image
 * @param *palette a struct for palettedata to provide the palette-image
 * @param *stego_palette a struct for palettedata to return the steganogram
 * @param *message returnpointer for the extracted message
 * @param msglen the length of the message
 * @param *para additional parameters for T, including passphrase
 * @return an errorcode or 0 if success
 * @todo implement
 */
uint8_t t_embed(const jpeg_data_t *jpeg, const palette_data_t *palette, palette_data_t *stego_palette, uint8_t *message, int32_t msglen, const t_parameter *param) {
    /*   uint8_t i, j, k; 
       uint32_t patternlen = 8;
       uint8_t capacity = 0;
       uint32_t block = 0;
       uint8_t comp = 0;
       bool embeded;
    
       uint8_t *matrix = (uint8_t*)malloc(sizeof (int32_t) * patternlen * patternlen);
       uint8_t embed_matrix = (uint8_t*)malloc(sizeof(uint8_t) * 64);
       if (matrix == 0) {
           FAIL(LSTG_E_MALLOC);
       }
       i = 0;
       while (i < msglen * 8) {
           while (capacity == 0) {
               capacity = _t_calc_weightmatrix(message+i, msglen, param->key+i, param->keylen, patternlen, matrix);
               if (capacity == 0) {
                   patternlen--;
               }
               if (patternlen <= 0) {
                   //This will never happen!
                   return LSTG_ERROR;
               }
           }
           for (k = 0; k < 64; k++) {
               embed_matrix[k] = 0;
           }
           embeded = false;
           for (j = 0; j < patternlen * patternlen; j++) {
               embeded = false;
               for (k = 0; k < 64; k++) {
                   if (matrix[j] == jpeg->comp[comp]->blocks[block]->values[k]) {
                       embed_matrix[k] = j;
                       embeded = true;
                       break;
                   }
               }
               if (!embeded) {
                   _t_embed_coordinates(NULL, gif);
                   j--;
                   if (block < jpeg->comp[comp]->nblocks) {
                       block++;
                   }
                   else {
                       comp++;
                       block = 0;
                   }
               }
           }
           _t_embed_coordinates(embed_matrix, gif);
           i += patternlen;
           block++;
       }
       SAFE_DELETE(embed_matrix);
       SAFE_DELETE(matrix);*/
    return LSTG_OK;
}

/**
 * Extracts a T-embedded-message from a stegonogram
 * ...
 * @param *jpeg a struct for jpegdata to provide the jpeg-image
 * @param *palette a struct for palettedata to provide the palette-image
 * @param *message returnpointer for the extracted message
 * @param msglen the length of the message
 * @param *para additional parameters for T, including passphrase
 * @return an errorcode or 0 if success
 * @todo implement
 */
uint8_t t_extract(const jpeg_data_t *jpeg, const palette_data_t *stego_palette, uint8_t *message, int32_t msglen, const t_parameter *para) {

    return LSTG_OK;
}

/**
 * Returns the length of an embedded message
 * ...
 * @param *jpeg a struct for jpegdata to provide the jpeg-image
 * @param *palette a struct for palettedata to provide the palette-image
 * @param *para additional parameters T including passphrase
 * @return length of embedded message
 * @todo implement
 */
uint8_t t_get_message_length(const jpeg_data_t *jpeg, const t_parameter *param) {

    return LSTG_OK;
}

/**
 * Returns the capacity of a cover image
 * ...
 * @param *jpeg a struct for jpegdata to provide the jpeg-image
 * @param *palette a struct for palettedata to provide the palette-image
 * @param *para additional parameters for BattleSteg including passphrase
 * @return capacity of cover image
 * @todo implement
 */
uint8_t t_check_capacity(const jpeg_data_t *jpeg, const t_parameter *param, uint32_t *capacity) {

    return LSTG_OK;
}

uint8_t _t_calc_weightmatrix(uint8_t *message, int32_t msglen, uint8_t *key, int32_t keylen, int32_t patternlen, uint8_t *matrix) {
    int i, j, k;
    int capacity = patternlen;
    /*
    bool valid;
    bool stable;

    for (i = 0; i < patternlen * patternlen; i++) {
        matrix[i] = 0;
    }

    valid = false;
    while (!valid) {
        for (i = 0; i < capacity; i++) {
            for (ii = 0; ii < patternlen; ii++) {
                for (iii = 0; iii < patternlen; iii++) {
                    matrix[ii * patternlen + iii] += BIPOLAR(&(message+(i*patternlen)+ii)) * BIPOLAR(&(key+(i*patternlen)+iii));
                }
            }
        }
        valid = _t_check_net(matrix, key, message, capacity, patternlen);
        if (!valid) {
            capacity--;
        }
    }
     **/
    return capacity;
}

uint8_t _t_embed_coordinates(uint8_t *matrix, const rgb_data_t *gif) {
    if (matrix == NULL) {
        //Embed empty block
    } else {

    }
}

bool _t_check_net(uint8_t weightmatrix, uint8_t *input, uint8_t *output, uint32_t capacity, uint32_t patternlen) {
    uint32_t i, ii, iii;

    bool stable, valid;
    /*
        uint8_t *out = (uint8_t*) malloc(sizeof (int8_t) * patternlen);
        uint8_t *in = (uint8_t*) malloc(sizeof (int8_t) * patternlen);

        valid = true;
        for (i = 0; i < capacity; i++) {
            for (ii = 0; ii < patternlen; ii++) {
                in[ii] = BIPOLAR(&(input + i * patternlen + ii));
                out[ii] = 0;
            }
            stable = false;
            while (!stable) {
                stable = true;
                for (ii = 0; ii < patternlen * patternlen; ii++) {
                    for (iii = 0; iii < patternlen; iii++) {
                        out[ii] += matrix[ii * patternlen + iii] * in[iii];
                    }
                }
                for (ii = 0; ii < patternlen; ii++) {
                    if (out[ii] > 0) {
                        out[ii] = 1;
                    } else if (out[ii] < 0) {
                        out[ii] = -1;
                    }
                    in[ii] = 0;
                }
                for (ii = 0; ii < patternlen * patternlen; ii++) {
                    for (iii = 0; iii < patternlen; iii++) {
                        in[ii] += matrix[ii * patternlen + iii] * out[iii];
                    }
                }
                for (ii = 0; ii < patternlen; ii++) {
                    if (in[ii] > 0) {
                        in[ii] = 1;
                    } else if (in[ii] < 0) {
                        in[ii] = -1;
                    }
                }
                for (ii = 0; ii < patternlen; ii++) {
                    if (in[ii] != out[ii]) {
                        stable = false;
                    }
                }
                if (!stable) {
                    for (ii = 0; ii < patternlen; ii++) {
                        out[ii] = 0;
                    }
                }
            }
            for (ii = 0; ii < patternlen; ii++) {
                if (BIPOLAR(&(output + ii)) != out[ii]) {
                    valid = false;
                }
            }
        }

        SAFE_DELETE(in);
        SAFE_DELETE(out);
     **/
    return valid;
}
