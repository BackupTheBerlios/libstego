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
 
 
 
#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>


#ifndef NULL
    #define NULL (void *) (0)
#endif

/**
 * Data structure for LSB. Contains an array of bytes.
 */
typedef struct {
	/** The cover data, as an array of bytes */
	uint8_t *data;
	/** The size of the cover data (number of bytes) */
	uint32_t size;
} lsb_data_t;

/**
 * Data structure for one RGB pixel, as used in raster graphic formats, such as
 * BMP or PNG.
 */
typedef union {
    /**
     * The colour values as an array, mapped to the same memory as the
     * components.
     */
    uint8_t rgb[3];
    /** The colour is made accessible by component by this struct. */
    struct {
	/** Red colour component */
	uint8_t r;
	/** Green colour component */
	uint8_t g;

	/** Blue colour component */
	uint8_t b;
    } comp;
} rgb_pixel_t;


/**
 * Data struture for a whole RGB image, as used in raster graphic formats, such
 * as BMP or PNG.
 */
typedef struct {
    /** Pixel data of the image. */
    rgb_pixel_t *data;

    /** Horizontal size of the image (X-axis). */
    uint32_t size_x;

    /** Vertical size of the image (Y-axis). */
    uint32_t size_y;
} rgb_data_t;

/**
 * Data structure for a WAVE-file.
 */
typedef struct {
    /** The Sampledata representing an audiosignal*/
    int16_t **data;
    /** The number of channels of the audiodata.
     * for mono it is 1 for stereo it is 2*/
    uint8_t num_channels;
    /** The number of Samples per channel*/
    uint32_t size;
} wav_data_t;

/**
 * Data structure for a palette image, using a 'palette and indexed graphics'
 * format.
 */
typedef struct {
    /** Colour table (palette). */
    rgb_pixel_t *table;

    /** Size of the table. */
    uint32_t tbl_size;

    /** Unique Colors from palette. */
    rgb_pixel_t *unique_colors;

    /** Amount of unique colours in palette. */
    uint32_t unique_length;

    /** Non unique colours in palette. */
    rgb_pixel_t *nonunique_colors;

    /** Amount of non unique colours in palette */
    uint32_t nonunique_length;

    /** Image data, saved as indices into the colour table. */
    uint8_t *img_data;

    /** Horizontal size of the image (X-axis). */
    uint32_t size_x;

    /** Vertical size of the image (Y-axis). */
    uint32_t size_y;
} palette_data_t;

/**
 * Data structure for a jpeg quantization table with 8x8 integers.
 *
 */
typedef struct {
    uint16_t values[64];
} jpeg_quant_t;

/**
 * Data structure for a single 8x8 Block of JPEG coefficients
 *
 */
typedef struct {
    int16_t values[64];
} jpeg_block_t;

/**
 * Data structure for a jpeg component consisting of a quantisation table
 * and a number of blocks
 *
 */
typedef struct {
    jpeg_quant_t *quant;
    jpeg_block_t *blocks;
    uint32_t nblocks;
} jpeg_comp_t;

/**
 * Data structure for a jpeg image with with a number
 * of jpeg_matrices and a x- and y-size.
 */
typedef struct {
    /** Number of components (1 = gray scale, 3 = color) */
    uint8_t comp_num;

    /** Components tables */
    jpeg_comp_t *comp;

    /** Width of the image in blocks (X-axis). */
    uint32_t size_x;

    /** Height of the image in blocks (Y-axis). */
    uint32_t size_y;

} jpeg_data_t;

/**
 * Data structure for a 1bit-color-image.
 *
 */
typedef struct {
    /** Bit data of the 1bit-color-image. */
    uint8_t *data;

    /** Horizontal size of the image (X-axis). */
    uint32_t size_x;

    /** Vertical size of the image (Y-axis). */
    uint32_t size_y;
} cpt_data_t;


/**
 * Data structure for an SVG attribute. Used within svg_data_t.
 */
typedef struct {
    /** The content of the attribute (null terminated string) */
    uint8_t *data;
    /** The xmlNodePtr pointing to the node this attribute is part of. */
    void *node;
} svg_attrib_t;

/**
 * Data structure for the SVG algorithm.
 */
typedef struct {
    /** A list of attributes */
    svg_attrib_t *attributes;
    /** The number of attributes */
    uint32_t num_attribs;
} svg_data_t;


/**
* Data structure for BattleSteg-Parameter
*/
typedef struct {
    /** the most significant bit to embed. BattleSteg embeds in this
     *  and all lesser significant bit. The more bits are used the
     *  higher is the capacity but the lower is the security.
     *  Default: 6 to embed in the two least significant bits
     *  Possible values: 1 til 7 with 7 as maximum security
     */
    uint32_t startbit;

    /**
     *  Filteralgorithm, 0 for Laplace. Still not in use because only
     *  Laplace is implemented yet.
     */
    uint32_t filter;
    /** How much ranged shot will be done before move away to do a
     *  normal shot.
     *  Default: 3
     *  Possible values: 0 til 10 (more are possible but not wise)
     */
    uint32_t move_away;
    /** The range of the ranged shots
     *  Default: 5
     *  Possible values: 1 til maxint (not wise)
     */
    uint32_t range;
    /** Save the last hit for ranged shots, only for internal use
     */
    int32_t last_hit_x;
    int32_t last_hit_y;
    /** The password as chararray
     */
    uint8_t *password;
    /** The length of the password
     */
    uint32_t pwlen;

} battlesteg_parameter;

