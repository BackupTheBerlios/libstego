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
 *  Author: Christian Kuka <ckuka@madkooky.de> 
 *
 *  For more info visit <http://parsys.informatik.uni-oldenburg.de/~stego/>
 */




#include <libstego/pq.h>
#include <libstego/random.h>

static int32_t _pq_get_ggt(int16_t a, int16_t b);

static double _pq_get_tau(uint32_t msglen, uint32_t index);

static double _pq_get_rho(uint32_t msglen, uint32_t index);

static double _pq_get_beta(uint32_t msglen);

static uint16_t _pq_get_quality(const jpeg_data_t * src_data);

static uint32_t _pq_get_header_message_bits(const jpeg_data_t * src_data);

static uint8_t *_pq_gauss(uint8_t ** matrix, uint8_t * vector,
                          uint32_t size);

static uint8_t *_pq_get_rhs_vector(uint8_t * msg_vector,
                                   uint8_t ** prng_matrix,
                                   uint8_t * cover_vector,
                                   uint32_t cover_size,
                                   uint32_t message_bits,
                                   uint32_t encoding_bits);

static uint32_t _pq_extract_head(const jpeg_data_t * stego_data,
                                 const pq_parameter * param,
                                 uint32_t * message_length,
                                 uint8_t * modkey);

/**
 * Embeds a message in a rasterimage using PQ
 * ...
 * @param *src_data a struct for jpegdata to provide the original image
 * @param *stego_data a struct for jpegdata to return the steganogram
 * @param *message returnpointer for the extracted message
 * @param msglen the length of the message
 * @param *param additional parameters for PQ, including passphrase
 * @return an errorcode or 0 if success
 */
