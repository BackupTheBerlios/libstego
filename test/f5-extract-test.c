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
 
 
 
#include <libstego/jpeg.h>
#include <stdio.h>
#include <libstego/errors.h>
#include <libstego/libstego.h>

int main(int argc, char *argv[]) {
    printf("f5-extract-test\n");
    jpeg_data_t stego_data;
    jpeg_internal_data_t jpeg_internal;
    int ret = 0;
    int i;
    uint32_t error = io_jpeg_read("f5.jpeg", &stego_data, &jpeg_internal);
    if(error != LSTG_OK) {
        printf("Error in io_jpeg_read: %d %d\n", lstg_errno, error);
        return 1;   
    }

    
    uint32_t width = stego_data.size_x;
    printf("Size: %dx%dpx\n", stego_data.size_x, stego_data.size_y);

    f5_parameter para;
    para.password = "foobar1";
    para.pwlen = 7;
    uint32_t capa = 0;

    uint8_t *msg = NULL;
    uint32_t msg_len;
    printf("Begin extract\n");
    error = f5_extract(&stego_data, &msg, &msg_len, &para);
    if(error != LSTG_OK) {
        printf("error\n");
    }
    printf("%s\n", msg);
    free(msg);
    io_jpeg_cleanup_data(&stego_data);
    io_jpeg_cleanup_internal_data(&jpeg_internal);
    return 0;
}

