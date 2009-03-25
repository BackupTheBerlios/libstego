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
 *  Author: Jan C. Busch <jan.c.busch@uni-oldenburg.de>
 *
 *  For more info visit <http://parsys.informatik.uni-oldenburg.de/~stego/>
 */



#include "libstego/svg_steg.h"

/* Local structs */
typedef struct svg_llist {
    uint8_t *str;
    struct svg_llist *next;
} svg_llist_t;

typedef svg_llist_t* svg_llist_head_t;

enum svg_wrap_mode {
    SVG_WRAP_EMBED,
    SVG_WRAP_EXTRACT,
    SVG_WRAP_MSGLEN
};

/* Local Prototypes */
uint32_t split_matrix(uint8_t *matrix, svg_llist_head_t *head);
uint32_t join_matrix(uint8_t **matrix, svg_llist_head_t head);
uint32_t free_list(svg_llist_head_t head);

uint32_t svg_wrap_lsb(
        const svg_data_t *data,
        uint8_t **message,
        uint32_t *msglen,
        const svg_parameter_t *param,
        enum svg_wrap_mode wrap_mode);


/**
 * Wrapper function for using LSB with SVG data. Depending on the wrap_mode
 * parameter, this function is equivalent to a call to lsb_embed, lsb_extract or
 * lsb_get_message_length.
 *
 * @param data The SVG data which should be used for embedding or extracting.
 * @param message Pointer to the message, which will be embedded or extracted.
 * @param msglen Pointer to the message length.
 * @parem param Pointer to a set of parameters for the SVG algorithm.
 * @param wrap_mode The function this function should wrap, one of
 * SVG_WRAP_EMBED, SVG_WRAP_EXTRACT or SVG_WRAP_MSGLEN.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t svg_wrap_lsb(
        const svg_data_t *data,
        uint8_t **message,
        uint32_t *msglen,
        const svg_parameter_t *param,
        enum svg_wrap_mode wrap_mode) {

    uint32_t capacity = 0;
    uint32_t bytes_selected = 0;
    uint32_t bit_length = 0;
    uint32_t i = 0, dec_len = 0, k = 0;
    uint8_t *start = 0, *end = 0;
    uint8_t return_code = LSTG_OK;
    svg_llist_head_t *matrices;
    svg_llist_t *current;
    lsb_parameter_t lsb_param;
    uint8_t **bytes = 0;

    // length may not be bigger than one eighth of the maximum of uint32_t, else
    // we risk a buffer overflow and resulting madnesses when we multiply it
    // with eight later on
    if (*msglen > (0xFFFFFFFF - 32) / 8) {
        FAIL(LSTG_E_INSUFFCAP);
    }

    bit_length = *msglen * 8 + 32;

    // we need a password
    if (param->pwlen == 0) {
        FAIL(LSTG_E_INVALIDPARAM);
    }

    svg_check_capacity(data, &capacity, param);
    // check_capacity compensates for the 32 bit message length -- we have to correct for that
    capacity += 32;

    if (wrap_mode == SVG_WRAP_EMBED) {
        // check for capacity
        if (capacity < bit_length) {
            FAIL(LSTG_E_INSUFFCAP);
        }
    }

    // create a buffer to hold split matrices
    matrices = (svg_llist_head_t*)malloc(sizeof(svg_llist_head_t) * data->num_attribs);
    if (matrices == NULL) {
        lstg_errno = LSTG_E_MALLOC;
        return_code = LSTG_ERROR;
        goto svg_wrap_cleanup;
    }

    // create array of pointers, will point to bytes which can be embedded into
    bytes = (uint8_t**)malloc(sizeof(uint8_t*) * capacity);
    if (bytes == NULL) {
        lstg_errno = LSTG_E_MALLOC;
        return_code = LSTG_ERROR;
        goto svg_wrap_cleanup;
    }

    for (i = 0; i < data->num_attribs; ++i) {
        // initialize pointer to zero
        matrices[i] = 0;

        // we only embed in transform matrices
        if (strncmp("matrix(", (char*)data->attributes[i].data, 7) == 0) {
            if (split_matrix(data->attributes[i].data, &matrices[i]) != LSTG_OK) {
                // error code alredy set
                return_code = LSTG_ERROR;
                goto svg_wrap_cleanup;
            }
            for (current = matrices[i]; current != NULL; current = current->next) {
                // get decimal part of number (between '.' and the exponent (if any)

                // find decimal point
                start = (uint8_t*)strchr((char*)current->str, '.');
                if (start == NULL) {
                    // there was no '.' in this number, skip it
                    continue;
                }
                // find exponent (if any)
                end = (uint8_t*)strchr((char*)current->str, 'e');
                if (end == NULL) {
                    // there was no exponent, set end to end of string
                    end = current->str + strlen((char*)current->str);
                }
                // calculate length of decimal part
                dec_len = end - start - 1;

                // set pointers to usable bytes
                for (k = param->first_embed_digit; k < dec_len; ++k) {
                    bytes[bytes_selected++] = &start[k];
                }
            }
        }
    }

    // prepare lsb_parameter_t
    lsb_param.password = (uint8_t*)malloc(sizeof(uint8_t) * (param->pwlen + 1));
    if (lsb_param.password == NULL) {
        lstg_errno = LSTG_E_MALLOC;
        return_code = LSTG_ERROR;
        goto svg_wrap_cleanup;
    }
    memcpy(lsb_param.password, param->password, sizeof(uint8_t) * (param->pwlen + 1));
    lsb_param.pwlen = param->pwlen;
    lsb_param.select_mode = LSB_SELECT_RANDOM;
    lsb_param.use_msb = 0;

    switch (wrap_mode) {
        case SVG_WRAP_EMBED:
            // embed using LSB
            if (lsb_embed_indirect(bytes, bytes_selected, *message, *msglen, &lsb_param)
                    != LSTG_OK) {
                // error code alredy set
                return_code = LSTG_ERROR;
                goto svg_wrap_cleanup;
            }

            // re-assemble the changed matrices
            for (i = 0; i < data->num_attribs; ++i ) {
                if (matrices[i]) {
                    // free space, will be replaced by new matrix
                    SAFE_DELETE(data->attributes[i].data);
                    join_matrix(&data->attributes[i].data, matrices[i]);
                }
            }
            break;

        case SVG_WRAP_EXTRACT:
            // extract using LSB
            if (lsb_extract_indirect(bytes, bytes_selected, message, msglen, &lsb_param)
                    != LSTG_OK) {
                // error code alredy set
                return_code = LSTG_ERROR;
                goto svg_wrap_cleanup;
            }
            break;

        case SVG_WRAP_MSGLEN:
            // get message length using LSB
            if (lsb_get_message_length_indirect(bytes, bytes_selected, msglen, &lsb_param)
                    != LSTG_OK) {
                // error code alredy set
                return_code = LSTG_ERROR;
                goto svg_wrap_cleanup;
            }
            break;
    };

svg_wrap_cleanup:
    for (i = 0; i < data->num_attribs; ++i) {
        if (matrices[i]) {
            free_list(matrices[i]);
        }
    }
    SAFE_DELETE(lsb_param.password);
    SAFE_DELETE(matrices);
    SAFE_DELETE(bytes);

    return return_code;
}


/**
 * Reads the length of an embedded message from the given SVG data. If there
 * is no embedded message, there will still be a result, since the length is
 * assumed to be embedded in the LSBs of the first four bytes. It is in the
 * responsibility of the caller to ensure there really is a message embedded in
 * the bytes given to this function.
 *
 * @param data Pointer to a svg_data_t structure containing the cover data.
 * @param msglen Pointer to a uint32_t which will hold the message length, in
 * Bits, after the function returns.
 * @param param Pointer to a svg_parameter_t structure, containing the
 * parameters for this algorithm.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t svg_get_message_length(
        const svg_data_t *data,
        uint32_t *msglen,
        const svg_parameter_t *param) {

    return svg_wrap_lsb(data, 0, msglen, param, SVG_WRAP_MSGLEN);
}

/**
 * Embeds a message into the least significant bits of certain bytes in an
 * array of XML-attributes.
 * Note that the order in which the bits of the message will be embedded into
 * the bytes of the cover can be configured by the lsb param structure.
 *
 * @param src Pointer to a svg_data_t structure, containing the original cover
 * bytes.
 * @param stego Pointer to a svg_data_t structure, which will contain the
 * modified cover bytes (steganogram) after the function returns.
 * @param message The message that is to be embedded.
 * @param msglen The length of the message, in bytes.
 * @param param A set of parameters for this algorithm.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t svg_embed(
        const svg_data_t *src,
        svg_data_t *stego,
        const uint8_t *message,
        const uint32_t msglen,
        const svg_parameter_t *param) {

    uint32_t capacity = 0;
    uint32_t bit_length = 0, data_len = 0;
    uint32_t i = 0;
    uint8_t *msg = 0;
    uint32_t len = msglen;

    uint8_t ret = 0;

    // length may not be bigger than one eighth of the maximum of uint32_t, else
    // we risk a buffer overflow and resulting madnesses when we multiply it
    // with eight later on
    if (msglen > (0xFFFFFFFF - 32) / 8) {
        FAIL(LSTG_E_INSUFFCAP);
    }

    bit_length = msglen * 8 + 32;

    // check for capacity
    svg_check_capacity(src, &capacity, param);
    // check_capacity compensates for the 32 bit message length -- we have to correct for that
    capacity += 32;
    if (capacity < bit_length) {
        FAIL(LSTG_E_INSUFFCAP);
    }

    // copy original data into stego
    stego->num_attribs = src->num_attribs;
    stego->attributes = (svg_attrib_t*)malloc(sizeof(svg_attrib_t) * src->num_attribs);
    if (stego->attributes == NULL) {
        FAIL(LSTG_E_MALLOC);
    }

    for (i = 0; i < src->num_attribs; ++i) {
        data_len = sizeof(uint8_t) * (strlen((char*)src->attributes[i].data)+1);
        stego->attributes[i].data = (uint8_t*)malloc(sizeof(uint8_t) * data_len);
        if (stego->attributes[i].data == NULL) {
            SAFE_DELETE(stego->attributes);
            FAIL(LSTG_E_MALLOC);
        }
        memcpy(stego->attributes[i].data, src->attributes[i].data, data_len);
        stego->attributes[i].node = src->attributes[i].node;
    }

    // svg_wrap_lsb expects 'message' to be _not_ const, since it is also called
    // for extracting. so we have to make a temporary copy of the message, to
    // keep in line with the interface. the same goes for 'msglen', see above
    msg = (uint8_t*)malloc(sizeof(uint8_t) * msglen);
    if (msg == NULL) {
        for (i = 0; i< src->num_attribs; ++i)
            SAFE_DELETE(stego->attributes[i].data);
        SAFE_DELETE(stego->attributes);
        FAIL(LSTG_E_MALLOC);
    }
    memcpy(msg, message, msglen);

    ret = svg_wrap_lsb(stego, &msg, &len, param, SVG_WRAP_EMBED);

    free(msg);

    return ret;
}

/**
 * Extracts a message from the least significant bits of certain bytes in an
 * array of XML-attributes.
 *
 * @param data Pointer to a svg_data_t structure containing the cover data.
 * @param message A Pointer that will point to the extracted message after this
 * function returns.
 * @param msglen Pointer to a uint32_t which will hold the message length after
 * the function returns.
 * @param param A set of parameters for this algorithm.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t svg_extract(
        const svg_data_t *data,
        uint8_t **message,
        uint32_t *msglen,
        const svg_parameter_t *param) {

    return svg_wrap_lsb(data, message, msglen, param, SVG_WRAP_EXTRACT);
}

/**
 * Get the maximum capacity of the cover with the given parameters.
 *
 * @param data Pointer to a svg_data_t structure containing the cover data.
 * @param capacity The maximum capacity, in bits.
 * @param param The parameters for this algorithm.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t svg_check_capacity(
        const svg_data_t *data,
        uint32_t *capacity,
        const svg_parameter_t *param){

    uint32_t i = 0, dec_len = 0;
    uint8_t *start = 0, *end = 0;
    svg_llist_head_t head;
    svg_llist_t *current;

    *capacity = 0;

    for (i = 0; i < data->num_attribs; ++i) {
        // we only embed in transform matrices
        if (strncmp("matrix(", (char*)data->attributes[i].data, 7) == 0) {
            if (split_matrix(data->attributes[i].data, &head) != LSTG_OK) {
                // error code already set
                return LSTG_ERROR;
            }
            for (current = head; current != NULL; current = current->next) {
                // get decimal part of number (between '.' and the exponent (if any)

                // find decimal point
                start = (uint8_t*)strchr((char*)current->str, '.');
                if (start == NULL) {
                    // there was no '.' in this number, skip it
                    continue;
                }
                // find exponent (if any)
                end = (uint8_t*)strchr((char*)current->str, 'e');
                if (end == NULL) {
                    // there was no exponent, set end to end of string
                    end = current->str + strlen((char*)current->str);
                }
                // calculate length of decimal part
                dec_len = end - start - 1;

                // check if there are enough digits to embed anything
                if (dec_len > param->first_embed_digit) {
                    *capacity += dec_len - param->first_embed_digit;
                }
            }
            free_list(head);
        }
    }
    if (*capacity > 32)
        *capacity -= 32;
    else
        *capacity = 0;

    return LSTG_OK;
}


/**
 * Splits a matrix-string in the form of "matrix(a.b, c.d, e.f, g.h, i.j, k.l)"
 * into a linked list of strings "a.b"->"c.d"->"e.f"->"g.h"->"i.j"->"k.l".
 *
 * @param matrix The string that is to be slplit.
 * @param head The head of the linked list that will contain the seperated
 * matrix elements.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t split_matrix(uint8_t *matrix, svg_llist_head_t *head) {
    uint8_t *cur_pos = matrix;
    uint32_t matrix_len = strlen((char*)matrix);
    uint8_t *nxt_pos = 0;
    uint32_t str_len = 0;
    svg_llist_t *elem = 0, *current = 0, *next = 0;
    svg_llist_head_t first = 0;

    // skip the leading "matrix("
    cur_pos += 7;

    while (cur_pos < matrix + matrix_len) {
        // find next ","
        nxt_pos = (uint8_t*)strchr((char*)cur_pos, ',');
        if (nxt_pos == NULL) {
            nxt_pos = matrix + matrix_len;
            // we don't want the trailing ')'
            str_len = nxt_pos - cur_pos - 1;
        } else {
            str_len = nxt_pos - cur_pos;
            // skip over the ',', too
            nxt_pos++;
        }


        // create new list entry
        elem = (svg_llist_t*)malloc(sizeof(svg_llist_t));
        if (elem == NULL) {
            free_list(*head);
            FAIL(LSTG_E_MALLOC);
        }
        elem->str = 0;

        // attach new element to the list
        elem->next = first;
        first = elem;

        // copy found string into list
        elem->str = (uint8_t*)malloc(sizeof(uint8_t) * (str_len + 1));
        if (elem->str == NULL) {
            free_list(*head);
            FAIL(LSTG_E_MALLOC);
        }
        memcpy(elem->str, cur_pos, str_len);

        // set null termination for string
        elem->str[str_len] = '\0';

        // advance to next number
        cur_pos = nxt_pos;
    }

    // reverse order (so first numbers are first in list)
    current = first;
    next = current->next;
    current->next = NULL;
    elem = current;
    current = next;

    while (true) {
        next = current->next;
        current->next = elem;

        if (next == NULL)
            break;

        elem = current;
        current = next;
    }

    *head = current;

    return LSTG_OK;
}

/**
 * Joins a linked list of matrix elements into a string in the form of
 * "matrix(a.b, c.d, e.f, g.h, i.j, k.l)".
 *
 * @param matrix Pointer to a string, which will contain the assembled matrix.
 * @param head The head of the linked list containing the seperate matrix elements.
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t join_matrix(uint8_t **matrix, svg_llist_head_t head) {
    uint32_t matrix_length = 0;
    uint8_t *cur_str_pos = 0;
    svg_llist_t *current = 0;

    // get length of final string
    matrix_length = 9; // "matrix(" + ")" + "\0"
    for (current = head; current != NULL; current = current->next) {
        matrix_length += strlen((char*)current->str);

        // if there'll be another number after this, add a ","
        if (current->next != NULL) {
            matrix_length++;
        }
    }

    // allocate enough space to hold the final string
    *matrix = (uint8_t*)malloc(sizeof(uint8_t) * matrix_length);
    if (*matrix == NULL) {
        FAIL(LSTG_E_MALLOC);
    }

    // create matrix string
    cur_str_pos = *matrix;
    memcpy(cur_str_pos, "matrix(", 7);
    cur_str_pos += 7;
    for (current = head; current != NULL; current = current->next) {
        memcpy(cur_str_pos, current->str, strlen((char*)current->str));
        cur_str_pos += strlen((char*)current->str);

        // if there'll be another number after this, add a ","
        if (current->next != NULL) {
            *cur_str_pos = ',';
            cur_str_pos++;
        }
    }
    *cur_str_pos = ')';
    cur_str_pos++;
    *cur_str_pos = '\0';

    return LSTG_OK;
}


/**
 * Frees a linked list.
 *
 * @param head The head of the linked list that should be freed
 *
 * @return LSTG_OK on success, LSTG_ERROR if there was an error.
 */
uint32_t free_list(svg_llist_head_t head) {
    svg_llist_t *current = head;
    svg_llist_t *next;

    while (current != NULL) {
        SAFE_DELETE(current->str);

        next = current->next;
        SAFE_DELETE(current);

        current = next;
    }

    head = 0;

    return LSTG_OK;
}