uint32_t pq_embed(const jpeg_data_t * src_data, jpeg_data_t * stego_data,
                  uint8_t * message, uint32_t msglen,
                  const pq_parameter * param)
{
    if ((msglen + 1) >= (0xFFFFFFF8 >> 3)) {
        FAIL(LSTG_E_MSGTOOLONG);
    }

    uint32_t encoding_bits = 0;
    uint32_t max_encoding_bits = 0;
    uint32_t message_bits = 0;
    uint32_t cover_size = 0;
    uint32_t header_size = param->header_size;
    uint32_t body_size = 0;
    uint32_t hamming_weight = 0;
    // Beta value
    double beta = 0.0;
    uint32_t degree = 0;
    double random = 0.0;
    uint8_t lt_pass = 0;
    uint8_t gauss_pass = 0;
    // Quality of the jpeg image
    uint16_t quality = 0;
    // Scale factor
    long scale_factor = 0.0;
    // Capacity of the jpeg image
    uint32_t capacity = 0;
    uint8_t modkey = 0;
    // Counter
    uint32_t i, ii, iii, iv, v, vi;
    uint32_t column = 0;
    uint32_t row = 0;
    uint32_t bit_idx = 0;
    uint32_t tmp_uint_value = 0;
    uint8_t flag = 0;
    int32_t tmp = 0;
    long freq = 0.0;
    int16_t value = 0;
    int16_t base = 0;
    // State pointer for PRNG
    prng_state *state = NULL;
    uint8_t *tmp_ptr = NULL;
    uint8_t *password = NULL;
    uint8_t *rhs_vector = NULL;
    uint8_t *message_vector = NULL;
    uint8_t **prng_matrix = NULL;
    uint8_t **parity_matrix = NULL;
    uint8_t *embed_vector = NULL;
    uint8_t *cover_vector = NULL;
    uint8_t *marker_vector = NULL;
    uint32_t *order_vector = NULL;
    double *range_vector = NULL;

    // Copy Structure (src->stego)
    stego_data->comp = ALLOCN(jpeg_comp_t, src_data->comp_num);
    if (!stego_data->comp) {
        FAIL(LSTG_E_MALLOC);
    }
    stego_data->comp_num = src_data->comp_num;
    stego_data->size_x = src_data->size_x;
    stego_data->size_y = src_data->size_y;

    for (i = 0; i < src_data->comp_num; i++) {
        //Create jpeg structure
        (stego_data->comp + i)->nblocks = (src_data->comp + i)->nblocks;
        (stego_data->comp + i)->quant = ALLOC(jpeg_quant_t);
        if (!(stego_data->comp + i)->quant) {
            for (ii = 0; ii < i; ii++) {
                SAFE_DELETE((stego_data->comp + ii)->quant);
                if (ii < i - 1) {
                    SAFE_DELETE((stego_data->comp + ii)->blocks);
                }
            }
            SAFE_DELETE(stego_data->comp);
            FAIL(LSTG_E_MALLOC);
        }
        (stego_data->comp + i)->blocks =
            ALLOCN(jpeg_block_t, (src_data->comp + i)->nblocks);
        if (!(stego_data->comp + i)->blocks) {
            for (ii = 0; ii <= i; ii++) {
                SAFE_DELETE((stego_data->comp + ii)->quant);
                if (ii < i) {
                    SAFE_DELETE((stego_data->comp + ii)->blocks);
                }
            }
            SAFE_DELETE(stego_data->comp);
            FAIL(LSTG_E_MALLOC);
        }
    }

    // Calc the capacity
    pq_check_capacity(src_data, param, &capacity);
    if (capacity < msglen) {
        for (i = 0; i < src_data->comp_num; i++) {
            SAFE_DELETE((stego_data->comp + i)->blocks);
            SAFE_DELETE((stego_data->comp + i)->quant);
        }
        SAFE_DELETE(stego_data->comp);
        FAIL(LSTG_E_MSGTOOLONG);
    }

    if (param->quality == 0) {
        quality = (_pq_get_quality(src_data) - 50) << 1;
    } else {
        quality = param->quality;
    }
    if (quality < 50) {
        scale_factor = 5000 / quality;
    } else {
        scale_factor = 200 - (quality << 1);
    }

    cover_size = 0;
    for (i = 0; i < src_data->comp_num; i++) {
        cover_size += (src_data->comp + i)->nblocks;
    }
    cover_size = (cover_size << 8);
    if (header_size == 0) {
        header_size = 64;
    }
    if (header_size > cover_size) {
        for (i = 0; i < src_data->comp_num; i++) {
            SAFE_DELETE((stego_data->comp + i)->blocks);
            SAFE_DELETE((stego_data->comp + i)->quant);
        }
        SAFE_DELETE(stego_data->comp);
        FAIL(LSTG_E_INSUFFCAP);
    }
    body_size = cover_size - header_size;

    marker_vector = ALLOCN(uint8_t, cover_size);
    cover_vector = ALLOCN(uint8_t, cover_size);
    if ((!cover_vector) || (!marker_vector)) {
        for (i = 0; i < src_data->comp_num; i++) {
            SAFE_DELETE((stego_data->comp + i)->blocks);
            SAFE_DELETE((stego_data->comp + i)->quant);
        }
        SAFE_DELETE(stego_data->comp);
        SAFE_DELETE(cover_vector);
        SAFE_DELETE(marker_vector);
        FAIL(LSTG_E_MALLOC);
    }
    memset(marker_vector, 0, cover_size);
    memset(cover_vector, 0, cover_size);

    iv = 0;
    for (i = 0; i < src_data->comp_num; i++) {
        for (ii = 0; ii < 64; ii++) {
            if (i == 0) {
                freq =
                    ((long) *(_pq_luminance_quant_tbl + ii) *
                     scale_factor + 50L) / 100L;
            } else {
                freq =
                    ((long) *(_pq_chrominance_quant_tbl + ii) *
                     scale_factor + 50L) / 100L;
            }
            if (freq <= 0L)
                freq = 1L;
            //if (temp > 32767L) temp = 32767L;
            if (freq > 255L)
                freq = 255L;
            base =
                freq / (2 *
                        _pq_get_ggt(freq,
                                    *((src_data->comp + i)->quant->values +
                                      ii)));
            if ((base & 0x1) == 0) {
                for (tmp = -254; tmp < 255; tmp++) {
                    value = (int16_t) (2 * tmp + 1) * base;
                    for (iii = 0; iii < (src_data->comp + i)->nblocks;
                         iii++) {
                        if (*
                            (((src_data->comp + i)->blocks + iii)->values +
                             ii) == value) {
                            *(marker_vector +
                              ((iv + (iii << 6) + ii) >> 3)) |=
                (1 << ((iv + (iii << 6) + ii) & 0x7));
                        }
                    }
                }
            }
            *((stego_data->comp + i)->quant->values + ii) = freq;
        }
        iv += ((src_data->comp + i)->nblocks << 6);
    }
    iv = 0;
    for (i = 0; i < src_data->comp_num; i++) {
        for (ii = 0; ii < (src_data->comp + i)->nblocks; ii++) {
            for (iii = 0; iii < 64; iii++) {
                if (*((stego_data->comp + i)->quant->values + iii) != 0) {
                    *(((stego_data->comp + i)->blocks + ii)->values +
                      iii) =
(uint32_t) ceil((double) *(((src_data->comp + i)->blocks + ii)->values + iii) *
*((src_data->comp + i)->quant->values +
iii) / *((stego_data->comp + i)->quant->values + iii));
                } else {
                    *(((stego_data->comp + i)->blocks + ii)->values +
                      iii) = 0;
                }
                if (*(((stego_data->comp + i)->blocks + ii)->values + iii)
                    & 0x1) {
                    *(cover_vector + (iv >> 3)) |= (1 << (iv & 0x7));
                }
                iv++;
            }
        }
    }
    // Begin Message Embed Process
    message_bits = msglen << 3;
    beta = _pq_get_beta(message_bits);
    encoding_bits = ((uint32_t) ceil((double) beta * message_bits)) + 1;

    range_vector = ALLOCN(double, message_bits);
    prng_matrix = ALLOCN(uint8_t *, message_bits);
    parity_matrix = ALLOCN(uint8_t *, message_bits);
    password = ALLOCN(uint8_t, param->pwlen + 1);
    embed_vector =
        ALLOCN(uint8_t,
               (encoding_bits >> 3) + ((encoding_bits & 0x7) ? 1 : 0));
    order_vector = ALLOCN(uint32_t, encoding_bits);

    if ((!range_vector) || (!prng_matrix) || (!parity_matrix)
        || (!embed_vector) || (!order_vector) || (!password)) {
        for (i = 0; i < src_data->comp_num; i++) {
            SAFE_DELETE((stego_data->comp + i)->blocks);
            SAFE_DELETE((stego_data->comp + i)->quant);
        }
        SAFE_DELETE(stego_data->comp);
        SAFE_DELETE(cover_vector);
        SAFE_DELETE(marker_vector);
        SAFE_DELETE(range_vector);
        SAFE_DELETE(prng_matrix);
        SAFE_DELETE(parity_matrix);
        SAFE_DELETE(embed_vector);
        SAFE_DELETE(order_vector);
        SAFE_DELETE(password);
        FAIL(LSTG_E_MALLOC);
    }
    memset(embed_vector, 0,
           (encoding_bits >> 3) + ((encoding_bits & 0x7) ? 1 : 0));

    // Create the range vector
    for (i = 0; i < message_bits; i++) {
        if (header_size > body_size) {
            *(prng_matrix + i) = ALLOCN(uint8_t, header_size);
        } else {
            *(prng_matrix + i) = ALLOCN(uint8_t, body_size);
        }
        if (!*(prng_matrix + i)) {
            for (ii = 0; ii < src_data->comp_num; ii++) {
                SAFE_DELETE((stego_data->comp + ii)->blocks);
                SAFE_DELETE((stego_data->comp + ii)->quant);
            }
            SAFE_DELETE(stego_data->comp);
            SAFE_DELETE(cover_vector);
            SAFE_DELETE(marker_vector);
            SAFE_DELETE(range_vector);
            SAFE_DELETE(embed_vector);
            SAFE_DELETE(order_vector);
            SAFE_DELETE(password);
            for (ii = 0; ii < i; ii++) {
                SAFE_DELETE(*(prng_matrix + ii));
                if (ii < (i - 1)) {
                    SAFE_DELETE(*(parity_matrix + ii));
                }
            }
            SAFE_DELETE(prng_matrix);
            SAFE_DELETE(parity_matrix);

            FAIL(LSTG_E_MALLOC);
        }
        *(parity_matrix + i) =
            ALLOCN(uint8_t,
                   (encoding_bits >> 3) + ((encoding_bits & 0x7) ? 1 : 0));
        if (!*(parity_matrix + i)) {
            for (i = 0; i < src_data->comp_num; i++) {
                SAFE_DELETE((stego_data->comp + i)->blocks);
                SAFE_DELETE((stego_data->comp + i)->quant);
            }
            SAFE_DELETE(stego_data->comp);
            SAFE_DELETE(cover_vector);
            SAFE_DELETE(marker_vector);
            SAFE_DELETE(range_vector);
            SAFE_DELETE(embed_vector);
            SAFE_DELETE(order_vector);
            SAFE_DELETE(password);
            for (ii = 0; ii <= i; ii++) {
                SAFE_DELETE(*(prng_matrix + ii));
                if (ii < i) {
                    SAFE_DELETE(*(parity_matrix + ii));
                }
            }
            SAFE_DELETE(prng_matrix);
            SAFE_DELETE(parity_matrix);

            FAIL(LSTG_E_MALLOC);
        }
        if (i == 0) {
            *range_vector =
                (_pq_get_rho(message_bits, i + 1) +
                 _pq_get_tau(message_bits, i + 1)) / beta;
        } else {
            *(range_vector + i) =
                *(range_vector + (i - 1)) +
                (double) ((_pq_get_rho(message_bits, i + 1) +
                           _pq_get_tau(message_bits, i + 1)) / beta);
        }
    }
    lt_pass = 0;
    modkey = 0;
    while ((!lt_pass) && (modkey < 32)) {
        memcpy(password, param->password, param->pwlen);
        *(password + param->pwlen) = modkey;
        random_init(&state, password, param->pwlen + 1);

        for (i = 0; i < message_bits; i++) {
            memset(*(prng_matrix + i), 0, body_size);
            memset(*(parity_matrix + i), 0,
                   (encoding_bits >> 3) + ((encoding_bits & 0x7) ? 1 : 0));
        }

        // Set the degree of each encoding bit
        for (i = 0; i < (body_size << 3); i++) {
            random = (double) random_next(state) / 0xFFFFFFFF;
            degree = 0;
            for (ii = 0; ii < message_bits; ii++) {
                if (*(range_vector + ii) > random) {
                    degree = ii + 1;
                    break;
                }
            }
            for (ii = 0; ii < degree; ii++) {
                *(*(prng_matrix + random_next(state) % message_bits) +
                  (i >> 3)) |= 1 << (i & 0x7);
            }
        }

        iv = 0;
        v = 0;
        for (i = 0; i < src_data->comp_num; i++) {
            for (ii = 0; ii < (src_data->comp + i)->nblocks; ii++) {
                for (iii = 0; iii < 64; iii++) {
                    if ((iv >= (header_size << 3))
                        && (*(marker_vector + (iv >> 3)) &
                            (1 << (iv & 0x7))) && (v < encoding_bits)) {
                        for (vi = 0; vi < message_bits; vi++) {
                            *(*(parity_matrix + vi) + (v >> 3)) |=
                                ((*
                                  (*(prng_matrix + vi) +
                                   ((iv -
                                     (header_size << 3)) >> 3)) & (1 <<
                                                                   ((iv -
                                                                     (header_size
                                                                      <<
                                                                      3)) &
                                                                    0x7)))
                                 >> ((iv - (header_size << 3)) & 0x7)) <<
                                (v & 0x7);
                        }
                        v++;
                    }
                    iv++;
                }
            }
        }
        if (v < encoding_bits) {
            for (i = 0; i < src_data->comp_num; i++) {
                SAFE_DELETE((stego_data->comp + i)->blocks);
                SAFE_DELETE((stego_data->comp + i)->quant);
            }
            SAFE_DELETE(stego_data->comp);
            SAFE_DELETE(cover_vector);
            SAFE_DELETE(marker_vector);
            SAFE_DELETE(range_vector);
            SAFE_DELETE(embed_vector);
            SAFE_DELETE(order_vector);
            SAFE_DELETE(password);
            for (i = 0; i < message_bits; i++) {
                SAFE_DELETE(*(prng_matrix + i));
                SAFE_DELETE(*(parity_matrix + i));
            }
            SAFE_DELETE(prng_matrix);
            SAFE_DELETE(parity_matrix);
            random_cleanup(state);
            FAIL(LSTG_E_INSUFFCAP);
        }

        rhs_vector =
            _pq_get_rhs_vector(message, prng_matrix,
                               cover_vector + header_size, body_size,
                               message_bits, encoding_bits);
        for (i = 0; i < encoding_bits; i++) {
            *(order_vector + i) = i;
        }

        column = 0;
        row = 0;
        bit_idx = 0;
        tmp_ptr = NULL;
        for (column = 0; column < encoding_bits; column++) {
            for (i = column; i < encoding_bits; i++) {
                hamming_weight = 0;
                for (ii = row; ii < message_bits; ii++) {
                    if (*(*(parity_matrix + ii) + (i >> 3)) &
                        (1 << (i & 0x7))) {
                        hamming_weight++;
                        bit_idx = ii;
                    }
                }
                if (hamming_weight == 1) {
                    if (column != i) {
                        for (ii = 0; ii < message_bits; ii++) {
                            flag =
                                ((*(*(parity_matrix + ii) + (column >> 3))
                                  & (1 << (column & 0x7))) >> (column &
                                                               0x7));
                            *(*(parity_matrix + ii) + (column >> 3)) =
                                (*(*(parity_matrix + ii) + (column >> 3)) &
                                 (~(1 << (column & 0x7)))) |
                                (((*(*(parity_matrix + ii) + (i >> 3)) &
                                   (1 << (i & 0x7))) >> (i & 0x7)) <<
                                 (column & 0x7));
                            *(*(parity_matrix + ii) + (i >> 3)) =
                                (*(*(parity_matrix + ii) + (i >> 3)) &
                                 (~(1 << (i & 0x7)))) | (flag << (i &
                                                                  0x7));
                        }
                        tmp_uint_value = *(order_vector + column);
                        *(order_vector + column) = *(order_vector + i);
                        *(order_vector + i) = tmp_uint_value;
                    }
                    if (row != bit_idx) {
                        tmp_ptr = *(parity_matrix + row);
                        *(parity_matrix + row) =
                            *(parity_matrix + bit_idx);
                        *(parity_matrix + bit_idx) = tmp_ptr;

                        flag =
                            ((*(rhs_vector + (row >> 3)) &
                              (1 << (row & 0x7))) >> (row & 0x7));
                        *(rhs_vector + (row >> 3)) =
                            (*(rhs_vector + (row >> 3)) &
                             (~(1 << (row & 0x7)))) |
                            (((*(rhs_vector + (bit_idx >> 3)) &
                               (1 << (bit_idx & 0x7))) >> (bit_idx & 0x7))
                             << (row & 0x7));
                        *(rhs_vector + (bit_idx >> 3)) =
                            (*(rhs_vector + (bit_idx >> 3)) &
                             (~(1 << (bit_idx & 0x7)))) | (flag << (bit_idx
                                                                    &
                                                                    0x7));
                    }
                    row++;
                    i = encoding_bits;
                }
            }
        }
        random_cleanup(state);
        lt_pass = 1;
        for (i = 0; i < message_bits; i++) {
            if (!(*(*(parity_matrix + i) + (i >> 3)) & (1 << (i & 0x7)))) {
                lt_pass = 0;
            }
            if (lt_pass) {
                for (ii = 0; ii < i; ii++) {
                    if ((*(*(parity_matrix + i) + (ii >> 3)) &
                         (1 << (ii & 0x7)))) {
                        lt_pass = 0;
                        break;
                    }
                }
            }
            if (!lt_pass) {
                break;
            }
        }
        if (!lt_pass) {
            SAFE_DELETE(rhs_vector);
            modkey++;
        }
    }
    memset(password, 0, param->pwlen + 1);
    SAFE_DELETE(password);
    SAFE_DELETE(range_vector);
    if (modkey > 31) {
        for (i = 0; i < src_data->comp_num; i++) {
            SAFE_DELETE((stego_data->comp + i)->blocks);
            SAFE_DELETE((stego_data->comp + i)->quant);
        }
        SAFE_DELETE(stego_data->comp);
        SAFE_DELETE(cover_vector);
        SAFE_DELETE(marker_vector);
        SAFE_DELETE(embed_vector);
        SAFE_DELETE(order_vector);
        SAFE_DELETE(rhs_vector);
        for (i = 0; i < message_bits; i++) {
            SAFE_DELETE(*(prng_matrix + i));
            SAFE_DELETE(*(parity_matrix + i));
        }
        SAFE_DELETE(prng_matrix);
        SAFE_DELETE(parity_matrix);
        FAIL(LSTG_E_PQ_LTFAIL);
    }

    hamming_weight = 1;
    while (hamming_weight == 1) {
        row = message_bits;
        while (row != 0) {
            row--;
            hamming_weight = 0;
            for (column = 0; column < message_bits; column++) {
                if (*(*(parity_matrix + row) + (column >> 3)) &
                    (1 << (column & 0x7))) {
                    hamming_weight++;
                    bit_idx = column;
                }
            }
            if (hamming_weight == 1) {
                *(embed_vector + (*(order_vector + bit_idx) >> 3)) |=
                    (((*(rhs_vector + (row >> 3)) & (1 << (row & 0x7))) >>
                      (row & 0x7)) << (*(order_vector + bit_idx) & 0x7));
                *(*(parity_matrix + row) + (bit_idx >> 3)) &=
                    (~(1 << (bit_idx & 0x7)));
                for (i = 0; i < message_bits; i++) {
                    if (*(*(parity_matrix + i) + (bit_idx >> 3)) &
                        (1 << (bit_idx & 0x7))) {
                        *(rhs_vector + (i >> 3)) ^=
                            (((*
                               (embed_vector +
                                (*(order_vector + bit_idx) >> 3)) & (1 <<
                                                                     (*
                                                                      (order_vector
                                                                       +
                                                                       bit_idx)
                                                                      &
                                                                      0x7)))
                              >> (*(order_vector + bit_idx) & 0x7)) << (i &
                                                                        0x7));
                        *(*(parity_matrix + i) + (bit_idx >> 3)) &=
                            (~(1 << (bit_idx & 0x7)));
                    }
                }
                break;
            }
        }
    }

    SAFE_DELETE(rhs_vector);
    SAFE_DELETE(order_vector);

    v = 0;
    iv = 0;
    for (i = 0; i < src_data->comp_num; i++) {
        for (ii = 0; ii < (src_data->comp + i)->nblocks; ii++) {
            for (iii = 0; iii < 64; iii++) {
                if ((iv >= header_size << 3)
                    && (*(marker_vector + (iv >> 3)) & (1 << (iv & 0x7)))
                    && (v < encoding_bits)) {
                    value =
                        (*
                         (((src_data->comp + i)->blocks + ii)->values +
                          iii) * *((src_data->comp + i)->quant->values +
                                   iii) -
                         (*((stego_data->comp + i)->quant->values + iii) >>
                          1)) / *((stego_data->comp + i)->quant->values +
                                  iii);
                    if (*(embed_vector + (v >> 3)) & (1 << (v & 0x7))) {
                        if (!
                            (*(cover_vector + (iv >> 3)) &
                             (1 << (iv & 0x7)))) {
                            if (value & 0x1) {
                                (*
                                 (((stego_data->comp + i)->blocks +
                                   ii)->values + iii)) = value;
                            } else {
                                (*
                                 (((stego_data->comp + i)->blocks +
                                   ii)->values + iii)) = value + 1;
                            }
                        } else {
                            if (value & 0x1) {
                                (*
                                 (((stego_data->comp + i)->blocks +
                                   ii)->values + iii)) = value + 1;
                            } else {
                                (*
                                 (((stego_data->comp + i)->blocks +
                                   ii)->values + iii)) = value;
                            }
                        }
                    }
                    v++;
                }
                iv++;
            }
        }
    }
    SAFE_DELETE(embed_vector);
    if (v < encoding_bits) {
        for (i = 0; i < src_data->comp_num; i++) {
            SAFE_DELETE((stego_data->comp + i)->blocks);
            SAFE_DELETE((stego_data->comp + i)->quant);
        }
        SAFE_DELETE(stego_data->comp);
        SAFE_DELETE(cover_vector);
        SAFE_DELETE(marker_vector);
        for (i = 0; i < message_bits; i++) {
            SAFE_DELETE(*(prng_matrix + i));
            SAFE_DELETE(*(parity_matrix + i));
        }
        SAFE_DELETE(prng_matrix);
        SAFE_DELETE(parity_matrix);

        FAIL(LSTG_E_INSUFFCAP);
    }

    // Begin Message Length Embed Process
    message_bits = _pq_get_header_message_bits(src_data);
    encoding_bits = message_bits;
    max_encoding_bits = 0;
    for (i = 0; i < (header_size << 3); i++) {
        if ((*(marker_vector + (i >> 3)) & (1 << (i & 0x7)))) {
            max_encoding_bits++;
        }
    }
    if (max_encoding_bits < message_bits) {
        for (i = 0; i < src_data->comp_num; i++) {
            SAFE_DELETE((stego_data->comp + i)->blocks);
            SAFE_DELETE((stego_data->comp + i)->quant);
        }
        SAFE_DELETE(stego_data->comp);
        SAFE_DELETE(cover_vector);
        SAFE_DELETE(marker_vector);
        for (i = 0; i < (msglen << 3); i++) {
            SAFE_DELETE(*(prng_matrix + i));
            SAFE_DELETE(*(parity_matrix + i));
        }
        SAFE_DELETE(prng_matrix);
        SAFE_DELETE(parity_matrix);
        FAIL(LSTG_E_INSUFFCAP);
    }
    random_init(&state, param->password, param->pwlen);
    message_vector =
        ALLOCN(uint8_t,
               (message_bits >> 3) + ((message_bits & 0x7) ? 1 : 0));
    if (!message_vector) {
        for (i = 0; i < src_data->comp_num; i++) {
            SAFE_DELETE((stego_data->comp + i)->blocks);
            SAFE_DELETE((stego_data->comp + i)->quant);
        }
        SAFE_DELETE(stego_data->comp);
        SAFE_DELETE(cover_vector);
        SAFE_DELETE(marker_vector);
        random_cleanup(state);
        for (i = 0; i < (msglen << 3); i++) {
            SAFE_DELETE(*(prng_matrix + i));
            SAFE_DELETE(*(parity_matrix + i));
        }
        SAFE_DELETE(prng_matrix);
        SAFE_DELETE(parity_matrix);
        FAIL(LSTG_E_MALLOC);
    }
    memset(message_vector, 0,
           (message_bits >> 3) + ((message_bits & 0x7) ? 1 : 0));
    if (max_encoding_bits > (msglen << 3)) {
        for (i = 0; i < (msglen << 3); i++) {
            SAFE_DELETE(*(prng_matrix + i));
            SAFE_DELETE(*(parity_matrix + i));
        }
        SAFE_DELETE(prng_matrix);
        SAFE_DELETE(parity_matrix);

        prng_matrix = ALLOCN(uint8_t *, message_bits);
        parity_matrix = ALLOCN(uint8_t *, max_encoding_bits);
        if ((!prng_matrix) || (!parity_matrix)) {
            for (ii = 0; ii < src_data->comp_num; ii++) {
                SAFE_DELETE((stego_data->comp + ii)->blocks);
                SAFE_DELETE((stego_data->comp + ii)->quant);
            }
            SAFE_DELETE(stego_data->comp);
            SAFE_DELETE(cover_vector);
            SAFE_DELETE(marker_vector);
            SAFE_DELETE(message_vector);
            random_cleanup(state);
            SAFE_DELETE(prng_matrix);
            SAFE_DELETE(parity_matrix);
            FAIL(LSTG_E_MALLOC);
        }

        for (i = 0; i < message_bits; i++) {
            *(prng_matrix + i) = ALLOCN(uint8_t, header_size);
            if (!*(prng_matrix + i)) {
                for (ii = 0; ii < src_data->comp_num; ii++) {
                    SAFE_DELETE((stego_data->comp + ii)->blocks);
                    SAFE_DELETE((stego_data->comp + ii)->quant);
                }
                SAFE_DELETE(stego_data->comp);
                SAFE_DELETE(cover_vector);
                SAFE_DELETE(marker_vector);
                SAFE_DELETE(message_vector);
                random_cleanup(state);
                for (ii = 0; ii < i; ii++) {
                    SAFE_DELETE(*(prng_matrix + ii));
                }
                SAFE_DELETE(prng_matrix);
                SAFE_DELETE(parity_matrix);

                FAIL(LSTG_E_MALLOC);
            }
        }
        for (i = 0; i < max_encoding_bits; i++) {
            *(parity_matrix + i) =
                ALLOCN(uint8_t,
                       (max_encoding_bits >> 3) +
                       ((max_encoding_bits & 0x7) ? 1 : 0));
            if (!*(parity_matrix + i)) {
                for (ii = 0; ii < src_data->comp_num; ii++) {
                    SAFE_DELETE((stego_data->comp + ii)->blocks);
                    SAFE_DELETE((stego_data->comp + ii)->quant);
                }
                SAFE_DELETE(stego_data->comp);
                SAFE_DELETE(cover_vector);
                SAFE_DELETE(marker_vector);
                SAFE_DELETE(message_vector);
                random_cleanup(state);
                for (ii = 0; ii < message_bits; ii++) {
                    SAFE_DELETE(*(prng_matrix + ii));
                }
                for (ii = 0; ii < i; ii++) {
                    SAFE_DELETE(*(parity_matrix + ii));
                }
                SAFE_DELETE(prng_matrix);
                SAFE_DELETE(parity_matrix);

                FAIL(LSTG_E_MALLOC);
            }
        }
    }
    // Create the random matrix and the message length vector
    for (i = 0; i < message_bits; i++) {
        memset(*(prng_matrix + i), 0, header_size);
        for (ii = 0; ii < (header_size << 3); ii++) {
            *(*(prng_matrix + i) + (ii >> 3)) |=
                ((random_next(state) & 0x1) ? 1 : 0) << (ii & 0x7);
        }
        if (i < 5) {
            *(message_vector + (i >> 3)) |=
                (((modkey & (1 << i)) >> i) << (i & 0x7));
        } else {
            *(message_vector + (i >> 3)) |=
                (((msglen & (1 << (i - 5))) >> (i - 5)) << (i & 0x7));
        }
    }
    random_cleanup(state);

    //Create the parity matrix (all columns from the random matrix with the index of a changeable coefficient)
    gauss_pass = 0;
    while ((!gauss_pass) && (encoding_bits < max_encoding_bits)) {
        for (i = 0; i < encoding_bits; i++) {
            memset(*(parity_matrix + i), 0,
                   (uint32_t) ceil((double) encoding_bits / 8));
        }
        iv = 0;
        v = 0;
        for (i = 0; i < src_data->comp_num; i++) {
            for (ii = 0;
                 (ii < ((src_data->comp + i)->nblocks << 6))
                 && (v < (header_size << 3)); ii++) {
                if ((*(marker_vector + (v >> 3)) & (1 << (v & 0x7)))
                    && (iv < encoding_bits)) {
                    for (iii = 0; iii < message_bits; iii++) {
                        *(*(parity_matrix + iii) + (iv >> 3)) |=
                            ((*(*(prng_matrix + iii) + (v >> 3)) &
                              (1 << (v & 0x7))) >> (v & 0x7)) << (iv &
                                                                  0x7);
                    }
                    iv++;
                }
                v++;
            }
        }
        if (iv < encoding_bits) {
            for (ii = 0; ii < src_data->comp_num; ii++) {
                SAFE_DELETE((stego_data->comp + ii)->blocks);
                SAFE_DELETE((stego_data->comp + ii)->quant);
            }
            SAFE_DELETE(stego_data->comp);
            SAFE_DELETE(cover_vector);
            SAFE_DELETE(marker_vector);
            SAFE_DELETE(message_vector);
            if (max_encoding_bits > (msglen << 3)) {
                for (ii = 0; ii < max_encoding_bits; ii++) {
                    SAFE_DELETE(*(parity_matrix + ii));
                    if (ii < message_bits) {
                        SAFE_DELETE(*(prng_matrix + ii));
                    }
                }
            } else {
                for (ii = 0; ii < (msglen << 3); ii++) {
                    SAFE_DELETE(*(parity_matrix + ii));
                    SAFE_DELETE(*(prng_matrix + ii));
                }
            }
            SAFE_DELETE(prng_matrix);
            SAFE_DELETE(parity_matrix);

            FAIL(LSTG_E_INSUFFCAP);
        }
        rhs_vector =
            _pq_get_rhs_vector(message_vector, prng_matrix, cover_vector,
                               header_size, message_bits, encoding_bits);
        if (!rhs_vector) {
            for (ii = 0; ii < src_data->comp_num; ii++) {
                SAFE_DELETE((stego_data->comp + ii)->blocks);
                SAFE_DELETE((stego_data->comp + ii)->quant);
            }
            SAFE_DELETE(stego_data->comp);
            SAFE_DELETE(cover_vector);
            SAFE_DELETE(marker_vector);
            SAFE_DELETE(message_vector);
            if (max_encoding_bits > (msglen << 3)) {
                for (ii = 0; ii < max_encoding_bits; ii++) {
                    SAFE_DELETE(*(parity_matrix + ii));
                    if (ii < message_bits) {
                        SAFE_DELETE(*(prng_matrix + ii));
                    }
                }
            } else {
                for (ii = 0; ii < (msglen << 3); ii++) {
                    SAFE_DELETE(*(parity_matrix + ii));
                    SAFE_DELETE(*(prng_matrix + ii));
                }
            }
            SAFE_DELETE(prng_matrix);
            SAFE_DELETE(parity_matrix);

            FAIL(LSTG_E_MALLOC);
        }
        // Solve equation with gauss
        embed_vector = _pq_gauss(parity_matrix, rhs_vector, encoding_bits);
        if (!embed_vector) {
            for (ii = 0; ii < src_data->comp_num; ii++) {
                SAFE_DELETE((stego_data->comp + ii)->blocks);
                SAFE_DELETE((stego_data->comp + ii)->quant);
            }
            SAFE_DELETE(stego_data->comp);
            SAFE_DELETE(cover_vector);
            SAFE_DELETE(marker_vector);
            SAFE_DELETE(message_vector);
            if (max_encoding_bits > (msglen << 3)) {
                for (ii = 0; ii < max_encoding_bits; ii++) {
                    SAFE_DELETE(*(parity_matrix + ii));
                    if (ii < message_bits) {
                        SAFE_DELETE(*(prng_matrix + ii));
                    }
                }
            } else {
                for (ii = 0; ii < (msglen << 3); ii++) {
                    SAFE_DELETE(*(parity_matrix + ii));
                    SAFE_DELETE(*(prng_matrix + ii));
                }
            }
            SAFE_DELETE(prng_matrix);
            SAFE_DELETE(parity_matrix);
            SAFE_DELETE(rhs_vector);
            FAIL(LSTG_E_MALLOC);
        }

        SAFE_DELETE(rhs_vector);
        gauss_pass = 1;
        for (i = 0; i < message_bits; i++) {
            if (!(*(*(parity_matrix + i) + (i >> 3)) & (1 << (i & 0x7)))) {
                gauss_pass = 0;
            }
            if (gauss_pass) {
                for (ii = 0; ii < i; ii++) {
                    if ((*(*(parity_matrix + i) + (ii >> 3)) &
                         (1 << (ii & 0x7)))) {
                        gauss_pass = 0;
                        break;
                    }
                }
            }
            if (!gauss_pass) {
                break;
            }
        }
        if (!gauss_pass) {
            encoding_bits++;
            SAFE_DELETE(embed_vector);
        }
    }
    SAFE_DELETE(message_vector);
    for (i = 0; i < message_bits; i++) {
        if (!(*(*(parity_matrix + i) + (i >> 3)) & (1 << (i & 0x7)))) {
            for (ii = 0; ii < src_data->comp_num; ii++) {
                SAFE_DELETE((stego_data->comp + ii)->blocks);
                SAFE_DELETE((stego_data->comp + ii)->quant);
            }
            SAFE_DELETE(stego_data->comp);
            SAFE_DELETE(cover_vector);
            SAFE_DELETE(marker_vector);
            SAFE_DELETE(embed_vector);
            if (max_encoding_bits > (msglen << 3)) {
                for (ii = 0; ii < max_encoding_bits; ii++) {
                    SAFE_DELETE(*(parity_matrix + ii));
                    if (ii < message_bits) {
                        SAFE_DELETE(*(prng_matrix + ii));
                    }
                }
            } else {
                for (ii = 0; ii < (msglen << 3); ii++) {
                    SAFE_DELETE(*(parity_matrix + ii));
                    SAFE_DELETE(*(prng_matrix + ii));
                }
            }
            SAFE_DELETE(prng_matrix);
            SAFE_DELETE(parity_matrix);
            FAIL(LSTG_E_PQ_GAUSSFAIL);
        }
        for (ii = 0; ii < i; ii++) {
            if ((*(*(parity_matrix + i) + (ii >> 3)) & (1 << (ii & 0x7)))) {
                for (iii = 0; iii < src_data->comp_num; iii++) {
                    SAFE_DELETE((stego_data->comp + iii)->blocks);
                    SAFE_DELETE((stego_data->comp + iii)->quant);
                }
                SAFE_DELETE(stego_data->comp);
                SAFE_DELETE(cover_vector);
                SAFE_DELETE(marker_vector);
                SAFE_DELETE(embed_vector);
                if (max_encoding_bits > (msglen << 3)) {
                    for (iii = 0; iii < max_encoding_bits; iii++) {
                        SAFE_DELETE(*(parity_matrix + iii));
                        if (iii < message_bits) {
                            SAFE_DELETE(*(prng_matrix + iii));
                        }
                    }
                } else {
                    for (iii = 0; iii < (msglen << 3); iii++) {
                        SAFE_DELETE(*(parity_matrix + iii));
                        SAFE_DELETE(*(prng_matrix + iii));
                    }
                }
                SAFE_DELETE(prng_matrix);
                SAFE_DELETE(parity_matrix);

                FAIL(LSTG_E_PQ_GAUSSFAIL);
            }
        }
    }

    if (max_encoding_bits > (msglen << 3)) {
        for (i = 0; i < max_encoding_bits; i++) {
            if (i < message_bits) {
                SAFE_DELETE(*(prng_matrix + i));
            }
            SAFE_DELETE(*(parity_matrix + i));
        }
    } else {
        for (i = 0; i < (msglen << 3); i++) {
            SAFE_DELETE(*(prng_matrix + i));
            SAFE_DELETE(*(parity_matrix + i));
        }
    }
    SAFE_DELETE(prng_matrix);
    SAFE_DELETE(parity_matrix);

    // Embed header
    iii = 0;
    iv = 0;
    for (i = 0; i < src_data->comp_num; i++) {
        for (ii = 0;
             (ii < ((src_data->comp + i)->nblocks << 6))
             && (iv < (header_size << 3)); ii++) {
            if ((*(marker_vector + (iv >> 3)) & (1 << (iv & 0x7)))
                && (iii < encoding_bits)) {
                value =
                    (*
                     (((src_data->comp + i)->blocks + (ii >> 6))->values +
                      (ii & 0x3F)) * *((src_data->comp +
                                        i)->quant->values + (ii & 0x3F)) -
                     (*
                      ((stego_data->comp + i)->quant->values +
                       (ii & 0x3F)) >> 1)) / *((stego_data->comp +
                                                i)->quant->values +
                                               (ii & 0x3F));
                if (*(embed_vector + (iii >> 3)) & (1 << (iii & 0x7))) {
                    if (!(*(cover_vector + (iv >> 3)) & (1 << (iv & 0x7)))) {
                        if (value & 0x1) {
                            (*
                             (((stego_data->comp + i)->blocks +
                               (ii >> 6))->values + (ii & 0x3F))) = value;
                        } else {
                            (*
                             (((stego_data->comp + i)->blocks +
                               (ii >> 6))->values + (ii & 0x3F))) =
                               value + 1;
                        }
                    } else {
                        if (value & 0x1) {
                            (*
                             (((stego_data->comp + i)->blocks +
                               (ii >> 6))->values + (ii & 0x3F))) =
                               value + 1;
                        } else {
                            (*
                             (((stego_data->comp + i)->blocks +
                               (ii >> 6))->values + (ii & 0x3F))) = value;
                        }
                    }
                }
                iii++;
            }
            iv++;
        }
    }

    SAFE_DELETE(embed_vector);
    SAFE_DELETE(marker_vector);
    SAFE_DELETE(cover_vector);
    if (iii < encoding_bits) {
        for (ii = 0; ii < src_data->comp_num; ii++) {
            SAFE_DELETE((stego_data->comp + ii)->blocks);
            SAFE_DELETE((stego_data->comp + ii)->quant);
        }
        SAFE_DELETE(stego_data->comp);
        FAIL(LSTG_E_INSUFFCAP);
    }
    return LSTG_OK;
}

