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




#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <jpeglib.h>
#include <string.h>
#include <setjmp.h>
#include "libstego/errors.h"
#include "libstego/utils.h"
#include "libstego/jpeg.h"

static long jround_up(long a, long b) {
    a += b - 1L;
    return a - (a % b);
}

struct io_jpeg_error_mgr {
    struct jpeg_error_mgr pub; /* "public" fields */

    jmp_buf setjmp_buffer; /* for return to caller */
};

typedef struct io_jpeg_error_mgr * io_jpeg_error_ptr;

void io_jpeg_error_exit(j_common_ptr cinfo) {
    io_jpeg_error_ptr err = (io_jpeg_error_ptr) cinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message) (cinfo);

    /* Return control to the setjmp point */
    longjmp(err->setjmp_buffer, 1);
}

/**
 * Function to read a jpeg-image from a file and return a jpeg_data_t
 * ...
 * @param *filename the path and filename of the jpeg-file
 * @param *jpeg_data a struct to return the jpegdata
 * @param *jpeg_struct a struct provided by the jpeg-library to hold the original image
 * @return an errorcode or 0 if success
 */
uint32_t io_jpeg_read(const uint8_t *filename, jpeg_data_t *jpeg_data,
        jpeg_internal_data_t *jpeg_struct) {
    //Damn 48 lines-> optimize!
    struct io_jpeg_error_mgr jerr;
    jvirt_barray_ptr *coef_array;
    JBLOCKARRAY coef_buffer;
    FILE *file;
    jpeg_struct->cinfo = NULL;
    jpeg_struct->dinfo = SALLOC(jpeg_decompress_struct);
    jpeg_struct->dinfo->err = jpeg_std_error(&jerr.pub);

    if ((file = fopen(filename, "rb")) == NULL) {
        FAIL(LSTG_E_OPENFAILED);
    }
    jpeg_create_decompress(jpeg_struct->dinfo);

    jpeg_stdio_src(jpeg_struct->dinfo, file);
    jpeg_read_header(jpeg_struct->dinfo, TRUE);

    coef_array = jpeg_read_coefficients(jpeg_struct->dinfo);

    jpeg_data->comp_num = jpeg_struct->dinfo->num_components;
    jpeg_data->comp = malloc(sizeof (jpeg_comp_t) * jpeg_struct->dinfo->num_components);
    jpeg_data->size_x = jpeg_struct->dinfo->image_width;
    jpeg_data->size_y = jpeg_struct->dinfo->image_height;

    for (JDIMENSION components = 0; components < jpeg_struct->dinfo->num_components; components++) {
        jpeg_comp_t *comp = &jpeg_data->comp[components];
        jpeg_component_info *compinfo = (jpeg_struct->dinfo->comp_info + components);
        JDIMENSION nblocks = compinfo->width_in_blocks * compinfo->height_in_blocks;
        comp->blocks = malloc(sizeof (jpeg_block_t) * nblocks);
        jpeg_quant_t *quant = (jpeg_quant_t*) malloc(sizeof (jpeg_quant_t));
        memcpy(quant, compinfo->quant_table->quantval, sizeof (jpeg_quant_t));
        for (JDIMENSION row_num = 0; row_num < compinfo->height_in_blocks; row_num++) {
            coef_buffer = (jpeg_struct->dinfo->mem->access_virt_barray)
                    ((j_common_ptr) jpeg_struct->dinfo, coef_array[components], row_num, (JDIMENSION) 1, FALSE);
            JBLOCKROW block_row = coef_buffer[0];
            for (JDIMENSION block_num = 0; block_num < compinfo->width_in_blocks; block_num++) {
                memcpy(&comp->blocks[(row_num * compinfo->width_in_blocks) + block_num].values,
                        &block_row[block_num], sizeof (jpeg_block_t));
            }

        }
        comp->quant = quant;
        comp->nblocks = nblocks;
    }
    fclose(file);

    return LSTG_OK;
}

/**
 * Function to integrate a changed jpeg_data_t into a jpeg_struct
 * ...
 * @param *jpeg_struct a struct provided by the jpeg-library to hold the original image
 * @param *jpeg_data a struct to give the changed jpegdata (steganogram)
 * @return an errorcode or 0 if success
 */
