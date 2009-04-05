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



#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_matrix.h>
#include <stdint.h>

#include "libstego/types.h"
#include "libstego/cpt.h"
#include "libstego/utils.h"
#include "libstego/random.h"
#include "libstego/errors.h"

#define MOD(n,m) ((n<0)? -n % m: n % m)
#define CEIL(val, div) ((val/div) + ((val % div) ? 1 : 0))

typedef struct {
    gsl_matrix *key_matrix;
    gsl_matrix *weight_matrix;
    uint8_t    *image_data;
    uint32_t   image_data_length;
    uint32_t   r;
    uint32_t   image_width;
    uint32_t   image_height;
    uint32_t   block_width;
    uint32_t   block_height;
    prng_state *random;
} cpt_state;

/**
 * Debugging function that prints a matrix
 * @param *matrix the matrix to be printed
 */
void matrix_print(const gsl_matrix *matrix)
{
    uint32_t width = matrix->size2;
    uint32_t height = matrix->size1;
    for(uint32_t y = 0; y < height; y++) {
	for (uint32_t x = 0; x < width; x++) {
	    printf("%d ", (uint32_t)gsl_matrix_get(matrix, y, x));
	}
	printf("\n");
    }
    printf("\n");
}

/**
 * Function that returns a zeroed matrix the size of an image block
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 * @return a zeroed matrix the size of an image block. The matrix is
 * allocated inside the function.
 */

gsl_matrix *empty_block(cpt_state *context)
{
    return gsl_matrix_calloc(context->block_height, context->block_width);
}

/**
 * Function to check the parameter set for validity
 * @param *src_data the rgb-image to be embedded in / extracted from
 * NOTE: Does not need to contain actual image data, but the image
 * dimensions must be set up.
 * @param *para the parameter set
 * @return LSTG_OK if the parameters are in order, LSTG_E_INVALIDPARAM
 * otherwise
 */
uint32_t cpt_check_params(const rgb_data_t *src_data, const cpt_parameter *para)
{
    uint32_t m = para->block_width;
    uint32_t n = para->block_height;
    uint32_t x = src_data->size_x;
    uint32_t y = src_data->size_y;

    if(src_data == NULL || para == NULL ||
       m==0 || n==0 || x==0 || y==0) {
        return LSTG_E_INVALIDPARAM;
    }
    return LSTG_OK;
}

/**
 * Function to set up the key matrix.
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 */

void setup_key_matrix(cpt_state *context)
{
    uint8_t count = 0;
    uint32_t rand = 0;
    context->key_matrix = empty_block(context);
    for(uint32_t y = 0; y < context->block_height; y++) {
	for(uint32_t x = 0; x < context->block_width; x++) {
	    if (count == 0) {
		rand = random_next(context->random);
	    }
	    uint32_t element = (rand >> count) & 0x01;
	    gsl_matrix_set(context->key_matrix, y, x, element);
	    count = (count + 1) % 32;
	}
    }
}

/**
 * Function to set up the weight matrix.
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 */
void setup_weight_matrix(cpt_state *context)
{
    uint32_t matrix_elements = context->block_width * context->block_height;
    uint32_t *shufflevector = (uint32_t*) malloc(sizeof(uint32_t) * matrix_elements);
    uint32_t mod = pow(context->r, 2) - 1;

    printf("r:%d, matrix_elements:%d", context->r, matrix_elements);
    context->weight_matrix = empty_block(context);
    for (uint32_t i=0; i < matrix_elements; i++) {
	shufflevector[i] = (i % mod) + 1;
    }
    for (uint32_t i=0; i < matrix_elements; i++) {
	uint32_t index = (random_next(context->random) % (matrix_elements-1)) + 1;
	uint32_t temp = shufflevector[0];
	shufflevector[0] = shufflevector[index];
	shufflevector[index] = temp;
    }
    uint32_t i = 0;
    for (uint32_t y = 0; y < context->block_height; y++) {
	for (uint32_t x = 0; x < context->block_width; x++) {
	    gsl_matrix_set(context->weight_matrix, y, x, shufflevector[i]);
	    i++;
	}
    }
    free(shufflevector);
}