/**
 * Extracts a PQ-embedded-message from a stegonogram
 * ...
 * @param *stego_data a struct for jpegdata containing the stegonagram
 * @param *message returnpointer for the extracted message
 * @param msglen the length of the message
 * @param *param additional parameters for PQ including passphrase
 * @return an errorcode or 0 if success
 */
uint32_t pq_extract(const jpeg_data_t * stego_data, uint8_t ** message,
                    uint32_t * msglen, const pq_parameter * param)
{
    // Size of the header
    uint32_t header_size = param->header_size;
    // Size of the body
    uint32_t body_size = 0;
    // Size of the cover
    uint32_t cover_size = 0;
    // Number of message bits
    uint32_t message_bits = 0;
    // Modkey
    uint8_t modkey = 0;
    // Beta value
    double beta = 0.0;
    // Counter
    uint32_t i, ii, iii;
    uint32_t degree;
    uint8_t tmp;
    double random;

    // State pointer for PRNG for the body
    prng_state *state = NULL;
    // Cover vector of the image
    uint8_t *cover_vector = NULL;
    // Random bullshit matrix
    uint8_t **prng_matrix = NULL;
    // Password + modkey
    uint8_t *password = NULL;
    // Range vector of the RSD matrix
    double *range_vector = NULL;

    if (param->header_size == 0) {
        header_size = 64;
    }
    // Extract the message length and the modkey
    *(msglen) = 0;
    _pq_extract_head(stego_data, param, msglen, &modkey);
    if (*(msglen) == 0) {
        FAIL(LSTG_E_MALLOC);
    }
    if ((*(msglen) + 1) >= (0xFFFFFFF8 >> 3)) {
        FAIL(LSTG_E_MSGTOOLONG);
    }
    message_bits = (*(msglen) << 3);

    // Set cover and body size
    cover_size = 0;
    for (i = 0; i < stego_data->comp_num; i++) {
        cover_size += (stego_data->comp + i)->nblocks;
    }
    cover_size = (cover_size << 3);
    if (header_size == 0) {
        header_size = 64;
    }
    if (header_size > cover_size) {
        FAIL(LSTG_E_INSUFFCAP);
    }
    body_size = cover_size - header_size;

    // Calculate beta
    beta = _pq_get_beta(message_bits);

    range_vector = ALLOCN(double, message_bits);
    cover_vector = ALLOCN(uint8_t, body_size);
    prng_matrix = ALLOCN(uint8_t *, message_bits);
    password = ALLOCN(uint8_t, param->pwlen + 1);
    *(message) = ALLOCN(uint8_t, *(msglen));

    if ((!range_vector) || (!cover_vector) || (!prng_matrix) || (!password)
        || (!*(message))) {
        SAFE_DELETE(range_vector);
        SAFE_DELETE(cover_vector);
        SAFE_DELETE(prng_matrix);
        SAFE_DELETE(password);
        SAFE_DELETE(*(message));
        FAIL(LSTG_E_MALLOC);
    }

    memcpy(password, param->password, param->pwlen);
    *(password + param->pwlen) = modkey;
    memset(*(message), 0, *(msglen));
    memset(cover_vector, 0, body_size);
    random_init(&state, password, param->pwlen + 1);

    for (i = 0; i < message_bits; i++) {
        *(prng_matrix + i) = ALLOCN(uint8_t, body_size);
        if (!*(prng_matrix + i)) {
            SAFE_DELETE(range_vector);
            SAFE_DELETE(cover_vector);
            SAFE_DELETE(password);
            SAFE_DELETE(*(message));
            for (ii = 0; ii < i; ii++) {
                SAFE_DELETE(*(prng_matrix + ii));
            }
            SAFE_DELETE(prng_matrix);
            FAIL(LSTG_E_MALLOC);
        }
        memset(*(prng_matrix + i), 0, body_size);
    }

    // Create the cover vector (even=0, odd=1)
    iii = 0;
    for (i = 0; i < stego_data->comp_num; i++) {
        for (ii = 0; ii < ((stego_data->comp + i)->nblocks << 6); ii++) {
            if ((i > 0) || ((i == 0) && (ii >= (header_size << 3)))) {
                if (*
                    (((stego_data->comp + i)->blocks + (ii >> 6))->values +
                     (ii & 0x3F)) & 0x1) {
                    *(cover_vector + (iii >> 3)) |= 1 << (iii & 0x7);
                }
                iii++;
            }
        }
    }

    // Create the range vector
    for (i = 0; i < message_bits; i++) {
        if (i == 0) {
            *range_vector =
                (_pq_get_rho(message_bits, i + 1) +
                 _pq_get_tau(message_bits, i + 1)) / beta;
        } else {
            *(range_vector + i) =
                *(range_vector + (i - 1)) +
                (double) ((_pq_get_rho(message_bits, i + 1) +
                           _pq_get_tau(message_bits, i + 1)) / beta);
        }
    }

    // Create the random matrix
    for (i = 0; i < body_size * 8; i++) {
        random = (double) random_next(state) / 0xFFFFFFFF;
        degree = 0;
        for (ii = 0; ii < message_bits; ii++) {
            if (*(range_vector + ii) > random) {
                degree = ii + 1;
                break;
            }
        }
        for (ii = 0; ii < degree; ii++) {
            *(*(prng_matrix + random_next(state) % message_bits) +
              (i >> 3)) |= 1 << (i & 0x7);
        }
    }

    memset(password, 0, param->pwlen + 1);
    SAFE_DELETE(password);
    SAFE_DELETE(range_vector);
    random_cleanup(state);

    // Calculate message through matrix multiplication
    for (i = 0; i < message_bits; i++) {
        for (ii = 0; ii < body_size; ii++) {
            tmp = (*(*(prng_matrix + i) + ii) & *(cover_vector + ii));
            for (iii = 0; iii < 8; iii++) {
                *(*(message) + (i >> 3)) ^=
                    (((tmp & (1 << iii)) >> iii) << (i & 0x7));
            }
        }
    }

    // Cleanup
    SAFE_DELETE(cover_vector);
    for (i = 0; i < message_bits; i++) {
        SAFE_DELETE(*(prng_matrix + i));
    }
    SAFE_DELETE(prng_matrix);
    return LSTG_OK;
}

