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
 * Author: Lars Herrmann, Marko Krause <zeratul2099@googlemail.com>
 *
 *  For more info visit <http://parsys.informatik.uni-oldenburg.de/~stego/>
 */
 
 
 
#include "libstego/f5.h"


/**
* Embeds a message in a jpeg using F5
* @param *src_data a struct for jpegdata to provide the original image
* @param *stego_data a struct for jpegdata to return the steganogram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for F5, including passphrase
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t f5_embed(const jpeg_data_t *src_data, jpeg_data_t *stego_data, uint8_t *message, int32_t msglen, const f5_parameter *para)
{
    //check the algorithm conditioned maximum message length
    if (msglen >= 16777216) {
        FAIL(LSTG_E_MSGTOOLONG);
    }
    uint32_t error = 0;
    f5_context context;
    jpeg_data_copy(src_data, stego_data);
    context.stego_data = stego_data;
    context.message = message;
    context.msg_len = msglen;
    prng_state *random;
    random_init(&random, para->password, para->pwlen);
    context.random = random;
    f5_create_coeff_list(&context);
    f5_choose_code(&context);
    if (context.k == 0) {
        random_cleanup(random);
        jpeg_data_cleanup(stego_data);
        FAIL(LSTG_E_MSGTOOLONG);
    }
    f5_embed_message_length(&context);
    error = f5_matrix_encode(&context);
    if(error) {
        random_cleanup(random);
        jpeg_data_cleanup(stego_data);
        free(context.coeff_list);
        FAIL(error);
    }
    random_cleanup(random);
    free(context.coeff_list);
    return LSTG_OK;
}

/**
* Extracts a F5-embedded-message from a steganogram
* @param *stego_data a struct for jpegdata containing the stegonagram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for F5 including passphrase
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t f5_extract(const jpeg_data_t *stego_data, uint8_t **message, int32_t *msglen, const f5_parameter *para)
{
    uint32_t error;
    f5_context context;
    context.stego_data = stego_data;
    prng_state *random;
    random_init(&random, para->password, para->pwlen);
    context.random = random;
    f5_create_coeff_list(&context);
    f5_get_message_length(&context);
    if((error = f5_extract_message(&context))!=LSTG_OK) {
        random_cleanup(random);
        free(context.coeff_list);
        FAIL(error);
    }
    
    *msglen = context.msg_len;
    *message = context.message;
    random_cleanup(random);
    free(context.coeff_list);
    return LSTG_OK;
}

uint32_t f5_extract_message(f5_context *context)
{
    uint32_t msg_len = context->msg_len * 8;
    context->message = (uint8_t*)calloc(msg_len, 1);
    uint32_t msgblocks = CEIL(msg_len, context->k);
    uint32_t current_coeff = 32; //start extracting after the embedded message length
    uint32_t msgblocks_left = msgblocks;
    uint8_t code = 1;
    uint32_t hash = 0;
    while (current_coeff < context->list_len) {
        int16_t coeff = *(context->coeff_list[current_coeff++]);
        uint8_t bit = 0;
        if(coeff == 0) continue; // skip 0's
        
        /* if(coeff > 0) { */
        /*     bit = coeff&1; */
        /* } else { */
        /*     bit = 1 - (coeff &1) ; */
        /* } */

        bit = ((uint8_t) abs(coeff)) & 1;
        if(bit) {
            hash ^= code;
        }
        if(code++ == context->n) {   // block finished
            code = 1;
            set_msg_block(context->message, hash, msgblocks - msgblocks_left, context->k);
            hash = 0;
            msgblocks_left--;
        }
        if(!msgblocks_left) {
            return LSTG_OK;
        }
    }
    FAIL(LSTG_E_MSGTOOLONG);
}