#define CPT_GS_BLOCK_GET 0
#define CPT_GS_BLOCK_SET 1

/**
 * worker function for get_block and set_block.
 */
uint32_t getset_block(cpt_state *context, gsl_matrix *block, uint32_t index, uint8_t mode)
{
    uint32_t width_in_blocks = context->image_width / context->block_width;
    uint32_t x = index % width_in_blocks;
    uint32_t y = index / width_in_blocks;
    uint32_t block_start_row = y * context->block_height;
    uint32_t block_start_col = x * context->block_width;
    for (uint32_t ix = block_start_col; ix < block_start_col + context->block_width;
	 ++ix) {
        for (uint32_t iy = block_start_row;
	     iy < block_start_row + context->block_height; ++iy) {
	    uint32_t row = iy * context->image_width / 8 ;
	    uint32_t column = ix/8;
	    uint8_t bit = 1 << (ix % 8);
	    if(mode == CPT_GS_BLOCK_GET) {
		uint32_t pixel = (bit & *(context->image_data + row + column)) ? 1 : 0;
		gsl_matrix_set(block, iy-block_start_row, ix-block_start_col, pixel);
	    }
	    else {
		uint8_t val = (uint8_t) gsl_matrix_get(block, iy-block_start_row,
						       ix-block_start_col);
		uint8_t *byte = &context->image_data[row + column];
		if (val == 1) {
		    *byte = *byte | bit;
		}
		else {
		    *byte = *byte & (~bit);
		}
	    }
        }
    }
    return LSTG_OK;
}
/**
 * Copies a block of pixels into a matrix.
 * The matrix must be initialized and of the appropriate
 * dimensions. The size of the block is taken from the given
 * context. The coordinates are the position of the block, not pixel
 * coordinates.
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 * @param *target the matrix that recieves the block
 * @param index the position the block in the image
 * @return LSTG_OK on success
 */

void get_block(cpt_state *context, gsl_matrix *target, uint32_t index)
{
    getset_block(context, target, index, CPT_GS_BLOCK_GET);
}

/**
 * Function to copy a matrix back into the image
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 * @param *block the block to be copied
 * @param index the position the block is to be copied into
 * @return LSTG_OK on success
 */
uint32_t set_block(cpt_state *context, gsl_matrix *block, uint32_t index)
{
    return getset_block(context, block, index, CPT_GS_BLOCK_SET);
}

/**
 * Function to extract the lsb-plane of an rgb-image
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 * @param *image the source image
 * @param mode not used yet
 */
void select_bits(cpt_state *context, const rgb_data_t *image, uint32_t mode)
{
    uint32_t length = image->size_x * image->size_y * 3 ; //3 colour channels
    uint8_t chan=0;
    context->image_data = (uint8_t*) malloc(sizeof(uint8_t) * CEIL(length, 8) + 1);
    for (uint32_t i = 0; i < length; ++i) {
	if( !(i%8) ) {

	    context->image_data[i/8] = 0;
	}
	uint8_t bit = 1 & image->data[i/3].rgb[chan];
	context->image_data[i/8] = context->image_data[i/8] | (bit << (i % 8));
	chan++;
	chan=chan % 3;
    }
    context->image_data_length = length;
}

/**
 *Debugging function to print the lsb data in binary form.
 *@param *context a struct that contains everything that describes
 * the current embed/extract operation
 */
void print_image_data_binary(cpt_state *context)
{
    uint8_t r = 0;
    uint32_t length = context->image_data_length;
    for (uint32_t i = 0; i < length/8; ++i) {
	for(uint8_t b = 0; b < 8; ++b) {
	    printf("%d", (context->image_data[i] & 1 << b)? 1:0);
	}
	printf(" ");
	if(++r == 8) {
	    printf("\n");
	    r = 0;
	}

    }
    printf("\n");
}

/**
 * Integrate a set of bits into the lsb-plane of an rgb image
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 * @param *src_image the original image data
 * @param *dst_image the new image data (must be allocated)
 * @param mode not used yet
 */