/**
 * Returns the length of an embedded message
 * ...
 * @param *stego_data a struct for jpegdata containing the stegonagram
 * @param *param additional parameters for PQ including passphrase
 * @return length of embedded message
 */
int32_t pq_get_message_length(const jpeg_data_t * stego_data,
                              const pq_parameter * param)
{
    uint32_t message_length = 0;
    uint8_t modkey = 0;
    if (!_pq_extract_head(stego_data, param, &message_length, &modkey)) {
        return message_length;
    } else {
        return 0;
    }
}

/**
 * Returns the capacity of a cover image
 *
 * @param *src_data a struct for jpegdata to provide the original image
 * @param *param additional parameters for PQ including passphrase
 * @param *capacity Pointer for the capacity
 * @return error code
 */
int32_t pq_check_capacity(const jpeg_data_t * src_data,
                          const pq_parameter * param, uint32_t * capacity)
{

    // Number of possible encoding bits
    uint32_t encoding_bits = 0;
    // Number of header encoding bits;
    uint32_t header_encoding_bits = 0;
    // Quality of the resulting image
    uint16_t quality = 0;
    // Scale factor
    long scale_factor = 0.0;
    // Size of the header
    uint32_t header_size = param->header_size;
    // Counter
    uint32_t i, ii, iii, iv;
    int16_t value = 0;
    int16_t base = 0;
    int32_t tmp;
    long freq = 0.0;

    // Calculate the new quality and the necessary scale factor
    if (param->quality == 0) {
        quality = (_pq_get_quality(src_data) - 50) << 1;
    } else {
        quality = param->quality;
    }
    if (header_size == 0) {
        header_size = 64;
    }
    if (quality < 50) {
        scale_factor = 5000 / quality;
    } else {
        scale_factor = 200 - (quality << 1);
    }
    encoding_bits = 0;
    header_encoding_bits = 0;
    iv = 0;
    for (i = 0; i < src_data->comp_num; i++) {
        for (ii = 0; ii < 64; ii++) {
            if (i == 0) {
                freq =
                    ((long) *(_pq_luminance_quant_tbl + ii) *
                     scale_factor + 50L) / 100L;
            } else {
                freq =
                    ((long) *(_pq_chrominance_quant_tbl + ii) *
                     scale_factor + 50L) / 100L;
            }
            if (freq <= 0L)
                freq = 1L;
            //if (freq > 32767L) freq = 32767L;
            if (freq > 255L)
                freq = 255L;
            base =
                freq / (2 *
                        _pq_get_ggt(freq,
                                    *((src_data->comp + i)->quant->values +
                                      ii)));
            if ((base % 2) == 0) {
                for (tmp = -254; tmp < 255; tmp++) {
                    value = (int16_t) (2 * tmp + 1) * base;
                    for (iii = 0; iii < (src_data->comp + i)->nblocks;
                         iii++) {
                        if (*
                            (((src_data->comp + i)->blocks + iii)->values +
                             ii) == value) {
                            if ((iv + (iii << 6) + ii) >=
                                (header_size << 3)) {
                                encoding_bits++;
                            } else {
                                header_encoding_bits++;
                            }
                        }
                    }
                }
            }
        }
        iv += (src_data->comp + i)->nblocks << 6;
    }
    if (header_encoding_bits >= _pq_get_header_message_bits(src_data)) {
        encoding_bits =
            (uint32_t) floor((double) encoding_bits /
                             _pq_get_beta(encoding_bits));
    } else {
        encoding_bits = 0;
    }
    *(capacity) = (encoding_bits >> 3);
    return LSTG_OK;
}

