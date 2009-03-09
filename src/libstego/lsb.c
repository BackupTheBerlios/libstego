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
 
 
 
#include "libstego/lsb.h"

// DEBUG
#include <stdio.h>

/* Local Prototypes */
uint32_t lsb_select_continuous_bytes(
        const lsb_data_t *cover,
        uint8_t **sel_bytes,
        const uint32_t num_bytes_selected,
        const lsb_parameter_t *param);

uint32_t lsb_select_random_bytes(
        const lsb_data_t *cover,
        uint8_t **bytes,
        const uint32_t num_bytes_select,
        const lsb_parameter_t *param);


/**
 * Selects coverbytes continuously one after another from an array of bytes,
 * starting at the first byte in the array.
 *
 * @param cover A pointer to a lsb_data_t structure, containing the cover data.
 * @param sel_bytes Pointer to an array which will contain the pointers to the
 * selected bytes after the function returns.
 * @param num_bytes_selected The number of bytes to select.
 * @param param Unused.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t lsb_select_continuous_bytes(
        const lsb_data_t *cover,
        uint8_t **sel_bytes,
        const uint32_t num_bytes_selected,
        const lsb_parameter_t *param) {
    uint32_t i = 0;

    // DEBUG
    printf("lsb_select_continuous_bytes()\n");
    fflush(stdout);

    // check if there are enough bytes in the image to fulfill the request
    if (cover->size < num_bytes_selected) {
        FAIL(LSTG_E_INSUFFCAP);
    }

    // fill sel_bytes with pointers to the data
    for (i = 0; i < num_bytes_selected; ++i) {
        sel_bytes[i] = &cover->data[i];
    }

    // DEBUG
    printf("~lsb_select_continuous_bytes()\n");
    fflush(stdout);

    // success
    return LSTG_OK;
}

/**
 * Randomly selects a number of bytes from an array of bytes.
 *
 * @param data Pointer to a lsb_data_t structure, containing the cover bytes.
 * @param sel_bytes Pointer to an array that will contain the pointers to the
 * selected bytes after the function returns.
 * @param num_bytes_select The number of bytes that should be selected.
 * @param param A pointer to a lsb_parameter_t structure, containing the
 * password that is used as a seed for the random number generator.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t lsb_select_random_bytes(
        const lsb_data_t *cover,
        uint8_t **sel_bytes,
        const uint32_t num_bytes_select,
        const lsb_parameter_t *param) {
    uint32_t i = 0, last_selected_byte = 0;;
    uint32_t rnd = 0;
    uint8_t *buffer = 0;
    uint8_t **tmp_copy = 0;
    prng_state *prng = 0;

    // DEBUG
    printf("lsb_select_random_bytes()\n");
    fflush(stdout);

    if (cover->size < num_bytes_select) {
        FAIL(LSTG_E_INSUFFCAP);
    }

    // make a temporary array with pointers to all the bytes in the cover
    tmp_copy = (uint8_t**)malloc(sizeof(uint8_t*) * cover->size);
    if (tmp_copy == 0) {
        FAIL(LSTG_E_MALLOC);
    }

    for (i = 0; i < cover->size; ++i) {
        tmp_copy[i] = &cover->data[i];
    }

    // create random order
    random_init(&prng, param->password, param->pwlen);

    for (i = 0; i < cover->size; ++i) {
        rnd = random_next(prng) % (cover->size - i);
        buffer = tmp_copy[i];
        tmp_copy[i] = tmp_copy[rnd];
        tmp_copy[rnd] = buffer;
    }
    memcpy(sel_bytes, tmp_copy, sizeof(uint8_t*) * num_bytes_select);

    random_cleanup(prng);
    // free pointers (NOT the actual data!)
    SAFE_DELETE(tmp_copy);

    // DEBUG
    printf("~lsb_select_random_bytes()\n");
    fflush(stdout);

    return LSTG_OK;
}

/**
 * Embeds a message in the least significant bit of an array of bytes.
 * Note that the order in which the bits of the message will be embedded into
 * the bytes of the cover can be configured by the lsb param structure.
 *
 * @param src Pointer to a lsb_data_t structure, containing the original cover
 * bytes.
 * @param stego Pointer to a lsb_data_t structure, which will contain the
 * modified cover bytes (steganogram) after the function returns.
 * @param message The message that is to be embedded.
 * @param msglen The length of the message, in bytes.
 * @param param A set of parameters for this algorithm.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t lsb_embed(
        const lsb_data_t *src,
        lsb_data_t *stego,
        const uint8_t *message,
        const uint32_t msglen,
        const lsb_parameter_t *param) {
    uint32_t i = 0, n = 0, num_bits = 0;
    uint8_t **bytes = 0;

    // DEBUG
    printf("lsb_embed()\n");
    fflush(stdout);

    // length may not be bigger than one eigth of the maximum of uint32_t, else
    // we risk a buffer overflow and resulting madnesses when we multiply it
    // with eight later on
    if (msglen > (0xFFFFFFFF - 32)/ 8) {
        FAIL(LSTG_E_INSUFFCAP);
    }

    // check the size of the cover, if the message will fit in
    num_bits = msglen * 8 + 32;

    if (src->size < num_bits) {
        FAIL(LSTG_E_INSUFFCAP);
    }

    // copy the unmodified source data into the stego data
    stego->size = src->size;
    memcpy(stego->data, src->data, sizeof(uint8_t) * src->size);

    // prepare array of pointers
    bytes = (uint8_t**)malloc(sizeof(uint8_t*) * num_bits);
    if (bytes == NULL) {
        FAIL(LSTG_E_MALLOC);
    }

    // check select mode
    switch (param->select_mode) {
        // select a subset of the given bytes
        case LSB_SELECT_CONTINUOUS:
            if (lsb_select_continuous_bytes(stego, bytes, num_bits, param)
                    != LSTG_OK) {
                SAFE_DELETE(bytes);
                // error code already set
                return LSTG_ERROR;
            }
            break;

        // select a random subsect of the given bytes
        case LSB_SELECT_RANDOM:
            if (lsb_select_random_bytes(stego, bytes, num_bits, param)
                    != LSTG_OK) {
                SAFE_DELETE(bytes);
                // error code already set
                return LSTG_ERROR;
            }
            break;

        default:
            SAFE_DELETE(bytes);
            FAIL(LSTG_E_INVALIDPARAM);
            break;
    };

    // DEBUG
    printf("lsb_embed() 0010\n");
    printf("lsb_embed() param->use_msb %d\n", param->use_msb);
    fflush(stdout);

    if (param->use_msb == 0) {
        // Embed the length of the message, so we can extract it later.
        // If we don't do this, the length of the message can not be determined on
        // extraction.
        for (i = 0; i < 32; ++i) {
        	// set the lsb to zero, leaving all other bits intact
        	*bytes[i] &= 0xFE;
        	// get the i'th bit of the length, and write it into the LSB
            *bytes[i] |= (msglen & (1 << i)) >> i;
        }

        // embed the message, start at 33rd byte (first 32: length of message)
        for (i = 0, n = msglen * 8; i < n; ++i) {
        	// set the lsb to zero, leaving all other bits intact
        	*bytes[i+32] &= 0xFE;
            *bytes[i+32] |= (message[i/8] & (1 << i%8)) >> (i%8);
        }
    } else {
        // ATTENTION! FOR DEMONSTRATION ONLY! COMPLETELY USELESS FOR STEGANOGRAPHY!
        // Also: Beware, butt ugly code ahead!

        // Embed the length of the message, so we can extract it later.
        // If we don't do this, the length of the message can not be determined on
        // extraction.
        for (i = 0; i < 32; ++i) {
        	// set the msb to zero, leaving all other bits intact
        	*bytes[i] &= 0x7f;
        	// get the i'th bit of the length, and write it into the LSB
            *bytes[i] |= ((msglen & (1 << i)) >> i) << 7;
        }

        // embed the message, start at 33rd byte (first 32: length of message)
        for (i = 0, n = msglen * 8; i < n; ++i) {
        	// set the msb to zero, leaving all other bits intact
        	*bytes[i+32] &= 0x7f;
            *bytes[i+32] |= ((message[i/8] & (1 << i%8)) >> (i%8)) << 7;
        }
    }

    SAFE_DELETE(bytes);

    // DEBUG
    printf("~lsb_embed()\n");
    fflush(stdout);

    // success
    return LSTG_OK;
}


/**
 * Reads the length of an embedded message from the given coverbytes. If there
 * is no embedded message, there will still be a result, since the length is
 * assumed to be embedded in the LSBs of the first four bytes. It is in the
 * responsibility of the caller to ensure there really is a message embedded in
 * the bytes given to this function.
 *
 * @param data Pointer to a lsb_data_t structure containing the cover data.
 * @param msglen Pointer to a uint32_t which will hold the message length, in
 * Bits, after the function returns.
 * @param param Pointer to a lsb_parameter_t structure, containing the
 * parameters for this algorithm.
 * @see lsb_parameter_t
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t lsb_get_message_length(
        const lsb_data_t *data,
        uint32_t *msglen,
        const lsb_parameter_t *param) {
    uint8_t i = 0;
    uint32_t num_bits = 0;
    uint8_t **bytes = 0;

	*msglen = 0;

    num_bits = 32;

    bytes = (uint8_t**)malloc(sizeof(uint8_t*) * num_bits);
    if (bytes == NULL) {
        FAIL(LSTG_E_MALLOC);
    }

    switch (param->select_mode) {
        // select a subset of the given bytes
        case LSB_SELECT_CONTINUOUS:
            if (lsb_select_continuous_bytes(data, bytes, num_bits, param)
                    != LSTG_OK) {
                SAFE_DELETE(bytes);
                // error code already set
                return LSTG_ERROR;
            }
            break;

        // select a random subsect of the given bytes
        case LSB_SELECT_RANDOM:
            if (lsb_select_random_bytes(data, bytes, num_bits, param)
                    != LSTG_OK) {
                SAFE_DELETE(bytes);
                // error code already set
                return LSTG_ERROR;
            }
            break;

        default:
            SAFE_DELETE(bytes);
            FAIL(LSTG_E_INVALIDPARAM);
            break;
    };

    if (!param->use_msb) {
        for ( i = 0; i < num_bits; ++i ) {
            *msglen |= (*bytes[i] & 1) << i;
        }
    } else {
        // ATTENTION! FOR DEMONSTRATION ONLY! COMPLETELY USELESS FOR STEGANOGRAPHY!
        // Also: Beware, butt ugly code ahead!
        for ( i = 0; i < num_bits; ++i ) {
            *msglen |= ((*bytes[i] & 0x80) >> 7) << i;
        }
    }

    free(bytes);

    return LSTG_OK;
}


/**
 * Extracts a message from the least significant bits of an array of bytes.
 *
 * @param data Pointer to a lsb_data_t structure containing the cover data.
 * @param message A Pointer that will point to the extracted message after this
 * function returns.
 * @param msglen Pointer to a uint32_t which will hold the message length after
 * the function returns.
 * @param param A set of parameters for this algorithm.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t lsb_extract(
        const lsb_data_t *data,
        uint8_t **message,
        uint32_t *msglen,
        const lsb_parameter_t *param) {
    uint32_t i = 0, n = 0, num_bits = 0;
    uint32_t length = 0;
    uint8_t **bytes = 0;

    // get message length
    if (lsb_get_message_length(data, &length, param) == LSTG_ERROR) {
    	// don't set lstg_errno, this has already been done
    	return LSTG_ERROR;
    }

    // length may not be bigger than one eigth of the maximum of uint32_t, else
    // we risk a buffer overflow and resulting madnesses when we multiply it
    // with eight later on
    if (length > (0xFFFFFFFF - 32)/ 8) {
        FAIL(LSTG_E_INVALIDFILE);
    }

    num_bits = length * 8 + 32;

    // allocate enough space for the message
    *message = (uint8_t*)malloc(sizeof(uint8_t) * length);
    if (*message == NULL) {
        FAIL(LSTG_E_MALLOC);
    }

    bytes = (uint8_t**)malloc(sizeof(uint8_t*) * num_bits);
    if (bytes == NULL) {
        SAFE_DELETE(*message);
        FAIL(LSTG_E_MALLOC);
    }

    switch (param->select_mode) {
        // select a subset of the given bytes
        case LSB_SELECT_CONTINUOUS:
            if (lsb_select_continuous_bytes(data, bytes, num_bits, param)
                    != LSTG_OK) {
                SAFE_DELETE(bytes);
                // error code already set
                return LSTG_ERROR;
            }
            break;

        // select a random subsect of the given bytes
        case LSB_SELECT_RANDOM:
            if (lsb_select_random_bytes(data, bytes, num_bits, param)
                    != LSTG_OK) {
                SAFE_DELETE(bytes);
                // error code already set
                return LSTG_ERROR;
            }
            break;

        default:
            SAFE_DELETE(bytes);
            FAIL(LSTG_E_INVALIDPARAM);
            break;
    };

    for (i = 0; i < length; ++i) {
        (*message)[i] = 0;
    }

    if (!param->use_msb) {
        for ( i = 0, n = length * 8; i < n; ++i ) {
            (*message)[i/8] |= (*bytes[i+32] & 1) << i%8;
        }
    } else {
        // ATTENTION! FOR DEMONSTRATION ONLY! COMPLETELY USELESS FOR STEGANOGRAPHY!
        // Also: Beware, butt ugly code ahead!
        for ( i = 0, n = length * 8; i < n; ++i ) {
            (*message)[i/8] |= ((*bytes[i+32] & 0x80) >> 7) << i%8;
        }
    }

    if (msglen) {
        *msglen = length;
    }

    SAFE_DELETE(bytes);

    // success
    return LSTG_OK;
}

/**
 * Get the maximum capacity of the cover with the given parameters.
 *
 * @param data Pointer to a lsb_data_t structure containing the cover data.
 * @param capacity The maximum capacity, in bits.
 * @param param The parameters for this algorithm.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t lsb_check_capacity(
        const lsb_data_t *data,
        uint32_t *capacity,
        const lsb_parameter_t *param) {
	*capacity = data->size - 32;
	return LSTG_OK;
}



/*  FORMAT WRAPPER FUNCTIONS  */