void integrate_bits(cpt_state *context, const rgb_data_t *src_image,
		    rgb_data_t *dst_image, uint32_t mode)
{
    uint32_t length = context->image_data_length;
    uint8_t chan = 0;
    dst_image->size_x = src_image->size_x;
    dst_image->size_y = src_image->size_y;
    //print_image_data_binary(context);
    for (uint32_t i = 0; i < length; ++i) {
	//printf("%d\n", i);
	if(!chan) {
	    memcpy(&dst_image->data[i/3], &src_image->data[i/3], sizeof(rgb_pixel_t));
	}
	uint8_t lsb = ((context->image_data[i/8] & ((1 << (i % 8))))? 1 : 0);
        uint8_t *dst_pixel = &dst_image->data[i/3].rgb[chan];
        if (lsb) {
            if (!(*dst_pixel % 2)) {
                (*dst_pixel)++;
            }
        } else {
            if (*dst_pixel % 2) {
                (*dst_pixel)--;
            }
        }



        //printf("embedding %d in %d ->", pixel & 1, dst_image->data[i/3].rgb[chan]);
	//dst_image->data[i/3].rgb[chan] = dst_image->data[i/3].rgb[chan] & pixel;
	//printf("%d\n", dst_image->data[i/3].rgb[chan]);
	chan++;
	chan = chan % 3;
    }
}

/**
 * Function to calculate the exclusive OR of two matrices of the same
 * size
 * @param *a the first matrix (modified in place)
 * @param *b the second matrix
 */
void matrix_xor(gsl_matrix *a, const gsl_matrix *b)
{
    //ASSERT(a->size1 == b->size1 && a->size2 == b->size2);
    uint32_t rows = a->size1;
    uint32_t cols = a->size2;
    for (uint32_t y = 0; y < rows; ++y) {
        for (uint32_t x = 0; x < cols; ++x) {
            uint32_t val1 = gsl_matrix_get(a, y, x);
            uint32_t val2 = gsl_matrix_get(b, y, x);
            val1 = (val1 + val2) % 2; //simulate boolean value range
            gsl_matrix_set(a, y, x, val1);
        }
    }
}

/**
 * Function to calculate the sum of the elements of a given matrix.
 * @param *a the target matrix
 * @return the sum of the elements in a
 */
uint32_t matrix_sum(const gsl_matrix * a)
{
    uint32_t sum = 0;
    uint32_t rows = a->size1;
    uint32_t cols = a->size2;
    for (uint32_t y = 0; y < rows; ++y) {
        for (uint32_t x = 0; x < cols; ++x) {
            sum += gsl_matrix_get(a, y, x);
        }
    }
    return sum;
}

/**
 * Function to calculate the weight of a given block
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 * @param *block the target block
 * @return the weight of the given block
 */
uint32_t block_weight(cpt_state *context, gsl_matrix *block)
{
    gsl_matrix *work = empty_block(context);
    gsl_matrix_memcpy(work, block);
    matrix_xor(work, context->key_matrix);
    gsl_matrix_mul_elements(work, context->weight_matrix);
    uint32_t weight = matrix_sum(work) % (uint32_t) pow(2, context->r);
    gsl_matrix_free(work);
    return weight;
}

/**
 * Function that calculates the weight of a block in an image that is
 * referenced by its position.
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 * @param *block_index the position of the block in the image
 * @return the weight of the block at block_index
 */

uint32_t block_weight_by_index(cpt_state *context, uint32_t block_index)
{
    gsl_matrix *block = empty_block(context);
    get_block(context, block, block_index);
    uint32_t weight = block_weight(context, block);
    gsl_matrix_free(block);
    return weight;
}

/**
 * Find the coefficient to select the bits that have to be flipped
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 * @param *weight_adjust_set a matrix with information about the
 * weight of the pixels in a block
 * @param diff the difference between the current and the target
 * weight
 * @param *w0 the first weight component (used as return value)
 * @param *w1 the second weight component (used as return value)
 * @return 0 on success 1 on error
 */