/**
 * Extracts the header of the stego image
 *
 * @param *stego_data
 * @param *param PQ Parameter including the password
 * @param *message_length Pointer for the length of the embeded message
 * @param *modkey Pointer for the modkey for the required password
 * @return error code
 */
static uint32_t _pq_extract_head(const jpeg_data_t * stego_data,
                                 const pq_parameter * param,
                                 uint32_t * message_length,
                                 uint8_t * modkey)
{
    uint32_t header_size = param->header_size;
    uint32_t message_bits = _pq_get_header_message_bits(stego_data);
    // Counter
    int32_t i, ii, iii;
    uint8_t tmp = 0;
    // State pointer for PRNG
    prng_state *state = NULL;
    // Cover parity vector
    uint8_t *cover_vector = NULL;
    // Random bullshit matrix
    uint8_t **prng_matrix = NULL;

    if (header_size == 0) {
        header_size = 64;
    }
    *message_length = 0;
    *modkey = 0;


    cover_vector = ALLOCN(uint8_t, header_size);
    prng_matrix = ALLOCN(uint8_t *, message_bits);
    if ((!cover_vector) || (!prng_matrix)) {
        SAFE_DELETE(cover_vector);
        SAFE_DELETE(prng_matrix);
        FAIL(LSTG_E_MALLOC);
    }
    memset(cover_vector, 0, header_size);

    // Init random function
    random_init(&state, param->password, param->pwlen);

    // Create the random matrix
    for (i = 0; i < message_bits; i++) {
        *(prng_matrix + i) = ALLOCN(uint8_t, header_size);
        if (!*(prng_matrix + i)) {
            for (ii = 0; ii < i; ii++) {
                SAFE_DELETE(*(prng_matrix + ii));
            }
            SAFE_DELETE(prng_matrix);
            SAFE_DELETE(cover_vector);
            random_cleanup(state);
            FAIL(LSTG_E_MALLOC);
        }
        memset(*(prng_matrix + i), 0, header_size);
        for (ii = 0; ii < (header_size << 3); ii++) {
            *(*(prng_matrix + i) + (ii >> 3)) |=
                ((random_next(state) & 0x1) ? 1 : 0) << (ii & 0x7);
        }
    }

    // Create the cover vector (even=0 odd=1)
    iii = 0;
    for (i = 0; i < stego_data->comp_num; i++) {
        for (ii = 0;
             (ii < ((stego_data->comp + i)->nblocks << 6))
             && (iii < (header_size * 8)); ii++) {
            if (*
                (((stego_data->comp + i)->blocks + (ii >> 6))->values +
                 (ii & 0x3F)) & 0x1) {
                *(cover_vector + (iii >> 3)) |= 1 << (iii & 0x7);
            }
            iii++;
        }
    }

    // Calculate the message length and the modkey through matrix multiplication
    for (i = 0; i < message_bits; i++) {
        for (ii = 0; ii < header_size; ii++) {
            tmp = (*(*(prng_matrix + i) + ii) & *(cover_vector + ii));
            for (iii = 0; iii < 8; iii++) {
                // The first 5 bits are the modkey
                if (i < 5) {
                    *modkey ^= (((tmp & (1 << iii)) >> iii) << (i));
                } else {
                    *message_length ^=
                        (((tmp & (1 << iii)) >> iii) << (i - 5));
                }
            }
        }
    }
    // Cleanup
    SAFE_DELETE(cover_vector);
    for (i = 0; i < message_bits; i++) {
        SAFE_DELETE(*(prng_matrix + i));
    }
    SAFE_DELETE(prng_matrix);
    random_cleanup(state);
    return LSTG_OK;
}

