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
 
 
 
#include <libstego/pq.h>
#include <libstego/jpeg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {


    jpeg_data_t jpeg_data;
    jpeg_internal_data_t jpeg_internal;
    int ret = 0;

    uint8_t error = io_jpeg_read("bild.jpg", &jpeg_data, &jpeg_internal);
    if (error != 0) {
        printf("Error: %d\n", lstg_errno);
        return EXIT_FAILURE;
    }

    uint32_t width = jpeg_data.size_y;
    printf("Size: %dx%dpx\n", jpeg_data.size_x, jpeg_data.size_y);

    pq_parameter param;
    param.password = "password";
    param.pwlen = 8;
    param.quality = 0;
    param.header_size = 64;
    uint32_t capacity = 0;
    pq_check_capacity(&jpeg_data, &param, &capacity);
    printf("Capacity = %d Bits = %d Bytes\n", capacity, capacity / 8);
    jpeg_data_t stego_data;
    char *message = "HALLOWELT";
    printf("Embedding \"%s\"\n", message);
    error = pq_embed(&jpeg_data, &stego_data, message, 10, &param);
    if (!error) {
        io_jpeg_integrate(&jpeg_internal, &stego_data);
        io_jpeg_write("test-embed.jpg", &jpeg_internal);
    }
    else {
        printf("Error (%d) %s\n",lstg_errno, lstg_strerror(lstg_errno));
    }
    io_jpeg_cleanup_internal_data(&jpeg_internal);
    io_jpeg_cleanup_data(&stego_data);
    io_jpeg_cleanup_data(&jpeg_data);
    return EXIT_SUCCESS;
}