uint32_t find_h(cpt_state *context, gsl_matrix *weight_adjust_set, uint32_t diff, uint32_t *w0,
		 uint32_t *w1)
{
    uint32_t max_h = pow(2, context->r)-1;
    for(uint32_t i=0; i <= max_h; ++i) {
	*w0 = (i * diff) % (max_h + 1);
	*w1 = MOD((-(i-1) * diff), (max_h + 1));
	if(*w0 != 0 && gsl_matrix_get(weight_adjust_set, *w0, 0) == 0)
	    continue;
	if(*w1 != 0 && gsl_matrix_get(weight_adjust_set, *w1, 0) == 0)
	    continue;
	return 0;
    }
    return 1; //Should never reach this
}

/**
 * Function to invert (flip)  a single bit in an image block.
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 * @param *block the target image block. This block is modified in
 * place
 * @param index the position of the bit to be inverted
 */
void flip_bit_in_block(cpt_state *context, gsl_matrix *block, uint32_t index)
{
    uint32_t x = index % context->block_width;
    uint32_t y = index / context->block_width;
    uint8_t bit = gsl_matrix_get(block, y, x);
    bit = bit?0:1;
    gsl_matrix_set(block, y, x, bit);
}

/**
 * Function to select bits that have to be flipped in an image block
 * to embed a message block.
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 * @param *block the image block
 * @param target_weight the message block
 */
uint32_t adjust_block_weight(cpt_state *context, gsl_matrix *block,
			      uint32_t target_weight)
{
    uint32_t max_weight = pow(2,context->r);
    uint32_t block_bits = context->block_width * context->block_height;
    gsl_matrix *weight_adjust_set = gsl_matrix_calloc(max_weight+1, block_bits);
    // The weight adjust set is a max_weight(m) x block_bits(n) matrix that
    //stores the index positions of bits, the swapping of which will
    //alter the block weight by m. The first column denotes the number
    //of elements in each row.
    //TODO: Implement a less wasteful way to store the weight_adjust_set
    gsl_matrix *work = empty_block(context);
    gsl_matrix_memcpy(work, block);
    matrix_xor(work, context->key_matrix);
    uint32_t elements = context->block_width * context->block_height;
    for (uint32_t i=0; i < elements; ++i) {  //Fill weight_adjust_set
	uint32_t x = i % context->block_width;
	uint32_t y = i / context->block_width;
	uint32_t weight_bit = gsl_matrix_get(context->weight_matrix, y, x);
	uint32_t key_block_bit = gsl_matrix_get(work, y, x);
	uint32_t set_index = 0;
	if(key_block_bit) {
	    set_index = pow(2, context->r) - weight_bit;
	} else {
	    set_index = weight_bit;
	}
	uint32_t set_position = gsl_matrix_get(weight_adjust_set, set_index, 0);
	set_position++;
	//printf("block element %d: diff %d, pos %d\n", i, set_index, set_position);
	gsl_matrix_set(weight_adjust_set, set_index, 0, set_position);
	gsl_matrix_set(weight_adjust_set, set_index, set_position, i);
    }
    uint32_t weight = block_weight(context, block);
    uint32_t diff = MOD((target_weight - weight),(uint32_t)(pow(2, context->r)));
    uint32_t w[2];
    if (diff != 0) {
	if (find_h(context, weight_adjust_set, diff, &w[0], &w[1])) {
	    printf("FATAL: could not find a pair of pixels to flip\n");
	    printf("tried to embed a diff of %d\n", diff);
	    matrix_print(weight_adjust_set);
	} else {
	    printf("embedding %d in a block with weight %d (diff %d)\n",
		   target_weight, weight, diff);
	    for(uint32_t j=0; j<2; ++j) {
		if(w[j] != 0) {
		    uint32_t choices = gsl_matrix_get(weight_adjust_set, w[j], 0);
		    uint32_t choice = (random_next(context->random) % choices) + 1;
		    uint32_t flipindex = gsl_matrix_get(weight_adjust_set, w[j], choice);
		    printf("flipping bit %d\n", flipindex);
		    flip_bit_in_block(context, block, flipindex);
		}
	    }
	}
    }
    uint32_t newweight = block_weight(context, block);
    if (newweight != target_weight) {
	printf("============================\n ERROR: Blockweight should be %d, but is %d\n",
	       target_weight, newweight);
    }
    gsl_matrix_free(weight_adjust_set);
    gsl_matrix_free(work);
    return LSTG_OK;
}