uint32_t lsb_convert_png(
        const rgb_data_t *data,
        uint8_t ***bytes,
        uint32_t *num_bytes) {

    uint32_t row, col, byte_idx = 0;

    *num_bytes = data->size_x * data->size_y * 3;

    *bytes = 0;
    *bytes = (uint8_t**)malloc(sizeof(uint8_t*) * (*num_bytes));
    if (*bytes == NULL) {
        FAIL(LSTG_E_MALLOC);
    }

    for (row = 0; row < data->size_y; ++row) {
        for (col = 0; col < data->size_x; ++col) {
            (*bytes)[byte_idx++] = &(data->data[row * data->size_x + col].rgb[0]);
            (*bytes)[byte_idx++] = &(data->data[row * data->size_x + col].rgb[1]);
            (*bytes)[byte_idx++] = &(data->data[row * data->size_x + col].rgb[2]);
        }
    }

    return LSTG_OK;

}


uint32_t lsb_cleanup_converted_data(
        uint8_t **bytes) {
    // DEBUG
    printf("lsb_cleanup_converted_data()\n");
    fflush(stdout);

    free(bytes);

    // DEBUG
    printf("~lsb_cleanup_converted_data()\n");
    fflush(stdout);
}


/*  INDIRECT FUNCTIONS  */