uint32_t io_jpeg_integrate(jpeg_internal_data_t *jpeg_struct, const jpeg_data_t *jpeg_data) {
    FILE *file;
    struct io_jpeg_error_mgr jerr;
    jpeg_component_info *compptr = NULL;
    jvirt_barray_ptr *coef_buffer = NULL;
    unsigned int quant_table[DCTSIZE];
    JQUANT_TBL *quant_table_ptr = NULL;
    if ((!jpeg_struct) || (!jpeg_data)) {
        return FALSE;
    }
    if (jpeg_struct->cinfo) {
        // Delete old compress info structure
        jpeg_destroy_compress(jpeg_struct->cinfo);
        SAFE_DELETE(jpeg_struct->cinfo);

    }
    if ((file = tmpfile()) == NULL) {
        return FALSE;
    }

    jpeg_struct->cinfo = SALLOC(jpeg_compress_struct);
    jpeg_struct->cinfo->err = jpeg_std_error(&jerr.pub);
    jpeg_create_compress(jpeg_struct->cinfo);
    jpeg_copy_critical_parameters(jpeg_struct->dinfo, jpeg_struct->cinfo);
    jpeg_stdio_dest(jpeg_struct->cinfo, file);

    coef_buffer = (jvirt_barray_ptr *) (jpeg_struct->cinfo->mem->alloc_small)
            ((j_common_ptr) jpeg_struct->cinfo, JPOOL_IMAGE, sizeof (jvirt_barray_ptr) * jpeg_data->comp_num);

    for (int i = 0; i < jpeg_struct->cinfo->num_components; i++) {
        compptr = (jpeg_struct->dinfo->comp_info + i);

        JDIMENSION width = (JDIMENSION) jround_up((long) compptr->width_in_blocks,
                (long) compptr->h_samp_factor);
        JDIMENSION height = (JDIMENSION) jround_up((long) compptr->height_in_blocks,
                (long) compptr->v_samp_factor);


        *(coef_buffer + i) = (jpeg_struct->cinfo->mem->request_virt_barray)
                ((j_common_ptr) jpeg_struct->cinfo, JPOOL_IMAGE, false, width, height,
                (JDIMENSION) compptr->v_samp_factor);
    }


    for (int i = 0; i < NUM_QUANT_TBLS; i++) {
        quant_table_ptr = jpeg_struct->cinfo->quant_tbl_ptrs[i];
        if (quant_table_ptr) {

            // Prepare buffers for each JPEG-component
            for (int ii = 0; ii < DCTSIZE2; ii++) {
                *(quant_table_ptr->quantval + ii) = (unsigned int) *((jpeg_data->comp + i)->quant->values + ii);
            }
        }
    }
    jpeg_write_coefficients(jpeg_struct->cinfo, coef_buffer);
    JBLOCKARRAY buf;
    for (int i = 0; i < jpeg_struct->dinfo->num_components; i++) {
        compptr = (jpeg_struct->dinfo->comp_info + i);
        for (int row = 0; row < compptr->height_in_blocks; row++) {
            buf = (jpeg_struct->cinfo->mem->access_virt_barray)
                    ((j_common_ptr) jpeg_struct->cinfo, *(coef_buffer + i), row, (JDIMENSION) 1, true);
            for (int block = 0; block < compptr->width_in_blocks; block++) {
                for (int ii = 0; ii < DCTSIZE2; ii++) {
                    buf[0][block][ii] = *(((jpeg_data->comp + i)->blocks + block + row * compptr->width_in_blocks)->values + ii);
                }
            }
        }
    }

    jpeg_finish_decompress(jpeg_struct->dinfo);
    jpeg_destroy_decompress(jpeg_struct->dinfo);
    SAFE_DELETE(jpeg_struct->dinfo);
    fclose(file);

}

/**
 * Function to write a jpeg_struct to a file by using the jpeg-library
 * ...
 * @param *filename the path and filename of the jpeg-file
 * @param *jpeg_struct a struct provided by the jpeg-library to hold the original image
 * @return an errorcode or 0 if success
 */
uint32_t io_jpeg_write(const uint8_t *filename, jpeg_internal_data_t *jpeg_struct) {
    FILE *file = NULL;
    if (!jpeg_struct) {
        return FALSE;
    }
    if ((file = fopen(filename, "wb")) == NULL) {
        return FALSE;
    }
    jpeg_stdio_dest(jpeg_struct->cinfo, file);
    jpeg_finish_compress(jpeg_struct->cinfo);
    fclose(file);
    jpeg_destroy_compress(jpeg_struct->cinfo);
}

uint32_t io_jpeg_copy_internal(jpeg_internal_data_t *target,
        const jpeg_internal_data_t *source) {
    if ((target) && (source)) {
        memcpy(target, source, sizeof (jpeg_internal_data_t));
    }
}

uint32_t io_jpeg_cleanup_data(jpeg_data_t *data) {
    uint32_t i;
    if (data) {
        for (i = 0; i < data->comp_num; i++) {
            SAFE_DELETE((data->comp + i)->quant);
            SAFE_DELETE((data->comp + i)->blocks);
        }
        SAFE_DELETE(data->comp);
    }
}

uint32_t io_jpeg_cleanup_internal_data(jpeg_internal_data_t *data) {
    if (data) {
        if (data->cinfo) {
            jpeg_destroy_compress(data->cinfo);
            SAFE_DELETE(data->cinfo);
        }
        if (data->dinfo) {
            jpeg_destroy_decompress(data->dinfo);
            SAFE_DELETE(data->dinfo);
        }
    }
}