/**
 * Function to emded a single message data block into the image data.
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 * @param block the index of the message/data block to be embedded
 * @param *message the message to be embedded into the image
 * @return 0 or errorcode on failure
 */
uint32_t cpt_embed_block(cpt_state *context, uint32_t block, uint8_t *message)
{
    uint32_t msg_block = get_msg_block(message, block, context->r);
    gsl_matrix *workblock = empty_block(context);
    get_block(context, workblock, block);
    adjust_block_weight(context, workblock, msg_block);
    set_block(context, workblock, block);
    gsl_matrix_free(workblock);
    return LSTG_OK;
}




/**
* Embeds a message in a color-image using CPT.
* It uses two key matrices to embed a secret message into the lsbs of an rgb-bitmap
*
* @param *src_data a struct for one-bit-color-data to provide the original image
* @param *stego_data a struct for one-bit-color-data to return the steganogram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for CPT, including passphrase
* @return an errorcode or 0 if success
*/
uint32_t cpt_embed(const rgb_data_t *src_data, rgb_data_t *stego_data,
                  const uint8_t *message, const int32_t msglen, const cpt_parameter *para)
{
    if(cpt_check_params(src_data, para)!=LSTG_OK) {
        FAIL(LSTG_E_INVALIDPARAM);
    }
    //Setup
    cpt_state context;
    uint32_t msgbytes = msglen;
    context.block_width = para->block_width;
    context.block_height = para->block_height;
    context.image_width = src_data->size_x;
    context.image_height = src_data->size_y;
    context.r = (uint32_t)floor(log2(para->block_width * para->block_height - 1));
    select_bits(&context, src_data, 0);
    //print_image_data_binary(&context);
    random_init(&context.random, para->password, para->pwlen);
    setup_key_matrix(&context);
    setup_weight_matrix(&context);
    matrix_print(context.key_matrix);
    matrix_print(context.weight_matrix);
    uint8_t msglen_size = 4;
    uint8_t *message_with_length = (uint8_t*)calloc(msgbytes + msglen_size, 1);
    *((uint32_t*)message_with_length) = msglen;
    memcpy(message_with_length + msglen_size, message, msgbytes);
    //Embed
    uint32_t blocks = CEIL((msglen*8 + msglen_size*8), context.r);
    for (uint32_t block = 0; block < blocks; ++block) {
	cpt_embed_block(&context, block, message_with_length);
    }

    //Write back
    stego_data->data = (rgb_pixel_t*)malloc(sizeof(rgb_pixel_t)*context.image_width * context.image_height);
    stego_data->size_x = context.image_width;
    stego_data->size_y = context.image_height;
    integrate_bits(&context, src_data, stego_data, 0);

    //Cleanup
    gsl_matrix_free(context.weight_matrix);
    gsl_matrix_free(context.key_matrix);
    free(message_with_length);
    random_cleanup(context.random);
    free(context.image_data);

    return LSTG_OK;
}

/**
 * Internal function that reads the first 32 bits embedded in a
 * steganographic image.
 *
 * @param *context a struct that contains everything that describes
 * the current embed/extract operation
 * @return the message length
 */
uint32_t get_message_length(cpt_state *context)
{
    uint32_t nblocks = CEIL(32, context->r);
    uint32_t messagelength=0;
    uint32_t last_block_mask = ~0;
    uint32_t bits_to_mask = 0;
    if((bits_to_mask = (32 % context->r))) {
	// set the last [bits_to_mask] bits to zero
	last_block_mask = last_block_mask ^ ((1<<(bits_to_mask))-1);
    }
    for(uint32_t i = 0; i< nblocks; i++) {
	uint32_t msgbits = block_weight_by_index(context, i);
	if(i == nblocks-1) { //last block
	    msgbits = msgbits & last_block_mask;
	}
	set_msg_block((uint8_t*)&messagelength, msgbits, i, context->r);
    }
    return messagelength;
}