/**
* Data structure for CPT-Parameter
* Block width and height don't have to be equal
* Values between 2 and 8 are recommended
* with 4 being a good default value.
* a value of 1 would essentially turn cpt into
* a plain lsb algorithm, higher values decrease the
* capacity
*/
typedef struct {
    uint8_t *password;
    uint8_t pwlen;
    uint8_t block_width;
    uint8_t block_height;
} cpt_parameter;

/**
* Data structure for Echo-Hiding-Parameter
*/
typedef struct {
    /** The amplitude of the integrated echo.
     * 0<amplitude<=1
     * For default use 0.4.
     */
    float amplitude;
    /** The delay of the echo that represents a zero.
     * This value shows after how much samples the
     * echo is apllied to the original signal.
     * For a WAV-data with 44100 Hz, which is standard,
     * the value should be lesser than 2000.
     * For default use 1000.
     */
    uint32_t zero_offset;
    /** The delay of the echo that represents a one.
     * This value shows after how much samples the
     * echo is apllied to the original signal.
     * It must be greater than the zero_offset.
     * For a WAV-data with 44100 Hz, which is standard,
     * the value should be lesser than 2000.
     * For default use 1500.
     */
    uint32_t one_offset;
    /** The number of samples per channel, in that
     * a bit is embedded. It must be large enough to contain
     * the echos.
     * The value should be greater than the one_offset
     * For default use 2500.
     */
    uint32_t block_size;
} eh_parameter;

/**
* Data structure for F5-Parameter
* @todo implement
*/
typedef struct {
    uint8_t *password;
    uint32_t pwlen;
} f5_parameter;

/**
* Data structure for FriRui-Parameter
* @todo implement
*/
typedef struct {
    uint32_t method;
    uint32_t size;
    uint32_t threshold;
    uint8_t *password;
    uint32_t pwlen;
} frirui_parameter;

/**
* Data structure for GifShuffle-Parameter
* method be 1 if normal gifshuffle is used
* or 2 if used for the second time in adv gifshuffle
* @todo implement
*/
typedef struct {
    uint32_t method;
    uint8_t *password;
    uint32_t pwlen;
    palette_data_t *adv_pal;
} gifshuffle_parameter;

/**
* Data structure for MB2-Parameter
* @todo implement
*/
typedef struct {
} mb2_parameter;

/**
* Data structure for PDFShuffle-Parameter
* @todo implement
*/
typedef struct {
} pdf_parameter;

/**
* Data structure for Phasecoding-Parameter
*/
typedef struct {
    /** The audiosignal is divided into
     * parts with the the amount of
     * segment_size samples.
     * No Suggested Defaultvalue yet.
     */
    uint32_t segment_size;
} phasecoding_parameter;

/**
* Data structure for PQ-Parameter
* @todo implement
*/
typedef struct {
    //Password
    uint8_t *password;
    //Length of the password
    uint32_t pwlen;
    //Size of the PQ Header
    //Depends on the cover image
    uint32_t header_size;
    //Quality of the resulting image
    uint32_t quality;
} pq_parameter;

/**
* Data structure for Sort/Unsort-Parameter
* @todo implement
*/
typedef struct {
    uint8_t *password;
    uint32_t pwlen;
} sortunsort_parameter;

/**
* Data structure for T-Parameter
* @todo implement
*/
typedef struct {
} t_parameter;

typedef struct {
} pc_parameter_t;



enum LSB_SELECT_MODE {
    LSB_SELECT_CONTINUOUS = 1,
    LSB_SELECT_RANDOM
};

/**
 * Data structure for LSB-Parameter
 */
typedef struct {
    /** Password to be used as the seed for the random generator */
    uint8_t *password;
    /** Length of the password in bytes */
    uint32_t pwlen;

    /**
     * Function to use when selecting bytes from the cover to embed in.
     * Has to be one of LSB_SELECT_CONTINUOUS (=1) or LSB_SELECT_RANDOM (=2).
     */
    uint32_t select_mode;

    /**
     * @note ATTENTION! FOR DEMONSTRATION ONLY! Makes LSB COMPLETELY USELESS for steganography!
     * For demonstration only, makes LSB actually embed into the _most_
     * significant bit.
     */
    uint32_t use_msb;
} lsb_parameter_t;

/**
 * Data structure for SVG-parameter
 */
typedef struct {
    /**
     * Denotes the position first after-the-decimal-point digit to embed in.
     * For example, the number to embed in is 2.8765432 and first_embed_digit is
     * 4, then the message will be embedded in the least significant bits of the
     * digits 5, 4, 3 and 2. Higher places mean a more securely embedded message,
     * but also less space to embed in.
     * Values begin at 1 (first digit after the decimal point). The highest
     * number depends on the number of decimal places, but usually there are
     * only 8 to 10 decimal places. A sensible default is 4.
     */
    uint32_t first_embed_digit;

    /** Password to be used as the seed for the random generator */
    uint8_t *password;

    /** Length of the password in bytes */
    uint32_t pwlen;
} svg_parameter_t;

#endif // __TYPES_H__