/**
* Extracts the length and the code of an embedded message
* @param *context a context-structure for F5
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t f5_get_message_length(f5_context *context)
{
    uint32_t msg_len = 0;
    int32_t i;
    for( i = 0; i < 32; i++) {
        if(abs(*(context->coeff_list[i])%2) == 1) {
            msg_len += 1;
        }
        if(i!=31) {
            msg_len = msg_len << 1;
        }
        
    }
    // extract choosen code from lsg_len
    context->k = msg_len / 16777216;
    context->n = (1 << context->k) - 1;
    // extract msg_len
    context->msg_len = msg_len % 16777216;
    //printf("Extraction: msg_len = %d, code = (1, %d, %d)\n", context->msg_len, context->k, context->n);
    return LSTG_OK;
}

/**
* Returns the capacity of a cover image
* @param *src_data a struct for jpegdata to provide the original image
* @param *capacity the guessed capacity of the image
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t f5_check_capacity(const jpeg_data_t *src_data, uint32_t *capacity)
{
    uint32_t num_blocks = 0;
    uint32_t i, j, k, num_coeff, coeff, ng;
    uint32_t m0 = 0;
    uint32_t m1 = 0;
    uint32_t m2 = 0;
    // iterate components
    for (i = 0; i < src_data->comp_num; i++) {
	    num_blocks += src_data->comp[i].nblocks;
	    // iterate blocks
	    for (j = 0; j < src_data->comp[i].nblocks; j++) {
	        // iterate coefficient
	        for (k = 0; k < 64; k++) {
                coeff = src_data->comp[i].blocks[j].values[k];
		        if (coeff == -2 || coeff == 2) {
		            m2++;
		        }
		        else if (coeff == -1 || coeff == 1) {
		            m1++;
		        } 
		        else if (coeff == 0) {
		            m0++;
		        }
	        }
	    }
    }
    num_coeff = num_blocks * 64;
    ng = num_coeff - m0 - m1 - m2/4 - num_coeff/64 + m1/3;
    *capacity = ng;
    return LSTG_OK;



}
/**
* Creates a pointer list pointing to the coeffitient
* @param *context a context-structure for F5
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t f5_create_coeff_list(f5_context *context)
{
    uint32_t i, j, k, l, num_coeff = 0;
    // count non-zero coefficients
    // iterate components
    for (i = 0; i < context->stego_data->comp_num; i++) {
	    num_coeff += (context->stego_data->comp[i].nblocks * 63);

    }
    // initiate coeff list
    context->coeff_list = (int16_t**)malloc(num_coeff*sizeof(int16_t*));
    l = 0;
    for (i = 0; i < context->stego_data->comp_num; i++) {

	    // iterate blocks
	    for (j = 0; j < context->stego_data->comp[i].nblocks; j++) {
	        // iterate coefficient, skip DC
	        for (k = 1; k < 64; k++) {
		        context->coeff_list[l] = &context->stego_data->comp[i].blocks[j].values[k];
		        l++;
	        }
	    }
    }

    context->list_len = num_coeff;
    uint32_t rand_num;
    int16_t *temp;
    // shuffle coeff_list
    for( i = 0; i < num_coeff; i++) {
	    rand_num = random_next(context->random) % num_coeff;
	    temp = context->coeff_list[0];
	    context->coeff_list[0] = context->coeff_list[rand_num];
	    context->coeff_list[rand_num] = temp;
    }
    return LSTG_OK;

}
/**
* Chooses a f5 code for the message
* @param *context a context-structure for F5
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t f5_choose_code(f5_context *context)
{
    uint8_t i, n;
    uint32_t n_g;

    f5_check_capacity(context->stego_data, &n_g);
    //printf("Starting choose code with n_g = %d\n", n_g);
    for( i = 1; i < 8; i++) {
	    n = (1 << i) - 1;
	    uint32_t usable = (uint32_t)(n_g * i / n - (n_g * i / n) % n);
	    usable /= 8;
		    if (usable == 0) {
		        break;
	        }
		    if (usable < context->msg_len + 4) {
			    break;
	        }
    }
    context->k = i-1;
    context->n = (1 << context->k) - 1;

    //printf("Using (1,%d,%d) code for embed\n", context->k,context->n);
    return LSTG_OK;
}
/**
* Embeds the length and the code of the message
* @param *context a context-structure for F5
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t f5_embed_message_length(f5_context *context)
{
    //printf("embed %d bits\n", context->msg_len);
    // embeds the message length ...
    uint32_t msg_len = context->msg_len;
    // ... and the code via lsb
    msg_len = msg_len + context->k*16777216;
    int32_t i;
    for (i = 31; i >= 0; i--) {
        if (msg_len % 2 == 1) {
            if(*(context->coeff_list[i]) % 2 == 0) {
                *(context->coeff_list[i])+=1;
                //printf("0->1\n");
            }
        }
        else {
            if(*(context->coeff_list[i]) % 2 == 1) {
                  *(context->coeff_list[i])-=1;
            }
            else if(*(context->coeff_list[i]) % 2 == -1) {
                *(context->coeff_list[i])+=1;
            }
            
        }
        msg_len = msg_len >> 1;
    }
    return LSTG_OK;
    
}
/**
* embeds a message in the coefflist via code k
* @param *context a context-structure for F5
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t f5_matrix_encode(f5_context *context)
{

    uint8_t k = context->k;;
    uint8_t n = context->n;
    uint8_t k_bits_to_embed;
    uint8_t hash;
    uint8_t s, success, i;
    // embed after the msg_len
    uint32_t start_of_coeff_block = 32;
    uint32_t current_coeff;
    uint32_t shrinkage = 0;
    uint32_t msg_block_count = 0, coeff_block_count = 0;
    uint32_t num_msg_blocks = CEIL(context->msg_len*8, k);
    // embed
    int16_t **codecoeffs = (int16_t**) calloc(n, sizeof(int16_t*));
    current_coeff = start_of_coeff_block;
    // embed the message blockwise of length k in n coefficients
    for(msg_block_count = 0; msg_block_count < num_msg_blocks; msg_block_count++) {
    // get k messagebits
	k_bits_to_embed = (uint8_t)get_msg_block(context->message, msg_block_count, k);
	success = 0;
	do {
	    if(coeff_block_count+n >= context->list_len) {
		FAIL(LSTG_E_INSUFFCAP);
	    }

	    hash = 0;
	    // put n non-zero coeffs in a buffer an calculate a hash
	    for(i = 0; i < n; current_coeff++) {
		// choose a coeff != 0 
		    while(*(context->coeff_list[current_coeff]) == 0) {
		        current_coeff++;
		    }
		    codecoeffs[i] = context->coeff_list[current_coeff];
            int16_t coeff = *(codecoeffs[i]);
		    if((int16_t)abs(coeff) & 1) {
		    hash ^= i+1;
		    }
		    i++;
	    }
	    s = hash ^ k_bits_to_embed;
	    if (s == 0) {
		success = 1;
                start_of_coeff_block = current_coeff;   
	    }
	    else {
		s--;
		int16_t *coeff = codecoeffs[s];
		if(*coeff > 0) {
		  (*coeff)-=1;
		}
		else {
		  (*coeff)+=1;
		}
		if(*coeff) {
		    success = 1;
                    start_of_coeff_block = current_coeff;   
                }
                else {
                    shrinkage++;
                    current_coeff = start_of_coeff_block;
                }
                    
                        
	    }
    	} while(!success);
    
    }
    free(codecoeffs);
    return LSTG_OK;
}