/**
 * Return greatest common divisor for two numbers
 *
 * @param a
 * @param b
 * @return ggt
 */
static int32_t _pq_get_ggt(int16_t a, int16_t b)
{
    int32_t temp;
    if (a < b) {
        temp = a;
        a = b;
        b = temp;
    }
    while (a % b != 0) {
        temp = b;
        b = a % b;
        a = temp;
    }
    return b;
}

/**
 * Return tau for each bit in the RSD matrix
 *
 * delta and c are fixed (see paper for more details)
 * @param length Number of message bits
 * @param index Index of the bit
 * @return tau
 */
static double _pq_get_tau(uint32_t length, uint32_t index)
{
    double delta = 5.0;
    double c = 0.1;
    double R = 0.0;
    double tau = 0.0;

    R = c * log(length / delta) * sqrt(length);

    if (index == (uint32_t) (length / R)) {
        tau = (R * log(R / delta)) / length;
    } else if ((index > 0) || (index < (uint32_t) (length / R))) {
        tau = R / (index * length);
    } else {
        tau = 0.0;
    }
    return tau;
}

/**
 * Return rho for each bit in the RSD matrix
 *
 * @param length Number of message bits
 * @param index Index of the bit
 * @return rho
 */
static double _pq_get_rho(uint32_t length, uint32_t index)
{
    double rho = 0.0;
    if (index == 1) {
        rho = (double) 1 / length;
    } else {
        rho = (double) 1 / (index * (index - 1));
    }
    return rho;
}

