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
 * Author: Marko Krause <zeratul2099@googlemail.com>
 *
 *
 *  For more info visit <http://parsys.informatik.uni-oldenburg.de/~stego/>
 */
 
 
 
#include "libstego/png.h"


/**
* Function to read a png-file from HDD and return a rgb_data_t
* @param *filename the path and filename of the png-file
* @param *rgb_data a struct to return the pixeldata
* @param *png_internal_data_t a struct to hold the original image structures provided by the libpng
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t io_png_read(const uint8_t *filename, rgb_data_t *rgb_data, png_internal_data_t *png_internal)
{
    // Open the file
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        // File not found
        FAIL(LSTG_E_OPENFAILED);
    }
    uint8_t header[8];
    uint32_t number = 8;
    // read out the first byte to verify that this is a png-file
    fread(header, 1, number, fp);
    uint32_t is_png = !png_sig_cmp(header, 0, number);
    if (!is_png) {
        // File no valid png
        fclose(fp);
        FAIL(LSTG_E_READFAILED);
    }

    // create png_struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        FAIL(LSTG_E_EXTLIB);
    }

    // create png_info
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        FAIL(LSTG_E_EXTLIB);
    }

    // put file into the png_struct
    png_init_io(png_ptr, fp);
    // let libpng know that we already read the first byte of the file
    png_set_sig_bytes(png_ptr, number);
    // Read the image with no transformations
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    // Read image size an print out
    uint32_t width = png_get_image_width(png_ptr, info_ptr);
    uint32_t height = png_get_image_height(png_ptr, info_ptr);
    uint32_t bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    //printf("Width: %d\nHeight: %d\nBit Depth: %d\n", width, height, bit_depth);
    uint32_t color_type = png_get_color_type(png_ptr, info_ptr);
    uint8_t if_alpha = 0;
    // Convert color type or bit depth if necessary
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        TO8BIT(png_ptr);
    }
    else if (color_type == PNG_COLOR_TYPE_RGB) {

    }
    else if (bit_depth == 16) {
        png_set_strip_16(png_ptr);
    }
    else if (color_type & PNG_COLOR_MASK_ALPHA) {
        if_alpha = 1;
    }
    else if (color_type != PNG_COLOR_TYPE_RGB || color_type != PNG_COLOR_TYPE_RGB_ALPHA || bit_depth != 8) {
        // No supported png
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        fclose(fp);
        FAIL(LSTG_E_FRMTNOTSUPP);
    }
    // Write size to the rgb_data-structure
    rgb_data->size_x = width;
    rgb_data->size_y = height;
    // Allocate memory for the rgb_data-structure
    rgb_data->data = (rgb_pixel_t*)malloc(sizeof(rgb_pixel_t)*width*height);
    if (rgb_data->data == NULL) {
        // malloc failed, panic!
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        fclose(fp);
        FAIL(LSTG_E_MALLOC);
    }
    // Get rows
    png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
    uint32_t row, column;


    // Write pixeldata to the rgb_data-structure
    for (row = 0; row < height; row++) {
        for (column = 0; column < width; column++) {
            // Red
            rgb_data->data[column+(row*width)].rgb[0] = row_pointers[row][(3+if_alpha)*column];
            // Green
            rgb_data->data[column+(row*width)].rgb[1] = row_pointers[row][(3+if_alpha)*column+1];
            // Blue
            rgb_data->data[column+(row*width)].rgb[2] = row_pointers[row][(3+if_alpha)*column+2];
        }
    }

    // Also return the png_ptr-structure
    png_internal->png_struct = png_ptr;
    png_internal->info_struct = info_ptr;
    fclose(fp);


    return LSTG_OK;
}

/**
* Function to integrate a changed rgb_data_t into a png_struct
* @param *png_internal_data_t a struct to hold the original image structures provided by the libpng
* @param *rgb_data a struct to give the changed pixeldata (steganogram)
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t io_png_integrate(png_internal_data_t *png_internal, const rgb_data_t *rgb_data)
{
    // Cast png-struct
    png_structp png_ptr = png_internal->png_struct;
    // Cast png_info
    png_infop info_ptr = png_internal->info_struct;
    // Get row pointers
    png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
    if (rgb_data->size_x != info_ptr->width || rgb_data->size_y != info_ptr->height) {
       // Dimensions not equal
        FAIL(LSTG_E_SIZEMISMATCH);
    }
    // case if image is RGBA istead of RGB
    uint32_t color_type = png_get_color_type(png_ptr, info_ptr);
    uint8_t if_alpha = 0;
    if (color_type & PNG_COLOR_MASK_ALPHA) {
        if_alpha = 1;
    }
    uint32_t width, height, row, column;
    width = info_ptr->width;
    height = info_ptr->height;
    // Write the rgb_data to png-structure
    for (row = 0; row < height; row++) {
        for (column = 0; column < width; column++) {

            // Red
            row_pointers[row][(3+if_alpha)*column] = rgb_data->data[column+(row*width)].rgb[0];
            // Green
            row_pointers[row][(3+if_alpha)*column+1] = rgb_data->data[column+(row*width)].rgb[1];
            // Blue
            row_pointers[row][(3+if_alpha)*column+2] = rgb_data->data[column+(row*width)].rgb[2];
        }
    }
    return LSTG_OK;
}

/**
* Function to write a png_struct to HDD by using the png-library
* @param *filename the path and filename of the png-file
* @param *png_internal_data_t a struct to hold the original image structures provided by the libpng
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t io_png_write(const uint8_t *filename, png_internal_data_t *png_internal)
{
    // Cast png-struct
    png_structp png_ptr = png_internal->png_struct;

    // Cast png_info
    png_infop info_ptr = png_internal->info_struct;

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        FAIL(LSTG_E_EXTLIB);
    }

    // Create row-pointers
    png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);

    // Create png_write-pointer
    png_structp png_ptr_wrt = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr_wrt) {
        FAIL(LSTG_E_EXTLIB);
    }
    // Open file to write
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        // Open file error
        png_destroy_write_struct(&png_ptr_wrt, (png_infopp)NULL);
        FAIL(LSTG_E_OPENFAILED);
    }
    // Open file stream
    png_init_io(png_ptr_wrt, fp);
    // Write info-struct
    png_write_info(png_ptr_wrt, info_ptr);
    // Write pixeldata
    png_write_image(png_ptr_wrt, row_pointers);
    // Write end
    png_write_end(png_ptr_wrt, end_info);
    // Cleaning up
    fclose(fp);
    png_destroy_write_struct(&png_ptr_wrt, &end_info);
    return LSTG_OK;
}



/**
* Destroys a rgb_data_t structure
* @param *rgb_data the struct to destroy
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t io_png_cleanup_data(rgb_data_t *rgb_data)
{
    if (!rgb_data) {
        FAIL(LSTG_E_INVALIDPARAM);
    }
    free(rgb_data->data);

    return LSTG_OK;
}

/**
* Destroys a png_internal_data_t structure
* @param *png_internal the struct to destroy
* @return LSTG_ERROR if error or LSTG_OK if success
*/
uint32_t io_png_cleanup_internal(png_internal_data_t *png_internal)
{
    if (!png_internal->png_struct || !png_internal->info_struct) {
        FAIL(LSTG_E_INVALIDPARAM);
    }
    png_destroy_read_struct(&png_internal->png_struct, &png_internal->info_struct, (png_infopp)NULL);
    return LSTG_OK;
}
