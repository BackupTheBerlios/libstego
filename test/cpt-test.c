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
 
 
 
#include "libstego/cpt.h"
#include "libstego/types.h"
#include "libstego/errors.h"
#include "libstego/png.h"
#include "libstego/random.h"

#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    

    rgb_data_t cover_image, stego_image;
    png_internal_data_t png_internal;
    
    uint32_t error;
    error = io_png_read("bild.png", &cover_image, &png_internal);
    if(error != 0) {
        printf("Error: %d\n", lstg_errno);
        return 1;
    }
    
    cpt_parameter para;

    para.block_width = 4;
    para.block_height = 4;
    para.password = "asdfasdd";
    para.pwlen = 8;
    
    uint8_t *message = "It's a secret to everybody";
    uint32_t msglen = strlen(message);
    
    error = cpt_embed(&cover_image, &stego_image, message, msglen, &para);
    if(error!=LSTG_OK) {
        printf("failed to embed(%s)\n", lstg_strerror(lstg_errno));
        return lstg_errno;
    }


    /* for (uint32_t x = 0; x < stego_image.size_x; ++x) { */
    /*     for (uint32_t y = 0; y < stego_image.size_y; ++y) { */
    /*         rgb_pixel_t *cover_pixel = &cover_image.data[y*cover_image.size_x + x]; */
    /*         rgb_pixel_t *stego_pixel = &stego_image.data[y*stego_image.size_x + x]; */
    /*         if(cover_pixel->comp.r != stego_pixel->comp.r || */
    /*            cover_pixel->comp.g != stego_pixel->comp.g || */
    /*            cover_pixel->comp.b != stego_pixel->comp.b) { */
    /*             printf("diff\n"); */
    /*         } */
    /*     } */
    /* } */
    error = io_png_integrate(&png_internal, &stego_image);
    if(error!=LSTG_OK) {
	printf("failed to integrate stego data(%s)\n", lstg_strerror(lstg_errno));
	return lstg_errno;
    }
    
    error = io_png_write("cpt_stego.png", &png_internal);
    if(error!=LSTG_OK) {
	printf("failed to write stego file(%s)\n", lstg_strerror(lstg_errno));
	return lstg_errno;
    }
    
    
    error = io_png_read("cpt_stego.png", &stego_image, &png_internal);
    if(error!=LSTG_OK) {
	printf("failed to open stego file(%s)\n", lstg_strerror(lstg_errno));
	return lstg_errno;
    }
    uint8_t *stegomsg = 0;
    uint32_t stegolen = 0;
    
    error = cpt_extract(&stego_image, &stegomsg, &stegolen, &para);
    if(error!=LSTG_OK) {
        printf("failed to extract(%s)\n", lstg_strerror(lstg_errno));
        return lstg_errno;
    }

    printf("extracted %d bytes\n", stegolen);
    printf("the message was: %s \n", stegomsg);
    io_png_cleanup_data(&cover_image);
    io_png_cleanup_data(&stego_image);
    io_png_cleanup_internal(&png_internal);
    free(stegomsg);
    return 0;
}