/**
 * Return beta for RSD matrix
 *
 * @param length Number of message bits
 * @return beta
 */
static double _pq_get_beta(uint32_t length)
{
    uint32_t i;
    double beta = 0.0;
    for (i = 1; i <= length; i++) {
        beta += _pq_get_tau(length, i) + _pq_get_rho(length, i);
    }
    return beta;
}

/**
 * Gauss elimation algorithm
 *
 * @param **matrix
 * @param *vector
 * @param size Height of the matrix
 * @return result vector
 */
uint8_t *_pq_gauss(uint8_t ** matrix, uint8_t * vector, uint32_t size)
{
    int32_t i, ii, iii, iv;
    uint32_t result_size = size >> 3;
    if (size & 0x7)
        result_size++;
    uint8_t *result = ALLOCN(uint8_t, result_size);
    uint8_t *tmp_ptr = NULL;
    int32_t tmp_val = 0;
    if ((!result) || (!matrix) || (!vector)) {
        return NULL;
    }
    memset(result, 0, result_size);

    for (i = 0; i <= size; i++) {
        ii = i;
        while ((ii < size)
               && (!(*(*(matrix + ii) + (i >> 3)) & (1 << (i & 0x7))))) {
            ii++;
        }
        if (ii == size) {
            break;
        }
        tmp_ptr = *(matrix + i);
        *(matrix + i) = *(matrix + ii);
        *(matrix + ii) = tmp_ptr;

        tmp_val = (*(vector + (i >> 3)) & (1 << (i & 0x7))) >> (i & 0x7);
        *(vector + (i >> 3)) =
            (*(vector + (i >> 3)) & (~(1 << (i & 0x7)))) |
            (((*(vector + (ii >> 3)) & (1 << (ii & 0x7))) >> (ii & 0x7)) <<
             (i & 0x7));
        *(vector + (ii >> 3)) =
            (*(vector + (ii >> 3)) & (~(1 << (ii & 0x7)))) | (tmp_val <<
                                                              (ii & 0x7));

        for (iii = 0; iii < size; iii++) {
            if ((iii != i)
                && (*(*(matrix + iii) + (i >> 3)) & (1 << (i & 0x7)))) {
                for (iv = (i >> 3); iv < (size >> 3); iv++) {
                    *(*(matrix + iii) + iv) ^= *(*(matrix + i) + iv);
                }
                *(vector + (iii >> 3)) ^=
                    ((*(vector + (i >> 3)) & (1 << (i & 0x7))) >>
                     (i & 0x7)) << (iii & 0x7);
            }
        }
    }
    for (i = 0; i < result_size; i++) {
        *(result + i) = *(vector + i);
    }

    return result;
}