/**
 * Gets the embedded messages length. Identical to lsb_get_message_length, only
 * that this function works on bytes pointed to by an array of pointers, instead
 * of working on bytes directly.
 *
 * @param bytes An array of pointers to uint8_t's, that the message was embedded
 * into previously.
 * @param num_bytes The size of the array.
 * @param msglen Pointer to a uint32_t which will contain the messages length
 * after the function returns.
 * @param param A set of parameters for the LSB algorithm.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t lsb_get_message_length_indirect(
        uint8_t **bytes,
        const uint32_t num_bytes,
        uint32_t *msglen,
        const lsb_parameter_t *param) {
    lsb_data_t cover;
    uint32_t i = 0;

    cover.size = num_bytes;
    cover.data = (uint8_t*)malloc(sizeof(uint8_t) * num_bytes);
    if (cover.data == NULL) {
        FAIL(LSTG_E_MALLOC);
    }

    // copy the bytes 'bytes' points to into the cover
    for (i = 0; i < num_bytes; ++i) {
        cover.data[i] = *bytes[i];
    }

    // get message length normally
    lsb_get_message_length(&cover, msglen, param);

    SAFE_DELETE(cover.data);

    return LSTG_OK;
}


/**
 * Embeds data into bytes pointed to by an array of pointers. Useful when the
 * cover data is not a single block, or only specific parts of the cover should
 * be used. For example, a 16-Bit WAVE file is used as a cover, and only the
 * lower byte of every sample should be used by the LSB algorithm. The caller of
 * this funtion prepares an array of pointers, where each pointer points to one
 * lower byte of a sample. After this function returns, the message will be
 * embedded into the least significant bit of every byte the array pointed to.
 *
 * @param bytes An array of pointers to uint8_t's, that the message will be
 * embedded into.
 * @param num_bytes The size of the array.
 * @param message The message that should be embedded.
 * @param msglen The size of the message, in bytes.
 * @param param A set of parameters for the LSB algorithm.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t lsb_embed_indirect(
        uint8_t **bytes,
        const uint32_t num_bytes,
        const uint8_t *message,
        const uint32_t msglen,
        const lsb_parameter_t *param) {
    lsb_data_t cover, stego;
    uint32_t i = 0;

    // DEBUG
    printf("lsb_embed_indirect()\n");
    fflush(stdout);

    cover.size = num_bytes;
    cover.data = (uint8_t*)malloc(sizeof(uint8_t) * num_bytes);
    if (cover.data == NULL) {
        FAIL(LSTG_E_MALLOC);
    }
    cover.size = num_bytes;
    stego.data = (uint8_t*)malloc(sizeof(uint8_t) * num_bytes);
    if (stego.data == NULL) {
        SAFE_DELETE(cover.data);
        FAIL(LSTG_E_MALLOC);
    }

    // copy the bytes 'bytes' points to into the cover
    for (i = 0; i < num_bytes; ++i) {
        cover.data[i] = *bytes[i];
    }

    // embed normally
    if (lsb_embed(&cover, &stego, message, msglen, param) != LSTG_OK) {
        SAFE_DELETE(cover.data);
        SAFE_DELETE(stego.data);
        // error code already set
        return LSTG_ERROR;
    }

    // DEBUG
    printf("lsb_embed_indirect() 0010\n");
    fflush(stdout);

    // 'write back' the stego bytes
    for (i = 0; i < num_bytes; ++i) {
        *bytes[i] = stego.data[i];
    }

    // DEBUG
    printf("lsb_embed_indirect() 0020\n");
    fflush(stdout);

    SAFE_DELETE(cover.data);
    SAFE_DELETE(stego.data);

    // DEBUG
    printf("~lsb_embed_indirect()\n");
    fflush(stdout);

    return LSTG_OK;
}

/**
 * Extracts a message from the least significant bits of an array of bytes.
 * Identical to lsb_extract, only that this version works on bytes pointed to by
 * an array of pointers, instead of on the bytes directly.
 *
 * @param bytes An array of pointers to uint8_t's, that the message will be
 * extracted from.
 * @param num_bytes The size of the array.
 * @param message A Pointer that will point to the extracted message after this
 * function returns.
 * @param msglen Pointer to a uint32_t which will hold the message length after
 * the function returns.
 * @param param A set of parameters for this algorithm.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t lsb_extract_indirect(
        uint8_t **bytes,
        const uint32_t num_bytes,
        uint8_t **message,
        uint32_t *msglen,
        const lsb_parameter_t *param) {
    lsb_data_t cover;
    uint32_t i = 0;

    cover.size = num_bytes;
    cover.data = (uint8_t*)malloc(sizeof(uint8_t) * num_bytes);
    if (cover.data == NULL) {
        FAIL(LSTG_E_MALLOC);
    }

    // copy the bytes 'bytes' points to into the cover
    for (i = 0; i < num_bytes; ++i) {
        cover.data[i] = *bytes[i];
    }

    // extract normally
    if (lsb_extract(&cover, message, msglen, param) != LSTG_OK) {
        // don't set error code
        return LSTG_ERROR;
    }

    SAFE_DELETE(cover.data);

    return LSTG_OK;
}

/**
 * Get the maximum capacity of the cover with the given parameters.
 *
 * @param bytes An array of pointers to uint8_t's, that the message will be
 * embedded into.
 * @param num_bytes The size of the array.
 * @param capacity Pointer to an uint32_t, will containt the maximum capacity,
 * in bits, after the function returns.
 * @param param The parameters for this algorithm.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t lsb_check_capacity_indirect(
        uint8_t **bytes,
        const uint32_t num_bytes,
        uint32_t *capacity,
        const lsb_parameter_t *param) {
    *capacity = num_bytes;
    if (*capacity > 32)
        *capacity -= 32;
    else
        *capacity = 0;
    return LSTG_OK;
}
