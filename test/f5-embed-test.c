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
    jpeg_data_t jpeg_data;
    jpeg_internal_data_t jpeg_internal;
    int ret = 0;

    uint8_t error = io_jpeg_read("bild.jpg", &jpeg_data, &jpeg_internal);
    if(error != 0) {
        printf("Error: %d\n", lstg_errno);
        return 1;   
    }
    uint32_t width = jpeg_data.size_x;
    printf("Size: %dx%dpx\n", jpeg_data.size_x, jpeg_data.size_y);

    f5_parameter para;
    para.password = "foobar1";
    para.pwlen = 7;
    uint32_t capa = 0;
    //battlesteg_check_capacity(&rgb_data, &para, &capa);
    //printf("Capacity = %d Bits = %d Bytes\n", capa, capa/8);
    jpeg_data_t stego_data;
    char *message = "Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................Hallo Welt! Test............................";
    //char *message = "Hallo Welt! Test............................";
    printf("Embedding \"%s\"\n", message);
    error = f5_embed(&jpeg_data, &stego_data, message, 705, &para);
    if(error != 0) {
        printf("Error: %d\n", lstg_errno);
        return 1;   
    }
    int i, j, k, diff = 0;
    for (i = 0; i < stego_data.comp_num; i++) {
        
        // iterate blocks
        for (j = 0; j < stego_data.comp[i].nblocks; j++) {
            // iterate coefficient, skip DC
            for (k = 0; k < 64; k++) {
                //printf("Compare comp %d, block %d, value %d\n", i,j,k);
                if (stego_data.comp[i].blocks[j].values[k] != jpeg_data.comp[i].blocks[j].values[k]) {
                    diff++;
                    /* printf("%d -> %d\n",  */
                    /*        jpeg_data.comp[i].blocks[j].values[k], */
                    /*        stego_data.comp[i].blocks[j].values[k]);  */
                }
            }
        }
    }
    printf("%d diffs\n", diff);
    io_jpeg_integrate(&jpeg_internal, &stego_data);
    io_jpeg_write("f5.jpeg", &jpeg_internal);
    io_jpeg_cleanup_data(&stego_data);
    io_jpeg_cleanup_data(&jpeg_data);
    io_jpeg_cleanup_internal_data(&jpeg_internal);
    return 0;
}