/**
 * Returns the quality for the stego cover
 *
 * @param *src_data a struct for jpegdata to provide the original image
 * @return quality of the stego cover
 */
static uint16_t _pq_get_quality(const jpeg_data_t * src_data)
{
    return (200 -
            (((*(src_data->comp->quant->values) * 100) -
              50) / *(_pq_luminance_quant_tbl))) >> 1;
}

/**
 * Returns the nessary number of message_bits for the header
 *
 * @param *src_data a struct for jpegdata to provide the original image
 * @return number of message bits for the header
 */
static uint32_t _pq_get_header_message_bits(const jpeg_data_t * src_data)
{
    uint32_t i;
    uint32_t message_bits = 0;
    for (i = 0; i < src_data->comp_num; i++) {
        message_bits += (src_data->comp + i)->nblocks;
    }
    message_bits = log(message_bits << 6);

    return message_bits;
}

/**
 * Solves the right hand side of the embedding equation:
 *
 * message + prng_matrix cover
 *
 * message is a message_bitsx1 vector
 * prng_matrix is a message_bitsxencoding_bits matrix
 * cover_vector is a (cover_size*8)x1 vector
 *
 * @param *message_vector a struct for jpegdata to provide the original image
 * @param **prng_matrix additional parameters for PQ including passphrase
 * @param *cover_vector binary (cover_size*8)x1 vector with the parity of each DCT-Coefficient
 * @param cover_size Size (in bytes) of the cover_vector
 * @param message_bits Height of the prng_matrix
 * @param encoding_bits Size (in bits) of the resulting vector
 * @return result vector of the equation
 */
static uint8_t *_pq_get_rhs_vector(uint8_t * message_vector,
                                   uint8_t ** prng_matrix,
                                   uint8_t * cover_vector,
                                   uint32_t cover_size,
                                   uint32_t message_bits,
                                   uint32_t encoding_bits)
{
    uint8_t *result = NULL;
    uint8_t tmp;
    uint32_t i, ii, iii;
    uint32_t message_size, encoding_size;

    message_size = message_bits >> 3;
    if (message_bits & 0x7)
        message_size++;
    encoding_size = encoding_bits >> 3;
    if (encoding_bits & 0x7)
        encoding_size++;
    result = ALLOCN(uint8_t, encoding_size);
    if ((!result) || (!message_vector) || (!prng_matrix)
        || (!cover_vector)) {
        return NULL;
    }
    memset(result, 0, encoding_size);

    // D * b_x
    for (i = 0; i < message_bits; i++) {
        for (ii = 0; ii < cover_size; ii++) {
            tmp = (*(*(prng_matrix + i) + ii) & *(cover_vector + ii));
            for (iii = 0; iii < 8; iii++) {
                *(result + (i >> 3)) ^=
                    (((tmp & (1 << iii)) >> iii) << (i & 0x7));
            }
        }
    }

    // s - D * b_x
    for (i = 0; i < encoding_size; i++) {
        if (i < message_size) {
            *(result + i) ^= *(message_vector + i);
        } else {
            *(result + i) = 0;
        }
    }

    return result;
}