/**
* Extracts a CPT-embedded-message from a steganogram.
* It needs the same two matrices used for embedding to extract a secret message
* from an rgb steganographic bitmap
* @param *stego_data a struct for one-bit-color-data containing the stegonagram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for CPT including passphrase
* @return an errorcode or 0 if success
*/
uint32_t cpt_extract(const rgb_data_t *stego_data, uint8_t **message,
                    uint32_t *msglen, const cpt_parameter *para)
{
    //    printf("width:%d, height:%d, password: %s\n", para->size_x, para->size_y,
    //       para->passwd);
    if(cpt_check_params(stego_data, para)!=LSTG_OK) {
        FAIL(LSTG_E_INVALIDPARAM);
    }
    cpt_state context;
    context.block_width = para->block_width;
    context.block_height = para->block_height;
    context.image_width = stego_data->size_x;
    context.image_height = stego_data->size_y;
    prng_state *random;
    random_init(&random, para->password, para->pwlen);
    context.random = random;
    context.r = (uint32_t)floor(log2(para->block_width * para->block_height - 1));
    select_bits(&context, stego_data, 0);
    setup_key_matrix(&context);
    setup_weight_matrix(&context);
    matrix_print(context.key_matrix);
    matrix_print(context.weight_matrix);

    //print_image_data_binary(&context);
    *msglen = get_message_length(&context);
    // Sanity check
    uint32_t maxcap = 0;
    cpt_check_capacity(stego_data, para, &maxcap);
    if (*msglen > maxcap) {
	//No sane capacity extracted, no secret data or wrong params
	FAIL(LSTG_E_INVALIDPARAM);
    }

    *message = (uint8_t*)calloc(*msglen+1, 1);
    uint8_t *msgtemp = (uint8_t*)calloc((*msglen)+5, 1);
    uint32_t msg_blocks = CEIL(*msglen * 8, context.r);
    uint8_t msglen_skip = 32 / context.r;
    printf("extracting %d blocks starting at position %d\n", msg_blocks, msglen_skip);
    for (uint32_t i = 0; i <= msg_blocks+msglen_skip; i++) {
	uint32_t msgbits = block_weight_by_index(&context, i);
	set_msg_block(msgtemp, msgbits, i, context.r);
    }
    memcpy(*message, msgtemp+4, *msglen);
    free(msgtemp);
    random_cleanup(random);
    free(context.image_data);
    gsl_matrix_free(context.weight_matrix);
    gsl_matrix_free(context.key_matrix);
    return LSTG_OK;
}

/**
* Returns the length of an embedded message
* ...
* @param *stego_data a struct for one-bit-color-data containing the steganogram
* @param *para additional parameters for CPT including passphrase
* @return length of embedded message
*/
uint32_t cpt_get_message_length(const rgb_data_t *stego_data,
                               const cpt_parameter *para,
			       uint32_t *msglen)
{
    if(cpt_check_params(stego_data, para)!=LSTG_OK) {
        FAIL(LSTG_E_INVALIDPARAM);
    }

    cpt_state context;
    context.block_width = para->block_width;
    context.block_height = para->block_height;
    context.image_width = stego_data->size_x;
    context.image_height = stego_data->size_y;
    select_bits(&context, stego_data, 0);
    prng_state *random = ALLOC(prng_state);
    context.random = random;
    random_init(&random, para->password, para->pwlen);
    setup_key_matrix(&context);
    setup_weight_matrix(&context);
    context.r = (uint32_t)floor(log2(para->block_width * para->block_height- 1));
    *msglen = get_message_length(&context);
    return LSTG_OK;
}

/**
* Returns the capacity of a cover image
*
* @param *src_data a struct for one-bit-color-data to provide the original image
* @param *para additional parameters for CPT including passphrase
* @return capacity of cover image
*/
uint32_t cpt_check_capacity(const rgb_data_t *src_data,
                           const cpt_parameter *para,
                           uint32_t *capacity)
{
    uint32_t m = para->block_width;
    uint32_t n = para->block_height;
    uint32_t x = src_data->size_x;
    uint32_t y = src_data->size_y;

    if(cpt_check_params(src_data, para)!=LSTG_OK) {
        FAIL(LSTG_E_INVALIDPARAM);
    }
    *capacity = (uint32_t)((double)((double)(x*y)/(double)(m*n)) * log2( (double)(m*n + 1)));
    return LSTG_OK;
}
